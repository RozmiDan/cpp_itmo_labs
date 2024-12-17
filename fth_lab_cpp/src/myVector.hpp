#pragma once
#include <unordered_map>
#include <map>
#include <cmath>
#include <algorithm>
#include <stdexcept>

template <typename T>
class SparseVector {
public:
    using Iterator = typename std::map<size_t, T>::iterator;
    using ConstIterator = typename std::map<size_t, T>::const_iterator;

    // Конструктор по умолчанию
    SparseVector() = default;

    // Конструктор, принимающий размер (size)
    explicit SparseVector(size_t size) : size_(size) {}

    // Доступ по индексу
    T operator[](size_t idx) const {
        auto it = mainData_.find(idx);
        return (it != mainData_.end()) ? it->second : T{};
    }

    // Установка значения по индексу
    void setElement(size_t idx, const T& value) {
        auto it = mainData_.find(idx);
        if (it != mainData_.end()) {
            // Элемент уже существует
            if (value == T{}) {
                mainData_.erase(idx);
                orderedData_.erase(idx);
            }
            else {
                it->second = value;
                orderedData_[idx] = value;
                maxIndex_ = std::max(maxIndex_, idx);
            }
        }
        else {
            // Элемента нет, добавляем только если value != 0
            if (value != T{}) {
                mainData_[idx] = value;
                orderedData_[idx] = value;
                maxIndex_ = std::max(maxIndex_, idx);
            }
        }
    }

    // Удаление элемента
    void removeElement(size_t idx) {
        mainData_.erase(idx);
        orderedData_.erase(idx);
        if (!orderedData_.empty()) {
            maxIndex_ = orderedData_.rbegin()->first;
        }
        else {
            maxIndex_ = 0;
        }
    }

    Iterator begin() {
        return orderedData_.begin();
    }

    Iterator end() {
        return orderedData_.end();
    }

    ConstIterator cbegin() const {
        return orderedData_.cbegin();
    }

    ConstIterator cend() const {
        return orderedData_.cend();
    }

    size_t size() const { return mainData_.size(); }

    void clearAll() {
        mainData_.clear();
        orderedData_.clear();
        maxIndex_ = 0;
    }

    // Унарный минус 
    SparseVector operator-() const {
        SparseVector result(size_);
        for (auto& [idx, val] : mainData_) {
            result.setElement(idx, -val);
        }
        return result;
    }

    // Операция сложения двух векторов
    SparseVector operator+(const SparseVector& other) const {
        SparseVector result(std::max(size_, other.size_));
        for (auto& [idx, val] : mainData_) {
            result.setElement(idx, val);
        }
        for (auto& [idx, val] : other.mainData_) {
            T newVal = result[idx] + val;
            result.setElement(idx, newVal);
        }
        return result;
    }

    // Операция вычитания двух векторов
    SparseVector operator-(const SparseVector& other) const {
        SparseVector result(std::max(size_, other.size_));
        for (auto& [idx, val] : mainData_) {
            result.setElement(idx, val);
        }
        for (auto& [idx, val] : other.mainData_) {
            T newVal = result[idx] - val;
            result.setElement(idx, newVal);
        }
        return result;
    }

    // Умножение на скаляр
    SparseVector operator*(const T& scalar) const {
        SparseVector result(size_);
        for (auto& [idx, val] : mainData_) {
            T newVal = val * scalar;
            result.setElement(idx, newVal);
        }
        return result;
    }

    // Деление на скаляр
    SparseVector operator/(const T& scalar) const {
        if (scalar == T{}) {
            throw std::invalid_argument("Division by zero");
        }
        SparseVector result(size_);
        for (auto& [idx, val] : mainData_) {
            T newVal = val / scalar;
            result.setElement(idx, newVal);
        }
        return result;
    }

    // Сложение с числом (скаляр)
    SparseVector operator+(const T& scalar) const {
        SparseVector result(size_);
        for (auto& [idx, val] : mainData_) {
            T newVal = val + scalar;
            result.setElement(idx, newVal);
        }
        return result;
    }

    // Вычитание скаляра
    SparseVector operator-(const T& scalar) const {
        SparseVector result(size_);
        for (auto& [idx, val] : mainData_) {
            T newVal = val - scalar;
            result.setElement(idx, newVal);
        }
        return result;
    }

    // Возведение в степень всех ненулевых элементов
    SparseVector powerAll(const T& exponent) const {
        SparseVector result(size_);
        for (auto& [idx, val] : mainData_) {
            T newVal = static_cast<T>(std::pow(static_cast<double>(val), static_cast<double>(exponent)));
            result.setElement(idx, newVal);
        }
        return result;
    }

    // Скалярное произведение 
    T dot(const SparseVector& other) const {
        T result = T{};
        for (auto& [idx, val] : mainData_) {
            result += val * other[idx];
        }
        return result;
    }

    // Операторы сравнения 
    bool operator==(const SparseVector& other) const {
        if (this->size() != other.size()) return false;
        // Проверяем все элементы
        for (auto& [idx, val] : mainData_) {
            if (other[idx] != val) return false;
        }
        // Проверяем, нет ли в other элементов, отсутствующих в this
        for (auto& [idx, val] : other.mainData_) {
            if ((*this)[idx] != val) return false;
        }
        return true;
    }

    bool operator!=(const SparseVector& other) const {
        return !(*this == other);
    }

    bool operator<(const SparseVector& other) const {
        return this->size() < other.size();
    }

    bool operator>(const SparseVector& other) const {
        return this->size() > other.size();
    }

private:
    std::unordered_map<size_t, T> mainData_;
    std::map<size_t, T> orderedData_;
    size_t maxIndex_ = 0;
    size_t size_ = 0; 
};

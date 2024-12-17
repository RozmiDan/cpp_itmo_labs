#pragma once
#include <cstddef>
#include <stdexcept>
#include <ostream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <cassert>
#include "myVector.hpp"

struct pair_hash {
    size_t operator()(const std::pair<size_t, size_t>& p) const {
        auto h1 = std::hash<size_t>()(p.first);
        auto h2 = std::hash<size_t>()(p.second);
        return h1 ^ (h2 + 0x9e3779b97f4a7c16ULL + (h1 << 6) + (h1 >> 2));
    }
};

template <typename T>
class SparseMatrix {
public:
    using Position = std::pair<size_t, size_t>;
    using Iterator = typename std::map<Position, T>::iterator;
    using ConstIterator = typename std::map<Position, T>::const_iterator;

    SparseMatrix() = default;
    SparseMatrix(size_t rows, size_t cols) : maxRow_(rows - 1), maxCol_(cols - 1) {}

    // Доступ к элементам
    T operator()(size_t row, size_t col) const {
        auto it = mainData_.find({ row, col });
        return (it != mainData_.end()) ? it->second : T{};
    }

    // Установка элемента
    void setElement(size_t row, size_t col, const T& value) {
        Position pos = { row, col };
        auto it = mainData_.find(pos);
        if (it != mainData_.end()) {
            // Элемент уже существует
            if (value == T{}) {
                mainData_.erase(pos);
                orderedData_.erase(pos);
            }
            else {
                it->second = value;
                orderedData_[pos] = value;
            }
        }
        else {
            if (value != T{}) {
                mainData_[pos] = value;
                orderedData_[pos] = value;
            }
        }
        maxRow_ = std::max(maxRow_, row);
        maxCol_ = std::max(maxCol_, col);
    }

    // Удаление элемента
    void removeElement(size_t row, size_t col) {
        Position pos = { row, col };
        mainData_.erase(pos);
        orderedData_.erase(pos);

        recalcMaxIndices();
    }

    // Получение размера (число ненулевых элементов)
    size_t size() const {
        return mainData_.size();
    }

    // Очистка
    void clearAll() {
        mainData_.clear();
        orderedData_.clear();
        maxRow_ = 0;
        maxCol_ = 0;
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

    // Транспонирование матрицы
    SparseMatrix transpose() const {
        SparseMatrix result(maxCol_ + 1, maxRow_ + 1);
        for (auto& kv : mainData_) {
            auto pos = kv.first;
            result.setElement(pos.second, pos.first, kv.second);
        }
        return result;
    }

    // Сложение с числом
    SparseMatrix operator+(const T& scalar) const {
        SparseMatrix result(maxRow_ + 1, maxCol_ + 1);
        for (auto& kv : mainData_) {
            auto pos = kv.first;
            T val = kv.second + scalar;
            result.setElement(pos.first, pos.second, val);
        }
        return result;
    }

    SparseMatrix operator-(const SparseMatrix& other) const {
        checkDimensions(other);

        SparseMatrix result(maxRow_ + 1, maxCol_ + 1);
        // Сначала копируем this
        for (auto& kv : mainData_) {
            T val = kv.second - other(kv.first.first, kv.first.second);
            result.setElement(kv.first.first, kv.first.second, val);
        }

        // Теперь учитываем те элементы other, которых не было в this
        for (auto& kv : other.mainData_) {
            if (mainData_.find(kv.first) == mainData_.end()) {
                T val = -kv.second;
                result.setElement(kv.first.first, kv.first.second, val);
            }
        }

        return result;
    }

    SparseMatrix operator-(const T& scalar) const {
        SparseMatrix result(maxRow_ + 1, maxCol_ + 1);
        for (auto& kv : mainData_) {
            auto pos = kv.first;
            T val = kv.second - scalar;
            result.setElement(pos.first, pos.second, val);
        }
        return result;
    }

    SparseMatrix operator*(const T& scalar) const {
        SparseMatrix result(maxRow_ + 1, maxCol_ + 1);
        for (auto& kv : mainData_) {
            auto pos = kv.first;
            T val = kv.second * scalar;
            result.setElement(pos.first, pos.second, val);
        }
        return result;
    }

    SparseMatrix operator/(const T& scalar) const {
        if (scalar == T{}) {
            throw std::invalid_argument("Division by zero");
        }
        SparseMatrix result(maxRow_ + 1, maxCol_ + 1);
        for (auto& kv : mainData_) {
            auto pos = kv.first;
            T val = kv.second / scalar;
            result.setElement(pos.first, pos.second, val);
        }
        return result;
    }

    // Сложение матриц
    SparseMatrix operator+(const SparseMatrix& other) const {
        checkDimensions(other);

        SparseMatrix result(maxRow_ + 1, maxCol_ + 1);
        // Сначала копируем this
        for (auto& kv : mainData_) {
            result.setElement(kv.first.first, kv.first.second, kv.second);
        }
        // Добавляем other
        for (auto& kv : other.mainData_) {
            T val = result(kv.first.first, kv.first.second) + kv.second;
            result.setElement(kv.first.first, kv.first.second, val);
        }
        return result;
    }

    // Матрично-векторное умножение
    SparseVector<T> operator*(const SparseVector<T>& vec) const {
        SparseVector<T> result(maxRow_ + 1);
        for (auto& kv : mainData_) {
            size_t row = kv.first.first;
            size_t col = kv.first.second;
            T val = result[row] + kv.second * vec[col];
            result.setElement(row, val);
        }
        return result;
    }

    // Матричное умножение
    SparseMatrix operator*(const SparseMatrix& other) const {
        if (maxCol_ != other.maxRow_) {
            throw std::runtime_error("Matrix dimensions do not match for multiplication.");
        }
        SparseMatrix result(maxRow_ + 1, other.maxCol_ + 1);

        // Умножение матриц: (A * B)[i,j] = sum_over_k A[i,k]*B[k,j]
        for (auto& kvA : mainData_) {
            size_t i = kvA.first.first;
            size_t k = kvA.first.second;
            T valA = kvA.second;

            for (auto& kvB : other.mainData_) {
                if (kvB.first.first == k) {
                    size_t j = kvB.first.second;
                    T valB = kvB.second;
                    T current = result(i, j) + valA * valB;
                    result.setElement(i, j, current);
                }
            }
        }

        return result;
    }

    // Поэлементное возведение в степень
    SparseMatrix powerAll(const T& exponent) const {
        SparseMatrix result(maxRow_ + 1, maxCol_ + 1);
        for (auto& kv : mainData_) {
            auto pos = kv.first;
            T val = std::pow(kv.second, exponent);
            result.setElement(pos.first, pos.second, val);
        }
        return result;
    }

    bool operator==(const SparseMatrix& other) const {
        if (maxRow_ != other.maxRow_ || maxCol_ != other.maxCol_)
            return false;
        for (auto& kv : mainData_) {
            if (other(kv.first.first, kv.first.second) != kv.second)
                return false;
        }
        for (auto& kv : other.mainData_) {
            if ((*this)(kv.first.first, kv.first.second) != kv.second)
                return false;
        }
        return true;
    }

    bool operator!=(const SparseMatrix& other) const {
        return !(*this == other);
    }

    bool isSquare() const {
        return maxRow_ == maxCol_;
    }

    static SparseMatrix identity(size_t size) {
        SparseMatrix result(size, size);
        for (size_t i = 0; i < size; ++i) {
            result.setElement(i, i, 1);
        }
        return result;
    }

    static SparseMatrix zeros(size_t rows, size_t cols) {
        // Пустая матрица без ненулевых элементов
        return SparseMatrix(rows, cols);
    }

    // Возведение в целочисленную степень
    SparseMatrix integerPower(int n) const {
        if (!isSquare()) {
            throw std::invalid_argument("Matrix must be square to raise to a power.");
        }
        if (n == 0) {
            return SparseMatrix::identity(maxRow_ + 1);
        }
        else if (n < 0) {
            SparseMatrix inv = this->inverse();
            return inv.integerPower(-n);
        }
        else {
            // Двоичное возведение в степень
            SparseMatrix base = *this;
            SparseMatrix result = SparseMatrix::identity(maxRow_ + 1);
            int exp = n;

            while (exp > 0) {
                if (exp & 1) {
                    result = result * base;
                }
                base = base * base;
                exp >>= 1;
            }

            return result;
        }
    }

    // Обращение матрицы (метод Гаусса) - упрощенная версия
    SparseMatrix inverse() const {
        if (!isSquare()) {
            throw std::invalid_argument("Matrix must be square to invert.");
        }

        size_t n = maxRow_ + 1;
        SparseMatrix aug(n, n * 2);
        // Формируем [A|I]
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                aug.setElement(i, j, (*this)(i, j));
            }
            aug.setElement(i, i + n, 1);
        }

        // Применяем метод Гаусса-Жордана
        for (size_t i = 0; i < n; ++i) {
            // Находим максимальный элемент в столбце i для выбора ведущей строки
            size_t maxRow = i;
            T maxVal = std::abs(aug(i, i));
            for (size_t r = i + 1; r < n; ++r) {
                T val = std::abs(aug(r, i));
                if (val > maxVal) {
                    maxVal = val;
                    maxRow = r;
                }
            }

            if (maxVal == T{}) {
                throw std::runtime_error("Matrix is singular and cannot be inverted.");
            }

            // Перестановка строк
            if (maxRow != i) {
                for (size_t c = 0; c < 2 * n; ++c) {
                    T temp = aug(i, c);
                    aug.setElement(i, c, aug(maxRow, c));
                    aug.setElement(maxRow, c, temp);
                }
            }

            // Нормируем ведущий элемент до 1
            T pivot = aug(i, i);
            for (size_t c = 0; c < 2 * n; ++c) {
                T val = aug(i, c) / pivot;
                aug.setElement(i, c, val);
            }

            // Обнуление элементов в других строках
            for (size_t r = 0; r < n; ++r) {
                if (r != i) {
                    T factor = aug(r, i);
                    for (size_t c = 0; c < 2 * n; ++c) {
                        T val = aug(r, c) - factor * aug(i, c);
                        aug.setElement(r, c, val);
                    }
                }
            }
        }

        SparseMatrix inverseMat(n, n);
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                inverseMat.setElement(i, j, aug(i, j + n));
            }
        }

        return inverseMat;
    }

    // Возведение в вещественную степень: A^p = exp(p * log(A))
    // Требует логарифма и экспоненты матрицы
    SparseMatrix doublePower(double p) const {
        if (!isSquare()) {
            throw std::invalid_argument("Matrix must be square to raise to a real power.");
        }
        SparseMatrix logA = this->log();
        SparseMatrix pLogA = logA * p;
        return pLogA.exp();
    }

    // Простая оценка нормы Фробениуса для проверки условий вычислений
    double frobeniusNorm() const {
        double norm = 0.0;
        for (size_t i = 0; i <= maxRow_; ++i) {
            for (size_t j = 0; j <= maxCol_; ++j) {
                double val = static_cast<double>((*this)(i, j));
                norm += val * val;
            }
        }
        return std::sqrt(norm);
    }

    // Приблизительный логарифм матрицы через ряд
    SparseMatrix log(int approxOrder = 50) const {
        if (!isSquare()) {
            throw std::invalid_argument("Matrix must be square.");
        }

        SparseMatrix I = SparseMatrix::identity(maxRow_ + 1);
        if (*this == I) {
            return SparseMatrix::zeros(maxRow_ + 1, maxCol_ + 1);
        }

        double norm = frobeniusNorm();
        if (norm >= 1)
            throw std::invalid_argument("Matrix norm >= 1, cannot approximate log.");

        SparseMatrix A_minus_I = (*this) - I;
        SparseMatrix result = SparseMatrix::zeros(maxRow_ + 1, maxCol_ + 1);
        SparseMatrix term = A_minus_I;

        for (int n = 1; n <= approxOrder; ++n) {
            if (n > 1) {
                term = term * A_minus_I;
            }
            if (n % 2 == 1) {
                result = result + (term * (1.0 / n));
            }
            else {
                result = result - (term * (1.0 / n));
            }
        }

        return result;
    }

    // Приблизительная экспонента матрицы
    SparseMatrix exp(int approxOrder = 20) const {
        if (!isSquare()) {
            throw std::invalid_argument("Matrix must be square to compute exp.");
        }

        SparseMatrix result = SparseMatrix::identity(maxRow_ + 1);
        SparseMatrix term = *this;

        result = result + term;

        for (int n = 2; n < approxOrder; ++n) {
            term = term * (*this);
            term = term / static_cast<T>(n);
            result = result + term;
        }

        return result;
    }


private:
    std::unordered_map<Position, T, pair_hash> mainData_;
    std::map<Position, T> orderedData_;
    size_t maxRow_ = 0;
    size_t maxCol_ = 0;

    void recalcMaxIndices() {
        maxRow_ = 0;
        maxCol_ = 0;
        for (auto& kv : mainData_) {
            maxRow_ = std::max(maxRow_, kv.first.first);
            maxCol_ = std::max(maxCol_, kv.first.second);
        }
    }

    void checkDimensions(const SparseMatrix& other) const {
        if (maxRow_ != other.maxRow_ || maxCol_ != other.maxCol_) {
            throw std::invalid_argument("Matrices must have the same dimensions.");
        }
    }
};

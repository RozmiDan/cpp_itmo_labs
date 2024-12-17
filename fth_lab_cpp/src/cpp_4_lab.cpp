#include <cassert>
#include <iostream>
#include <chrono>
#include <random>
#include "myVector.hpp" 
#include "myMatrix.hpp"

void testMatrixRealis();
void testVectorRealis();
void testAdvancedMatrixOperations();

template<typename T>
class DenseVector {
public:
    DenseVector(size_t size) : data_(size, T{}) {}
    T& operator[](size_t i) { return data_[i]; }
    const T& operator[](size_t i) const { return data_[i]; }
    size_t size() const { return data_.size(); }

    // Пример операции: скалярное произведение
    T dot(const DenseVector& other) const {
        if (other.size() != size()) throw std::invalid_argument("Size mismatch");
        T result = T{};
        for (size_t i = 0; i < size(); ++i) {
            result += data_[i] * other.data_[i];
        }
        return result;
    }

private:
    std::vector<T> data_;
};

// Плотная реализация матрицы 
template<typename T>
class DenseMatrix {
public:
    DenseMatrix(size_t rows, size_t cols) : rows_(rows), cols_(cols), data_(rows* cols, T{}) {}

    T& operator()(size_t r, size_t c) {
        return data_[r * cols_ + c];
    }

    const T& operator()(size_t r, size_t c) const {
        return data_[r * cols_ + c];
    }

    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }

    // Пример операции: умножение матриц
    DenseMatrix operator*(const DenseMatrix& other) const {
        if (cols_ != other.rows_) {
            throw std::invalid_argument("Dimension mismatch");
        }
        DenseMatrix result(rows_, other.cols_);
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < other.cols_; ++j) {
                T sum = T{};
                for (size_t k = 0; k < cols_; ++k) {
                    sum += (*this)(i, k) * other(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
    }

private:
    size_t rows_, cols_;
    std::vector<T> data_;
};


int main() {
    testVectorRealis();
    testMatrixRealis();
    testAdvancedMatrixOperations();

    using T = double;

    // Параметры теста
    size_t n = 500; 
    double sparsity = 0.01; // доля ненулевых элементов (1% ненулевых)

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<T> dist_val(0.0, 10.0);
    std::uniform_real_distribution<double> dist_sparse(0.0, 1.0);

    // Генерируем разреженную матрицу
    SparseMatrix<T> sparseMat(n, n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double p = dist_sparse(gen);
            if (p < sparsity) {
                T val = dist_val(gen);
                sparseMat.setElement(i, j, val);
            }
        }
    }

    // Генерируем плотную матрицу
    DenseMatrix<T> denseMat(n, n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double p = dist_sparse(gen);
            if (p < sparsity) {
                denseMat(i, j) = dist_val(gen);
            }
            else {
                denseMat(i, j) = 0.0;
            }
        }
    }

    // Сравним умножение матриц sparseMat*sparseMat и denseMat*denseMat
    // Измеряем время для разреженной матрицы
    auto start = std::chrono::high_resolution_clock::now();
    auto sparseRes = sparseMat * sparseMat;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> sparseTime = end - start;
    std::cout << "Sparse multiplication time: " << sparseTime.count() << " s\n";

    // Измеряем время для плотной матрицы
    start = std::chrono::high_resolution_clock::now();
    auto denseRes = denseMat * denseMat;
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> denseTime = end - start;
    std::cout << "Dense multiplication time: " << denseTime.count() << " s\n";

    // Возведение в степень (целочисленное)
    int exp = 2;
    start = std::chrono::high_resolution_clock::now();
    auto sparsePow = sparseMat.integerPower(exp);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Sparse integerPower time: " << (end - start).count() / 1e9 << " s\n";

    std::cout << "All performance tests done.\n";

    return 0;
}

void testAdvancedMatrixOperations() {
    // Тестируем обращение матрицы
    // A = [[1,2],[3,4]]
    // A^-1 = [[-2,1],[1.5,-0.5]]
    SparseMatrix<double> A(2, 2);
    A.setElement(0, 0, 1);
    A.setElement(0, 1, 2);
    A.setElement(1, 0, 3);
    A.setElement(1, 1, 4);

    auto A_inv = A.inverse();
    // Проверяем обратную матрицу
    assert(std::abs(A_inv(0, 0) - (-2.0)) < 1e-9);
    assert(std::abs(A_inv(0, 1) - 1.0) < 1e-9);
    assert(std::abs(A_inv(1, 0) - 1.5) < 1e-9);
    assert(std::abs(A_inv(1, 1) - (-0.5)) < 1e-9);

    // Проверим, что A * A_inv = I
    auto I_check = A * A_inv;
    // Должна получиться единичная матрица
    assert(std::abs(I_check(0, 0) - 1.0) < 1e-9);
    assert(std::abs(I_check(0, 1) - 0.0) < 1e-9);
    assert(std::abs(I_check(1, 0) - 0.0) < 1e-9);
    assert(std::abs(I_check(1, 1) - 1.0) < 1e-9);

    // Тестируем целочисленное возведение в степень
    // A^2 для A = [[1,2],[3,4]]
    // A^2 = [[7,10],[15,22]]
    auto A2 = A.integerPower(2);
    assert(A2(0, 0) == 7);
    assert(A2(0, 1) == 10);
    assert(A2(1, 0) == 15);
    assert(A2(1, 1) == 22);

    // Проверим возведение единичной матрицы в степень
    auto I = SparseMatrix<double>::identity(3);
    auto I3 = I.integerPower(3);
    // I^3 = I
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            double expected = (i == j) ? 1.0 : 0.0;
            assert(std::abs(I3(i, j) - expected) < 1e-9);
        }
    }

    // Тестируем вещественную степень
    // doublePower(p): I^p = I
    auto I_double_power = I.doublePower(2.5);
    // Должна получиться I, так как I^любая_степень = I
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            double expected = (i == j) ? 1.0 : 0.0;
            assert(std::abs(I_double_power(i, j) - expected) < 1e-9);
        }
    }

    // Тестируем умножение матриц
    // Возьмем A и умножим на A_inv, результат должен дать I
    auto AAI = A * A_inv;
    assert(std::abs(AAI(0, 0) - 1.0) < 1e-9);
    assert(std::abs(AAI(0, 1) - 0.0) < 1e-9);
    assert(std::abs(AAI(1, 0) - 0.0) < 1e-9);
    assert(std::abs(AAI(1, 1) - 1.0) < 1e-9);

    std::cout << "All advanced matrix operations tests passed successfully!" << std::endl;
}

void testMatrixRealis() {

        // Создадим разреженную матрицу 3x3
        SparseMatrix<int> mat(3, 3);

        // Проверим начальное состояние
        assert(mat.size() == 0);
        assert(mat(0, 0) == 0);

        // Установим некоторые значения
        mat.setElement(0, 0, 1);
        mat.setElement(1, 1, 2);
        mat.setElement(2, 2, 3);

        assert(mat.size() == 3);
        assert(mat(0, 0) == 1);
        assert(mat(1, 1) == 2);
        assert(mat(2, 2) == 3);

        // Проверим транспонирование
        auto trans = mat.transpose();
        assert(trans(0, 0) == 1);
        assert(trans(1, 1) == 2);
        assert(trans(2, 2) == 3);

        // Проверим сложение с числом
        auto mat_plus_scalar = mat + 5;
        // Каждый ненулевой элемент увеличится на 5
        assert(mat_plus_scalar(0, 0) == 6); // 1 + 5
        assert(mat_plus_scalar(1, 1) == 7); // 2 + 5
        assert(mat_plus_scalar(2, 2) == 8); // 3 + 5

        // Проверим вычитание числа
        auto mat_minus_scalar = mat - 1;
        assert(mat_minus_scalar(0, 0) == 0); // 1 - 1
        assert(mat_minus_scalar(1, 1) == 1); // 2 - 1
        assert(mat_minus_scalar(2, 2) == 2); // 3 - 1

        // Проверим умножение на число
        auto mat_times_scalar = mat * 2;
        assert(mat_times_scalar(0, 0) == 2); // 1*2
        assert(mat_times_scalar(1, 1) == 4); // 2*2
        assert(mat_times_scalar(2, 2) == 6); // 3*2

        // Проверим деление на число
        auto mat_div_scalar = mat / 1;
        // Деление на 1 не изменит значения
        assert(mat_div_scalar(0, 0) == 1);
        assert(mat_div_scalar(1, 1) == 2);
        assert(mat_div_scalar(2, 2) == 3);

        // Проверим поэлементное возведение в степень
        auto mat_pow = mat.powerAll(2); // Возведем в квадрат
        assert(mat_pow(0, 0) == 1); // 1^2 = 1
        assert(mat_pow(1, 1) == 4); // 2^2 = 4
        assert(mat_pow(2, 2) == 9); // 3^2 = 9

        // Проверим операцию сложения матриц
        SparseMatrix<int> mat2(3, 3);
        mat2.setElement(0, 0, 5);
        mat2.setElement(1, 1, 5);
        mat2.setElement(2, 2, 5);

        auto mat_sum = mat + mat2;
        assert(mat_sum(0, 0) == 6);  // 1+5
        assert(mat_sum(1, 1) == 7);  // 2+5
        assert(mat_sum(2, 2) == 8);  // 3+5

        // Проверим операцию вычитания матриц
        auto mat_sub = mat - mat2;
        assert(mat_sub(0, 0) == -4); // 1-5
        assert(mat_sub(1, 1) == -3); // 2-5
        assert(mat_sub(2, 2) == -2); // 3-5

        // Проверим умножение матрицы на вектор
        SparseVector<int> vec;
        vec.setElement(0, 2);
        vec.setElement(2, 4);
        // mat: (0,0)=1, (1,1)=2, (2,2)=3
        // умножение матрицы на вектор:
        // result[0] = 1*2 + 0*... = 2
        // result[1] = 0 + 2*0 + ... = 0
        // result[2] = 0 + ... + 3*4 = 12
        auto vec_res = mat * vec;
        assert(vec_res[0] == 2);
        assert(vec_res[1] == 0);
        assert(vec_res[2] == 12);

        // Проверим оператор сравнения
        SparseMatrix<int> mat3(3, 3);
        mat3.setElement(0, 0, 1);
        mat3.setElement(1, 1, 2);
        mat3.setElement(2, 2, 3);

        assert(mat == mat3); // Должны быть равны
        mat3.setElement(0, 1, 10);
        assert(mat != mat3); // Теперь не равны

        // Проверим удаление элемента
        mat.removeElement(0, 0);
        assert(mat(0, 0) == 0);
        assert(mat.size() == 2); // Было 3 элемента, один удалили

        // Проверим очистку
        mat.clearAll();
        assert(mat.size() == 0);

        std::cout << "All SparseMatrix tests passed successfully!" << std::endl;

}

void testVectorRealis() {
    // Создаем разреженный вектор
    SparseVector<int> vec;

    // Проверяем начальное состояние
    assert(vec.size() == 0);
    assert(vec[0] == 0); // Неинициализированный индекс возвращает 0

    // Устанавливаем некоторые значения
    vec.setElement(2, 10);
    vec.setElement(5, 3);
    vec.setElement(10, 0); // Нулевое значение не должно добавляться

    // Проверяем установку
    assert(vec.size() == 2); // Добавили два ненулевых элемента
    assert(vec[2] == 10);
    assert(vec[5] == 3);
    assert(vec[10] == 0); // Элемент не был добавлен, так как 0

    // Проверяем унарный минус
    auto neg = -vec;
    assert(neg[2] == -10);
    assert(neg[5] == -3);

    // Проверяем сложение с вектором
    SparseVector<int> vec2;
    vec2.setElement(2, 5);
    vec2.setElement(4, 20);

    auto sum_vec = vec + vec2;
    assert(sum_vec[2] == 15); // 10 + 5
    assert(sum_vec[4] == 20); // 0 + 20
    assert(sum_vec[5] == 3);  // 3 + 0

    // Проверяем вычитание
    auto sub_vec = vec - vec2;
    assert(sub_vec[2] == 5);   // 10 - 5
    assert(sub_vec[4] == -20); // 0 - 20
    assert(sub_vec[5] == 3);   // 3 - 0

    // Проверяем умножение на скаляр
    auto mul_vec = vec * 2;
    assert(mul_vec[2] == 20);
    assert(mul_vec[5] == 6);

    // Проверяем деление на скаляр
    auto div_vec = vec / 2;
    assert(div_vec[2] == 5);
    assert(div_vec[5] == 1);

    // Проверяем добавление скаляра
    auto add_scalar_vec = vec + 5;
    assert(add_scalar_vec[2] == 15); // 10 + 5
    assert(add_scalar_vec[5] == 8);  // 3 + 5

    // Проверяем вычитание скаляра
    auto sub_scalar_vec = vec - 2;
    assert(sub_scalar_vec[2] == 8); // 10 - 2
    assert(sub_scalar_vec[5] == 1); // 3 - 2

    // Проверяем возведение в степень
    // Допустим, возведем в квадрат
    auto pow_vec = vec.powerAll(2);
    assert(pow_vec[2] == 100); // 10^2
    assert(pow_vec[5] == 9);   // 3^2

    // Проверяем скалярное произведение
    // vec: index 2 -> 10, index 5 -> 3
    // vec2: index 2 -> 5, index 4 -> 20
    int dot_res = vec.dot(vec2);
    // скалярное произведение: (10 * 5) + (3 * 0)
    // элементы с индексом 4 в vec равны 0, а в vec2 значение 20 не повлияет
    assert(dot_res == 50);

    // Проверяем сравнение
    // vec = {2->10, 5->3}
    // Создадим еще один вектор vec3 с теми же элементами
    SparseVector<int> vec3;
    vec3.setElement(2, 10);
    vec3.setElement(5, 3);

    assert(vec == vec3);
    vec3.setElement(5, 4);
    assert(vec != vec3);

    // Проверим оператор< и >
    // Сравним по размеру: vec и vec2
    // vec.size() = 2, vec2.size() = 2
    // Если равны по размеру, вернет false
    assert(!(vec < vec2));
    assert(!(vec > vec2));

    // Удаление элемента
    vec.removeElement(5);
    assert(vec.size() == 1);
    assert(vec[5] == 0);

    // Очистка
    vec.clearAll();
    assert(vec.size() == 0);

    std::cout << "All tests passed successfully!" << std::endl;
}
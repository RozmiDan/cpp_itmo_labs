#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <random>
#include <numeric>
#include <iterator>
#include <set>
#include "myClass.hpp"

int main() {
    // Инициализация генератора случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());

    // Генерация размера вектора v1 от 500 до 1000
    std::uniform_int_distribution<> size_dist(500, 1000);
    int v1_size = size_dist(gen);

    // Создание вектора v1
    std::vector<myClass> v1;
    v1.reserve(v1_size); // Резервируем место для элементов

    // Генерация случайных значений для элементов класса myClass
    std::uniform_int_distribution<> data_dist(1, 1000);
    std::uniform_int_distribution<> priority_dist(1, 100);

    // Заполнение вектора v1 случайными экземплярами myClass
    std::generate_n(std::back_inserter(v1), v1_size, [&]() {
        return myClass(data_dist(gen), priority_dist(gen));
        });

    // Задание 2: Создание вектора v2
    int b = v1.size() > 200 ? v1.size() - 200 : 0;
    int e = v1.size();
    std::vector<myClass> v2(v1.begin() + b, v1.begin() + e);

    // Задание 3: Формирование списка list1
    std::uniform_int_distribution<> n_dist(20, 50);
    int n = n_dist(gen);
    n = std::min(n, static_cast<int>(v1.size())); // Убедимся, что n не больше размера v1

    std::vector<myClass> temp(n);
    std::partial_sort_copy(v1.begin(), v1.end(), temp.begin(), temp.end(), std::greater<myClass>());
    std::sort(temp.begin(), temp.end());
    std::list<myClass> list1(temp.begin(), temp.end());

    // Задание 4: Формирование списка list2
    int n2 = n_dist(gen);
    n2 = std::min(n2, static_cast<int>(v2.size())); // Убедимся, что n2 не больше размера v2

    std::nth_element(v2.begin(), v2.begin() + n2, v2.end());
    std::list<myClass> list2(v2.begin(), v2.begin() + n2);

    // Задание 5: Удаление перемещенных элементов из v1 и v2
    std::set<myClass> temp_set(temp.begin(), temp.end());
    v1.erase(std::remove_if(v1.begin(), v1.end(), [&](const myClass& x) {
        return temp_set.find(x) != temp_set.end();
        }), v1.end());

    v2.erase(v2.begin(), v2.begin() + n2);

    // Задание 6: Перегруппировка элементов в list1
    double avg = std::accumulate(list1.begin(), list1.end(), 0.0, [](double sum, const myClass& x) {
        return sum + x.getValue();
        }) / list1.size();

    list1.sort([](const myClass& a, const myClass& b) {
        return a.getValue() > b.getValue();
        });

    // Задание 7: Удаление нечётных элементов из list2
    list2.remove_if([](const myClass& x) {
        return x.getValue() % 2 != 0;
        });

    // Задание 8: Создание вектора v3 из общих элементов
    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());

    std::vector<myClass> v3;
    std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v3));

    // Задание 9: Формирование списка list3 из пар элементов
    if (list1.size() > list2.size()) {
        auto it = list1.begin();
        std::advance(it, list1.size() - list2.size());
        list1.erase(list1.begin(), it);
    }
    else if (list2.size() > list1.size()) {
        auto it = list2.begin();
        std::advance(it, list2.size() - list1.size());
        list2.erase(list2.begin(), it);
    }

    std::list<std::pair<myClass, myClass>> list3;
    std::transform(list1.begin(), list1.end(), list2.begin(), std::back_inserter(list3),
        [](const myClass& a, const myClass& b) {
            return std::make_pair(a, b);
        });

    // Задание 10: Создание вектора пар из v1 и v2 без приведения к одному размеру
    int min_size = std::min(v1.size(), v2.size());

    std::vector<std::pair<myClass, myClass>> v_pairs;
    std::transform(v1.begin(), v1.begin() + min_size, v2.begin(), std::back_inserter(v_pairs),
        [](const myClass& a, const myClass& b) {
            return std::make_pair(a, b);
        });

    // Для проверки можно вывести размеры контейнеров
    std::cout << "v1 size: " << v1.size() << std::endl;
    std::cout << "v2 size: " << v2.size() << std::endl;
    std::cout << "list1 size: " << list1.size() << std::endl;
    std::cout << "list2 size: " << list2.size() << std::endl;
    std::cout << "list3 size: " << list3.size() << std::endl;
    std::cout << "v_pairs size: " << v_pairs.size() << std::endl;

    return 0;
}

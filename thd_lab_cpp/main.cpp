#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <random>
#include <numeric>
#include <iterator>
#include <set>
#include "myClass.hpp"
#include <iterator>

int main() {
    // Инициализация генератора случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());

    // Генерация размера вектора v1 от 500 до 1000
    std::uniform_int_distribution<> size_dist(500, 1000);
    int v1_size = size_dist(gen);

    std::cout << "First task:" << std::endl;

    // Создание вектора v1
    std::vector<myClass> v1;
    v1.reserve(v1_size); // Резервируем место для элементов

    // Генерация случайных значений для элементов класса myClass
    std::uniform_int_distribution<> data_dist(1, 100);
    std::uniform_int_distribution<> priority_dist(1, 1000);

    // Заполнение вектора v1 случайными экземплярами myClass
    std::generate_n(std::back_inserter(v1), v1_size, [&]() {
        return myClass(data_dist(gen), priority_dist(gen));
        });

    std::cout << "Size of v1 = " << v1.size() << std::endl;

    //for (int i = 0; i < v1_size; ++i) {
    //    std::cout << v1[i].getPriority() << " ";
    //}
    //std::cout << std::endl;

    // Задание 2: Создание вектора v2
    std::cout << "Second task:" << std::endl;

    int b = v1.size() > 200 ? v1.size() - 200 : 0;
    int e = v1.size();
    std::vector<myClass> v2(v1.begin() + b, v1.begin() + e);

    std::cout << "Size of v2 = " << v2.size() << std::endl;

  /*  for (int i = 0; i < v2.size(); ++i) {
        std::cout << v2[i].getPriority() << " ";
    }
    std::cout << std::endl;*/

    // Задание 3: Формирование списка list1
    std::cout << "Third task:" << std::endl;

    std::uniform_int_distribution<> n_dist(20, 50);
    int n = n_dist(gen);
    n = std::min(n, static_cast<int>(v1.size())); 

    std::vector<myClass> temp(n);
    std::partial_sort_copy(v1.begin(), v1.end(), temp.begin(), temp.end(), std::greater<myClass>());
    //std::sort(temp.begin(), temp.end(), std::greater<myClass>());
    std::list<myClass> list1(temp.begin(), temp.end());

    //std::cout << "v1 contains prior:" << std::endl;
    //for (int i = 0; i < v1.size(); ++i) {
    //    std::cout << v1[i].getPriority() << " ";
    //}
    //std::cout << std::endl;

    //std::cout << "list1 contains prior:" << std::endl;
    //std::list<myClass>::iterator it_l = list1.begin();
    //for (; it_l != list1.end(); ++it_l) {
    //    std::cout << it_l->getPriority() << " ";
    //}
    //std::cout << std::endl;

    // Задание 4: Формирование списка list2
    std::cout << "Fourth task:" << std::endl;

    int n2 = n_dist(gen);
    std::vector<myClass> temp2(n2);
    n2 = std::min(n2, static_cast<int>(v2.size()));
    std::partial_sort_copy(v2.begin(), v2.end(), temp2.begin(), temp2.end(), std::less<myClass>());
    std::list<myClass> list2(temp2.begin(), temp2.end());

    //std::cout << "v2 contains prior:" << std::endl;
    //for (int i = 0; i < v2.size(); ++i) {
    //    std::cout << v2[i].getPriority() << " ";
    //}
    //std::cout << std::endl;

    //std::cout << "list2 contains prior:" << std::endl;
    //std::list<myClass>::iterator it_l2 = list2.begin();
    //for (; it_l2 != list2.end(); ++it_l2) {
    //    std::cout << it_l2->getPriority() << " ";
    //}
    //std::cout << std::endl;


    // Задание 5: Удаление перемещенных элементов из v1 и v2
    std::cout << "Fifth task:" << std::endl;

    std::set<myClass> temp1_set(temp.begin(), temp.end());
    v1.erase(std::remove_if(v1.begin(), v1.end(), [&](const myClass& x) {
        return temp1_set.find(x) != temp1_set.end();
        }), v1.end());

    std::set<myClass> temp2_set(temp2.begin(), temp2.end());
    v2.erase(std::remove_if(v2.begin(), v2.end(), [&](const myClass& x) {
        return temp2_set.find(x) != temp2_set.end();
        }), v2.end());

    //std::cout << "temp1 contains prior:" << std::endl;
    //for (int i = 0; i < temp.size(); ++i) {
    //    std::cout << temp[i].getPriority() << " ";
    //}
    //std::cout << std::endl;

    //std::cout << "v1 contains prior:" << std::endl;
    //for (int i = 0; i < v1.size(); ++i) {
    //    std::cout << v1[i].getPriority() << " ";
    //}
    //std::cout << std::endl;

    //std::cout << "temp2 contains prior:" << std::endl;
    //for (int i = 0; i < temp2.size(); ++i) {
    //    std::cout << temp2[i].getPriority() << " ";
    //}
    //std::cout << std::endl;

    //std::cout << "v2 contains prior:" << std::endl;
    //for (int i = 0; i < v2.size(); ++i) {
    //    std::cout << v2[i].getPriority() << " ";
    //}
    //std::cout << std::endl;

    // Задание 6: Перегруппировка элементов в list1
    std::cout << "Sixth task:" << std::endl;

    //std::cout << "list1 contains prior:" << std::endl;
    //std::list<myClass>::iterator it_l = list1.begin();
    //for (; it_l != list1.end(); ++it_l) {
    //    std::cout << it_l->getPriority() << " ";
    //}
    //std::cout << std::endl;

    double avg = std::accumulate(list1.begin(), list1.end(), 0.0, [](double sum, const myClass& x) {
        return sum + x.getPriority();
        }) / list1.size();

    std::cout << "Avg elem = " << avg << std::endl;

    list1.sort([](const myClass& a, const myClass& b) {
        return a.getPriority() > b.getPriority();
        });

    //std::cout << "list1 contains prior:" << std::endl;
    //for (it_l = list1.begin(); it_l != list1.end(); ++it_l) {
    //    std::cout << it_l->getPriority() << " ";
    //}
    //std::cout << std::endl;

    // Задание 7: Удаление нечётных элементов из list2
    std::cout << "Seventh task:" << std::endl;

    list2.remove_if([](const myClass& x) {
        return x.getPriority() % 2 != 0;
        });

    //std::cout << "list2 contains prior:" << std::endl;
    //std::list<myClass>::iterator it_l = list2.begin();
    //for (it_l = list2.begin(); it_l != list2.end(); ++it_l) {
    //    std::cout << it_l->getPriority() << " ";
    //}
    //std::cout << std::endl;

    // Задание 8: Создание вектора v3 из общих элементов
    std::cout << "Eigth task:" << std::endl;

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());

    std::vector<myClass> v3;
    std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v3));

   /* std::cout << "v3 contains prior:" << std::endl;
    for (int i = 0; i < v3.size(); ++i) {
        std::cout << v3[i].getPriority() << " ";
    }
    std::cout << std::endl;*/

    // Задание 9: Формирование списка list3 из пар элементов
    std::cout << "Ninth task:" << std::endl;
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

    std::cout << "Size list1 and list2 after delete: " << list1.size() << " "
        << list2.size() << std::endl;

    std::list<std::pair<myClass, myClass>> list3;
    std::transform(list1.begin(), list1.end(), list2.begin(), std::back_inserter(list3),
        [](const myClass& a, const myClass& b) {
            return std::make_pair(a, b);
        });

    //std::cout << "list3 contains prior:" << std::endl;
    //std::list<std::pair<myClass, myClass>>::iterator it_l3 = list3.begin();
    //for (; it_l3 != list3.end(); ++it_l3) {
    //    std::cout << it_l3->first.getPriority() << " " << it_l3->second.getPriority() 
    //        << std::endl;
    //}
    //std::cout << std::endl;

    // Задание 10: Создание вектора пар из v1 и v2 без приведения к одному размеру
    std::cout << "Tenth task:" << std::endl;

    int min_size = std::min(v1.size(), v2.size());

    std::vector<std::pair<myClass, myClass>> v_pairs;
    std::transform(v1.begin(), v1.begin() + min_size, v2.begin(), std::back_inserter(v_pairs),
        [](const myClass& a, const myClass& b) {
            return std::make_pair(a, b);
        });

    std::vector<std::pair<myClass, myClass>>::iterator it_v3 = v_pairs.begin();
    for (; it_v3 != v_pairs.end(); ++it_v3) {
        std::cout << it_v3->first.getPriority() << " " << it_v3->second.getPriority()
            << std::endl;
    }
    std::cout << std::endl;


    //std::cout << "v1 size: " << v1.size() << std::endl;
    //std::cout << "v2 size: " << v2.size() << std::endl;
    //std::cout << "list1 size: " << list1.size() << std::endl;
    //std::cout << "list2 size: " << list2.size() << std::endl;
    //std::cout << "list3 size: " << list3.size() << std::endl;
    //std::cout << "v_pairs size: " << v_pairs.size() << std::endl;

    return 0;
}

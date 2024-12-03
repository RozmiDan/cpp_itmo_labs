#include <iostream>
#include <typeinfo>
int f(void) {
int var1 = 1;
int var2 = 3;
if(typeid(var1).name() == typeid(var2).name()) {
std::cout << "Результат операции +: " << (var1 + var2) << std::endl;
std::cout << "Результат операции -: " << (var1 - var2) << std::endl;
std::cout << "Результат операции *: " << (var1 * var2) << std::endl;
std::cout << "Результат операции /: " << (var1 / var2) << std::endl;
} else {
std::cout << "Ошибка. Вы ввели разные типы"; }
return 0;
}

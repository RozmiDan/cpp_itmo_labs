#include <iostream>
#include <vector>
#include <list>
#include "myClass.hpp"

void byValue(myClass obj) {
    std::cout << "Function received object by value: " << obj.getValue() << ", " << obj.getPriority() << std::endl;
}

void byReference(const myClass& obj) {
    std::cout << "Function received object by reference: " << obj.getValue() << ", " << obj.getPriority() << std::endl;
}

myClass returnObject() {
    myClass temp(42, 100);
    std::cout << "Returning object from function" << std::endl;
    return temp;
}

void processVector(std::vector<myClass>& vec) {
    for (const auto& obj : vec) {
        std::cout << "Processing vector element: " << obj.getValue() << ", " << obj.getPriority() << std::endl;
    }
}

int main() {
    std::cout << "Creating static and dynamic instances of myClass:\n";

    myClass A(2, 3);
    myClass* B = new myClass(12, 4);

    std::cout << "\nCopying and moving objects:\n";

    myClass C = A; 
    myClass D = std::move(A); 

    std::cout << "\nUsing assignment operators:\n";
    myClass E;
    E = C;
    myClass F;
    F = std::move(C); 

    std::cout << "\nPassing objects to functions:\n";

    byValue(D);
    byReference(D);

    std::cout << "\nReturning objects from functions:\n";

    myClass G = returnObject();

    std::cout << "\nCreating vector and list of myClass instances:\n";

    std::vector<myClass> myVector;
    for (int i = 0; i < 5; ++i) {
        myVector.emplace_back(i, i + 1);
    }

    std::list<myClass> myList;
    for (int i = 5; i < 10; ++i) {
        myList.emplace_back(i, i + 1);
    }

    std::cout << "\nProcessing vector elements:\n";
    processVector(myVector);

    std::cout << "\nProcessing list elements:\n";
    for (const auto& obj : myList) {
        std::cout << "Processing list element: " << obj.getValue() << ", " << obj.getPriority() << std::endl;
    }

    delete B;

    std::cout << "\nEnd of main()\n";

    return 0;
}
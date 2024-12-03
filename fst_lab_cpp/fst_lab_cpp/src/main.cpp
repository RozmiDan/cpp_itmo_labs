#include <iostream>
#include <fstream>

int main(void) {
	using namespace std;
    string type1, type2, operation;
    ifstream fin("operations.txt"); 
    ofstream fout("../fst_lab_res/res_funk.cpp");

	if (!fout) {
 	   cerr << "Error opening file!" << endl;
    	return 1;
	}

    cout << "Введите первый тип данных: ";
    cin >> type1;
    cout << "Введите второй тип данных: ";
    cin >> type2;

    fout << "#include <iostream>" << endl;
    fout << "#include <typeinfo>" << endl;
    fout << "int f(void) {" << endl;

    fout << type1 << " var1 = 1;" << endl;
    fout << type2 << " var2 = 3;" << endl;

    fout << "if(typeid(var1).name() == typeid(var2).name()) {" << std::endl;

    while (getline(fin, operation)) {
        fout << "std::cout << \"Результат операции " << operation << ": \" << (var1 " 
             << operation << " var2) << std::endl;" << endl;
    }

    fout << "} else {" << std::endl;
    fout << "std::cout << \"Ошибка. Вы ввели разные типы\"; }" << std::endl;
    fout << "return 0;" << endl;
    fout << "}" << endl;

    fin.close();
    fout.close();
    return 0;
}

#include "myClass.hpp"

myClass::myClass(int data, int priority) :root(new Node(data, priority)) {
	std::cout << "Constructor was called with param: " << data << " " 
		<< priority << std::endl;
}

myClass::myClass(const myClass& other) {
	root = new Node(other.root->value, other.root->priority);
	std::cout << "Copy constructor was called with param: " << root->value << " "
		<< root->priority<< std::endl;
}

myClass::myClass(myClass&& other) noexcept {
	root = other.root;
	other.root = nullptr;
	std::cout << "Move constructor was called with param: " << root->value << " "
		<< root->priority << std::endl;
}

myClass& myClass::operator=(const myClass& other) {
	if (this != &other) {
		delete root;
		root = (other.root != nullptr) ? new Node(other.root->value, other.root->priority)
			: nullptr;
	}
	std::cout << "Assignment operator was called with param : " << root->value << " "
		<< root->priority << std::endl;
	return *this;
}

myClass& myClass::operator=(myClass&& other) noexcept {
	if (this != &other) {
		delete root;
		root = other.root;
		other.root = nullptr;
	}
	std::cout << "Copy assignment operator was called with param : " << root->value << " "
		<< root->priority << std::endl;
	return *this;
}

myClass::~myClass() { 
	std::cout << "Destructor was called" << std::endl;
	delete root;
}

int myClass::getValue() const {
	return root->value;
}

int myClass::getPriority() const {
	return root->priority;
}
int& myClass::setValue() {
	if (root == nullptr) {
		throw std::runtime_error("Root is nullptr");
	}
	return root->value;
}

int& myClass::setPriority() {
	if (root == nullptr) {
		throw std::runtime_error("Root is nullptr");
	}
	return root->priority;
}
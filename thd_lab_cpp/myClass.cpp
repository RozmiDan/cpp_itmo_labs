#include "myClass.hpp"

myClass::myClass(int data, int priority) :root(new Node(data, priority)) {

}

myClass::myClass(const myClass& other) {
	root = new Node(other.root->value, other.root->priority);
}

myClass::myClass(myClass&& other) noexcept {
	root = other.root;
	other.root = nullptr;
}

myClass& myClass::operator=(const myClass& other) {
	if (this != &other) {
		delete root;
		root = (other.root != nullptr) ? new Node(other.root->value, other.root->priority)
			: nullptr;
	}
	return *this;
}

myClass& myClass::operator=(myClass&& other) noexcept {
	if (this != &other) {
		delete root;
		root = other.root;
		other.root = nullptr;
	}
	return *this;
}

myClass::~myClass() {
	delete root;
}

bool myClass::operator<(const myClass& other) const {
	return this->getPriority() < other.getPriority();
}

bool myClass::operator>(const myClass& other) const {
	return other < *this;
}

bool myClass::operator==(const myClass& other) const {
	return this->getPriority() == other.getPriority();
}

int myClass::getValue() const {
	if (!root) {
		throw std::runtime_error("Root is nullptr");
	}
	return root->value;
}

int myClass::getPriority() const {
	if (!root) {
		throw std::runtime_error("Root is nullptr");
	}
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
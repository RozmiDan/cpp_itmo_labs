#pragma once
#include <iostream>

class myClass {
private:

	struct Node {
		int value;
		int priority;
		Node(int val, int prior) :value(val), priority(prior) {}
	};

	Node* root = nullptr;

public:
	myClass(int data = 0, int priority = 0);
	myClass(const myClass& other);
	myClass(myClass&& other) noexcept;
	myClass& operator=(const myClass& other);
	myClass& operator=(myClass&& other) noexcept;
	~myClass();

	bool operator<(const myClass& other) const;
	bool operator>(const myClass& other) const;
	bool operator==(const myClass& other) const;

	int getValue() const;
	int getPriority() const;
	int& setValue();
	int& setPriority();
};
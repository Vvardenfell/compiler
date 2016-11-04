#include "string.h"

String& String::operator=(value_type source) {
	string = Vector<value_type>(String::INITIAL_CAPACITY);
	string.push_back(source);
	string.push_back('\0');
	return *this;
}

String String::operator+(const String& source) const {
	String tmp(this->size() + source.size() + 1);
	tmp.string.insert(tmp.string.end(), this->string.begin(), this->string.end());
	tmp.string.insert(tmp.string.end(), source.string.begin(), source.string.end() + 1);
	return tmp;
}

String String::operator+(value_type source) const {
	String tmp(this->size() + 2);
	tmp.string.insert(tmp.string.end(), this->string.begin(), this->string.end());
	tmp.string.push_back(source);
	tmp.string.push_back('\0');
	return tmp;
}

String& String::operator+=(const String& source) {
	if (this->string.free_capacity() >= source.size()) {
		this->string.insert(this->string.end() - 1, source.string.begin(), source.string.end());
	}
	else {
		String tmp = *this + source;
		swap(*this, tmp);
	}
	return *this;
}

String& String::operator+=(value_type source) {
	this->string.pop_back();
	this->string.push_back(source);
	this->string.push_back('\0');
	return *this;
}

bool operator==(const String& left, const String& right) {
	if (left.size() != right.size()) return false; 

	String::const_iterator left_iterator = left.cbegin();
	String::const_iterator right_iterator = right.cbegin();
	String::const_iterator left_iterator_end = left.cend(); 

	for (; left_iterator != left_iterator_end; left_iterator++, right_iterator++) {
		if (*left_iterator != *right_iterator) return false;
	}

	return true;
}
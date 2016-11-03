#ifndef STRING_H
#define STRING_H

#include <algorithm>
#include <cstring>
#include "vector.h"

class String {
public:
        typedef char value_type;
        typedef char* iterator;
        typedef const char* const_iterator;

private:
	const static std::size_t INITIAL_CAPACITY = 16;

	Vector<value_type> string;

	String(std::size_t capacity) : string(capacity) {
		string.push_back('\0');
	}

public:
	friend void swap(String& left, String& right);

	String() : String(INITIAL_CAPACITY) {}

	String(const String& source) : string(source.string) {}

	String(const char* source, std::size_t bytes) : string(bytes + 1) {
	    this->string.insert(this->string.end(), source, source + bytes);
	    this->string.push_back('\0');
	}

	String(const char* source) : String(source, std::strlen(source)) {}

	String& operator=(String source) {
		swap(*this, source);
		return *this;
	}

	String& operator=(value_type source) {
		string = Vector<value_type>(String::INITIAL_CAPACITY);
		string.push_back(source);
		string.push_back('\0');
		return *this;
	}

	String operator+(const String& source) const {
		String tmp(this->size() + source.size() + 1);
		tmp.string.insert(tmp.string.end(), this->string.begin(), this->string.end());
		tmp.string.insert(tmp.string.end(), source.string.begin(), source.string.end() + 1);
		return tmp;
	}

	String operator+(value_type source) const {
		String tmp(this->size() + 2);
		tmp.string.insert(tmp.string.end(), this->string.begin(), this->string.end());
		tmp.string.push_back(source);
		tmp.string.push_back('\0');
		return tmp;
	}

	String& operator+=(const String& source) {
		if (this->string.free_capacity() >= source.size()) {
			this->string.insert(this->string.end() - 1, source.string.begin(), source.string.end());
		}
		else {
			String tmp = *this + source;
			swap(*this, tmp);
		}
		return *this;
	}

	String& operator+=(value_type source) {
		this->string.pop_back();
		this->string.push_back(source);
		this->string.push_back('\0');
		return *this;
	}

	const value_type& operator[](std::size_t index) const {
		return this->string[index];
	}

	value_type& operator[](std::size_t index) {
		return this->string[index];
	}

	const_iterator c_str() const {
		return &this->operator[](0);
	}

	iterator c_str() {
		return &this->operator[](0);
	}

	std::size_t size() const {
		return this->string.size() - 1;
	}

	iterator begin() {
		return this->string.begin();
	}

	iterator end() {
		return this->string.end() - 1;
	}

	const_iterator begin() const {
		return this->string.begin();
	}

	const_iterator end() const {
		return this->string.end() - 1;
	}
};


void swap(String& left, String& right) {
        using std::swap;
        swap(left.string, right.string);
}

#endif /* STRING_H */

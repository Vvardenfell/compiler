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

	String& operator=(value_type source);

	String operator+(const String& source) const;
	String operator+(value_type source) const;

	String& operator+=(const String& source);
	String& operator+=(value_type source);

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

	const_iterator cbegin() const {
		return this->string.cbegin();
	}

	const_iterator cend() const {
		return this->string.cend() - 1;
	}
};

bool operator==(const String& left, const String& right);

bool operator!=(const String& left, const String& right) {
	return !(left == right);
}


void swap(String& left, String& right) {
	using std::swap;
	swap(left.string, right.string);
}

#endif /* STRING_H */

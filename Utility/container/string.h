#ifndef STRING_H
#define STRING_H

#include <algorithm>
#include <cstring>
#include <iterator>
#include "vector.h"

class String {
public:
        typedef char value_type;
        typedef char* iterator;
        typedef const char* const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

private:
	const static std::size_t INITIAL_CAPACITY = 16;

	Vector<value_type> string;

	String(std::size_t capacity) : string(capacity) {
		this->string.push_back('\0');
	}

public:
	friend void swap(String& left, String& right);

	String() : String(INITIAL_CAPACITY) {}

	String(const String& source) : string(source.string) {}
	String(const String& source, std::size_t capacity) : string(capacity + 1) {
		std::size_t offset = (capacity + 1 < this->source.size() ? capacity : this->source.size());

		Vector<value_type>::const_iterator source_begin = source.cbegin();
		this->string.insert(this->string.begin(), source_begin, source_begin + offset);

		this->string.push_back('\0');
	}

	String(String&& source) : String(1) {
		using std::swap;
		swap(*this, source);
	}

	String(const char* source, std::size_t bytes) : string(bytes + 1) {
	    this->string.insert(this->string.end(), source, source + bytes);
	    this->string.push_back('\0');
	}

	String(const char* source) : String(source, std::strlen(source)) {}

	String(const_iterator begin, const_iterator end) : String(begin, std::distance(begin, end)) {}
	
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

	reverse_iterator rbegin() {
		return std::reverse_iterator<iterator>(this->end());
	}

	reverse_iterator rend() {
		return std::reverse_iterator<iterator>(this->begin());
	}

	const_reverse_iterator rbegin() const {
		return std::reverse_iterator<const_iterator>(this->cend());
	}

	const_reverse_iterator rend() const {
		return std::reverse_iterator<const_iterator>(this->cbegin());
	}

	const_iterator find_last_of(const char* sequence) const;

	String& clear() {
		this->string.clear();
		this->string.push_back('\0');
		return *this;
	}
};

bool operator==(const String& left, const String& right);
bool operator!=(const String& left, const String& right);

void swap(String& left, String& right);


namespace std {

	template<> class hash<String> {
		public:
		/*
		 * Calculates a hash with the SDBM hash function algorithm
		 */
		std::size_t operator()(const String& string) const {
			std::size_t hash = 0;

			for (String::const_iterator iterator = string.cbegin(), end = string.cend(); iterator != end; ++iterator) {
				hash = *iterator + (hash << 6) + (hash << 16) - hash;
			}

			return hash;
		}
	};

}

#endif /* STRING_H */

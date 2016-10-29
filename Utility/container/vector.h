/*
 * vector
 *
 *  Created on: 21.10.2016
 *      Author: Tim
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <cstdlib>
#include <utility>


template<typename T>
class Vector {
private:
	T* objects;
	T* next_free_space;
	T* end_free_space;
	const static size_t INITIAL_CAPACITY = 16;
	const static double RESIZE_FACTOR;

	void resize() {
		resize(static_cast<size_t>(this->end_free_space - this->objects * RESIZE_FACTOR));
	}

	void resize(size_t new_capacity) {
		Vector tmp(begin(), end(), new_capacity);
		swap(*this, tmp);
	}

public:

	friend void swap(Vector<T>& left, Vector<T>& right);

	typedef T* iterator;
	typedef const T* const_iterator;
	typedef ptrdiff_t difference_type;

	explicit Vector(size_t capacity = INITIAL_CAPACITY) {
		this->next_free_space = this->objects = new T[capacity];
		this->end_free_space = this->objects + capacity;
	}

	Vector(iterator begin, iterator end) : Vector(end - begin) {
		insert(begin(), begin, end);
	}

	Vector(iterator begin, iterator end, size_t capacity) : Vector(capacity) {
		insert(this->begin(), begin, end);
	}

	size_t size() const {
		return this->next_free_space - this->objects;
	}

	iterator begin() {
		return this->objects;
	}

	iterator end() {
		return this->next_free_space;
	}

	T& operator[](size_t index) {
		return this->objects[index];
	}

	const T& operator[](size_t index) const {
		return this->objects[index];
	}

	iterator insert(iterator target, const_iterator begin, const_iterator end) {
		size_t element_count = end - begin;
		difference_type offset = target - this->begin();

		if (free_capacity() < element_count) {
			resize(size() + element_count);
		}
		target = this->begin() + offset;

		for (iterator copy = this->end(); --copy >= target; ) {
			*(copy + element_count) = *copy;
		}
		this->next_free_space += element_count;

		for ( ; begin < end; begin++, target++) {
			*target = *begin;
		}

		return target;
	}

	size_t free_capacity() {
		return end_free_space - next_free_space;
	}
	
	void push_back(const T& object) {
		insert(end(), &object, (&object) + 1);
	}

	~Vector() {
		delete[] this->objects;
	}
};


template<typename T> void swap(Vector<T>& left, Vector<T>& right) {
	std::swap(left.objects, right.objects);
	std::swap(left.next_free_space, right.next_free_space);
	std::swap(left.end_free_space, right.end_free_space);
}


#endif /* VECTOR_H */

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

template <typename T>
class Vector {
private:
	T* objects;
	T* next_free_space;
	T* end_free_space;
	const static size_t INITIAL_CAPACITY = 16;

	void resize() {
		resize(static_cast<size_t>(this->end_free_space - this->objects * 1.4));
	}

	void resize(size_t new_capacity) {
		Vector& tmp(new_capacity);
		std::swap(tmp, *this);
	}

public:
	typedef T* iterator;
	typedef ptrdiff_t difference_type;

	Vector(size_t size = INITIAL_CAPACITY) {
		this->next_free_space = this->objects = new T[size];
		this->end_free_space = objects + size;
	}

	Vector(iterator begin, iterator end) : Vector(end - begin) {
		insert(begin(), begin, end);
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

	void insert(T object, size_t index) {
		if (index > 0 & index < this->size) {
			T* temp = new T[this->size - index];
			for (int i = index, k = 0; i < this->size; i++, k++) {
				temp[k] = this->objects[i];
			}
			this->objects[index] = object;
			for (int i = index + 1, k = 0; i < this->size; i++, k++) {			//index++ oder ++index?
				this->objects[i] = temp[k];
			}
			delete[] temp;
		} else if (index >= this->size) {
			T* temp = new T[index];
			temp[index] = object;
			for (int i = 0; i < this->size; i++) {
				temp[i] = this->objects[i];
			}
			delete[] this->objects;
			this->objects = temp;
			this->size = index;
		}
	}

	iterator insert(iterator target, iterator begin, iterator end) {
		if (free_capacity() < end - begin) resize(size() + (end - begin));
	}

	size_t free_capacity() {
		return end_free_space - next_free_space;
	}
	
	void push_back(T object) {

	}

	~Vector() {
		delete[] this->objects;
	}
};



#endif /* VECTOR_H */

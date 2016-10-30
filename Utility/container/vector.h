#ifndef VECTOR_H
#define VECTOR_H

#include <cstdlib>
#include <utility>
#include <iterator>
#include <algorithm>
#include <type_traits>

template<typename T> class Vector;

template<typename T> void swap(Vector<T>& left, Vector<T>& right) {
	std::swap(left.objects, right.objects);
	std::swap(left.next_free_space, right.next_free_space);
	std::swap(left.end_free_space, right.end_free_space);
}

template<typename T>
class Vector {
private:
	const static std::size_t INITIAL_CAPACITY = 16;
	const static double RESIZE_FACTOR;

	T* objects, next_free_space, end_free_space;

	void resize() {
		resize(static_cast<std::size_t>((this->end_free_space - this->objects) * RESIZE_FACTOR));
	}

	void resize(std::size_t new_capacity) {
		Vector tmp(this->begin(), this->end(), new_capacity);
		swap(*this, tmp);
	}

	void resize_on_demand(size_t required_space) {
		if (this->end_free_space - this->next_free_space < required_space) {
			this->resize((this->size() + required_space) * RESIZE_FACTOR);
		}
	}

	template<typename U = T> typename std::enable_if<std::is_pod<U>::value>::type destruct() {} // TODO: use std::is_trivially_copyable<T>::value in g++ 5 instead
	template<typename U = T> typename std::enable_if<!std::is_pod<U>::value>::type destruct() { // TODO: use std::is_trivially_copyable<T>::value in g++ 5 instead
		reverse_iterator end = this->rend();
		for (reverse_iterator iterator = this->rbegin(); iterator < end; ++iterator) {
			try {
				iterator->~U();
			}
			catch(...) {}
		}

		::operator delete(this->objects);
	}

public:

	friend void swap<>(Vector<T>& left, Vector<T>& right);

	typedef T value_type;
	typedef T* iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef const T* const_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef std::ptrdiff_t difference_type;

	explicit Vector(std::size_t capacity = INITIAL_CAPACITY) {
		this->next_free_space = this->objects = static_cast<T*>(::operator new(sizeof(T) * capacity));
		this->end_free_space = this->objects + capacity;
	}

	template<typename InputIterator> Vector(InputIterator begin, InputIterator end) : Vector(begin, end, std::distance(begin, end) * RESIZE_FACTOR) {}

	template<typename InputIterator> Vector(InputIterator begin, InputIterator end, std::size_t capacity) : Vector(capacity) {
		this->next_free_space = std::uninitialized_copy(begin, end, this->begin());
	}

	std::size_t size() const {
		return this->next_free_space - this->objects;
	}

	iterator begin() {
		return this->objects;
	}

	iterator end() {
		return this->next_free_space;
	}

	const_iterator begin() const {
		return this->objects;
	}

	const_iterator end() const {
		return this->next_free_space;
	}

	reverse_iterator rbegin() {
		return std::reverse_iterator<iterator>(this->end());
	}

	reverse_iterator rend() {
		return std::reverse_iterator<iterator>(this->begin());
	}

	const_reverse_iterator rbegin() const {
		return std::reverse_iterator<const_iterator>(this->end());
	}

	const_reverse_iterator rend() const {
		return std::reverse_iterator<const_iterator>(this->begin());
	}

	T& operator[](std::size_t index) {
		return this->objects[index];
	}

	const T& operator[](std::size_t index) const {
		return this->objects[index];
	}

	template<typename InputIterator> iterator insert(iterator target, InputIterator begin, InputIterator end) {
		std::size_t element_count = std::distance(begin, end);
		difference_type offset = std::distance(this->begin(), target);

		this->resize_on_demand(element_count);
		target = this->begin() + offset;

		std::copy_backward(target, this->end(), this->end() + element_count);
		this->next_free_space += element_count;
		std::copy(begin, end, target);

		return target;
	}

	std::size_t capacity() const {
		return this->end_free_space - this->objects;
	}
	
	void push_back(const T& object) {
		resize_on_demand(1);
		new (static_cast<void*>(&*this->end())) T(object);
		++this->next_free_space;
	}

	~Vector() {
		this->destruct();
	}

};


#endif /* VECTOR_H */

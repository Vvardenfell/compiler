#ifndef VECTOR_H
#define VECTOR_H

#include <cstdlib>
#include <utility>
#include <iterator>
#include <algorithm>
#include <type_traits>

template<typename T> class Vector;

template<typename T> void swap(Vector<T>& left, Vector<T>& right) {
	using std::swap;
	swap(left.objects, right.objects);
	swap(left.next_free_space, right.next_free_space);
	swap(left.end_free_space, right.end_free_space);
}

template<typename T>
class Vector {
public:
        typedef T value_type;
        typedef T* iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef const T* const_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef std::ptrdiff_t difference_type;

private:
	const static std::size_t INITIAL_CAPACITY = 16;
	const static double RESIZE_FACTOR;

	value_type *objects, *next_free_space, *end_free_space;

	void resize() {
		resize(static_cast<std::size_t>((this->end_free_space - this->objects) * RESIZE_FACTOR));
	}

	void resize(std::size_t new_capacity) {
		Vector<T> tmp(std::make_move_iterator(this->begin()), std::make_move_iterator(this->end()), new_capacity);
		swap(*this, tmp);
	}

	void resize_on_demand(std::size_t required_space) {
		if (this->free_capacity() < required_space) {
			this->resize((this->size() + required_space) * RESIZE_FACTOR);
		}
	}

	template<typename U = value_type> typename std::enable_if<std::is_trivially_destructible<U>::value>::type destruct() {}
	template<typename U = value_type> typename std::enable_if<!std::is_trivially_destructible<U>::value>::type destruct() {
		reverse_iterator end = this->rend();
		for (reverse_iterator iterator = this->rbegin(); iterator != end; ++iterator) {
			try {
				iterator->~U();
			}
			catch(...) {}
		}
	}

public:

	friend void swap<>(Vector<value_type>& left, Vector<value_type>& right);

	const static Vector<value_type> EMPTY;

	explicit Vector(std::size_t capacity = INITIAL_CAPACITY) {
		this->next_free_space = this->objects = static_cast<value_type*>(::operator new[](sizeof(value_type) * capacity));
		this->end_free_space = this->objects + capacity;
	}

	Vector(std::size_t size, const value_type& default_value) : Vector(size) {
		for (size_t index = 0; index < size; index++) this->push_back(default_value);
	}

	Vector(std::initializer_list<value_type> initializer) : Vector(initializer.begin(), initializer.end()) {}

	template<typename InputIterator> Vector(InputIterator begin, InputIterator end) : Vector(begin, end, std::abs(std::distance(begin, end)) * RESIZE_FACTOR) {}

	template<typename InputIterator> Vector(InputIterator begin, InputIterator end, std::size_t capacity) : Vector(capacity) {
		this->next_free_space = std::uninitialized_copy(begin, end, this->begin());
	}

	Vector(const Vector<value_type>& source) : Vector(source.cbegin(), source.cend(), source.capacity()) {}
	Vector(Vector<value_type>&& source) : objects(nullptr), next_free_space(nullptr), end_free_space(nullptr) {
		swap(*this, source);
	}

	Vector<value_type>& operator=(Vector<value_type> source) {
		swap(*this, source);
		return *this;
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

	const_iterator cbegin() const {
		return this->objects;
	}

	const_iterator cend() const {
		return this->next_free_space;
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

	value_type& operator[](std::size_t index) {
		return this->objects[index];
	}

	const value_type& operator[](std::size_t index) const {
		return this->objects[index];
	}

	template<typename InputIterator> iterator insert(iterator target, InputIterator begin, InputIterator end) {
		std::size_t element_count = std::abs(std::distance(begin, end));
		difference_type offset = std::distance(this->begin(), target);

		this->resize_on_demand(element_count);
		target = this->begin() + offset;

		iterator copy_begin = target;
		iterator copy_end = copy_begin + (this->end() - target - element_count);
		iterator uninitialized_copy_begin = (this->end() - element_count < target) ? target : this->end() - element_count;
		iterator uninitialized_copy_end = this->end();

		std::uninitialized_copy(uninitialized_copy_begin, uninitialized_copy_end, this->end());
		if (copy_begin <= copy_end) {
			std::copy_backward(copy_begin, copy_end, this->end() - (copy_end - copy_begin));
			std::copy(begin, end, target);
		}
		else {
			std::size_t elements_to_copy_count = uninitialized_copy_end - uninitialized_copy_begin;
			target = std::copy(begin, begin + elements_to_copy_count, target);
			target = std::uninitialized_copy(begin + elements_to_copy_count, end, target);
		}

		this->next_free_space += element_count;

		return target;
	}

	std::size_t capacity() const {
		return this->end_free_space - this->objects;
	}

	std::size_t free_capacity() const {
		return this->end_free_space - this->next_free_space;
	}

	void push_back(const value_type& object) {
		this->resize_on_demand(1);
		new (static_cast<void*>(&*this->end())) value_type(object);
		++this->next_free_space;
	}

	void push_back(value_type&& object) {
		this->resize_on_demand(1);
		new (static_cast<void*>(&*this->end())) value_type(object);
		++this->next_free_space;
	}

	template<typename... Args> void emplace_back(Args&&... args) {
		this->resize_on_demand(1);
		new (static_cast<void*>(&*this->end())) value_type(std::forward<Args>(args)...);
		++this->next_free_space;
	}

	template<typename U = value_type> typename std::enable_if<std::is_trivially_destructible<U>::value, value_type>::type pop_back() {
		return *(--this->next_free_space);
	}

	template<typename U = value_type> typename std::enable_if<!std::is_trivially_destructible<U>::value, value_type>::type pop_back() {
		value_type tmp(*(--this->next_free_space));

		this->next_free_space->~value_type();

		return tmp;
	}

	iterator find(const value_type& value) {
		for (iterator iterator = this->begin(); iterator != this->end(); iterator++) {
			if (*iterator == value) return iterator;
		}

		return this->end();
	}

	const_iterator find(const value_type& value) const {
		for (const_iterator iterator = this->cbegin(); iterator != this->cend(); iterator++) {
			if (*iterator == value) return iterator;
		}

		return this->cend();
	}

	bool contains(const value_type& value) const {
		return this->find(value) != this->cend();
	}

	template<typename U = value_type> typename std::enable_if<std::is_trivially_destructible<U>::value, Vector<value_type>&>::type clear() {
		this->next_free_space = this->objects;
	}

	template<typename U = value_type> typename std::enable_if<!std::is_trivially_destructible<U>::value, Vector<value_type>&>::type clear() {
		this->destruct();
		this->next_free_space = this->objects;
	}

	~Vector() {
		this->destruct();
		::operator delete[](this->objects);
	}

};

template<typename T> const double Vector<T>::RESIZE_FACTOR = 1.4;
template<typename T> const Vector<T> Vector<T>::EMPTY(0);

#endif /* VECTOR_H */

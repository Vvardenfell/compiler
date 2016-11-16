#ifndef UNORDERED_MAP_H
#define UNORDERED_MAP_H

#include "../../Utility/container/string.h"
#include "../../Utility/container/vector.h"
#include <functional>
#include <cstring>
#include <iterator>

template<typename T> class UnorderedMapIterator : public std::iterator<std::bidirectional_iterator_tag, T, std::size_t> {
private:
	typedef typename Vector<Vector<T>>::iterator bucket_iterator_type;
	typedef typename Vector<T>::iterator iterator_type;

	bucket_iterator_type bucket_iterator;
	iterator_type iterator;

public:
	UnorderedMapIterator(bucket_iterator_type bucket_iterator, iterator_type iterator) : bucket_iterator(bucket_iterator), iterator(iterator) {}
	UnorderedMapIterator(const UnorderedMapIterator<T>& source) : bucket_iterator(source.bucket_iterator), iterator(source.iterator) {}

	UnorderedMapIterator& operator++() {
		if (this->iterator == (*this->bucket_iterator).end()) {
			++this->bucket_iterator;
			this->iterator = (*this->bucket_iterator).begin();
		}
		else ++this->iterator;

		return *this;
	}

	UnorderedMapIterator operator++(int) {
		UnorderedMapIterator tmp(*this);
		++(*this);
		return tmp;
	}

	bool operator==(const UnorderedMapIterator& other) const {
		return this->iterator == other.iterator && this->bucket_iterator == other.bucket_iterator;
	}

	bool operator!=(const UnorderedMapIterator& other) const {
		return !(*this == other);
	}

	decltype(Vector<T>::iterator::operator*()) operator*() {
		return *this->iterator;
	}

	decltype(Vector<T>::const_iterator::operator*()) operator*() const {
		return *this->iterator;
	}

};

template<typename F, typename S> struct Pair;

template<typename F, typename S> void swap(Pair<F, S>& left, Pair<F, S>& right) {
	using std::swap;
	swap(left.first, right.first);
	swap(left.second, right.second);
}

template<typename F, typename S> struct Pair {
	F first;
	S second;

	Pair(F&& first, S&& second) : first(std::forward(first)), second(std::forward(second)) {}

	// TODO: implement special copy and move constructor, depending on if L or R are trivially copyable and they must not be references
};

template<typename K, typename V, typename Hash, typename Compare> class UnorderedMap;

template<typename K, typename V, typename Hash, typename Compare> void swap(UnorderedMap<K, V, Hash, Compare>& left, UnorderedMap<K, V, Hash, Compare>& right) {
	using std::swap;
	swap(left.buckets, right.buckets);
	swap(left.hash, right.hash);
	swap(left.comparator, right.comparator);
	swap(left.size, right.size);
	swap(left.maximum_load, right.maximum_load);
}

template<typename K, typename V, typename Hash = std::hash<K>, typename Compare = std::equal_to<K>> class UnorderedMap {
public:
	typedef K key_type;
	typedef V value_type;
	typedef Hash hash_type;
	typedef Compare comparator_type;
	typedef Pair<key_type, value_type> entry_type;
	typedef UnorderedMapIterator<entry_type> iterator;
	typedef UnorderedMapIterator<const entry_type> const_iterator;

private:
	const static std::size_t INITIAL_BUCKET_CAPACITY = 256;
	const static double RESIZE_FACTOR, LOAD_FACTOR;

	Vector<Vector<entry_type>> buckets;
	hash_type hash;
	comparator_type comparator;
	std::size_t entry_count, maximum_load;

	std::size_t bucket_index(const key_type& key) const {
		return this->hash(key) % this->capacity();
	}

	typename Vector<entry_type>::const_iterator find_entry(const Vector<entry_type>& values, const key_type& key) const {
		for (typename Vector<entry_type>::const_iterator iterator = values.cbegin(), end = values.cend(); iterator != end; ++iterator) {
			if (this->comparator(iterator->first, key)) return iterator;
		}

		return values.cend();
	}

	typename Vector<entry_type>::iterator find_entry(const Vector<entry_type>& values, const key_type& key) {
		for (typename Vector<entry_type>::iterator iterator = values.begin(), end = values.end(); iterator != end; ++iterator) {
			if (this->comparator(iterator->first, key)) return iterator;
		}

		return values.end();
	}

	void resize(std::size_t new_capacity) {
		UnorderedMap<key_type, value_type, hash_type, comparator_type> tmp(std::move(*this), new_capacity);
		swap(*this, tmp);
	}

	void resize_on_demand(std::size_t required_space) {
		if (this.size() + required_space > this->maximum_load()) {
			this->resize((this->size() + required_space) * RESIZE_FACTOR);
		}
	}

	UnorderedMap(UnorderedMap<key_type, value_type, hash_type, comparator_type>&& source, std::size_t bucket_capacity)
		: UnorderedMap(bucket_capacity, source.hash, source.comparator) {

		auto end = source.end();
		for (auto iterator = source.begin(); iterator != end; ++iterator) {
			this->buckets[this->bucket_index(iterator->first)].push_back(std::move(*iterator));
		}
	}

public:
	explicit UnorderedMap(std::size_t bucket_capacity = INITIAL_BUCKET_CAPACITY, hash_type hash = hash_type(), comparator_type comparator = comparator_type())
		: buckets(bucket_capacity, Vector<entry_type>()), hash(hash), entry_count(0), comparator(comparator), maximum_load(bucket_capacity * LOAD_FACTOR) {}

	UnorderedMap(hash_type hash, comparator_type comparator = comparator_type()) : UnorderedMap(INITIAL_BUCKET_CAPACITY, hash, comparator) {}
	UnorderedMap(comparator_type comparator) : UnorderedMap(hash_type(), comparator) {}
	UnorderedMap(std::size_t bucket_capacity, comparator_type comparator) : UnorderedMap(bucket_capacity, hash_type(), comparator) {}

	template<typename InputIterator> UnorderedMap(InputIterator begin, InputIterator end, std::size_t bucket_capacity = INITIAL_BUCKET_CAPACITY)
		: UnorderedMap(bucket_capacity) {
	}

	UnorderedMap(const UnorderedMap<key_type, value_type, hash_type, comparator_type>& source)
		: buckets(source.capacity()), hash(source.hash), entry_count(source.size()), comparator(source.comparator), maximum_load(source.capacity() * LOAD_FACTOR) {

		this->buckets.insert(this->buckets.begin(), source.buckets.begin(), source.buckets.end());
	}

	UnorderedMap(UnorderedMap<key_type, value_type, hash_type, comparator_type>&& source)
		: buckets(0), hash(), entry_count(0), comparator(), maximum_load(0) {

		swap(*this, source);
	}

	UnorderedMap& operator=(UnorderedMap<key_type, value_type, hash_type, comparator_type> source) {
		swap(*this, source);
		return *this;
	}

	std::size_t capacity() const {
		return this->buckets.size();
	}

	iterator insert(const key_type& key, const value_type& value) {
		Vector<entry_type>& values = this->buckets[this->bucket_index(key)];

		typename Vector<entry_type>::iterator iterator = this->find_entry(values, key);

		if (iterator == values.end()) return this->force_insert(key, value, iterator);
		return UnorderedMapIterator<entry_type>(&values, iterator);
	}

	iterator force_insert(const key_type& key, const value_type& value, iterator hint) {
		this->resize_on_demand(1);

		auto& bucket = *hint.bucket_iterator;
		bucket.emplace_back(key, value);
		++this->size;

		return UnorderedMapIterator<entry_type>(&bucket, bucket.end() - 1);
	}

	iterator find(const key_type& key) {
		Vector<entry_type>& values = this->buckets[this->bucket_index(key)];
		typename Vector<entry_type>::iterator iterator = this->find_entry(values, key);

		return UnorderedMapIterator<entry_type>(&values, iterator);
	}

	const_iterator find(const key_type& key) const {
		Vector<entry_type>& values = this->buckets[this->bucket_index(key)];
		typename Vector<entry_type>::const_iterator iterator = this->find_entry(values, key);

		return UnorderedMapIterator<const entry_type>(&values, iterator);
	}

	std::size_t size() const {
		return this->entry_count;
	}

	iterator begin() {
		typename Vector<Vector<entry_type>>::iterator bucket_iterator = this->buckets.begin();
		return UnorderedMap::iterator(bucket_iterator, bucket_iterator.begin());
	}

	const_iterator cbegin() const {
		typename Vector<Vector<entry_type>>::const_iterator bucket_iterator = this->buckets.cbegin();
		return UnorderedMap::const_iterator(bucket_iterator, bucket_iterator.cbegin());
	}

	iterator end() {
		typename Vector<Vector<entry_type>>::iterator bucket_iterator = this->buckets.end() - 1;
		return UnorderedMap::iterator(bucket_iterator, bucket_iterator.end());
	}

	const_iterator cend() const {
		typename Vector<Vector<entry_type>>::const_iterator bucket_iterator = this->buckets.cend() - 1;
		return UnorderedMap::const_iterator(bucket_iterator, bucket_iterator.cbegin());
	}
};

template<typename K, typename V, typename Hash, typename Compare> const double UnorderedMap<K, V, Hash, Compare>::RESIZE_FACTOR = 1.4;
template<typename K, typename V, typename Hash, typename Compare> const double UnorderedMap<K, V, Hash, Compare>::LOAD_FACTOR = 0.7;

#endif /* UNORDERED_MAP_H */

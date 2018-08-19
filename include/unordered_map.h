#ifndef UNORDERED_MAP_H
#define UNORDERED_MAP_H

#include "string.h"
#include "vector.h"
#include <functional>
#include <iterator>
#include <type_traits>

template<typename T> class UnorderedMapIterator : public std::iterator<std::forward_iterator_tag, T, std::size_t> {
private:
	template<typename K, typename V, typename Hash, typename Compare> friend class UnorderedMap;

	typedef typename std::remove_const<T>::type NonConstT;
	typedef typename Vector<Vector<NonConstT>>::iterator bucket_iterator_type;
	typedef typename Vector<NonConstT>::iterator iterator_type;
	typedef typename Vector<Vector<NonConstT>>::const_iterator const_bucket_iterator_type;
	typedef typename Vector<NonConstT>::const_iterator const_iterator_type;

	bucket_iterator_type bucket_iterator;
	iterator_type iterator;
	iterator_type end;

	void next() {
		iterator_type bucket_iterator_end = (*this->bucket_iterator).end();
		if (this->iterator == bucket_iterator_end || ++this->iterator == bucket_iterator_end) {
			while (this->iterator == bucket_iterator_end && bucket_iterator_end != this->end) {
				++this->bucket_iterator;
				this->iterator = (*this->bucket_iterator).begin();
				bucket_iterator_end = (*this->bucket_iterator).end();
			}
		}
	}

public:
	UnorderedMapIterator(bucket_iterator_type bucket_iterator, iterator_type iterator, iterator_type end, bool end_iterator = false)
		: bucket_iterator(bucket_iterator), iterator(iterator), end(end) {
		if (!end_iterator && iterator == (*bucket_iterator).end()) this->next();
	}

    UnorderedMapIterator(const_bucket_iterator_type bucket_iterator, const_iterator_type iterator, const_iterator_type end, bool end_iterator = false)
		: bucket_iterator(const_cast<bucket_iterator_type>(bucket_iterator)), iterator(const_cast<iterator_type>(iterator)), end(const_cast<iterator_type>(end)) {
		if (!end_iterator && iterator == (*bucket_iterator).cend()) this->next();
	}

	UnorderedMapIterator& operator++() {
		this->next();
		return *this;
	}

	UnorderedMapIterator operator++(int) {
		UnorderedMapIterator tmp(*this);
		++(*this);
		return tmp;
	}

	bool operator==(const UnorderedMapIterator& other) const {
		return (this->iterator == other.iterator && this->bucket_iterator == other.bucket_iterator)
			|| (this->iterator == (*this->bucket_iterator).end() && other.iterator == (*other.bucket_iterator).end());
	}

	bool operator!=(const UnorderedMapIterator& other) const {
		return !(*this == other);
	}

    typename std::iterator_traits<typename std::conditional<std::is_const<T>::value, typename Vector<NonConstT>::const_iterator, typename Vector<NonConstT>::iterator>::type>::value_type& operator*() {
		return *this->iterator;
	}

	const typename std::iterator_traits<typename Vector<T>::const_iterator>::value_type& operator*() const {
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
	Pair(const F& first, const S& second) : first(first), second(second) {}

	// TODO: implement special copy and move constructor, depending on if L or R are trivially copyable and they must not be references
};

template<typename K, typename V, typename Hash, typename Compare> class UnorderedMap;

template<typename K, typename V, typename Hash, typename Compare> void swap(UnorderedMap<K, V, Hash, Compare>& left, UnorderedMap<K, V, Hash, Compare>& right) {
	using std::swap;
	swap(left.buckets, right.buckets);
	swap(left.hash, right.hash);
	swap(left.comparator, right.comparator);
	swap(left.entry_count, right.entry_count);
	swap(left.maximum_load, right.maximum_load);
}

template<typename K, typename V, typename Hash = std::hash<K>, typename Compare = std::equal_to<K>> class UnorderedMap {
public:
	typedef K key_type;
	typedef typename std::conditional<std::is_pointer<key_type>::value, typename std::add_pointer<typename std::add_const<typename std::remove_pointer<key_type>::type>::type>::type, const key_type>::type const_key_type;
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

	std::size_t bucket_index(const const_key_type& key) const {
		return this->hash(key) % this->capacity();
	}

	typename Vector<entry_type>::const_iterator find_entry(const Vector<entry_type>& values, const const_key_type& key) const {
		for (typename Vector<entry_type>::const_iterator iterator = values.cbegin(), end = values.cend(); iterator != end; ++iterator) {
			if (this->comparator(iterator->first, key)) return iterator;
		}

		return values.cend();
	}

	typename Vector<entry_type>::iterator find_entry(Vector<entry_type>& values, const const_key_type& key) {
		for (typename Vector<entry_type>::iterator iterator = values.begin(), end = values.end(); iterator != end; ++iterator) {
			if (this->comparator((*iterator).first, key)) return iterator;
		}

		return values.end();
	}

	void resize(std::size_t new_capacity) {
		UnorderedMap<key_type, value_type, hash_type, comparator_type> tmp(std::move(*this), new_capacity);
		swap(*this, tmp);
	}

	bool resize_on_demand(std::size_t required_space) {
		if (this->size() + required_space > this->maximum_load) {
			this->resize((this->buckets.size() + required_space) * RESIZE_FACTOR);
			return true;
		}

		return false;
	}

	typename Vector<entry_type>::iterator force_insert(const key_type& key, const value_type& value, typename Vector<Vector<entry_type>>::iterator hint) {
		Vector<entry_type>* bucket = &*hint;

		if (this->resize_on_demand(1)) bucket = &this->buckets[this->bucket_index(key)];

		bucket->emplace_back(key, value);
		++this->entry_count;

		return bucket->end() - 1;
	}

	UnorderedMap(UnorderedMap<key_type, value_type, hash_type, comparator_type>&& source, std::size_t bucket_capacity)
		: UnorderedMap(bucket_capacity, source.hash, source.comparator) {
		this->entry_count = source.entry_count;

		auto end = source.end();
		for (auto iterator = source.begin(); iterator != end; ++iterator) {
			const K& key = (*iterator).first;
			this->buckets[this->bucket_index(key)].push_back(*iterator);
		}
	}

	typename Vector<entry_type>::const_iterator end_iterator() const {
		return this->buckets[this->buckets.size() - 1].cend();
	}

	typename Vector<entry_type>::iterator end_iterator() {
		return this->buckets[this->buckets.size() - 1].end();
	}

public:
	friend void swap<>(UnorderedMap<key_type, value_type, hash_type, comparator_type>& left, UnorderedMap<key_type, value_type, hash_type, comparator_type>& right);

	explicit UnorderedMap(std::size_t bucket_capacity = INITIAL_BUCKET_CAPACITY, hash_type hash = hash_type(), comparator_type comparator = comparator_type())
		: buckets(bucket_capacity, Vector<entry_type>(2)), hash(hash), comparator(comparator), entry_count(0), maximum_load(bucket_capacity * LOAD_FACTOR) {}

	UnorderedMap(hash_type hash, comparator_type comparator = comparator_type()) : UnorderedMap(INITIAL_BUCKET_CAPACITY, hash, comparator) {}
	UnorderedMap(comparator_type comparator) : UnorderedMap(hash_type(), comparator) {}
	UnorderedMap(std::size_t bucket_capacity, comparator_type comparator) : UnorderedMap(bucket_capacity, hash_type(), comparator) {}

	template<typename InputIterator> UnorderedMap(InputIterator begin, InputIterator end, std::size_t bucket_capacity = INITIAL_BUCKET_CAPACITY)
		: UnorderedMap(bucket_capacity) {
	}

	UnorderedMap(const UnorderedMap<key_type, value_type, hash_type, comparator_type>& source)
		: buckets(source.buckets), hash(source.hash), comparator(source.comparator), entry_count(source.size()), maximum_load(source.capacity() * LOAD_FACTOR) {}

	UnorderedMap(UnorderedMap<key_type, value_type, hash_type, comparator_type>&& source)
		: buckets(0), hash(), comparator(), entry_count(0), maximum_load(0) {

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
		this->resize_on_demand(1);

		Vector<entry_type>& values = this->buckets[this->bucket_index(key)];

		typename Vector<entry_type>::iterator iterator = this->find_entry(values, key);

		if (iterator == values.end()) return UnorderedMapIterator<entry_type>(&values, this->force_insert(key, value, &values), this->end_iterator());
		return UnorderedMapIterator<entry_type>(&values, iterator, this->end_iterator());
	}

	iterator force_insert(const key_type& key, const value_type& value, iterator hint) {
		typename Vector<entry_type>::iterator iterator = this->force_insert(key, value, hint.bucket_iterator);

		return UnorderedMapIterator<entry_type>(hint.bucket_iterator, iterator, this->end_iterator());
	}

	iterator find(const const_key_type& key) {
		Vector<entry_type>& values = this->buckets[this->bucket_index(key)];
		typename Vector<entry_type>::iterator iterator = this->find_entry(values, key);

		return UnorderedMapIterator<entry_type>(&values, iterator, this->end_iterator(), true);
	}

	const_iterator find(const const_key_type& key) const {
		const Vector<entry_type>& values = this->buckets[this->bucket_index(key)];
		typename Vector<entry_type>::const_iterator iterator = this->find_entry(values, key);

		return UnorderedMapIterator<const entry_type>(&values, iterator, this->end_iterator(), true);
	}

	std::size_t size() const {
		return this->entry_count;
	}

	iterator begin() {
		typename Vector<Vector<entry_type>>::iterator bucket_iterator = this->buckets.begin();
		return UnorderedMap::iterator(bucket_iterator, (*bucket_iterator).begin(), this->end_iterator());
	}

	iterator end() {
		typename Vector<Vector<entry_type>>::iterator bucket_iterator = this->buckets.end() - 1;
		return UnorderedMap::iterator(bucket_iterator, (*bucket_iterator).end(), this->end_iterator());
	}

	const_iterator cbegin() const {
		typename Vector<Vector<entry_type>>::const_iterator bucket_iterator = this->buckets.cbegin();
		return UnorderedMap::const_iterator(bucket_iterator, (*bucket_iterator).cbegin(), this->end_iterator());
	}

	const_iterator cend() const {
		typename Vector<Vector<entry_type>>::const_iterator bucket_iterator = this->buckets.cend() - 1;
		return UnorderedMap::const_iterator(bucket_iterator, (*bucket_iterator).cend(), this->end_iterator());
	}

};

template<typename K, typename V, typename Hash, typename Compare> const double UnorderedMap<K, V, Hash, Compare>::RESIZE_FACTOR = 1.4;
template<typename K, typename V, typename Hash, typename Compare> const double UnorderedMap<K, V, Hash, Compare>::LOAD_FACTOR = 0.7;

#endif /* UNORDERED_MAP_H */

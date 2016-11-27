#include "vector.h"

template<typename T> class BinaryGraveyard;

template<typename T> class BinaryGraveyardIterator : public std::iterator<std::forward_iterator_tag, T, std::size_t> {
private:
	typedef T value_type;
	typedef T* pointer_type;

	Vector<pointer_type>* graves;
	typename Vector<pointer_type>::iterator active_grave;
	pointer_type iterator;
	std::size_t corpses_before_grave_end;

	void next() {
		if (--this->corpses_before_grave_end == 0) {
			if (++this->active_grave == this->graves->end()) this->iterator = nullptr;
			else {
				this->iterator = *this->active_grave;
				this->corpses_before_grave_end = BinaryGraveyard<value_type>::grave_size(this->iterator);
			}
		}
		else ++this->iterator;
	}

public:
	BinaryGraveyardIterator(Vector<pointer_type>* graves, typename Vector<pointer_type>::iterator active_grave)
		: graves(graves), active_grave(active_grave - 1), iterator(nullptr), corpses_before_grave_end(1) {
		this->next();
	}

	BinaryGraveyardIterator& operator++() {
		this->next();
		return *this;
	}

	BinaryGraveyardIterator operator++(int) {
		BinaryGraveyardIterator<value_type> tmp(*this);
		++(*this);
		return tmp;
	}

	bool operator==(const BinaryGraveyardIterator<value_type>& other) const {
		return this->iterator == other.iterator;
	}

	bool operator!=(const BinaryGraveyardIterator<value_type>& other) const {
		return !(*this == other);
	}

	value_type& operator*() {
		return *this->iterator;
	}

	const value_type& operator*() const {
		return *this->iterator;
	}
};

template<typename T> void swap(BinaryGraveyard<T>& left, BinaryGraveyard<T>& right) {
	using std::swap;
	swap(left.graveyard, right.graveyard);
	swap(left.next_free_space, right.next_free_space);
	swap(left.end_free_space, right.end_free_space);
}

template<typename T> class BinaryGraveyard {
public:

	typedef T value_type;
	typedef T* pointer_type;
	typedef const T* const_pointer_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef BinaryGraveyardIterator<value_type> iterator;

private:

	const static std::size_t GRAVE_CAPACITY = 4096;

	Vector<void*> graveyard;
	Vector<pointer_type> graves;
	char *next_free_space, *end_free_space;

	std::size_t required_space(std::size_t*& size_address, pointer_type& value_address, std::size_t count) {
		std::size_t* tmp_size_address = BinaryGraveyard::alignment<std::size_t>(this->next_free_space);
		value_address = BinaryGraveyard::alignment<value_type>(tmp_size_address + 1);
		size_address = BinaryGraveyard::locate_grave_size(value_address);

		return reinterpret_cast<char*>(value_address + count) - this->next_free_space;
	}

	template<typename D, typename S> static D* alignment(const S* address) {
		return reinterpret_cast<D*>(((reinterpret_cast<const std::size_t>(address) - 1) | (alignof(D) - 1)) + 1);
	}

	template<typename S> static std::size_t* locate_grave_size(const S* grave) {
		return reinterpret_cast<std::size_t*>((reinterpret_cast<std::size_t>(grave) - 1) & (~(alignof(std::size_t) - 1)));
	}

	void create_grave(std::size_t required_space) {
		this->next_free_space = static_cast<char*>(::operator new(((required_space - 1) | (GRAVE_CAPACITY - 1)) + 1));
		this->end_free_space = this->next_free_space + required_space;

		this->graveyard.push_back(this->next_free_space);
	}

	bool create_grave_on_demand(std::size_t required_space) {
		if (required_space > std::distance(this->next_free_space, this->end_free_space)) {
			this->create_grave(required_space + alignof(value_type));
			return true;
		}
		return false;
	}

	template<typename U = value_type> typename std::enable_if<std::is_trivially_destructible<U>::value>::type destruct() {}
	template<typename U = value_type> typename std::enable_if<!std::is_trivially_destructible<U>::value>::type destruct() {
		for (iterator iterator = this->begin(), end = this->end(); iterator != end; ++iterator) {
			try {
				(*iterator).~U();
			}
			catch(...) {}
		}
	}
public:

	friend void swap<>(BinaryGraveyard<value_type>& left, BinaryGraveyard<value_type>& right);

	BinaryGraveyard() : graveyard(), next_free_space(nullptr), end_free_space(nullptr) {}

	BinaryGraveyard(const BinaryGraveyard<value_type>& source) : graveyard(source.graveyard), next_free_space(nullptr), end_free_space(nullptr) {
		if (source.graveyard.size() > 0) {
			void* source_grave_begin = &*(source.graveyard.end() - 1);
			void* this_grave_begin = &*(this->graveyard.end() - 1);

			this->next_free_space = this_grave_begin + std::distance(source_grave_begin, source.next_free_space);
			this->end_free_space = this_grave_begin + std::distance(source_grave_begin, source.end_free_space);
		}
	}

	BinaryGraveyard(BinaryGraveyard<value_type>&& source) : graveyard(0), next_free_space(nullptr), end_free_space(nullptr) {
		swap(*this, source);
	}

	BinaryGraveyard<value_type>& operator=(BinaryGraveyard<value_type> source) {
		swap(*this, source);
		return *this;
	}

	reference bury(const_reference corpse) {
		return *this->bury(&corpse, 1);
	}

	pointer_type bury(const_pointer_type corpse, std::size_t count) {
		std::size_t* size_address;
		pointer_type value_address;
		std::size_t required_space = this->required_space(size_address, value_address, count);

		if (this->create_grave_on_demand(required_space)) this->required_space(size_address, value_address, count);

		*size_address = count;
		std::uninitialized_copy(corpse, corpse + count, value_address);

		this->graves.push_back(value_address);
		this->next_free_space = reinterpret_cast<char*>(value_address) + (count * sizeof(value_type));

		return value_address;
	}

	/**
	 * Returns the size of a grave as a multiple of sizeof(value_type)
	 **/
	static std::size_t grave_size(const_pointer_type grave) {
		return *BinaryGraveyard::locate_grave_size(grave);
	}

	iterator begin() {
		return BinaryGraveyard::iterator(&this->graves, this->graves.begin());
	}

	iterator end() {
		return BinaryGraveyard::iterator(&this->graves, this->graves.end());
	}

	~BinaryGraveyard() {
		this->destruct();

		Vector<void*>::iterator end = this->graveyard.end();
		for (Vector<void*>::iterator iterator = this->graveyard.begin(); iterator != end; ++iterator) {
			::operator delete(*iterator);
		}
	}


};

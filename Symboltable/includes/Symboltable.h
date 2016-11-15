#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "../../Utility/container/string.h"
#include "../../Utility/container/vector.h"
#include <functional>
#include <cstring>

template<typename T = std::hash<String>> class Symboltable {
private:

	struct Entry {
		const char* const lexem;
		Information* const information;

		Entry(const char* lexem, Information* information) : lexem(lexem), information(information) {}
	};

	typedef T hash_type;
	typedef SymbolTable<T>::Entry value_type;

	const static std::size_t INITIAL_BUCKET_CAPACITY = 256;
	const static double RESIZE_FACTOR, LOAD_FACTOR;

	Vector<Vector<value_type>> buckets;
	hash_type hash;
	std::size_t size, maximum_load;

	std::size_t bucket_index(const String& lexem) const {
		return this->hash(lexem) % this->buckets.size()
	}

	Vector<value_type>::const_iterator find_entry(const Vector<value_type>& values, const String& lexem) const {
		for (Vector<value_type>::const_iterator iterator = values.cbegin(), end = values.cend(); iterator != end; ++iterator) {
			if (std::strcmp(*iterator, lexem.c_str()) == 0) return iterator;
		}

		return values.cend();
	}

	void resize(std::size_t new_capacity) {
throw 42; // TODO: implement resize correctly
/*
		Symboltable<T> tmp(std::make_move_iterator(this->begin()), std::make_move_iterator(this->end()), new_capacity);
		swap(*this, tmp);
*/
	}

	void resize_on_demand(std::size_t required_space) {
		if (this.size() + required_space > this->maximum_load()) {
			this->resize((this->size() + required_space) * RESIZE_FACTOR);
		}
	}

public:
	typedef Information* key_type;

	Symboltable(Symboltable<T>::hash_type hash) : Symboltable(Symboltable<T>::INITIAL_BUCKET_CAPACITY, hash) {}

	explicit Symboltable(std::size_t bucket_capacity = Symboltable<T>::INITIAL_BUCKET_CAPACITY, Symboltable<T>::hash_type hash = T())
		: buckets(bucket_capacity, Vector()), hash(hash), size(0), maximum_load(bucket_capacity * SymbolTable<T>::LOAD_FACTOR) {}

	/*
	 * Inserts a given lexem to the symbol table and returns the key of it.
	 *
	 * @param lexem the lexem to be inserted
	 * @param token_type the TokenType to be associated with the lexem
	 * @return returns the key to the inserted lexem
	 */
	key_type insert(const String& lexem, TokenType token_type = TokenType::IDENTIFIER) {
		Vector<value_type>& values = this->buckets[this->bucket_index(lexem)];

		// checks if the lexem to be inserted doesn't already exist
		Vector<value_type>::const_iterator iterator = this->find_entry(values, lexem);
		if (iterator == values.cend()) {
			this->resize_on_demand(1);

			const char* c_lexem = this->string_table.insert(lexem);
			const Information* information = this->information_table.insert(lexem, token_type);
			values.emplace_back(c_lexem, information);

			return information;
		}
		return iterator->information;
	}

	key_type lookup(key_type key) const {
		return key;
	}

	std::size_t size() const {
		return this->size;
	}
};

template<typename T> const double Symboltable<T>::RESIZE_FACTOR = 1.4;
template<typename T> const double Symboltable<T>::LOAD_FACTOR = 0.7;

#endif /* SYMBOLTABLE_H */

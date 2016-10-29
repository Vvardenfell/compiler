/*
 * Symboltable.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: knad0001
 */

#include "../includes/Symboltable.h"
#include "../includes/Information.h"
#include "../includes/LinkedList.h"
#include "../includes/Key.h"
#include <cstddef>

/*
 * Creates a new table consisting of linked lists and initializes predefined symbols.
 */
Symboltable::Symboltable() {
	//the size of the table should be a prime number
	this->table_length = 503;

	//create an array of linked lists
	this->array_of_linked_lists = new LinkedList*[this->table_length];

	//create a new linked list for every single index
	for (std::size_t i = 0; i < this->table_length; i++) {
		this->array_of_linked_lists[i] = new LinkedList();
	}

	//initialize predefined symbols to the table
	this->init_symbols();
}


/*
 * Inserts a given lexem to the symbol table and returns the key of it.
 *
 * @param lexem the lexem to be inserted
 * @return returns the key to the inserted lexem
 */
Key Symboltable::insert(const char* lexem) {
	std::size_t size = this->determine_size(lexem);
	Key index(Symboltable::hash(lexem, size));

	// checks if the lexem to be inserted already exists
	if (this->lookup(index, lexem)) {
		return index;
	}

	this->array_of_linked_lists[index.get_key()]->add(lexem, size);

	return index;
}

/*
 * Searches for information to a given key and lexem and returns it.
 * If no such information is found, NULL will be returned.
 *
 * @param key the key to lookup in the symbol table
 * @param lexem the lexem to the key to look up
 * @return the information corresponding to the given key and lexem or NULL if no such was found
 */
const Information* Symboltable::lookup(const Key& key, const char* lexem) const {
	unsigned int index = key.get_key();
	SymboltableEntry* entry = this->array_of_linked_lists[index]->lookup_entry(lexem);

	//checks if entry is null
	if (entry) {
		return entry->get_entry_information();
	}
	else {
		return nullptr;
	}
}

/*
 * Calculates a hash with the SDBM hash function algorithm modified by a final
 * modulo operation.
 *
 * @param lexem the lexem to be hashed
 * @param size the length of the lexem
 * @return returns a hash as unsigned integer, that represents an index in the symbol table
 *
 */
unsigned int Symboltable::hash(const char* lexem, std::size_t size) const {
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < size; i++)
	{
		hash = (lexem[i]) + (hash << 6) + (hash << 16) - hash;
	}

	return hash % this->table_length;

}

/*
 * Initializes the Symboltable with some keywords.
 */
void Symboltable::init_symbols() {

	this->insert("while");
	this->insert("WHILE");
	this->insert("if");
	this->insert("IF");
}

/*
 * Determines the size of a given lexem.
 *
 * @param lexem the lexem to determine the size for
 * @return returns the size of the lexem
 */
std::size_t Symboltable::determine_size(const char* lexem) const {
	std::size_t size = 0;

	while (*lexem != '\0') {
		lexem++;
		size++;
	}

	return size;
}

/*
 * Gets the size of the table.
 *
 * @return returns the size
 */
std::size_t Symboltable::get_size() const {
	return this->table_length;
}

/*
 * Gets the symbol table array.
 *
 * @return returns the array
 */
const LinkedList* const * Symboltable::get_array() const {
	return this->array_of_linked_lists;
}

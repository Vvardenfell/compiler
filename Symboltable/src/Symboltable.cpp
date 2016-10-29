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
	this->tableLength = 503;

	//create an array of linked lists
	this->arrayOfLinkedLists = new LinkedList*[this->tableLength];

	//create a new linked list for every single index
	for (int i = 0; i < this->tableLength; i++) {
		this->arrayOfLinkedLists[i] = new LinkedList();
	}

	//initialize predefined symbols to the table
	this->initSymbols();
}

Symboltable::~Symboltable() {

}

/*
 * Inserts a given lexem to the symbol table and returns the key of it.
 *
 * @param lexem the lexem to be inserted
 * @return returns the key to the inserted lexem
 */
Key* Symboltable::insert(char* lexem) {
	int size = this->determineSize(lexem);
	Key* index = new Key(this->hash(lexem, size));

	// checks if the lexem to be inserted already exists
	if (this->lookup(index, lexem) != NULL) {
		return index;
	}

	this->arrayOfLinkedLists[index->getKey()]->add(lexem, size);

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
Information* Symboltable::lookup(Key* key, char* lexem) {
	unsigned int index = key->getKey();
	SymboltableEntry* entry = this->arrayOfLinkedLists[index]->lookupEntry(lexem);

	//checks if entry is null
	if (entry) {
		return entry->getEntryInformation();
	}
	else {
		return NULL;
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
unsigned int Symboltable::hash(char* lexem, unsigned int size) {
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < size; i++)
	{
		hash = (lexem[i]) + (hash << 6) + (hash << 16) - hash;
	}

	return hash % this->tableLength;

}

/*
 * Initializes the Symboltable with some keywords.
 */
void Symboltable::initSymbols() {

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
unsigned int Symboltable::determineSize(char* lexem) {
	int size = 0;

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
int Symboltable::getSize() {
	return this->tableLength;
}

/*
 * Gets the symbol table array.
 *
 * @return returns the array
 */
LinkedList** Symboltable::getArray() {
	return this->arrayOfLinkedLists;
}

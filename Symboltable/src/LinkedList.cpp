/*
 * LinkedList.cpp
 *
 *  Created on: 23.10.2016
 *      Author: lara
 */

#include "../includes/LinkedList.h"
#include "../includes/SymboltableEntry.h"
#include <cstddef>

/*
 * Creates a new linked list and sets its size to zero.
 */
LinkedList::LinkedList() {
	this->size = 0;
}

LinkedList::~LinkedList() {

}

/*
 * Creates a new entry for a given lexem and adds it to the list.
 *
 * @param lexem the lexem to be added
 * @param length the length of the lexem
 */
void LinkedList::add(char* lexem, int length) {
	SymboltableEntry* entry = new SymboltableEntry(lexem, length);

	//checks if the list is empty
	if (!this->firstEntry) {
		this->firstEntry = entry;
		this->lastEntry = entry;

		this->size++;
	}
	//adds the entry to the end of the list
	else {
		this->lastEntry->setNext(entry);
		this->lastEntry = entry;

		this->size++;
	}
}

/*
 * Gets the size of the list.
 *
 * @return returns the size of the list
 */
int LinkedList::getSize() {
	return this->size;
}

/*
 * Searches for an entry storing the given lexem in the linked list.
 *
 * @param lexem the lexem to lookup
 * @return returns the entry of the linked list storing the lexem or NULL otherwise
 */
SymboltableEntry* LinkedList::lookupEntry(char* lexem) {
	SymboltableEntry* entry = this->firstEntry;

	while (entry != NULL) {
		if (entry->getEntryInformation()->compareLexem(lexem)) {
			return entry;
		}
		else {
			entry = entry->getNext();
		}
	}
	return NULL;
}

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
LinkedList::LinkedList() : size(0) {}


/*
 * Creates a new entry for a given lexem and adds it to the list.
 *
 * @param lexem the lexem to be added
 * @param length the length of the lexem
 */
void LinkedList::add(const char* lexem, std::size_t length) {
	SymboltableEntry* entry = new SymboltableEntry(lexem, length);

	//checks if the list is empty
	if (!this->first_entry) {
		this->first_entry = entry;
	}
	//adds the entry to the end of the list
	else {
		this->last_entry->set_next(entry);
	}

	this->last_entry = entry;
	this->size++;
}

/*
 * Gets the size of the list.
 *
 * @return returns the size of the list
 */
std::size_t LinkedList::get_size() const {
	return this->size;
}

/*
 * Searches for an entry storing the given lexem in the linked list.
 *
 * @param lexem the lexem to lookup
 * @return returns the entry of the linked list storing the lexem or NULL otherwise
 */
SymboltableEntry* LinkedList::lookup_entry(const char* lexem) {
	SymboltableEntry* entry = this->first_entry;

	while (entry) {
		if (entry->get_entry_information()->compare_lexem(lexem)) {
			return entry;
		}
		else {
			entry = entry->get_next();
		}
	}
	return nullptr;
}

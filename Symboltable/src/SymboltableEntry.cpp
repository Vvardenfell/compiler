/*
 * SymboltableEntry.cpp
 *
 *  Created on: 23.10.2016
 *  Author: Lara
 */
#include "../includes/SymboltableEntry.h"
#include "../includes/Information.h"


/*
 * Creates a new entry in the linked list.
 *
 * @param name the entry to be created
 * @param length the length of the lexem (without '\0’)
 */
SymboltableEntry::SymboltableEntry(const char* lexem, std::size_t length) : info(new Information(lexem, length)) {}


/*
 * Sets the following entry to the current in the linked list.
 *
 * @param entry the entry to be set as next entry
 */
void SymboltableEntry::set_next(SymboltableEntry* entry) {
	this->next = entry;
}

/*
 * Gets the entry information of the current entry node.
 *
 * @return returns the entry information
 */
const Information* SymboltableEntry::get_entry_information() const {
	return this->info;
}

/*
 * Gets the entry that follows the current.
 *
 * @return returns the following entry
 */
SymboltableEntry* SymboltableEntry::get_next() const {
	return this->next;
}













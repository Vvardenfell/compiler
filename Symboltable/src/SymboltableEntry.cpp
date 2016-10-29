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
SymboltableEntry::SymboltableEntry(char* lexem, int length) {
	info = new Information(lexem, length);
}

SymboltableEntry::~SymboltableEntry() {

}

/*
 * Sets the following entry to the current in the linked list.
 *
 * @param entry the entry to be set as next entry
 */
void SymboltableEntry::setNext(SymboltableEntry* entry) {
	this->next = entry;
}

/*
 * Gets the entry information of the current entry node.
 *
 * @return returns the entry information
 */
Information* SymboltableEntry::getEntryInformation() {
	return this->info;
}

/*
 * Gets the entry that follows the current.
 *
 * @return returns the following entry
 */
SymboltableEntry* SymboltableEntry::getNext() {
	return this->next;
}













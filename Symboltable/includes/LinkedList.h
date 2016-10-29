/*
 * LinkedList.h
 *
 *  Created on: 23.10.2016
 *      Author: lara
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "../includes/SymboltableEntry.h"
#include <cstddef>

class LinkedList {
private:
	SymboltableEntry* first_entry = nullptr;
	SymboltableEntry* last_entry = nullptr;
	std::size_t size;

public:
	LinkedList();

	void add(const char* lexem, std::size_t length);
	std::size_t get_size() const;
	SymboltableEntry* lookup_entry(const char* lexem);
};

#endif /* LINKEDLIST_H */

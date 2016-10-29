/*
 * SymboltableEntry.h
 *
 *  Created on: 23.10.2016
 *      Author: lara
 */

#ifndef SYMBOLTABLE_ENTRY_H
#define SYMBOLTABLE_ENTRY_H

#include "../includes/Information.h"
#include <cstddef>

class SymboltableEntry {
private:
	SymboltableEntry* next = nullptr;
	const Information* const info;

public:
	SymboltableEntry(const char* lexem, std::size_t length);

	void set_next(SymboltableEntry* entry);
	const Information* get_entry_information() const;
	SymboltableEntry* get_next() const;

};

#endif /* SYMBOLTABLE_ENTRY_H */

/*
 * SymboltableEntry.h
 *
 *  Created on: 23.10.2016
 *      Author: lara
 */

#ifndef SYMBOLTABLE_SRC_SYMBOLTABLEENTRY_H_
#define SYMBOLTABLE_SRC_SYMBOLTABLEENTRY_H_

#include "../includes/Information.h"
#include <cstddef>

class SymboltableEntry {
private:
	SymboltableEntry* next = NULL;
	Information* info;

public:
	SymboltableEntry(char* lexem, int length);
	virtual ~SymboltableEntry();

	void setNext(SymboltableEntry* entry);
	Information* getEntryInformation();
	SymboltableEntry* getNext();

};

#endif /* SYMBOLTABLE_SRC_SYMBOLTABLEENTRY_H_ */

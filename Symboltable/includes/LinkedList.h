/*
 * LinkedList.h
 *
 *  Created on: 23.10.2016
 *      Author: lara
 */

#ifndef SYMBOLTABLE_SRC_LINKEDLIST_H_
#define SYMBOLTABLE_SRC_LINKEDLIST_H_

#include "../includes/SymboltableEntry.h"
#include <cstddef>

class LinkedList {
private:
	SymboltableEntry* firstEntry = NULL;
	SymboltableEntry* lastEntry = NULL;
	int size;

public:
	LinkedList();
	virtual ~LinkedList();

	void add(char* lexem, int length);
	int getSize();
	SymboltableEntry* lookupEntry(char* lexem);
};

#endif /* SYMBOLTABLE_SRC_LINKEDLIST_H_ */

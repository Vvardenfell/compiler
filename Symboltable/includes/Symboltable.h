/*
 * Symboltable.h
 *
 *  Created on: Sep 26, 2012
 *      Author: knad0001
 */

#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#include "../includes/Information.h"
#include "../includes/LinkedList.h"
#include "../includes/Key.h"

class Symboltable {
private:
	int tableLength;
	LinkedList** arrayOfLinkedLists;

public:
	Symboltable();
	virtual ~Symboltable();

	Information* lookup(Key* key, char* lexem);
	Key* insert(char* lexem);
	unsigned int hash(char* lexem, unsigned int size);
	void initSymbols();
	unsigned int determineSize(char* lexem);
	int getSize();
	LinkedList** getArray();
};

#endif /* SYMBOLTABLE_H_ */

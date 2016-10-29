/*
 * Symboltable.h
 *
 *  Created on: Sep 26, 2012
 *      Author: knad0001
 */

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "../includes/Information.h"
#include "../includes/LinkedList.h"
#include "../includes/Key.h"

class Symboltable {
private:
	std::size_t table_length;
	LinkedList** array_of_linked_lists;

public:
	Symboltable();

	const Information* lookup(const Key& key, const char* lexem) const;
	Key insert(const char* lexem);
	void init_symbols();
	unsigned int hash(const char* lexem, std::size_t size) const;
	std::size_t determine_size(const char* lexem) const;
	std::size_t get_size() const;
	const LinkedList* const * get_array() const;
};

#endif /* SYMBOLTABLE_H */

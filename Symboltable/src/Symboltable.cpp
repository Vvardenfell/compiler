#include "../includes/Symboltable.h"
#include "../includes/Information.h"
#include <cstddef>


/*
 * Initializes the Symboltable with some keywords.
 */
void Symboltable::init_symbols() {

	this->insert("while");
	this->insert("WHILE");
	this->insert("if");
	this->insert("IF");
}

/*
 * Information.cpp
 *
 *  Created on: 23.10.2016
 *      Author: lara
 */

#include "../includes/Information.h"

/*
 * Creates information for a certain entry containing the lexem and its length.
 *
 * @param lexem the lexem to be stored
 * @param length the size of the lexem
 */
Information::Information(char* lexem, int length) {
	this->lexem = lexem;
	this->size = length;
	//TODO: Token-Typ speichern?
}

Information::~Information() {

}

/*
 * Compares the lexem in the info to an other lexem.
 *
 * @param lexem the lexem to be compared
 * @return returns true if the two lexems are identical and false otherwise
 */
bool Information::compareLexem(char* lexem) {
	int i = 0;

	while (lexem[i] != '\0' && this->lexem[i] != '\0') {
		if (lexem[i] == this->lexem[i]) {
			i++;
		}
		else {
			return false;
		}
	}

	return true;
}

/*
 * Gets the name of the lexem.
 *
 * @return returns the name
 */
char* Information::getName() {
	return this->lexem;
}

/*
 * Gets the size of the lexem.
 *
 * @return returns the size
 */
int Information::getSize() {
	return this->size;
}

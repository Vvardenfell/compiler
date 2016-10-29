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
Information::Information(const char* lexem, std::size_t length) : lexem(lexem), size(length) {
	//TODO: Token-Typ speichern?
}

/*
 * Compares the lexem in the info to an other lexem.
 *
 * @param lexem the lexem to be compared
 * @return returns true if the two lexems are identical and false otherwise
 */
bool Information::compare_lexem(const char* lexem) const {
	std::size_t i = 0;

	while (lexem[i] != '\0' && this->lexem[i] != '\0') {
		if (lexem[i] == this->lexem[i]) {
			i++;
		}
		else {
			return false;
		}
	}

	return lexem[i] == '\0' && this->lexem[i] == '\0';
}

/*
 * Gets the name of the lexem.
 *
 * @return returns the name
 */
const char* Information::get_name() const {
	return this->lexem;
}

/*
 * Gets the size of the lexem.
 *
 * @return returns the size
 */
std::size_t Information::get_size() const {
	return this->size;
}

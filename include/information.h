#ifndef INFORMATION_H
#define INFORMATION_H

#include "string.h"

enum class TokenType : unsigned char;

struct Information {
public:
	const String* const lexem;
	const TokenType type;


	/*
	 * Creates information for a certain entry containing the lexem and its type.
	 *
	 * @param lexem the lexem to be stored
	 * @param type the TokenType associated with the lexem
	 */
	Information(const String* lexem, TokenType type) : lexem(lexem), type(type) {}
};

#endif /* INFORMATION_H */

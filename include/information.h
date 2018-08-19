#ifndef INFORMATION_H
#define INFORMATION_H

#include "string.h"


enum class FundamentalType : unsigned char {
    NONE,
    ERROR,
    INT,
    ARRAY,
    INT_ARRAY
};

std::ostream& operator<<(std::ostream& out, FundamentalType type);

enum class TokenType : unsigned char;

struct Information {
public:
	const String* const lexem;
	const TokenType token_type;
	FundamentalType data_type;



	/*
	 * Creates information for a certain entry containing the lexem and its type.
	 *
	 * @param lexem the lexem to be stored
	 * @param token_type the TokenType associated with the lexem
	 */
	Information(const String* lexem, TokenType token_type) : lexem(lexem), token_type(token_type), data_type(FundamentalType::NONE) {}
};

#endif /* INFORMATION_H */

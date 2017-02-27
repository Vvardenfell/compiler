#ifndef TOKEN_H
#define TOKEN_H

#include "information.h"

enum class TokenType : unsigned char {
    DEADBEEF,

    PLUS,
    MINUS,
    COLON,
    ASTERISK,
    LESS_THAN,
    GREATER_THAN,
    EQUALITY,
    ASSIGNMENT,
    WHATEVER,
    NOT,
    LOGICAL_AND,
    SEMICOLON,
    PARENTHESIS_OPEN,
    PARENTHESIS_CLOSE,
    CURLY_BRACKET_OPEN,
    CURLY_BRACKET_CLOSE,
    SQUARE_BRACKET_OPEN,
    SQUARE_BRACKET_CLOSE,

    INTEGER,
    OUT_OF_RANGE_INTEGER,
    IDENTIFIER,

    IF,
    ELSE,
    WHILE,

    READ,
    WRITE,

    INT,

    COMMENT,
    LINE_FEED
};

class Token {
public:
	union {
		Information* information;
		long integer;
		char error_char;
	} value;
	std::size_t line, column;
	TokenType type;

	Token() : line(0), column(0), type(TokenType::DEADBEEF) {
		this->value.information = nullptr;
	}

	Token(std::size_t line, std::size_t column, char error_char) : line(line), column(column), type(TokenType::DEADBEEF) {
		this->value.error_char = error_char;
	}

	Token(std::size_t line, std::size_t column, long integer) : line(line), column(column), type(TokenType::INTEGER) {
		this->value.integer = integer;
	}

	Token(std::size_t line, std::size_t column, TokenType type, Information* information) : line(line), column(column), type(type) {
		this->value.information = information;
	}

	Token(std::size_t line, std::size_t column, TokenType type) : line(line), column(column), type(type) {
		this->value.information = nullptr;
	}
};

std::ostream& operator<<(std::ostream& out, TokenType type);
std::ostream& operator<<(std::ostream& out, const Token& token);

#endif /* TOKEN_H */

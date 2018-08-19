#ifndef TOKEN_H
#define TOKEN_H

#include "information.h"
#include "string.h"

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
    LINE_FEED,

    EPSILON,

    ENUM_ENTRY_COUNT
};

class Token {
public:
	union {
		Information* information;
		long integer;
		char error_char;
	} value;
	std::size_t line, column;
	TokenType token_type;

	Token() : line(0), column(0), token_type(TokenType::DEADBEEF) {
		this->value.information = nullptr;
	}

	Token(std::size_t line, std::size_t column, char error_char) : line(line), column(column), token_type(TokenType::DEADBEEF) {
		this->value.error_char = error_char;
	}

	Token(std::size_t line, std::size_t column, long integer) : line(line), column(column), token_type(TokenType::INTEGER) {
		this->value.integer = integer;
	}

	Token(std::size_t line, std::size_t column, TokenType token_type, Information* information) : line(line), column(column), token_type(token_type) {
		this->value.information = information;
	}

	Token(std::size_t line, std::size_t column, TokenType token_type) : line(line), column(column), token_type(token_type) {
		this->value.information = nullptr;
	}

	TokenType get_token_type() const {
	    return this->token_type;
	}

	FundamentalType get_data_type() const {
	    return this->value.information->data_type;
	}

	void set_data_type(FundamentalType data_type) {
	    this->value.information->data_type = data_type;
	}

	String to_string() const;
};

std::ostream& operator<<(std::ostream& out, TokenType token_type);
std::ostream& operator<<(std::ostream& out, const Token& token);

#endif /* TOKEN_H */

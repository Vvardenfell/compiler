#include "token.h"
#include "exception.h"
#include <ostream>
#include <climits>
#include <cstdio>

String Token::to_string() const {

    switch(this->token_type) {

    case TokenType::PLUS: return String("+", 1);
    case TokenType::MINUS: return String("-", 1);
    case TokenType::COLON: return String(":", 1);
    case TokenType::ASTERISK: return String("*", 1);
    case TokenType::LESS_THAN: return String("<", 1);
    case TokenType::GREATER_THAN: return String(">", 1);
    case TokenType::EQUALITY: return String("=", 1);
    case TokenType::ASSIGNMENT: return String(":=", 2);
    case TokenType::WHATEVER: return String("=:=", 3);
    case TokenType::NOT: return String("!", 1);
    case TokenType::LOGICAL_AND: return String("&&", 2);
    case TokenType::SEMICOLON: return String(";", 1);
    case TokenType::PARENTHESIS_OPEN: return String("(", 1);
    case TokenType::PARENTHESIS_CLOSE: return String(")", 1);
    case TokenType::CURLY_BRACKET_OPEN: return String("{", 1);
    case TokenType::CURLY_BRACKET_CLOSE: return String("}", 1);
    case TokenType::SQUARE_BRACKET_OPEN: return String("[", 1);
    case TokenType::SQUARE_BRACKET_CLOSE: return String("]", 1);
    case TokenType::IF: return String("if", 2);
    case TokenType::ELSE: return String("else", 4);
    case TokenType::WHILE: return String("while", 5);
    case TokenType::READ: return String("read", 4);
    case TokenType::WRITE: return String("write", 5);
    case TokenType::INT: return String("int", 3);
    case TokenType::LINE_FEED: return String("\n", 1);
    case TokenType::EPSILON: return String();
    case TokenType::INTEGER: {
        const static std::size_t INTEGER_BITS = sizeof(long) * CHAR_BIT;
        const static std::size_t BITS_PER_DECIMAL_DIGIT = 3;
        const static std::size_t INTEGER_MEMORY_REQUIREMENT = (INTEGER_BITS / BITS_PER_DECIMAL_DIGIT) + 3;
        static char integer_storage[INTEGER_MEMORY_REQUIREMENT];

        std::snprintf(integer_storage, INTEGER_MEMORY_REQUIREMENT, "%li", this->value.integer);

        return String(integer_storage);
    }
    case TokenType::DEADBEEF:
	const static std::size_t BITS_PER_DECIMAL_DIGIT = 3;
	const static std::size_t CHAR_MEMORY_REQUIREMENT = (CHAR_BIT / BITS_PER_DECIMAL_DIGIT) + 3;
	static char char_storage[CHAR_MEMORY_REQUIREMENT];

	std::snprintf(char_storage, CHAR_MEMORY_REQUIREMENT, "%u", static_cast<unsigned int>(static_cast<unsigned char>(this->value.error_char)));

        return String(char_storage);
    case TokenType::IDENTIFIER:
    case TokenType::OUT_OF_RANGE_INTEGER:
    case TokenType::COMMENT:
        return *(this->value.information->lexem);
    default: throw UnsupportedTokenTypeException("Token::to_string() const", this->token_type);
    }
}


std::ostream& operator<<(std::ostream& out, TokenType token_type) {
	switch(token_type) {
	case TokenType::DEADBEEF: return out.write("DEADBEEF", 8);
	case TokenType::PLUS: return out.write("PLUS", 4);
	case TokenType::MINUS: return out.write("MINUS", 5);
	case TokenType::COLON: return out.write("COLON", 5);
	case TokenType::ASTERISK: return out.write("ASTERISK", 8);
	case TokenType::LESS_THAN: return out.write("LESS_THAN", 9);
	case TokenType::GREATER_THAN: return out.write("GREATER_THAN", 12);
	case TokenType::EQUALITY: return out.write("EQUALITY", 8);
	case TokenType::ASSIGNMENT: return out.write("ASSIGNMENT", 10);
	case TokenType::WHATEVER: return out.write("WHATEVER", 8);
	case TokenType::NOT: return out.write("NOT", 3);
	case TokenType::LOGICAL_AND: return out.write("LOGICAL_AND", 11);
	case TokenType::SEMICOLON: return out.write("SEMICOLON", 9);
	case TokenType::PARENTHESIS_OPEN: return out.write("PARENTHESIS_OPEN", 16);
	case TokenType::PARENTHESIS_CLOSE: return out.write("PARENTHESIS_CLOSE", 17);
	case TokenType::CURLY_BRACKET_OPEN: return out.write("CURLY_BRACKET_OPEN", 18);
	case TokenType::CURLY_BRACKET_CLOSE: return out.write("CURLY_BRACKET_CLOSE", 19);
	case TokenType::SQUARE_BRACKET_OPEN: return out.write("SQUARE_BRACKET_OPEN", 19);
	case TokenType::SQUARE_BRACKET_CLOSE: return out.write("SQUARE_BRACKET_CLOSE", 20);
	case TokenType::INTEGER: return out.write("INTEGER", 7);
	case TokenType::OUT_OF_RANGE_INTEGER: return out.write("OUT_OF_RANGE_INTEGER", 20);
	case TokenType::IDENTIFIER: return out.write("IDENTIFIER", 10);
	case TokenType::IF: return out.write("IF", 2);
	case TokenType::ELSE: return out.write("ELSE", 4);
	case TokenType::WHILE: return out.write("WHILE", 5);
	case TokenType::READ: return out.write("READ", 4);
	case TokenType::WRITE: return out.write("WRITE", 5);
	case TokenType::INT: return out.write("INT", 3);
	case TokenType::COMMENT: return out.write("COMMENT", 7);
	case TokenType::LINE_FEED: return out.write("LINE_FEED", 9);
	case TokenType::EPSILON: return out.write("EPSILON", 7);
	default: throw UnsupportedTokenTypeException("operator<<(std::ostream& out, TokenType token_type)", token_type);
	}
}

std::ostream& operator<<(std::ostream& out, const Token& token) {
	out << token.line << ':' << token.column;
	out.write(": ", 2);
	out << token.token_type;

	switch(token.token_type) {
	case TokenType::INTEGER:
		out << ' ' << token.value.integer;
		break;
	case TokenType::DEADBEEF:
		out << " '" << static_cast<unsigned int>(static_cast<unsigned char>(token.value.error_char)) << '\'';
		break;
	case TokenType::IDENTIFIER:
	case TokenType::OUT_OF_RANGE_INTEGER: {
		out << ' ';

		const Information& information = *token.value.information;
		out.write((information.lexem)->c_str(), (information.lexem)->size());
		break;
    }
	default: { /* pass - don't handle other TokenTypes in a special way */ }
	}

	return out;
}

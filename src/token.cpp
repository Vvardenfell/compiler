#include "token.h"
#include "exception.h"
#include <ostream>

std::ostream& operator<<(std::ostream& out, TokenType type) {
	switch(type) {
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
	case TokenType::WHILE: return out.write("WHILE", 5);
	case TokenType::COMMENT: return out.write("COMMENT", 7);
	case TokenType::LINE_FEED: return out.write("LINE_FEED", 9);
	default: throw UnsupportedTokenTypeException("operator<<(std::ostream& out, TokenType type)", type);
	}
}

std::ostream& operator<<(std::ostream& out, const Token& token) {
	out << token.line << ':' << token.column;
	out.write(": ", 2);
	out << token.type;

	switch(token.type) {
	case TokenType::INTEGER:
		out << ' ' << token.value.integer;
		break;
	case TokenType::DEADBEEF:
		out << ' ' << token.value.error_char;
		break;
	case TokenType::IDENTIFIER:
	case TokenType::OUT_OF_RANGE_INTEGER: {
		out << ' ';

		const Information& information = *token.value.information;
		out.write((information.lexem)->c_str(), (information.lexem)->size());
		break; }
	default: { /* pass - don't handle other TokenTypes in a special way */ }
	}

	return out;
}

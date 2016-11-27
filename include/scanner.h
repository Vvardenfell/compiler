#ifndef SCANNER_H
#define SCANNER_H

#include "string.h"
#include "file_position.h"
#include "symboltable.h"
#include "buffer.h"

class Scanner {
private:

	FilePosition file_position;
	std::function<void(Direction, char)> line_count_callback;
	FiniteStateMachine finite_state_machine;
	Symboltable symboltable;
	Buffer buffer;
	String lexem;
	Token token;

	void init_symboltable();
	Token* make_integer_token(const String& lexem);

public:

	Scanner(const String& file);
	Token next_token();
};

#endif /* SCANNER_H */

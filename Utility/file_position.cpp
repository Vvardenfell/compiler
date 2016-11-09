#include "file_position.h"
#include "../Automaton/includes/finite_state_machine.h"

void FilePosition::on_state_change(Direction direction, char symbol) {
	switch(direction) {
	case Direction::ENTER: {
		if (symbol == '\r') this->carriage_return = true;
		else this->increment_line();
		break;
	}
	case Direction::EXIT: {
		if (this->carriage_return && symbol != '\n') this->increment_line();
		break;
	}
	default: throw UnsupportedDirectionException("LineCount::on_state_change", direction);
	}
}

void FilePosition::increment_line() {
	this->carriage_return = false;
	this->line++;
	this->column = 1;
}

#ifndef FILE_POSITION_H
#define FILE_POSITION_H

#include <cstddef>
#include "finite_state_machine.h"

class FilePosition {
private:
	std::size_t line, column;
	bool carriage_return;

public:
	FilePosition() : line(1), column(1), carriage_return(false) {}

	void on_state_change(Direction direction, char symbol);
	void increment_column() { this->column++; }
	void increment_column(size_t column_count) { this->column += column_count; }
	void increment_line();

	std::size_t get_line() const { return this->line; }
	std::size_t get_column() const { return this->column; }
};

#endif /* FILE_POSITION_H */

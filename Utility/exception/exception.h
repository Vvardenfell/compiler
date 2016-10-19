#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>

class ParserException : public std::runtime_error {
protected:
	explicit ParserException(const std::string& description) : runtime_error(description) {}
};

class BufferInitializationException : public ParserException {
public:
	explicit BufferInitializationException(const std::string& description) : ParserException(description) {}
};

class BufferBoundsExceededException : public ParserException {
public:
	explicit BufferBoundsExceededException(const std::string& description) : ParserException(description) {}
};

#endif /* EXCEPTION_H */


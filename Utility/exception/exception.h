#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <string>

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

class TransitionCharacterProcessingException : public ParserException {
public:
	explicit TransitionCharacterProcessingException(const std::string& description) : ParserException(description) {}
};

class UncoveredCharClassException : public ParserException {
public:
	explicit UncoveredCharClassException(const std::string& description) : ParserException(description) {}
};

enum class Direction;
class UnsupportedDirectionException : public ParserException {
public:
	UnsupportedDirectionException(const std::string& occurrence, Direction direction) : ParserException(std::string("Direction with value ") + std::to_string(static_cast<const int>(direction)) + std::string(" not supported in ") + occurrence) {}
};

class UnsupportedCharacterEncodingException : public ParserException {
public:
	UnsupportedCharacterEncodingException(const std::string& occurrence, const std::string& requested_encoding, char invalid_value) : ParserException(occurrence + std::string(" detected an unsupported encoding, when processing '") + invalid_value + std::string("'. The required encoding is ") + requested_encoding) {}
};

class TooManyMachineStatesException : public ParserException {
public:
	TooManyMachineStatesException(const std::string& occurrence, size_t maximum, size_t given) : ParserException(occurrence + std::string(" got ") + std::to_string(given) + std::string(" States, which is above the maximum supported amount of ") + std::to_string(maximum) + std::string(" states")) {}
};

class FatalException : public ParserException {
public:
	FatalException(const std::string& occurrence, const std::string& description) : ParserException(std::string("Error in ") + occurrence + std::string(": ") + description) {}
};

enum class TransitionType;
class UnsupportedTransitionException : public ParserException {
public:
	UnsupportedTransitionException(const std::string& occurrence, TransitionType type) : ParserException(std::string("Unsupported Transition of TransitionType value ") + std::to_string(static_cast<int>(type)) + std::string(" in ") + occurrence) {}
};

#endif /* EXCEPTION_H */


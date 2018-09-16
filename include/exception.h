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
	UnsupportedDirectionException(const std::string& occurrence, Direction direction) : ParserException(std::string("Direction with value ") + std::to_string(static_cast<unsigned int>(direction)) + std::string(" not supported in ") + occurrence) {}
};

#include "token.h"
class UnsupportedTokenTypeException : public ParserException {
public:
	UnsupportedTokenTypeException(const std::string& occurrence, TokenType type) : ParserException(std::string("TokenType with value ") + std::to_string(static_cast<unsigned int>(type)) + std::string(" not supported in ") + occurrence) {}
};

class UnsupportedCharacterEncodingException : public ParserException {
public:
	UnsupportedCharacterEncodingException(const std::string& occurrence, const std::string& requested_encoding, char invalid_value) : ParserException(occurrence + std::string(" detected an unsupported encoding, when processing '") + std::to_string(static_cast<unsigned int>(static_cast<unsigned char>(invalid_value))) + std::string("'. The required encoding is ") + requested_encoding) {}
};

class CommandLineMissingArgumentsException : public ParserException {
public:
	CommandLineMissingArgumentsException(const char* executable) : ParserException(std::string("Usage: ") + std::string(executable) + std::string(" <IN FILE> <OUT FILE>")) {}
};

class TokenGeneratingException : public ParserException {
public:
	TokenGeneratingException(const std::string& occurrence) : ParserException(std::string("Failed to generate token in ") + occurrence) {}
};

class OutputFileFailureException : public ParserException {
public:
	OutputFileFailureException(const char* out) : ParserException(std::string("Failed to open file ") + std::string(out) + std::string(" for writing")) {}
};

class TooManyMachineStatesException : public ParserException {
public:
	TooManyMachineStatesException(const std::string& occurrence, size_t maximum, size_t given) : ParserException(occurrence + std::string(" got ") + std::to_string(given) + std::string(" States, which is above the maximum supported amount of ") + std::to_string(maximum) + std::string(" states")) {}
};

class FatalException : public ParserException {
public:
	FatalException(const std::string& occurrence, const std::string& description) : ParserException(std::string("Error in ") + occurrence + std::string(": ") + description) {}
};

enum class TransitionType : unsigned char;
class UnsupportedTransitionException : public ParserException {
public:
	UnsupportedTransitionException(const std::string& occurrence, TransitionType type) : ParserException(std::string("Unsupported Transition of TransitionType value ") + std::to_string(static_cast<int>(type)) + std::string(" in ") + occurrence) {}
};

class NoStartStateException : public ParserException {
public:
    NoStartStateException(const std::string& occurrence) : ParserException(std::string(occurrence + std::string("Can't create ParseMachine without a start state"))) {}
};

class TooManyProductionRulesException : public ParserException {
public:
	TooManyProductionRulesException(const std::string& occurrence, size_t maximum) : ParserException(occurrence + std::string(" exceeded the maximum amount of  ") + std::to_string(maximum) + std::string(" production rules")) {}
};

#include "grammar.h"
class UnsupportedVariableException : public ParserException {
public:
	UnsupportedVariableException(const std::string& occurrence, Grammar::Variable variable) : ParserException(std::string("Variable with value ") + std::to_string(static_cast<unsigned int>(variable)) + std::string(" not supported in ") + occurrence) {}
};

#include "information.h"
class UnsupportedFundamentalTypeException : public ParserException {
public:
	UnsupportedFundamentalTypeException(const std::string& occurrence, FundamentalType type) : ParserException(std::string("FundamentalType with value ") + std::to_string(static_cast<unsigned int>(type)) + std::string(" not supported in ") + occurrence) {}
};

class LabelsExhaustedException : public ParserException {
public:
    LabelsExhaustedException(const std::string& occurrence) : ParserException(std::string("Maximum amount of labels to be generated reached in ") + occurrence) {}
};

#endif /* EXCEPTION_H */


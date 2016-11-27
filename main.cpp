#include "Scanner/includes/Scanner.h"
#include <iostream>

enum Exit {
	EXIT_SUCCESS_0,
	EXIT_UNEXPECTED_EXCEPTION,
	EXIT_FATAL_ERROR,
	EXIT_UNSUPPORTED_ENCODING,
	EXIT_MISSING_COMMAND_LINE_ARGUMENTS,
	EXIT_INPUT_FILE_FAILURE,
	EXIT_OUTPUT_FILE_FAILURE
};

int main(int argc, char* argv[]) {
	try {
		if (argc <3) throw CommandLineMissingArgumentsException(argv[0]);

		std::ios_base::sync_with_stdio(false);
		std::cout.tie(nullptr);
		std::cin.tie(nullptr);

		Scanner scanner(argv[1]);
		Vector<Token> tokens;

		std::ofstream out(argv[2]);
		if (!out.is_open()) throw OutputFileFailureException(argv[2]);

		try {
			while(true) {
				tokens.push_back(scanner.next_token());
			}
		} catch(const BufferBoundsExceededException& end_of_file) {
			for (Vector<Token>::iterator iterator = tokens.begin(), end = tokens.end(); iterator != end; ++iterator) {
				switch(iterator->type) {
				case TokenType::DEADBEEF:
				case TokenType::OUT_OF_RANGE_INTEGER:
					std::cerr << *iterator << '\n';
					break;
				default:
					out << *iterator << '\n';
				}
			}
		} catch(const UnsupportedCharacterEncodingException& encoding_exception) {
			std::cerr << encoding_exception.what() << std::endl;
			return EXIT_UNSUPPORTED_ENCODING;
		}

		return EXIT_SUCCESS_0;

	} catch(const CommandLineMissingArgumentsException& exception) {
		std::cerr << exception.what() << std::endl;
		return EXIT_MISSING_COMMAND_LINE_ARGUMENTS;
	} catch(const BufferInitializationException& exception) {
		std::cerr << exception.what() << std::endl;
		return EXIT_INPUT_FILE_FAILURE;
	} catch(const OutputFileFailureException& exception) {
		std::cerr << exception.what() << std::endl;
		return EXIT_OUTPUT_FILE_FAILURE;
	} catch(const std::exception& exception) {
		std::cerr << "Unexpected exception: " << exception.what() << std::endl;
		return EXIT_UNEXPECTED_EXCEPTION;
	} catch(...) {
		std::cerr << "Fatal error!" << std::endl;
		return EXIT_FATAL_ERROR;
	}
}

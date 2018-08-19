#include "scanner.h"
#include "grammar.h"
#include "parser.h"
#include "parse_tree.h"
#include "type_check.h"
#include "make_code.h"
#include <iostream>

enum Exit {
	EXIT_SUCCESS_0,
	EXIT_UNEXPECTED_EXCEPTION,
	EXIT_FATAL_ERROR,
	EXIT_MISSING_COMMAND_LINE_ARGUMENTS,
	EXIT_INPUT_FILE_FAILURE,
	EXIT_OUTPUT_FILE_FAILURE
};


void parse(Scanner* scanner, Parser* parser, bool* is_scan_valid) {
    std::cout << "Checking syntax..." << std::endl;

    while(true) {
        try {
            Token token(scanner->next_token());

            if(token.get_token_type() == TokenType::DEADBEEF) {
                std::cerr << token << " - Unexpected character";
                *is_scan_valid = false;
            }
            else parser->process(token);

        } catch(const UnsupportedCharacterEncodingException& encoding_exception) {
			std::cerr << encoding_exception.what() << std::endl;
			*is_scan_valid = false;
		}
    }
}


bool check_types(Parser* parser) {
    if(parser->finalize()) {
        std::cout << "\nChecking types..." << std::endl;
        ParseTree<Parser::TreeData>& parse_tree = parser->parse_tree();

        return TypeCheck(&parse_tree, &std::cerr)();
    }

    return false;
}


int main(int argc, char* argv[]) {
	try {
		if (argc <3) throw CommandLineMissingArgumentsException(argv[0]);

		std::ios_base::sync_with_stdio(false);
        std::cin.tie(nullptr);
		std::cout.tie(nullptr);
		std::cerr.tie(nullptr);

		Grammar grammar(get_grammar_description());
		Parser parser(grammar.rules(), &std::cerr);
		Scanner scanner(argv[1]);
		bool is_scan_valid = true;

		std::ofstream out(argv[2], std::ofstream::out | std::ofstream::trunc);
		if (!out.is_open()) throw OutputFileFailureException(argv[2]);

		try {
            parse(&scanner, &parser, &is_scan_valid);
		} catch(const BufferBoundsExceededException& end_of_file) {

            if(check_types(&parser) && is_scan_valid) {
                std::cout << "\nGenerating code..." << std::endl;
                MakeCode(&parser.parse_tree(), &out)();
		    }
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

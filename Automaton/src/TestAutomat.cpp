#include "../includes/finite_state_machine.h"
#include "../../Utility/container/string.h"
#include "../../Utility/character_classification.h"
#include "../../Buffer/includes/Buffer.h"
#include "../../Utility/exception/exception.h"
#include "../../Utility/file_position.h"
#include <iostream>

int main(int argc, char* argv[]) {

	try {

		std::ios_base::sync_with_stdio(false);
		std::cout.tie(nullptr);
		std::cin.tie(nullptr);

		FilePosition line_count;
		std::function<void(Direction, char)> line_count_callback(std::bind(&FilePosition::on_state_change, &line_count, std::placeholders::_1, std::placeholders::_2));

		// Identifier States and Transitions
		FinalState identifier_final(TokenType::IDENTIFIER);

		TypeTransition to_identifier_final_by_alpha_numeric(&identifier_final, CharClass::ALPHA_NUMERIC);
		identifier_final.add(&to_identifier_final_by_alpha_numeric);

		TypeTransition to_identifier_final_by_alpha(&identifier_final, CharClass::ALPHA);
		identifier_final.add(&to_identifier_final_by_alpha);

		// Number States and Transitions
		FinalState integer_final(TokenType::INTEGER);

		TypeTransition to_integer_final_by_numeric(&integer_final, CharClass::NUMERIC);
		integer_final.add(&to_integer_final_by_numeric);

		// Plus States and Transitions
		FinalState plus_final(TokenType::PLUS);

		CharTransition to_plus_final_by_plus(&plus_final, '+');

		// Minus States and Transitions
		FinalState minus_final(TokenType::MINUS);

		CharTransition to_minus_final_by_minus(&minus_final, '-');

		// Comment States and Transitions (Depends upon Colon States and Transitions and serves as dependency for Linefeed States and Transitions within Comments)
		FinalState comment_entry(TokenType::COMMENT);

		TypeTransition to_comment_entry_by_any(&comment_entry, CharClass::ANY);
		comment_entry.add(&to_comment_entry_by_any);

		CharTransition to_comment_entry_by_asterisk(&comment_entry, '*');


		FinalState comment_exit(TokenType::COMMENT, &to_comment_entry_by_any);

		CharTransition to_comment_exit_by_asterisk(&comment_exit, '*');
		comment_entry.add(&to_comment_exit_by_asterisk);


		FinalState comment_final(TokenType::COMMENT);

		CharTransition to_comment_final_by_colon(&comment_final, ':');
		comment_exit.add(&to_comment_final_by_colon);

		// Linefeed States and Transitions within Comments (Depens upon Comment States and Transitions and serves as dependency for Comment States and Transitions)

		CallbackFinalState comment_mac_line_feed_final(TokenType::COMMENT, &to_comment_entry_by_any, line_count_callback);

		comment_mac_line_feed_final.add(&to_comment_exit_by_asterisk);

		CharTransition to_comment_mac_line_feed_final_by_carriage_return(&comment_mac_line_feed_final, '\r');
		comment_mac_line_feed_final.add(&to_comment_mac_line_feed_final_by_carriage_return);
		comment_entry.add(&to_comment_mac_line_feed_final_by_carriage_return);
		comment_exit.add(&to_comment_mac_line_feed_final_by_carriage_return);


		CallbackFinalState comment_unix_line_feed_final(TokenType::COMMENT, &to_comment_entry_by_any, line_count_callback);

		CharTransition to_comment_unix_line_feed_final_by_line_feed(&comment_mac_line_feed_final, '\n');
		comment_mac_line_feed_final.add(&to_comment_unix_line_feed_final_by_line_feed);
		comment_entry.add(&to_comment_unix_line_feed_final_by_line_feed);
		comment_exit.add(&to_comment_unix_line_feed_final_by_line_feed);
		comment_unix_line_feed_final.add(&to_comment_unix_line_feed_final_by_line_feed);

		comment_unix_line_feed_final.add(&to_comment_exit_by_asterisk);
		comment_unix_line_feed_final.add(&to_comment_mac_line_feed_final_by_carriage_return);

		// Assignment States and Transitions (Depends upon Colon States and Transitions)
		FinalState assignment_final(TokenType::ASSIGNMENT);

		CharTransition to_assignment_final_by_equal(&assignment_final, '=');

		// Colon States and Transitions (Serves as dependency for Assignment and Comment States and Transitions)
		FinalState colon_final(TokenType::COLON, {&to_assignment_final_by_equal, &to_comment_entry_by_asterisk});

		CharTransition to_colon_final_by_colon(&colon_final, ':');

		// Asterisk States and Transitions
		FinalState asterisk_final(TokenType::ASTERISK);

		CharTransition to_asterisk_final_by_asterisk(&asterisk_final, '*');

		// Less Than States and Transitions
		FinalState less_than_final(TokenType::LESS_THAN);

		CharTransition to_less_than_final_by_less_than(&less_than_final, '<');

		// Greater Than States and Transitions
		FinalState greater_than_final(TokenType::GREATER_THAN);

		CharTransition to_greater_than_final_by_greater_than(&greater_than_final, '>');

		// Whatever States and Transitions (Depends upon Equality States and Transitions)
		FinalState whatever_final(TokenType::WHATEVER);

		CharTransition to_whatever_final_by_equal(&whatever_final, '=');
		State whatever_non_final(&to_whatever_final_by_equal);

		CharTransition to_whatever_non_final_by_colon(&whatever_non_final, ':');

		// Equality States and Transitions (Serves as dependency for Whatever States and Transitions)
		FinalState equality_final(TokenType::EQUALITY, &to_whatever_non_final_by_colon);

		CharTransition to_equality_final_by_equal(&equality_final, '=');

		// Logical Not States and Transitions
		FinalState logical_not_final(TokenType::NOT);

		CharTransition to_logical_not_final_by_exclamation_mark(&logical_not_final, '!');

		// Semicolon States and Transitions
		FinalState semicolon_final(TokenType::SEMICOLON);

		CharTransition to_semicolon_final_by_semicolon(&semicolon_final, ';');

		// Parenthesis Open States and Transitions
		FinalState parenthesis_open_final(TokenType::PARENTHESIS_OPEN);

		CharTransition to_parenthesis_open_final_by_parenthesis_open(&parenthesis_open_final, '(');

		// Parenthesis Close States and Transitions
		FinalState parenthesis_close_final(TokenType::PARENTHESIS_CLOSE);

		CharTransition to_parenthesis_close_final_by_parenthesis_close(&parenthesis_close_final, ')');

		// Curly Bracket Open States and Transitions
		FinalState curly_bracket_open_final(TokenType::CURLY_BRACKET_OPEN);

		CharTransition to_curly_bracket_open_final_by_curly_bracket_open(&curly_bracket_open_final, '{');

		// Curly Bracket Close States and Transitions
		FinalState curly_bracket_close_final(TokenType::CURLY_BRACKET_CLOSE);

		CharTransition to_curly_bracket_close_final_by_curly_bracket_close(&curly_bracket_close_final, '}');

		// Square Bracket Open States and Transitions
		FinalState square_bracket_open_final(TokenType::SQUARE_BRACKET_OPEN);

		CharTransition to_square_bracket_open_final_by_square_bracket_open(&square_bracket_open_final, '[');

		// Square Bracket Close States and Transitions
		FinalState square_bracket_close_final(TokenType::SQUARE_BRACKET_CLOSE);

		CharTransition to_square_bracket_close_final_by_square_bracket_close(&square_bracket_close_final, ']');

		// Logical And States and Transitions
		FinalState logical_and_final(TokenType::LOGICAL_AND);

		CharTransition to_logical_and_final_by_and(&logical_and_final, '&');
		State logical_and_non_final(&to_logical_and_final_by_and);

		CharTransition to_logical_and_non_final_by_and(&logical_and_non_final, '&');

		// Linefeed States and Transitions
		CallbackFinalState mac_line_feed_final(TokenType::LINE_FEED, line_count_callback);

		CharTransition to_mac_line_feed_final_by_carriage_return(&mac_line_feed_final, '\r');
		mac_line_feed_final.add(&to_mac_line_feed_final_by_carriage_return);


		CallbackFinalState unix_line_feed_final(TokenType::LINE_FEED, &to_mac_line_feed_final_by_carriage_return, line_count_callback);

		CharTransition to_unix_line_feed_final_by_new_line(&unix_line_feed_final, '\n');
		unix_line_feed_final.add(&to_unix_line_feed_final_by_new_line);
		mac_line_feed_final.add(&to_unix_line_feed_final_by_new_line);



		State start({
			&to_identifier_final_by_alpha,
			&to_integer_final_by_numeric,
			&to_plus_final_by_plus,
			&to_minus_final_by_minus,
			&to_colon_final_by_colon,
			&to_asterisk_final_by_asterisk,
			&to_less_than_final_by_less_than,
			&to_greater_than_final_by_greater_than,
			&to_equality_final_by_equal,
			&to_logical_not_final_by_exclamation_mark,
			&to_semicolon_final_by_semicolon,
			&to_parenthesis_open_final_by_parenthesis_open,
			&to_parenthesis_close_final_by_parenthesis_close,
			&to_curly_bracket_open_final_by_curly_bracket_open,
			&to_curly_bracket_close_final_by_curly_bracket_close,
			&to_square_bracket_open_final_by_square_bracket_open,
			&to_square_bracket_close_final_by_square_bracket_close,
			&to_logical_and_non_final_by_and,
			&to_mac_line_feed_final_by_carriage_return,
			&to_unix_line_feed_final_by_new_line
		});

		FiniteStateMachine automaton(&start);

		//const char* input = "abcdef123&&&=:1337:==:=;!=:";
		//const char* input = "abcdef123&&&=:1337:==:=;!=:&";
		//const char* input = "abcdef123&&&=:1337:==:=;!=:a";
		//const char* input = "abcdef123&&&=:1337:==:=;!=:foobar";

		//Buffer buffer("/mnt/hgfs/foobar");
		Buffer buffer("/mnt/hgfs/test_input");
		String lexem;

		try {
			while (char current = buffer.get()) {
				lexem += current;
				if (!automaton.process(current)) {
					std::size_t steps_since_last_final_state = automaton.get_steps_since_last_final_state();
					if (steps_since_last_final_state == lexem.size()) {
						if (!is_space(lexem[0])) {
							std::cout << "generate error token for " << lexem[0] << " found at " << line_count.get_line() << ":" << line_count.get_column() << std::endl;
							for (std::size_t steps_back = steps_since_last_final_state; steps_back > 1; steps_back--) buffer.unget();
						}
						line_count.increment_column();
					}
					else {
						for (std::size_t steps_back = steps_since_last_final_state; steps_back > 0; steps_back--) buffer.unget();

						TokenType token_type = automaton.get_last_final_state().token();

						if (token_type != TokenType::LINE_FEED) {
							String actual_lexem(lexem.begin(), lexem.end() - steps_since_last_final_state);

							if (token_type == TokenType::COMMENT) {
								std::size_t last_line_length = std::distance(actual_lexem.find_last_of("\r\n"), actual_lexem.cend()) - 1;
								line_count.increment_column(last_line_length == -1 ? actual_lexem.size() : last_line_length);
							}
							else {
								std::cout << "generate " << static_cast<int>(token_type) << " for " << &actual_lexem[0] << " found at " << line_count.get_line() << ":" << line_count.get_column() << std::endl;
								line_count.increment_column(actual_lexem.size());
							}
						}
					}

					lexem.clear();
					automaton.reset();
				}
			}
		} catch(const BufferBoundsExceededException& end_of_file) {
			/* pass */
		} catch(const UnsupportedCharacterEncodingException& encoding_exception) {
			std::cerr << encoding_exception.what() << std::endl;
		}

	} catch(const std::exception& exception) {
		std::cerr << "Unexpected exception: " << exception.what() << std::endl;
	} catch(...) {
		std::cerr << "Fatal error!" << std::endl;
	}

}

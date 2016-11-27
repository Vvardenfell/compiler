#include "finite_state_machine.h"
#include "string.h"
#include "character_classification.h"
#include "buffer.h"
#include "exception.h"
#include "scanner.h"

FiniteStateMachine init_finite_state_machine(const std::function<void(Direction, char)>& line_count_callback) {

	// Identifier States and Transitions
	static FinalState identifier_final(TokenType::IDENTIFIER);

	static TypeTransition to_identifier_final_by_alpha_numeric(&identifier_final, CharClass::ALPHA_NUMERIC);
	identifier_final.add(&to_identifier_final_by_alpha_numeric);

	static TypeTransition to_identifier_final_by_alpha(&identifier_final, CharClass::ALPHA);
	identifier_final.add(&to_identifier_final_by_alpha);

	// Number States and Transitions
	static FinalState integer_final(TokenType::INTEGER);

	static TypeTransition to_integer_final_by_numeric(&integer_final, CharClass::NUMERIC);
	integer_final.add(&to_integer_final_by_numeric);

	// Plus States and Transitions
	static FinalState plus_final(TokenType::PLUS);

	static CharTransition to_plus_final_by_plus(&plus_final, '+');

	// Minus States and Transitions
	static FinalState minus_final(TokenType::MINUS);

	static CharTransition to_minus_final_by_minus(&minus_final, '-');

	// Comment States and Transitions (Depends upon Colon States and Transitions and serves as dependency for Linefeed States and Transitions within Comments)
	static FinalState comment_entry(TokenType::COMMENT);

	static TypeTransition to_comment_entry_by_any(&comment_entry, CharClass::ANY);
	comment_entry.add(&to_comment_entry_by_any);

	static CharTransition to_comment_entry_by_asterisk(&comment_entry, '*');


	static FinalState comment_exit(TokenType::COMMENT, &to_comment_entry_by_any);

	static CharTransition to_comment_exit_by_asterisk(&comment_exit, '*');
	comment_entry.add(&to_comment_exit_by_asterisk);


	static FinalState comment_final(TokenType::COMMENT);

	static CharTransition to_comment_final_by_colon(&comment_final, ':');
	comment_exit.add(&to_comment_final_by_colon);

	// Linefeed States and Transitions within Comments (Depens upon Comment States and Transitions and serves as dependency for Comment States and Transitions)

	static CallbackFinalState comment_mac_line_feed_final(TokenType::COMMENT, &to_comment_entry_by_any, line_count_callback);

	comment_mac_line_feed_final.add(&to_comment_exit_by_asterisk);

	static CharTransition to_comment_mac_line_feed_final_by_carriage_return(&comment_mac_line_feed_final, '\r');
	comment_mac_line_feed_final.add(&to_comment_mac_line_feed_final_by_carriage_return);
	comment_entry.add(&to_comment_mac_line_feed_final_by_carriage_return);
	comment_exit.add(&to_comment_mac_line_feed_final_by_carriage_return);


	static CallbackFinalState comment_unix_line_feed_final(TokenType::COMMENT, &to_comment_entry_by_any, line_count_callback);

	static CharTransition to_comment_unix_line_feed_final_by_line_feed(&comment_mac_line_feed_final, '\n');
	comment_mac_line_feed_final.add(&to_comment_unix_line_feed_final_by_line_feed);
	comment_entry.add(&to_comment_unix_line_feed_final_by_line_feed);
	comment_exit.add(&to_comment_unix_line_feed_final_by_line_feed);
	comment_unix_line_feed_final.add(&to_comment_unix_line_feed_final_by_line_feed);

	comment_unix_line_feed_final.add(&to_comment_exit_by_asterisk);
	comment_unix_line_feed_final.add(&to_comment_mac_line_feed_final_by_carriage_return);

	// Assignment States and Transitions (Depends upon Colon States and Transitions)
	static FinalState assignment_final(TokenType::ASSIGNMENT);

	static CharTransition to_assignment_final_by_equal(&assignment_final, '=');

	// Colon States and Transitions (Serves as dependency for Assignment and Comment States and Transitions)
	static FinalState colon_final(TokenType::COLON, {&to_assignment_final_by_equal, &to_comment_entry_by_asterisk});

	static CharTransition to_colon_final_by_colon(&colon_final, ':');

	// Asterisk States and Transitions
	static FinalState asterisk_final(TokenType::ASTERISK);

	static CharTransition to_asterisk_final_by_asterisk(&asterisk_final, '*');

	// Less Than States and Transitions
	static FinalState less_than_final(TokenType::LESS_THAN);

	static CharTransition to_less_than_final_by_less_than(&less_than_final, '<');

	// Greater Than States and Transitions
	static FinalState greater_than_final(TokenType::GREATER_THAN);

	static CharTransition to_greater_than_final_by_greater_than(&greater_than_final, '>');

	// Whatever States and Transitions (Depends upon Equality States and Transitions)
	static FinalState whatever_final(TokenType::WHATEVER);

	static CharTransition to_whatever_final_by_equal(&whatever_final, '=');
	static State whatever_non_final(&to_whatever_final_by_equal);

	static CharTransition to_whatever_non_final_by_colon(&whatever_non_final, ':');

	// Equality States and Transitions (Serves as dependency for Whatever States and Transitions)
	static FinalState equality_final(TokenType::EQUALITY, &to_whatever_non_final_by_colon);

	static CharTransition to_equality_final_by_equal(&equality_final, '=');

	// Logical Not States and Transitions
	static FinalState logical_not_final(TokenType::NOT);

	static CharTransition to_logical_not_final_by_exclamation_mark(&logical_not_final, '!');

	// Semicolon States and Transitions
	static FinalState semicolon_final(TokenType::SEMICOLON);

	static CharTransition to_semicolon_final_by_semicolon(&semicolon_final, ';');

	// Parenthesis Open States and Transitions
	static FinalState parenthesis_open_final(TokenType::PARENTHESIS_OPEN);

	static CharTransition to_parenthesis_open_final_by_parenthesis_open(&parenthesis_open_final, '(');

	// Parenthesis Close States and Transitions
	static FinalState parenthesis_close_final(TokenType::PARENTHESIS_CLOSE);

	static CharTransition to_parenthesis_close_final_by_parenthesis_close(&parenthesis_close_final, ')');

	// Curly Bracket Open States and Transitions
	static FinalState curly_bracket_open_final(TokenType::CURLY_BRACKET_OPEN);

	static CharTransition to_curly_bracket_open_final_by_curly_bracket_open(&curly_bracket_open_final, '{');

	// Curly Bracket Close States and Transitions
	static FinalState curly_bracket_close_final(TokenType::CURLY_BRACKET_CLOSE);

	static CharTransition to_curly_bracket_close_final_by_curly_bracket_close(&curly_bracket_close_final, '}');

	// Square Bracket Open States and Transitions
	static FinalState square_bracket_open_final(TokenType::SQUARE_BRACKET_OPEN);

	static CharTransition to_square_bracket_open_final_by_square_bracket_open(&square_bracket_open_final, '[');

	// Square Bracket Close States and Transitions
	static FinalState square_bracket_close_final(TokenType::SQUARE_BRACKET_CLOSE);

	static CharTransition to_square_bracket_close_final_by_square_bracket_close(&square_bracket_close_final, ']');

	// Logical And States and Transitions
	static FinalState logical_and_final(TokenType::LOGICAL_AND);

	static CharTransition to_logical_and_final_by_and(&logical_and_final, '&');
	static State logical_and_non_final(&to_logical_and_final_by_and);

	static CharTransition to_logical_and_non_final_by_and(&logical_and_non_final, '&');

	// Linefeed States and Transitions
	static CallbackFinalState mac_line_feed_final(TokenType::LINE_FEED, line_count_callback);

	static CharTransition to_mac_line_feed_final_by_carriage_return(&mac_line_feed_final, '\r');
	mac_line_feed_final.add(&to_mac_line_feed_final_by_carriage_return);


	static CallbackFinalState unix_line_feed_final(TokenType::LINE_FEED, &to_mac_line_feed_final_by_carriage_return, line_count_callback);

	static CharTransition to_unix_line_feed_final_by_new_line(&unix_line_feed_final, '\n');
	unix_line_feed_final.add(&to_unix_line_feed_final_by_new_line);
	mac_line_feed_final.add(&to_unix_line_feed_final_by_new_line);



	static State start({
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

	return FiniteStateMachine(&start);
}

Scanner::Scanner(const String& file)
	: file_position()
	, line_count_callback(std::bind(&FilePosition::on_state_change, &this->file_position, std::placeholders::_1, std::placeholders::_2))
	, finite_state_machine(init_finite_state_machine(this->line_count_callback)), symboltable(), buffer(file), lexem(), token() {

	this->init_symboltable();
}

/*
 * Initializes the Symboltable with some keywords.
 */
void Scanner::init_symboltable() {
	this->symboltable.insert("while", TokenType::WHILE);
	this->symboltable.insert("WHILE", TokenType::WHILE);
	this->symboltable.insert("if", TokenType::IF);
	this->symboltable.insert("IF", TokenType::IF);
}

Token* Scanner::make_integer_token(const String& lexem) {
	errno = 0;
	long integer = strtol(lexem.c_str(), nullptr, 10);

	if (errno == ERANGE) {
		Information* information = this->symboltable.insert(lexem, TokenType::OUT_OF_RANGE_INTEGER);
		return &(this->token = Token(this->file_position.get_line(), this->file_position.get_column(), TokenType::OUT_OF_RANGE_INTEGER, information));
	}

	return &(this->token = Token(this->file_position.get_line(), this->file_position.get_column(), integer));
}
	
Token Scanner::next_token() {

	while (char current = this->buffer.get()) {
		this->lexem += current;

		if (!this->finite_state_machine.process(current)) {
			Token* token = nullptr;
			std::size_t steps_since_last_final_state = this->finite_state_machine.get_steps_since_last_final_state();

			if (steps_since_last_final_state == this->lexem.size()) {
				if (!is_space(this->lexem[0])) {
					token = &(this->token = Token(this->file_position.get_line(), this->file_position.get_column(), this->lexem[0]));
					for (std::size_t steps_back = steps_since_last_final_state; steps_back > 1; steps_back--) this->buffer.unget();
				}
				this->file_position.increment_column();
			}
			else {
				for (std::size_t steps_back = steps_since_last_final_state; steps_back > 0; steps_back--) this->buffer.unget();

				TokenType token_type = this->finite_state_machine.get_last_final_state().token();

				if (token_type != TokenType::LINE_FEED) {
					String actual_lexem(this->lexem.begin(), this->lexem.end() - steps_since_last_final_state);

					switch(token_type) {
					case TokenType::COMMENT: {
						long last_line_length = std::distance(actual_lexem.find_last_of("\r\n"), actual_lexem.cend()) - 1;
						this->file_position.increment_column(last_line_length == -1 ? actual_lexem.size() : last_line_length);
						break; }
					case TokenType::INTEGER: {
						token = this->make_integer_token(actual_lexem);
						this->file_position.increment_column(actual_lexem.size());
						break; }
					case TokenType::IDENTIFIER: {
						Information* information = this->symboltable.insert(actual_lexem);
						token = &(this->token = Token(this->file_position.get_line(), this->file_position.get_column(), information->type, information));
						this->file_position.increment_column(actual_lexem.size());
						break; }
					default: {
						token = &(this->token = Token(this->file_position.get_line(), this->file_position.get_column(), token_type));
						this->file_position.increment_column(actual_lexem.size()); }
					}

				}
			}

			this->lexem.clear();
			this->finite_state_machine.reset();

			if (token) {
				return *token;
			}
		}
	}

	throw TokenGeneratingException("Scanner::next_token()");
}

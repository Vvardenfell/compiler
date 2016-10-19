/*
 * Automaton.cpp
 *
 */

#include "../../Utility/exception/exception.h"
#include <cctype>
#include "../includes/Automaton.h"

Automaton::Automaton() {
	State start_state;
	State end_state;
	State current_state;
	char symbol;

	int transition(int currentState);
}

const State& CharTransition::process(char symbol) const {
	if (this->symbol == symbol) return get_next_state();
	throw TransitionCharacterProcessingException("Given symbol does not match required symbol.");
}

const State& TypeTransition::process(char symbol) const {
	switch(this->type) {
	case CharClass::ALPHA: {
		if (std::isalpha(symbol)) return get_next_state();
		break;
	}
	case CharClass::ALPHA_NUMERIC: {
		if (std::isalnum(symbol)) return get_next_state();
		break;
	}
	case CharClass::NUMERIC: {
		if (std::isdigit(symbol)) return get_next_state();
		break;
	}
	default: throw UncoveredCharClassException(std::string("Switch-statement does not handle given CharClass value: ") + static_cast<int>(this->type));
	}

	throw TransitionCharacterProcessingException("Given symbol does not match required CharClass.");
}

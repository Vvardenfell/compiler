#include "../../Utility/exception/exception.h"
#include "../includes/Automaton.h"
#include "../../Utility/character_classification.h"
#include <limits>



FiniteStateMachine::FiniteStateMachine(const State& start, const std::initializer_list<State>& states) {

    if (states.size() > numeric_limits<unsigned char>::max) throw 42; // TODO: throw proper exception if the amount of states can not be represented by an unsigned char

    this->create_branch_table();

    unordered_map<State, unsigned char> state_map;

    for (auto& state : states) {
        unsigned char col = this->map_state_to_id(state_map, state);

        for (auto& transition : state.get_transitions()) {
            if (CharTransition *tmp = dynamic_cast<CharTransition*>(transition)) {
                unsigned char row = reinterpret_cast<unsigned char>(tmp->get_symbol());
                if (row > FiniteStateMachine::SUPPORTED_ENCODING_MAX_VALUE) throw 21; // TODO: throw proper exception, if the requested character is not in ASCII encoding
                this->branch_table_entry(col, row, this->map_state_to_id(state_map, tmp->get_next_state());
            }
            else if (TypeTransition *tmp = dynamic_cast<TypeTransition*>(transition)) {
                this->branch_table_entry(col, *tmp, this->map_state_to_id(state_map, tmp->get_next_state()));
            }
            else throw 56; // TODO: throw proper exception, if the given Transition type is not supported by this constructor
        }
    }
}

unsigned char FiniteStateMachine::next_free_state_id() {
    if (numeric_limits<unsigned char>::max == this->state_id) throw 42; // TODO: throw proper exception if the amount of states can not be represented by an unsigned char
    return this->state_id++;
}

unsigned char FiniteStateMachine::map_state_to_id(unordered_map<State, unsigned char> state_map, const State& state) {
    const auto& state_id = state_map.find(state);

    if (state_id == state_map.end()) return state_map[state] = this->next_free_state_id();
    return *state_id;
}

const State& CharTransition::process(char symbol) const {
	if (this->symbol == symbol) return get_next_state();
	throw TransitionCharacterProcessingException("Given symbol does not match required symbol.");
}

const State& TypeTransition::process(char symbol) const {
	switch(this->type) {
	case CharClass::ALPHA: {
		if (is_alpha(symbol)) return get_next_state();
		break;
	}
	case CharClass::ALPHA_NUMERIC: {
		if (is_alnum(symbol)) return get_next_state();
		break;
	}
	case CharClass::NUMERIC: {
		if (is_digit(symbol)) return get_next_state();
		break;
	}
	default: throw UncoveredCharClassException(std::string("Switch-statement does not handle given CharClass value: ") + static_cast<int>(this->type));
	}

	throw TransitionCharacterProcessingException("Given symbol does not match required CharClass.");
}


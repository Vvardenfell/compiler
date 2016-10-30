#include "../../Utility/exception/exception.h"
#include "../includes/Automaton.h"
//#include "../../Utility/character_classification.h"
#include <limits>



FiniteStateMachine::FiniteStateMachine(const State& start, const std::initializer_list<State> states)
    : current_state(FiniteStateMachine::CRASH_STATE_ID), branch_table(states.size(), SUPPORTED_ENCODING_MAX_VALUE, CRASH_STATE_ID), generated_tokens(states.size()) {

    if (states.size() > FiniteStateMachine::STATE_TYPE_MAX) throw 42; // TODO: throw proper exception if the amount of states can not be represented by a state_type

    this->generated_tokens.push_back(TokenType::DEADBEEF);
    unordered_map<State, state_type> state_map;

    this->init_branch_table(state_map, states);

    current_state = this->map_state_to_id(state_map, start);
}

FiniteStateMachine::state_type FiniteStateMachine::next_free_state_id() {
    if (FiniteStateMachine::STATE_TYPE_MAX == this->state_id) throw 42; // TODO: throw proper exception if the amount of states can not be represented by a state_type
    return this->state_id++;
}

FiniteStateMachine::state_type FiniteStateMachine::map_state_to_id(unordered_map<State, state_type>& state_map, const State& state) {
    const auto state_id = state_map.find(state);

    if (state_id == state_map.end()) return state_map[state] = this->next_free_state_id();
    return *state_id;
}

void FiniteStateMachine::init_branch_table(unordered_map<State, state_type>& state_map, const std::initializer_list<State> states) {
    for (auto& state : states) {
        state_type state_id = this->map_state_to_id(state_map, state);
        if (this->is_latest_state(state_id)) this->generated_tokens.push_back(state.token());

        for (const Transition& transition : state.get_transitions()) {
            this->branch_table_entry(state_map, state_id, transition);
        }
    }
}

void FiniteStateMachine::branch_table_entry(unordered_map<State, state_type>& state_map, state_type state_id, const Transition& transition) {
    state_type next_state = this->map_state_to_id(state_map, *transition->get_next_state());

    if (CharTransition *char_transition = dynamic_cast<CharTransition*>(transition)) {
        state_type symbol = reinterpret_cast<state_type>(char_transition->get_symbol());
        this->branch_table_entry(state_id, symbol, next_state);
    }
    else if (TypeTransition *type_transition = dynamic_cast<TypeTransition*>(transition)) {
        this->branch_table_entry(state_id, *type_transition, next_state);
    }
    else throw 56; // TODO: throw proper exception, if the given Transition type is not supported by this constructor
}

void FiniteStateMachine::branch_table_entry(state_type col, state_type row, state_type next_state) {
    check_encoding(row);
    this->branch_table.set(col, row, next_state);
}

void FiniteStateMachine::branch_table_entry(state_type col, const TypeTransition& transition, state_type next_state) {
    for (auto value : transition.values()) {
        value = reinterpret_cast<state_type>(value);
        check_encoding(value);
        this->branch_table.set(col, value, next_state);
    }
}

bool FiniteStateMachine::process(char symbol, TokenType& type) {
    state_type transformed_symbol = reinterpret_cast<state_type>(symbol);

    check_encoding(transformed_symbol);
    current_state = branch_table.get(current_state, transformed_symbol);
    type = generated_tokens[current_state];

    return current_state != FiniteStateMachine::CRASH_STATE_ID;
}

const State* CharTransition::process(char symbol) const {
	if (this->symbol == symbol) return get_next_state();
	throw TransitionCharacterProcessingException("Given symbol does not match required symbol.");
}

const State* TypeTransition::process(char symbol) const {
	switch(this->get_type()) {
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
	default: throw UncoveredCharClassException(std::string("Switch-statement does not handle given CharClass value: ") + static_cast<int>(this->get_type()));
	}

	throw TransitionCharacterProcessingException("Given symbol does not match required CharClass.");
}

const Vector<char>& TypeTransition::values() const {
    switch(this->get_type()) {
    case CharClass::NUMERIC: return this->NUMERIC_VALUES;
    case CharClass::ALPHA: return this->ALPHA_VALUES;
    case CharClass::ALPHA_NUMERIC: return this->ALPHA_NUMERIC_VALUES;
    default: throw UncoveredCharClassException(std::string("Switch-statement does not handle given CharClass value: ") + static_cast<int>(this->get_type()));
    }
}

const TypeTransition::NUMERIC_VALUES{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
const TypeTransition::ALPHA_VALUES{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
                                 , 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
const TypeTransition::ALPHA_NUMERIC_VALUES{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
                                 , 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
                                 , 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};


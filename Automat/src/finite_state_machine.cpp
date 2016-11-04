#include "../../Utility/exception/exception.h"
#include "../includes/finite_state_machine.h"
//#include "../../Utility/character_classification.h"
#include <limits>


/**
* FiniteStateMachine MUST NOT outlive the State* passed to it => TODO: implement shared_ptr
**/
FiniteStateMachine::FiniteStateMachine(const State* start)
    : states(FiniteStateMachine::gather_states(start))
    , current_state(FiniteStateMachine::START_STATE_ID)
    , branch_table(states.size(), FiniteStateMachine::SUPPORTED_ENCODING_MAX_VALUE, FiniteStateMachine::CRASH_STATE_ID)
    , steps_since_last_final_state(0)
    , last_final_state_index(FiniteStateMachine::CRASH_STATE_ID) {

    if (states.size() - 1 > FiniteStateMachine::STATE_TYPE_MAX) throw 42; // TODO: throw proper exception if the amount of states can not be represented by a state_type

    this->init_branch_table();
}

const Vector<const State*> FiniteStateMachine::gather_states(const State* start) {
    Vector<const State*> states;
    states.push_back(&FiniteStateMachine::CRASH_STATE);
    states.push_back(start);

    for (Vector<const State*>::iterator iterator = states.begin(); iterator != states.end(); iterator++) {
        const Vector<Transition>& transitions = (*iterator)->get_transitions();
        for (Vector<Transition>::const_iterator transition_iterator = transitions.cbegin(); transition_iterator != transitions.cend(); transition_iterator++) {
            const State* next_state = transition_iterator->get_next_state();
            if (!states.contains(next_state)) states.push_back(next_state);
        }
    }

    return states;
}

void FiniteStateMachine::init_branch_table() {
    Vector<const State*>::const_iterator begin = this->states.cbegin(), end = this->states.cend();

    for (Vector<const State*>::const_iterator iterator = begin; iterator != end; iterator++) {
        std::size_t current_state_index = std::distance(begin, iterator);
        const Vector<Transition>& transitions = (*iterator)->get_transitions();

        for (Vector<Transition>::const_iterator transition_iterator = transitions.cbegin(); transition_iterator != transitions.cend(); transition_iterator++) {
            const State* next_state = transition_iterator->get_next_state();
            Vector<const State*>::const_iterator element = this->states.find(next_state);

            if (element == end) throw 12; // TODO: throw an exception that something went horribly wrong, because the searched state must be in the vector or we have a bug

            std::size_t next_state_index = std::distance(begin, element);
            this->branch_table_entry(&*transition_iterator, static_cast<state_type>(current_state_index), static_cast<state_type>(next_state_index));
        }
    }
}

void FiniteStateMachine::branch_table_entry(const Transition *transition, state_type current_state_index, state_type next_state_index) {
    if (const CharTransition *char_transition = dynamic_cast<const CharTransition*>(transition)) {
        state_type symbol = static_cast<state_type>(char_transition->get_symbol());
        this->branch_table_entry(symbol, current_state_index, next_state_index);
    }
    else if (const TypeTransition *type_transition = dynamic_cast<const TypeTransition*>(transition)) {
        this->branch_table_entry(*type_transition, current_state_index, next_state_index);
    }
    else throw 56; // TODO: throw proper exception, if the given Transition type is not supported
}

void FiniteStateMachine::branch_table_entry(state_type symbol, state_type current_state_index, state_type next_state_index) {
    this->check_encoding(symbol);
    this->branch_table.set(current_state_index, symbol, next_state_index);
}

void FiniteStateMachine::branch_table_entry(const TypeTransition& transition, state_type current_state_index, state_type next_state_index) {
    const Vector<char> values = transition.values();
    for (Vector<char>::const_iterator iterator = values.cbegin(); iterator != values.cend(); iterator++) {
        state_type symbol = static_cast<state_type>(*iterator);
        this->check_encoding(symbol);
        this->branch_table.set(current_state_index, symbol, next_state_index);
    }
}

bool FiniteStateMachine::process(char symbol) {
    state_type transformed_symbol = static_cast<state_type>(symbol);

    this->check_encoding(transformed_symbol);
    this->current_state = this->branch_table.get(current_state, transformed_symbol);

    if (dynamic_cast<const FinalState*>(this->states[this->current_state])) {
        this->last_final_state_index = this->current_state;
        this->steps_since_last_final_state = 0;
    }
    else this->steps_since_last_final_state++;

    return this->current_state != FiniteStateMachine::CRASH_STATE_ID;
}

std::size_t FiniteStateMachine::get_steps_since_last_final_state() const {
    return this->steps_since_last_final_state;
}

const FinalState& FiniteStateMachine::get_last_final_state() const {
    return static_cast<const FinalState&>(*(this->states[this->last_final_state_index]));
}

void FiniteStateMachine::reset() {
    this->current_state = FiniteStateMachine::START_STATE_ID;
    this->steps_since_last_final_state = 0;
    this->last_final_state_index = FiniteStateMachine::CRASH_STATE_ID;
}

const FinalState FiniteStateMachine::CRASH_STATE = FinalState(TokenType::DEADBEEF);

const State* CharTransition::process(char symbol) const {
	if (this->symbol == symbol) return get_next_state();
	throw TransitionCharacterProcessingException("Given symbol does not match required symbol.");
}

const State* TypeTransition::process(char symbol) const {
	switch(this->get_type()) {
	case CharClass::ALPHA: {
		if (is_alpha(symbol)) return this->get_next_state();
		break;
	}
	case CharClass::ALPHA_NUMERIC: {
		if (is_alnum(symbol)) return this->get_next_state();
		break;
	}
	case CharClass::NUMERIC: {
		if (is_digit(symbol)) return this->get_next_state();
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


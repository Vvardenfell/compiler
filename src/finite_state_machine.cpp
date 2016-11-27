#include "exception.h"
#include "finite_state_machine.h"
#include "character_classification.h"
#include <limits>


/**
* FiniteStateMachine MUST NOT outlive the State* passed to it => TODO: implement shared_ptr
**/
FiniteStateMachine::FiniteStateMachine(const State* start)
    : states(FiniteStateMachine::gather_states(start))
    , current_state(FiniteStateMachine::START_STATE_ID)
    , branch_matrix(states.size(), FiniteStateMachine::SUPPORTED_ENCODING_MAX_VALUE, FiniteStateMachine::CRASH_STATE_ID)
    , steps_since_last_final_state(0)
    , last_final_state_index(FiniteStateMachine::CRASH_STATE_ID) {

    if (this->states.size() - 1 > FiniteStateMachine::STATE_TYPE_MAX) throw TooManyMachineStatesException("FiniteStateMachine::FiniteStateMachine(const State*)", FiniteStateMachine::STATE_TYPE_MAX, this->states.size() - 1);

    this->init_branch_matrix();
}

const Vector<const State*> FiniteStateMachine::gather_states(const State* start) {
    Vector<const State*> states;
    states.push_back(&FiniteStateMachine::CRASH_STATE);
    states.push_back(start);

    for (std::size_t state_index = 0; state_index != states.size(); state_index++) {
        const Vector<const Transition*>& transitions = states[state_index]->get_transitions();
        for (Vector<const Transition*>::const_iterator transition_iterator = transitions.cbegin(); transition_iterator != transitions.cend(); transition_iterator++) {
            const State* next_state = (*transition_iterator)->get_next_state();
            if (!states.contains(next_state)) states.push_back(next_state);
        }
    }

    return states;
}

void FiniteStateMachine::init_branch_matrix() {
    Vector<const State*>::const_iterator begin = this->states.cbegin(), end = this->states.cend();

    for (Vector<const State*>::const_iterator iterator = begin; iterator != end; iterator++) {
        std::size_t current_state_index = std::distance(begin, iterator);
        const Vector<const Transition*>& transitions = (*iterator)->get_transitions();

        for (Vector<const Transition*>::const_iterator transition_iterator = transitions.cbegin(); transition_iterator != transitions.cend(); transition_iterator++) {
            const State* next_state = (*transition_iterator)->get_next_state();
            Vector<const State*>::const_iterator element = this->states.find(next_state);

            if (element == end) throw FatalException("FiniteStateMachine::init_branch_matrix()", "There's a bug! An element which must be in the states Vector was not found");

            std::size_t next_state_index = std::distance(begin, element);
            this->branch_matrix_entry(*transition_iterator, static_cast<state_type>(current_state_index), static_cast<state_type>(next_state_index));
        }
    }
}

void FiniteStateMachine::branch_matrix_entry(const Transition *transition, state_type current_state_index, state_type next_state_index) {
    switch(transition->type()) {
    case TransitionType::CHAR: {
        const CharTransition *char_transition = static_cast<const CharTransition*>(transition);
        FiniteStateMachine::state_type symbol = static_cast<FiniteStateMachine::state_type>(char_transition->get_symbol());
        this->branch_matrix_entry(symbol, current_state_index, next_state_index);
	break;
    }
    case TransitionType::TYPE: {
        const TypeTransition *type_transition = static_cast<const TypeTransition*>(transition);
        this->branch_matrix_entry(*type_transition, current_state_index, next_state_index);
	break;
    }
    default: throw UnsupportedTransitionException("FiniteStateMachine::branch_matrix_entry(const Transition*, state_type, state_type)", transition->type());
    }
}

void FiniteStateMachine::branch_matrix_entry(state_type symbol, state_type current_state_index, state_type next_state_index) {
    this->check_encoding(symbol);

    //state_type stored_next_state_index = this->branch_matrix.get(current_state_index, symbol);
    //if (stored_next_state_index != FiniteStateMachine::CRASH_STATE_ID && stored_next_state_index != next_state_index) throw 1; // TODO: throw proper exception, if the given graph of States and Transitions results in a non-deterministic machine

    this->branch_matrix.set(current_state_index, symbol, next_state_index);
}

void FiniteStateMachine::branch_matrix_entry(const TypeTransition& transition, state_type current_state_index, state_type next_state_index) {
    const Vector<char>& values = transition.values();
    for (Vector<char>::const_iterator iterator = values.cbegin(); iterator != values.cend(); iterator++) {
        state_type symbol = static_cast<state_type>(*iterator);
	this->branch_matrix_entry(symbol, current_state_index, next_state_index);
    }
}

bool FiniteStateMachine::is_final_state(const State* state) const {
    switch(this->states[this->current_state]->type()) {
    case StateType::FINAL:
    case StateType::CALLBACK_FINAL:
        return true;
    case StateType::REGULAR:
        return false;
    }

    return false; // TODO:: throw exception if StateType is not supported
}

void FiniteStateMachine::trigger_callback_state_handler(const State* state, Direction direction, char symbol) const {
    if (state->type() == StateType::CALLBACK_FINAL) {
        const CallbackFinalState *callback_state = static_cast<const CallbackFinalState*>(state);
	(*callback_state)(direction, symbol);
    }
}

bool FiniteStateMachine::process(char symbol) {
    state_type transformed_symbol = static_cast<state_type>(symbol);
    this->check_encoding(transformed_symbol);

    const State *state = this->states[this->current_state];
    this->trigger_callback_state_handler(state, Direction::EXIT, symbol);

    this->current_state = this->branch_matrix.get(current_state, transformed_symbol);

    this->trigger_callback_state_handler(this->states[this->current_state], Direction::ENTER, symbol);

    if (this->current_state != FiniteStateMachine::CRASH_STATE_ID && is_final_state(state)) {
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
const FiniteStateMachine::state_type FiniteStateMachine::CRASH_STATE_ID = 0;
const FiniteStateMachine::state_type FiniteStateMachine::START_STATE_ID = 1;
const unsigned char FiniteStateMachine::SUPPORTED_ENCODING_MAX_VALUE = 127;
const std::size_t FiniteStateMachine::STATE_TYPE_MAX = std::numeric_limits<state_type>::max();

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
	case CharClass::ANY: {
		return this->get_next_state();
	}
	default: throw UncoveredCharClassException(std::string("Switch-statement does not handle given CharClass value: ") + std::to_string(static_cast<int>(this->get_type())));
	}

	throw TransitionCharacterProcessingException("Given symbol does not match required CharClass.");
}

const Vector<char>& TypeTransition::values() const {
    switch(this->get_type()) {
    case CharClass::NUMERIC: return this->NUMERIC_VALUES;
    case CharClass::ALPHA: return this->ALPHA_VALUES;
    case CharClass::ALPHA_NUMERIC: return this->ALPHA_NUMERIC_VALUES;
    case CharClass::ANY: return this->ANY_VALUES;
    default: throw UncoveredCharClassException(std::string("Switch-statement does not handle given CharClass value: ") + std::to_string(static_cast<int>(this->get_type())));
    }
}

const Vector<char> TypeTransition::NUMERIC_VALUES{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
const Vector<char> TypeTransition::ALPHA_VALUES{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
					      , 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
const Vector<char> TypeTransition::ALPHA_NUMERIC_VALUES{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
						      , 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
						      , 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
const Vector<char> TypeTransition::ANY_VALUES{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
				     , 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
				     , ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/'
				     , '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
				     , ':', ';', '<', '=', '>', '?', '@'
				     , 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T'
				     , 'U', 'V', 'W', 'X', 'Y', 'Z'
				     , '[', '\\', ']', '^', '_', '`'
				     , 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't'
				     , 'u', 'v', 'w', 'x', 'y', 'z'
				     , '{', '|', '}', '~', 0x7f
};


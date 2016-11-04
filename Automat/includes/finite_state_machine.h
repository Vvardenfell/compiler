#ifndef FINITE_STATE_MACHINE_H
#define FINITE_STATE_MACHINE_H

#include <limits>
#include <iterator>
#include "../../Utility/container/vector.h"
#include "../../Utility/token.h"

class Transition;

class State {
private:
    const Vector<Transition> transitions;

public:
    State(const Vector<Transition>& transitions) : transitions(transitions) {}
    virtual ~State() = default;

    const Vector<Transition>& get_transitions() const { return this->transitions; }
};


class FinalState : public State {
private:
    TokenType generate_token;

public:
    FinalState(TokenType generate_token, const Vector<Transition>& transitions = Vector<Transition>::EMPTY) : State(transitions), generate_token(generate_token) {};

    TokenType token() const { return this->generate_token; }
};

/**

* Important: Transition MUST NOT outlive the State* passed to it. => TODO: implement shared_ptr

**/
class Transition {
private:
	const State *next_state;

public:
	Transition(const State *next_state) : next_state(next_state) {}

	const State* get_next_state() const { return this->next_state; }
    virtual const State* process(char symbol) const = 0;
};


class CharTransition : public Transition {
private:
	char symbol;

public:
	CharTransition(const State *next_state, char symbol) : Transition(next_state), symbol(symbol) {}
	char get_symbol() const { return this->symbol; }
	const State* process(char symbol) const override;
};


enum class CharClass : char {
	NUMERIC,
	ALPHA,
	ALPHA_NUMERIC
};


class TypeTransition : public Transition {
private:
    const static Vector<char> NUMERIC_VALUES, ALPHA_VALUES, ALPHA_NUMERIC_VALUES;

	CharClass type;

public:
	TypeTransition(const State *next_state, CharClass type) : Transition(next_state), type(type) {}
	CharClass get_type() const { return this->type; }
	const State* process(char symbol) const override;
    const Vector<char>& values() const;
};


template<typename T> class BranchMatrix {
private:
	Vector<T> values;
	const std::size_t dimension_y;

public:
	BranchMatrix(const std::size_t dimension_x, const std::size_t dimension_y, const T& default_value = T())
		: values(dimension_x * dimension_y, default_value), dimension_y(dimension_y) {}

	void set(const std::size_t dimension_x, const std::size_t dimension_y, const T& object) {
		this->values[dimension_x * this->dimension_y + dimension_y] = object;
	}

	const T& get(const std::size_t dimension_x, const std::size_t dimension_y) const {
		return this->values[dimension_x * this->dimension_y + dimension_y];
	}

};


class FiniteStateMachine {
private:
    typedef unsigned char state_type;

    const static state_type CRASH_STATE_ID = 0, START_STATE_ID = 1;
    const static FinalState CRASH_STATE;
    const static unsigned char SUPPORTED_ENCODING_MAX_VALUE = 127;
    const static std::size_t STATE_TYPE_MAX = std::numeric_limits<state_type>::max();

    Vector<const State*> states;
    state_type current_state;
    BranchMatrix<state_type> branch_table;
    std::size_t steps_since_last_final_state;
    state_type last_final_state_index;

    static const Vector<const State*> gather_states(const State* start);
    void init_branch_table();
    void branch_table_entry(const Transition* transition, state_type current_state_index, state_type next_state_index);
    void branch_table_entry(state_type symbol, state_type current_state_index, state_type next_state_index);
    void branch_table_entry(const TypeTransition& transition, state_type current_state_index, state_type next_state_index);

    void check_encoding(state_type value) {
        if (value > FiniteStateMachine::SUPPORTED_ENCODING_MAX_VALUE) throw 21; // TODO: throw proper exception, if the requested character is not in ASCII encoding
    }

public:
    FiniteStateMachine(const State* start);
    bool process(char symbol);
    std::size_t get_steps_since_last_final_state() const;
    const FinalState& get_last_final_state() const;
    void reset();
};

#endif /* Automaton_H_ */

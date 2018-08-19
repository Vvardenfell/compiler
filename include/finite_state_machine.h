#ifndef FINITE_STATE_MACHINE_H
#define FINITE_STATE_MACHINE_H

#include <limits>
#include <iterator>
#include <functional>
#include "vector.h"
#include "token.h"
#include "exception.h"


enum class StateType {
	REGULAR,
	FINAL,
	CALLBACK_FINAL
};

class Transition;

/**
* State MUST NOT outlive the Transitions* passed to it! => TODO: implement shared_ptr
**/
class State {
private:
	Vector<const Transition*> transitions;

public:
	explicit State(const Vector<const Transition*>& transitions) : transitions(transitions) {}
	explicit State(const Transition *transition) : transitions(1, transition) {}
	explicit State(std::initializer_list<const Transition*> transitions) : transitions(transitions.begin(), transitions.end()) {}
	virtual ~State() = default;

	const Vector<const Transition*>& get_transitions() const { return this->transitions; }

	void add(const Vector<const Transition*>& transitions) {
		this->transitions.insert(this->transitions.end(), transitions.cbegin(), transitions.cend());
	}

	void add(const Transition *transition) {
		this->transitions.push_back(transition);
	}

	virtual StateType type() const {
		return StateType::REGULAR;
	}

	/**
	* TODO: implement destructor that checks transitions for Transitions that refer to the State being destructed and manually
	* remove ownership of this State from the shared_ptr, so it doesn't get destructed twice
	**/
};


class FinalState : public State {
private:
	TokenType generate_token;

public:
	explicit FinalState(TokenType generate_token, const Vector<const Transition*>& transitions = Vector<const Transition*>::EMPTY)
		: State(transitions), generate_token(generate_token) {};

	FinalState(TokenType generate_token, const Transition *transition) : State(transition), generate_token(generate_token) {}

	TokenType token() const { return this->generate_token; }

	StateType type() const override {
		return StateType::FINAL;
	}
};

enum class Direction {
	ENTER,
	EXIT
};

class CallbackFinalState : public FinalState {
private:
	std::function<void(Direction, char)> callback;
public:
	CallbackFinalState(TokenType generate_token, const Transition *transition, std::function<void(Direction, char)> callback)
		: FinalState(generate_token, transition), callback(callback) {}

	CallbackFinalState(TokenType generate_token, std::function<void(Direction, char)> callback) : FinalState(generate_token), callback(callback) {}

	StateType type() const override {
		return StateType::CALLBACK_FINAL;
	}

	void operator()(Direction direction, char symbol) const {
		this->callback(direction, symbol);
	}
};

enum class TransitionType : unsigned char {
	CHAR,
	TYPE
};

/**

* Important: Transition MUST NOT outlive the State* passed to it. => TODO: implement shared_ptr

**/
class Transition {
private:
	const State *next_state;

protected:
	Transition(const State *next_state) : next_state(next_state) {}

public:
	const State* get_next_state() const { return this->next_state; }
	virtual const State* process(char symbol) const = 0;
	virtual TransitionType type() const = 0;
};


class CharTransition : public Transition {
private:
	char symbol;

public:
	CharTransition(const State *next_state, char symbol) : Transition(next_state), symbol(symbol) {}
	char get_symbol() const { return this->symbol; }
	const State* process(char symbol) const override;
	TransitionType type() const override { return TransitionType::CHAR; }
};


enum class CharClass : char {
	NUMERIC,
	ALPHA,
	ALPHA_NUMERIC,
	ANY
};


class TypeTransition : public Transition {
private:
	const static Vector<char> NUMERIC_VALUES, ALPHA_VALUES, ALPHA_NUMERIC_VALUES, ANY_VALUES;

	CharClass char_class;

public:
	TypeTransition(const State *next_state, CharClass type) : Transition(next_state), char_class(type) {}
	CharClass get_type() const { return this->char_class; }
	const State* process(char symbol) const override;
	const Vector<char>& values() const;
	TransitionType type() const override { return TransitionType::TYPE; }
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
public:
	typedef unsigned char state_type;

private:
	const static state_type CRASH_STATE_ID, START_STATE_ID;
	const static FinalState CRASH_STATE;
	const static unsigned char SUPPORTED_ENCODING_MAX_VALUE;
	const static std::size_t STATE_TYPE_MAX;

	Vector<const State*> states;
	state_type current_state;
	BranchMatrix<state_type> branch_matrix;
	std::size_t steps_since_last_final_state;
	state_type last_final_state_index;

	static const Vector<const State*> gather_states(const State* start);
	void init_branch_matrix();
	void branch_matrix_entry(const Transition* transition, state_type current_state_index, state_type next_state_index);
	void branch_matrix_entry(state_type symbol, state_type current_state_index, state_type next_state_index);
	void branch_matrix_entry(const TypeTransition& transition, state_type current_state_index, state_type next_state_index);
	bool is_final_state(const State* state) const;
	void trigger_callback_state_handler(const State* state, Direction direction, char symbol) const;

	void check_encoding(state_type value) {
		if (value > FiniteStateMachine::SUPPORTED_ENCODING_MAX_VALUE) throw UnsupportedCharacterEncodingException("FiniteStateMachine::check_encoding(state_type)", "ASCII", value);
	}

public:
	FiniteStateMachine(const State* start);
	bool process(char symbol);
	std::size_t get_steps_since_last_final_state() const;
	const FinalState& get_last_final_state() const;
	void reset();
};

#endif /* Automaton_H_ */

#ifndef FINITE_STATE_MACHINE_H
#define FINITE_STATE_MACHINE_H

template<typename T> class BranchTable {
private:
	T* const objects;
	const size_t dimension_y;

public:
	BranchTable(const size_t dimension_x, const size_t dimension_y, const T& default_value = T())
		: objects(new T[dimension_x * dimension_y](default_value)), dimension_x(dimension_x), dimension_y(dimension_y) {}

	void set(const size_t dimension_x, const size_t dimension_y, const T& object) {
		objects[dimension_x * this->dimension_y + dimension_y] = object;
	}

	const T& get(const size_t dimension_x, const size_t dimension_y) const {
		return objects[dimension_x * this->dimension_y + dimension_y];
	}
};

class FiniteStateMachine {
public:
    FiniteStateMachine(const State& start, const std::initializer_list<State>& states);
};


class State {
private:
    const static unsigned char CRASH_STATE_ID = 0;

    const Vector<Transition> transitions;
    unsigned char state_id = 1;

    unsigned char next_free_state_id();
    static unsigned char map_state_to_id(unordered_map<State, unsigned char> state_map, const State& state);

public:
    State(const Vector<Transition>& transitions) : transitions(transitions) {}
    State(Vector<Transition>&& transitions) : transitions(std::forward(transitions))) {}
    virtual ~State() = default;

    const Vector<Transition>& get_transitions() { return this->transitions; }
};


class FinalState : public State {
private:
    TokenType generate_token;

public:
    FinalState(TokenType generate_token, const Vector<Transition>& transitions = Vector<Transition>::EMPTY) : State(transitions), generate_token(generate_token) {};
    FinalState(TokenType generate_token, Vector<Transition>&& transitions) : State(transitions), generate_token(generate_token) {}

    TokenType token() const { return this->generate_token; }
};


class Transition {
private:
	State next_state;

public:
	Transition(const State& next_state) : next_state(next_state) {}
	Transition(State&& next_state) : next_state(std::forward(next_state)) {}

	const State& get_next_state() const { return this->next_state; }
    virtual const State& process(char symbol) const = 0;
};


class CharTransition : public Transition {
private:
    const unsigned char SUPPORTED_ENCODING_MAX_VALUE = 127;
	char symbol;

public:
	CharTransition(char symbol) : symbol(symbol) {}
	char get_symbol() { return this->symbol; }
	const State& process(char symbol) const override;
};


enum class CharClass : char {
	NUMERIC,
	ALPHA,
	ALPHA_NUMERIC
};


class TypeTransition : public Transition {
private:
	CharClass type;

public:
	TypeTransition(CharClass type) : type(type) {}
	CharClass get_type() { return this->type; }
	const State& process(char symbol) const override;
};

#endif /* Automaton_H_ */

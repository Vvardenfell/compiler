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
    TokenType generate_token;

protected:
    State(const Vector<Transition>& transitions, TokenType generate_token) : transitions(transitions), generate_token(generate_token) {}
    State(Vector<Transition>&& transitions, TokenType generate_token) : transitions(transitions), generate_token(generate_token) {}

public:
    State(const Vector<Transition>& transitions) : State(transitions, TokenType::DEADBEEF) {}
    virtual ~State() = default;

    TokenType token() const { return this->generate_token; }
    const Vector<Transition>& get_transitions() { return this->transitions; }
};


class FinalState : public State {
public:
    FinalState(TokenType generate_token, const Vector<Transition>& transitions = Vector<Transition>::EMPTY) : State(transitions, generate_token) {};
    FinalState(TokenType generate_token, Vector<Transition>&& transitions) : State(transitions, generate_token) {}
};

/**

* Important: Transition MUST NOT outlive the State* passed to it.

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
	char get_symbol() { return this->symbol; }
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
	CharClass get_type() { return this->type; }
	const State* process(char symbol) const override;
    const Vector<char>& values() const;
};


template<typename T> class BranchTable {
private:
	T* const objects;
	const size_t size, dimension_y;

	template<typename U = T> typename std::enable_if<std::is_pod<U>::value>::type destruct() {} // TODO: use std::is_trivially_copyable<T>::value in g++ 5 instead
	template<typename U = T> typename std::enable_if<!std::is_pod<U>::value>::type destruct() { // TODO: use std::is_trivially_copyable<T>::value in g++ 5 instead
		reverse_iterator end = this->rend();
		for (reverse_iterator iterator = this->rbegin(); iterator < end; ++iterator) {
			try {
				iterator->~U();
			}
			catch(...) {}
		}

		::operator delete(this->objects);
	}

public:
	typedef T* iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;

	BranchTable(const size_t dimension_x, const size_t dimension_y, const T& default_value = T())
		: objects(static_cast<T*>(::operator new(dimension_x * dimension_y))), size(dimension_x * dimension_y), dimension_y(dimension_y) {
		const iterator end = this->end();
		for (iterator current = this->begin(); current < end; current++) {
			new (static_cast<void*>(&*current)) T(default_value);
		}
	}

	iterator begin() {
		return this->objects;
	}

	iterator end() {
		return this->objects + this->size;
	}

	reverse_iterator rbegin() {
		return std::reverse_iterator<iterator>(end());
	}

	reverse_iterator rend() {
		return std::reverse_iterator<iterator>(begin());
	}

	void set(const size_t dimension_x, const size_t dimension_y, const T& object) {
		this->objects[dimension_x * this->dimension_y + dimension_y] = object;
	}

	const T& get(const size_t dimension_x, const size_t dimension_y) const {
		return this->objects[dimension_x * this->dimension_y + dimension_y];
	}

	~BranchTable() {
		this->destruct();
	}
};


class FiniteStateMachine {
private:
    typedef unsigned char state_type;

    const static state_type CRASH_STATE_ID = 0;
    const static unsigned char SUPPORTED_ENCODING_MAX_VALUE = 127;
    const static size_t STATE_TYPE_MAX = std::numeric_limits<state_type>::max();

    state_type state_id = 1, current_state;
    BranchTable<state_type> branch_table;
    Vector<TokenType> generated_tokens;

    state_type map_state_to_id(unordered_map<State, state_type>& state_map, const State& state);
    state_type next_free_state_id();
    void init_branch_table(unordered_map<State, state_type>& state_map, const std::initializer_list<State> states);
    void branch_table_entry(unordered_map<State, state_type>& state_map, state_type state_id, const Transition& transition);
    void branch_table_entry(state_type col, state_type row, state_type next_state);
    void branch_table_entry(state_type col, const TypeTransition& transition, state_type next_state);

    void check_encoding(state_type value) {
        if (value > FiniteStateMachine::SUPPORTED_ENCODING_MAX_VALUE) throw 21; // TODO: throw proper exception, if the requested character is not in ASCII encoding
    }

public:
    FiniteStateMachine(const State& start, const std::initializer_list<State> states);
    bool process(char symbol, TokenType& type);
};

#endif /* Automaton_H_ */

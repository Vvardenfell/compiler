/*
 * Automat.h
 *
 *  Created on: Jul 5, 2012
 *      Author: knad0001
 */

#ifndef Automaton_H_
#define Automaton_H_

class Automaton {
public:
};

class State {
private:
	 //Vektor
};

class Transition {
private:
	State next_state;
public:
	virtual const State& process(char symbol) const = 0;
	Transition(const State& next_state) : next_state(next_state) {}
	const State& get_next_state() const { return this->next_state; }
};

class CharTransition : public Transition {
private:
	char symbol;
public:
	CharTransition(char symbol) : symbol(symbol) {}
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
	const State& process(char symbol) const override;
};

#endif /* Automaton_H_ */

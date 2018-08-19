#include "grammar.h"
#include "exception.h"
#include <type_traits>

Grammar::firsts_collection_type Grammar::calculate_firsts(Vector<Grammar::Rule>* rules) const {
    firsts_collection_type firsts;
    firsts_unresolved_collection_type unresolved;

    for(Vector<Grammar::Rule>::const_iterator rule_iterator = rules->cbegin(), end = rules->cend(); rule_iterator != end; ++rule_iterator) {

        auto firsts_of_rule = firsts.insert((*rule_iterator).variable(), firsts_type());
        auto unresolved_of_rule = unresolved.insert((*rule_iterator).variable(), firsts_unresolved_type());

        this->calculate_first_with_unresolved_variables(*rule_iterator, &(*firsts_of_rule).second, &(*unresolved_of_rule).second);
    }

    this->calculate_firsts_resolve_variables(&firsts, &unresolved);

    // TODO: check if there are any unresolved values left. if so, the grammar is left-recursive and therefore invalid

    for(Vector<Grammar::Rule>::iterator rule_iterator = rules->begin(), end = rules->end(); rule_iterator != end; ++rule_iterator) {
        Grammar::Rule& rule = (*rule_iterator);
        rule.firsts((*firsts.find(rule.variable())).second);
    }

    return firsts;
}


void Grammar::calculate_first_with_unresolved_variables(const Grammar::Rule& rule, firsts_type* first, firsts_unresolved_type* unresolved) const {
    const Rule::productions_type& productions = rule.productions();

    for(Rule::productions_type::const_iterator production_iterator = productions.cbegin(), end = productions.cend(); production_iterator != end; ++production_iterator) {
        first->emplace_back();
        unresolved->emplace_back();

        bool within_unresolved_region = false;
        for(Rule::productions_type::value_type::const_iterator value_iterator = (*production_iterator).cbegin(), value_end = (*production_iterator).cend(); value_iterator != value_end; ++value_iterator) {

            if((*value_iterator).is_terminal()) {
                    if(within_unresolved_region) (*(unresolved->end() - 1)).push_back(*value_iterator);
                    else (*(first->end() - 1)).push_back((*value_iterator).terminal());

                    break;
            }
            else {
                within_unresolved_region = true;
                 (*(unresolved->end() - 1)).push_back(*value_iterator);
            }
        }
    }
}


void Grammar::calculate_firsts_resolve_variables(firsts_collection_type* firsts, firsts_unresolved_collection_type* unresolved) const {

    bool progress;
    do {
        progress = false;

        for(std::remove_reference<decltype(*firsts)>::type::iterator entry = firsts->begin(), end = firsts->end(); entry != end; ++entry) {
            firsts_type& firsts_of_variable = (*entry).second;
            firsts_unresolved_type& unresolved_of_variable = (*(*unresolved).find((*entry).first)).second;

            firsts_type::iterator firsts_of_variable_iterator = firsts_of_variable.begin();
            firsts_unresolved_type::iterator unresolved_of_variable_iterator = unresolved_of_variable.begin();
            firsts_unresolved_type::iterator unresolved_of_variable_end = unresolved_of_variable.end();
            for(; unresolved_of_variable_iterator != unresolved_of_variable_end; ++firsts_of_variable_iterator, ++unresolved_of_variable_iterator) {

                if((*unresolved_of_variable_iterator).size() > 0) {
                    progress = this->resolve_single_production_rule(*firsts, *unresolved, &*firsts_of_variable_iterator, &*unresolved_of_variable_iterator) || progress;
                }
            }

        }
    } while(progress);
}


bool Grammar::resolve_single_production_rule(const firsts_collection_type& firsts, const firsts_unresolved_collection_type& unresolved, Vector<Grammar::Terminal>* terminals, Vector<Grammar::Value>* values) const {
    Vector<Grammar::Terminal> tmp_terminals;
    bool include_epsilon = true, finished = false;

    Vector<Grammar::Value>::iterator values_iterator = values->begin(), values_end = values->end();
    for(; values_iterator != values_end; ++values_iterator) {

        const Grammar::Value& value = (*values_iterator);
        if (!this->resolve_single_production_rule_collect_terminals(firsts, unresolved, &include_epsilon, &tmp_terminals, value, &finished)) break;
    }

    if(finished || values_iterator == values_end) {
        this->resolve_single_production_rule_merge_terminals(include_epsilon, terminals, tmp_terminals, values);
        return true;
    }

    return false;
}


bool Grammar::resolve_single_production_rule_collect_terminals(const firsts_collection_type& firsts, const firsts_unresolved_collection_type& unresolved
                                                                                                        , bool* include_epsilon, Vector<Grammar::Terminal>* tmp_terminals, const Grammar::Value& value, bool* finished) const {

    if(value.is_terminal()) {
        tmp_terminals->push_back(value.terminal());
        *include_epsilon = false;
    }
    else {
        Grammar::Variable variable = value.variable();

        if(this->is_resolved_variable(unresolved, variable)) {
            const Vector<Grammar::Terminal>& value_first = this->retrieve_first(firsts, variable);
            tmp_terminals->insert(tmp_terminals->end(), value_first.cbegin(), value_first.cend());

            if(!this->contains_epsilon(value_first)) {
                *finished = true;
                *include_epsilon = false;
                return false;
            }

        } else return false;
    }

    return true;
}


bool Grammar::is_resolved_variable(const firsts_unresolved_collection_type& unresolved, Grammar::Variable variable) const {
    const firsts_unresolved_type& unresolved_production_values = (*(unresolved.find(variable))).second; // TODO: before dereferencing the iterator returned by find(), check if find was even successful. if it was not, then the variable is not defined in the grammar and the grammar therefore invalid

    for(firsts_unresolved_type::const_iterator iterator = unresolved_production_values.cbegin(), end = unresolved_production_values.cend(); iterator != end; ++iterator) {
        if((*iterator).size() > 0) return false;
    }

    return true;
}


Vector<Grammar::Terminal> Grammar::retrieve_first(const firsts_collection_type& firsts, Grammar::Variable variable) const {
    const firsts_type& firsts_of_variable = (*(firsts.find(variable))).second;
    Vector<Grammar::Terminal> merged_firsts;

    for(firsts_type::const_iterator iterator = firsts_of_variable.cbegin(), end = firsts_of_variable.cend(); iterator != end; ++iterator) {
        merged_firsts.insert(merged_firsts.end(), (*iterator).cbegin(), (*iterator).cend());
    }

    return merged_firsts;
}


bool Grammar::contains_epsilon(const Vector<Grammar::Terminal>& terminals) const {
    return terminals.find(Grammar::Terminal::EPSILON) != terminals.cend();
}


void Grammar::resolve_single_production_rule_merge_terminals(bool include_epsilon, Vector<Grammar::Terminal>* terminals, const Vector<Grammar::Terminal>& tmp_terminals, Vector<Grammar::Value>* values) const {
    values->clear();

    if (include_epsilon) terminals->insert(terminals->end(), tmp_terminals.cbegin(), tmp_terminals.cend());
    else {

        for(Vector<Grammar::Terminal>::const_iterator tmp_iterator = tmp_terminals.cbegin(), tmp_end = tmp_terminals.cend(); tmp_iterator != tmp_end; ++tmp_iterator) {
            Grammar::Terminal terminal = *tmp_iterator;
            if(terminal != Grammar::Terminal::EPSILON) terminals->push_back(terminal);
        }
    }
}


void Grammar::calculate_follows(Vector<Grammar::Rule>* rules, const firsts_collection_type& firsts) const {
    follows_collection_type follows;
    follows_unresolved_collection_type unresolved;

    for(Vector<Grammar::Rule>::const_iterator rule_iterator = rules->cbegin(), end = rules->cend(); rule_iterator != end; ++rule_iterator) {
        follows.insert((*rule_iterator).variable(), follows_type());
        unresolved.insert((*rule_iterator).variable(), follows_unresolved_type());
    }

    (*(follows.find((*rules->cbegin()).variable()))).second.push_back(Grammar::Terminal::EPSILON); // add epsilon to follows of the start symbol

    for(Vector<Grammar::Rule>::const_iterator rule_iterator = rules->cbegin(), end = rules->cend(); rule_iterator != end; ++rule_iterator) {
        const Rule::productions_type& productions = (*rule_iterator).productions();
        Grammar::Variable variable = (*rule_iterator).variable();

        for(Rule::productions_type::const_iterator production_iterator = productions.cbegin(), productions_end = productions.cend(); production_iterator != productions_end; ++production_iterator) {
            this->collect_follows_for_single_production_rule(variable, *production_iterator, &follows, &unresolved, firsts);
        }
    }

    this->calculate_follows_resolve_variables(&follows, &unresolved);

    for(Vector<Grammar::Rule>::iterator rule_iterator = rules->begin(), end = rules->end(); rule_iterator != end; ++rule_iterator) {
        Grammar::Rule& rule = (*rule_iterator);
        rule.follows((*follows.find(rule.variable())).second);
    }
}


void Grammar::collect_follows_for_single_production_rule(Grammar::Variable variable, const Vector<Grammar::Value>& production, follows_collection_type* follows
                                                                                                , follows_unresolved_collection_type* unresolved, const firsts_collection_type& firsts) const {

    Vector<follows_collection_type::entry_type*> variables_processing;

    for(Vector<Grammar::Value>::const_iterator value_iterator = production.cbegin(), end = production.cend(); value_iterator != end; ++value_iterator) {

        const Grammar::Value value = *value_iterator;
        if(value.is_terminal()) {
            this->add_terminal_to_follows(&variables_processing, value.terminal());
            variables_processing.clear();
        }
        else {
            Vector<Grammar::Terminal> firsts_of_value(this->retrieve_first(firsts, value.variable()));
            this->add_firsts_of_variable_to_follows(&variables_processing, firsts_of_value);
            if(!this->contains_epsilon(firsts_of_value)) variables_processing.clear();

            variables_processing.push_back(&*(follows->find(value.variable())));
        }
    }

    this->add_variable_as_unresolved_follow(variable, variables_processing, unresolved);
}


void Grammar::add_terminal_to_follows(Vector<follows_collection_type::entry_type*>* variables_processing, Grammar::Terminal terminal) const {
    for(Vector<follows_collection_type::entry_type*>::iterator follows_iterator = variables_processing->begin(), end = variables_processing->end(); follows_iterator != end; ++follows_iterator) {
        (*follows_iterator)->second.push_back(terminal);
    }
}


void Grammar::add_firsts_of_variable_to_follows(Vector<follows_collection_type::entry_type*>* variables_processing, const Vector<Grammar::Terminal>& firsts_of_value) const {
    for(Vector<follows_collection_type::entry_type*>::iterator follows_iterator = variables_processing->begin(), end = variables_processing->end(); follows_iterator != end; ++follows_iterator) {
        for(Vector<Grammar::Terminal>::const_iterator firsts_iterator = firsts_of_value.cbegin(), firsts_end = firsts_of_value.cend(); firsts_iterator != firsts_end; ++firsts_iterator) {
            if(*firsts_iterator != Grammar::Terminal::EPSILON) (*follows_iterator)->second.push_back(*firsts_iterator);
        }
    }
}


void Grammar::add_variable_as_unresolved_follow(Grammar::Variable variable, const Vector<follows_collection_type::entry_type*>& variables_processing, follows_unresolved_collection_type* unresolved) const {
    for(Vector<follows_collection_type::entry_type*>::const_iterator follows_iterator = variables_processing.cbegin(), end = variables_processing.cend(); follows_iterator != end; ++follows_iterator) {
        if((*follows_iterator)->first != variable) (*(unresolved->find((*follows_iterator)->first))).second.push_back(variable);
    }
}


void Grammar::calculate_follows_resolve_variables(follows_collection_type* follows, follows_unresolved_collection_type* unresolved) const {
    bool progress;
    do {
        progress = false;

        for(follows_collection_type::iterator follows_iterator = follows->begin(), end = follows->end(); follows_iterator != end; ++follows_iterator) {
            follows_unresolved_type& unresolved_of_variable = (*(unresolved->find((*follows_iterator).first))).second;

            if(unresolved_of_variable.size() > 0) progress = this->resolve_single_follow(*follows, *unresolved, (*follows_iterator).first, &(*follows_iterator).second, &unresolved_of_variable) || progress;
        }

    } while(progress);
}


bool Grammar::resolve_single_follow(const follows_collection_type& follows, const follows_unresolved_collection_type& unresolved, Grammar::Variable variable, follows_type* terminals, follows_unresolved_type* values) const {
    Vector<Grammar::Terminal> tmp_terminals;
    follows_unresolved_type::const_iterator value_iterator = values->cbegin(), end = values->cend();

    for(; value_iterator != end; ++value_iterator) {
        if(this->is_resolved_variable(unresolved, variable, *value_iterator)) {
            const follows_type& follows_of_variable = (*(follows.find(*value_iterator))).second;
            tmp_terminals.insert(tmp_terminals.end(), follows_of_variable.cbegin(), follows_of_variable.cend());
        }
        else break;
    }

    if(value_iterator == end) {
        terminals->insert(terminals->end(), tmp_terminals.cbegin(), tmp_terminals.cend());
        values->clear();
        return true;
    }

    return false;
}


bool Grammar::is_resolved_variable(const follows_unresolved_collection_type& unresolved, Grammar::Variable request_origin, Grammar::Variable variable) const {
    const follows_unresolved_type& unresolved_variables = (*(unresolved.find(variable))).second;
    return unresolved_variables.size() == 0 || (unresolved_variables.size() == 1 && unresolved_variables[0] == request_origin);
}


Grammar::Grammar(Vector<Grammar::Rule> rules) : grammar_rules(rules) {
    firsts_collection_type firsts = this->calculate_firsts(&this->grammar_rules);
    this->calculate_follows(&this->grammar_rules, firsts);
}


std::ostream& operator<<(std::ostream& out, Grammar::Variable variable) {
	switch(variable) {
	case Grammar::Variable::PROG: return out.write("PROG", 4);
	case Grammar::Variable::DECLS: return out.write("DECLS", 5);
	case Grammar::Variable::DECL: return out.write("DECL", 4);
	case Grammar::Variable::ARRAY: return out.write("ARRAY", 5);
	case Grammar::Variable::STATEMENTS: return out.write("STATEMENTS", 10);
	case Grammar::Variable::STATEMENT: return out.write("STATEMENT", 9);
	case Grammar::Variable::EXP: return out.write("EXP", 3);
	case Grammar::Variable::EXP2: return out.write("EXP2", 4);
	case Grammar::Variable::INDEX: return out.write("INDEX", 5);
	case Grammar::Variable::OP_EXP: return out.write("OP_EXP", 6);
	case Grammar::Variable::OP: return out.write("OP", 2);
	default: throw UnsupportedVariableException("operator<<(std::ostream& out, Grammar::Variable variable)", variable);
	}
}


std::ostream& operator<<(std::ostream& out, const Grammar::Value& value) {
    if(value.is_terminal()) out << value.terminal();
    else out << value.variable();

    return out;
}


const Vector<Grammar::Rule>& get_grammar_description() {
    typedef Grammar::Variable V;
    typedef Grammar::Terminal T;

    const static Vector<Grammar::Rule> GRAMMAR{
        {V::PROG, {{V::DECLS, V::STATEMENTS}}},
        {V::DECLS, {{V::DECL, T::SEMICOLON, V::DECLS}, {T::EPSILON}}},
        {V::DECL, {{T::INT, V::ARRAY, T::IDENTIFIER}}},
        {V::ARRAY, {{T::SQUARE_BRACKET_OPEN, T::INTEGER, T::SQUARE_BRACKET_CLOSE}, {T::EPSILON}}},
        {V::STATEMENTS, {{V::STATEMENT, T::SEMICOLON, V::STATEMENTS}, {T::EPSILON}}},
        {V::STATEMENT, {{T::IDENTIFIER, V::INDEX, T::ASSIGNMENT, V::EXP}, {T::WRITE, T::PARENTHESIS_OPEN, V::EXP, T::PARENTHESIS_CLOSE}, {T::READ, T::PARENTHESIS_OPEN, T::IDENTIFIER, V::INDEX, T::PARENTHESIS_CLOSE}, {T::CURLY_BRACKET_OPEN, V::STATEMENTS, T::CURLY_BRACKET_CLOSE}, {T::IF, T::PARENTHESIS_OPEN, V::EXP, T::PARENTHESIS_CLOSE, V::STATEMENT, T::ELSE, V::STATEMENT}, {T::WHILE, T::PARENTHESIS_OPEN, V::EXP, T::PARENTHESIS_CLOSE, V::STATEMENT}}},
        {V::EXP, {{V::EXP2, V::OP_EXP}}},
        {V::EXP2, {{T::PARENTHESIS_OPEN, V::EXP, T::PARENTHESIS_CLOSE}, {T::IDENTIFIER, V::INDEX}, {T::INTEGER}, {T::MINUS, V::EXP2}, {T::NOT, V::EXP2}}},
        {V::INDEX, {{T::SQUARE_BRACKET_OPEN, V::EXP, T::SQUARE_BRACKET_CLOSE}, {T::EPSILON}}},
        {V::OP_EXP, {{V::OP, V::EXP}, {T::EPSILON}}},
        {V::OP, {{T::PLUS}, {T::MINUS}, {T::ASTERISK}, {T::COLON}, {T::LESS_THAN}, {T::GREATER_THAN}, {T::EQUALITY}, {T::WHATEVER}, {T::LOGICAL_AND}}}
    };

    return GRAMMAR;
}


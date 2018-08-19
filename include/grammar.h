#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "token.h"
#include "vector.h"
#include "unordered_map.h"
#include <functional>
#include <ostream>


class Grammar {
public:

    enum class Variable : unsigned char {
        PROG,
        DECLS,
        DECL,
        ARRAY,
        STATEMENTS,
        STATEMENT,
        EXP,
        EXP2,
        INDEX,
        OP_EXP,
        OP
    };

    typedef TokenType Terminal;

    class Value {
    private:

        enum class Type : unsigned char {
            VARIABLE,
            TERMINAL
        };

        union Storage {
            Grammar::Variable variable;
            Grammar::Terminal terminal;

            Storage(Grammar::Variable variable) : variable(variable) {}
            Storage(Grammar::Terminal terminal) : terminal(terminal) {}
        } value;
        Value::Type type;

    public:

        Value(Grammar::Variable variable) : value(variable), type(Value::Type::VARIABLE) {}
        Value(Grammar::Terminal terminal) : value(terminal), type(Value::Type::TERMINAL) {}

        bool is_terminal() const {
            return this->type == Value::Type::TERMINAL;
        }

        bool is_variable() const {
            return this->type == Value::Type::VARIABLE;
        }

        Grammar::Terminal terminal() const {
            return this->value.terminal;
        }

        Grammar::Variable variable() const {
            return this->value.variable;
        }
    };


    typedef Vector<Vector<Grammar::Terminal>> firsts_type;
    typedef Vector<Grammar::Terminal> follows_type;

private:

    typedef Vector<Vector<Grammar::Value>> firsts_unresolved_type;
    typedef UnorderedMap<Grammar::Variable, firsts_type> firsts_collection_type;
    typedef UnorderedMap<Grammar::Variable, firsts_unresolved_type> firsts_unresolved_collection_type;

    typedef Vector<Grammar::Variable> follows_unresolved_type;
    typedef UnorderedMap<Grammar::Variable, follows_type> follows_collection_type;
    typedef UnorderedMap<Grammar::Variable, follows_unresolved_type> follows_unresolved_collection_type;

public:

    class Rule {
    public:
        typedef Vector<Vector<Grammar::Value>> productions_type;

    private:

        friend class Grammar;

        Grammar::firsts_type rule_firsts;
        Grammar::follows_type rule_follows;
        productions_type rule_productions;
        Grammar::Variable production_variable;

        void firsts(Grammar::firsts_type firsts) {
            this->rule_firsts = firsts;
        }

        void follows(Grammar::follows_type follows) {
            this->rule_follows = follows;
        }

    public:

        Rule(Grammar::Variable variable, productions_type productions) : rule_productions(productions), production_variable(variable) {}

        Grammar::Variable variable() const {
            return this->production_variable;
        }

        const productions_type& productions() const {
            return this->rule_productions;
        }

        const Grammar::firsts_type& firsts() const {
            return this->rule_firsts;
        }

        const Grammar::follows_type& follows() const {
            return this->rule_follows;
        }

    };


private:

    Vector<Grammar::Rule> grammar_rules;

    firsts_collection_type calculate_firsts(Vector<Grammar::Rule>* rules) const;
    void calculate_first_with_unresolved_variables(const Grammar::Rule& rule, firsts_type* first, firsts_unresolved_type* unresolved) const;
    void calculate_firsts_resolve_variables(firsts_collection_type* firsts, firsts_unresolved_collection_type* unresolved) const;
    bool resolve_single_production_rule(const firsts_collection_type& firsts, const firsts_unresolved_collection_type& unresolved, Vector<Grammar::Terminal>* terminals, Vector<Grammar::Value>* values) const;

    bool resolve_single_production_rule_collect_terminals(const firsts_collection_type& firsts, const firsts_unresolved_collection_type& unresolved
                                                                                            , bool* include_epsilon, Vector<Grammar::Terminal>* tmp_terminals, const Grammar::Value& value, bool* finished) const;
    bool is_resolved_variable(const firsts_unresolved_collection_type& unresolved, Grammar::Variable variable) const;
    Vector<Grammar::Terminal> retrieve_first(const firsts_collection_type& firsts, Grammar::Variable variable) const;
    bool contains_epsilon(const Vector<Grammar::Terminal>& terminals) const;
    void resolve_single_production_rule_merge_terminals(bool include_epsilon, Vector<Grammar::Terminal>* terminals, const Vector<Grammar::Terminal>& tmp_terminals, Vector<Grammar::Value>* values) const;


    void calculate_follows(Vector<Grammar::Rule>* rules, const firsts_collection_type& firsts) const;
    void collect_follows_for_single_production_rule(Grammar::Variable variable, const Vector<Grammar::Value>& production, follows_collection_type* follows
                                                                                    , follows_unresolved_collection_type* unresolved, const firsts_collection_type& firsts) const;
    void add_terminal_to_follows(Vector<Pair<Grammar::Variable, Vector<Grammar::Terminal>>*>* variables_processing, Grammar::Terminal terminal) const;
    void add_firsts_of_variable_to_follows(Vector<Pair<Grammar::Variable, Vector<Grammar::Terminal>>*>* variables_processing, const Vector<Grammar::Terminal>& firsts_of_value) const;
    void add_variable_as_unresolved_follow(Grammar::Variable variable, const Vector<Pair<Grammar::Variable, Vector<Grammar::Terminal>>*>& variables_processing, follows_unresolved_collection_type* unresolved) const;
    void calculate_follows_resolve_variables(follows_collection_type* follows, follows_unresolved_collection_type* unresolved) const;
    bool resolve_single_follow(const follows_collection_type& follows, const follows_unresolved_collection_type& unresolved, Grammar::Variable variable, follows_type* terminals, follows_unresolved_type* values) const;
    bool is_resolved_variable(const follows_unresolved_collection_type& unresolved, Grammar::Variable request_origin, Grammar::Variable variable) const;

public:

    explicit Grammar(Vector<Grammar::Rule> rules);

    const Vector<Grammar::Rule>& rules() const {
        return this->grammar_rules;
    }
};


std::ostream& operator<<(std::ostream& out, Grammar::Variable variable);
std::ostream& operator<<(std::ostream& out, const Grammar::Value& value);


const Vector<Grammar::Rule>& get_grammar_description();


namespace std {

	template<> class hash<Grammar::Variable> {
    public:
		std::size_t operator()(Grammar::Variable value) const {
            return static_cast<std::size_t>(value);
		}
	};

}

#endif /* GRAMMAR_H */

#ifndef PARSER_H
#define PARSER_H

#include "vector.h"
#include "grammar.h"
#include "finite_state_machine.h"
#include "parse_tree.h"
#include <limits>
#include <ostream>


class Parser {
public:

    class TreeData {
    private:

        enum class Type : unsigned char {
            VARIABLE,
            TOKEN
        };

        union Storage {
            Grammar::Variable variable;
            Token token;

            Storage(Grammar::Variable variable) : variable(variable) {}
            Storage(const Token& token) : token(token) {}
        } data;
        TreeData::Type type;

    public:

        TreeData(Grammar::Variable variable) : data(variable), type(TreeData::Type::VARIABLE) {}
        TreeData(const Token& token) : data(token), type(TreeData::Type::TOKEN) {}

        ~TreeData();

        bool is_token() const;
        bool is_variable() const;

        Token& token();
        const Token& token() const;
        Grammar::Variable variable() const;
    };

private:

    typedef unsigned char cell_type;
    const static cell_type INVALID_RULE_REFERENCE_ID = std::numeric_limits<cell_type>::max();

    ParseTree<Parser::TreeData> tree;
    BranchMatrix<cell_type> branch_matrix;
    Grammar::Rule::productions_type lookup_table;
    Grammar::Rule::productions_type stack;
    ParseTree<TreeData>::Node* active_node;
    std::ostream* error_stream;
    bool recovery, valid;


    void init_branch_matrix(const Vector<Grammar::Rule>& rules);
    void init_branch_matrix_row(const Grammar::Rule& rule);
    void init_branch_matrix_row(Grammar::Variable variable, const Vector<Grammar::Value>& production, const Vector<Grammar::Terminal>& terminals);

    bool contains_epsilon(const Vector<Grammar::Terminal>& firsts);

    bool is_stack_empty() const;
    const Vector<Grammar::Value>& stack_peek() const;
    const Grammar::Value& stack_rule_peek() const;
    Grammar::Value stack_rule_pop();
    void cleanup_stack();

    bool has_rule(const Grammar::Value& value, TokenType type) const;
    void stack_push_rule(Grammar::Variable variable, TokenType type);

    void handle_unexpected_token(const Token& token, bool force = false);
    Vector<TokenType> gather_expected_token() const;
    bool is_eof_expectable() const;
    bool is_epsilon_replaceable(const Vector<Grammar::Value>& production_rule) const;
    void write_error_message(const Token& token, const Vector<TokenType>& expected) const;


public:

    Parser(const Vector<Grammar::Rule>& rules, std::ostream* error_stream);

    bool process(const Token& token);

    bool finalize();

    ParseTree<Parser::TreeData>& parse_tree() {
        return this->tree;
    }
};

#endif /* PARSER_H */

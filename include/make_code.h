#ifndef MAKE_CODE_H
#define MAKE_CODE_H

#include "token.h"
#include "parse_tree.h"
#include "parser.h"
#include "string.h"
#include <ostream>


class MakeCode {
private:

    typedef ParseTree<Parser::TreeData>::Node node_type;

    const ParseTree<Parser::TreeData>* parse_tree;
    std::ostream* code_stream;


    const Token& get_token(const node_type& node) const;
    const String& get_lexem(const node_type& node) const;
    long get_integer(const node_type& node) const;

    String generate_label() const;

    FundamentalType get_data_type(const node_type& node) const;

    void code_prog(const node_type& prog) const;

    void code_decls(const node_type& decls) const;

    void code_decl(const node_type& decl) const;

    void code_array(const node_type& array) const;

    void code_statements(const node_type& statements) const;

    void code_statement(const node_type& statement) const;
    void code_statement_identifier(const node_type& statement) const;
    void code_statement_write(const node_type& statement) const;
    void code_statement_read(const node_type& statement) const;
    void code_statement_curly_bracket_open(const node_type& statement) const;
    void code_statement_if(const node_type& statement) const;
    void code_statement_while(const node_type& statement) const;

    void code_exp(const node_type& exp) const;

    void code_index(const node_type& index) const;

    void code_exp2(const node_type& exp2) const;
    void code_exp2_parenthesis_open(const node_type& exp2) const;
    void code_exp2_identifier(const node_type& exp2) const;
    void code_exp2_integer(const node_type& exp2) const;
    void code_exp2_minus(const node_type& exp2) const;
    void code_exp2_not(const node_type& exp2) const;

    void code_op_exp(const node_type& op_exp) const;

    void code_op(const node_type& op) const;


public:

    MakeCode(const ParseTree<Parser::TreeData>* parse_tree, std::ostream* code_stream);

    void operator()() const;
};


#endif /* MAKE_CODE_H */

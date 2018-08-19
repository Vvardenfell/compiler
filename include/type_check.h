#ifndef TYPE_CHECK_H
#define TYPE_CHECK_H

#include "token.h"
#include "parse_tree.h"
#include "parser.h"
#include "string.h"
#include "unordered_map.h"
#include <ostream>

class TypeCheck {
private:

    enum class ErrorSubMessage {
        NONE,
        IDENTIFIER_ALREADY_DEFINED,
        INCOMPATIBLE_TYPES_IN_ASSIGNMENT,
        INCOMPATIBLE_TYPES_IN_READ,
        NOT_A_PRIMITIVE_TYPE
    };

    typedef ParseTree<Parser::TreeData>::Node node_type;

    ParseTree<Parser::TreeData>* parse_tree;
    UnorderedMap<const String*, const node_type*> identifier_dictionary;
    std::ostream* error_stream;
    bool valid;


    FundamentalType get_data_type(const node_type& node) const ;
    void set_data_type(node_type* node, FundamentalType data_type) const;

    const Token& get_token(const node_type& node) const;

    Vector<const Token*> collect_neighbours(const node_type& node, std::size_t hierarchy_levels = 1) const;

    void handle_error(node_type* source_node, const char* message, const node_type& node, ErrorSubMessage sub_message_type = ErrorSubMessage::NONE, std::size_t hierarchy_levels = 1);
    void handle_error_sub_message(TypeCheck::ErrorSubMessage sub_message_type, const node_type& node) const;
    String reconstruct_source(const Vector<const Token*>& token) const;
    FundamentalType determine_identifier_compound_type(const node_type& node, std::size_t index) const;

    void identifier_dictionary_add(const node_type& node);
    const node_type& identifier_dictionary_get(const node_type& node) const;


    void check_prog(node_type* prog);

    void check_decls(node_type* decls);

    void check_decl(node_type* decl);

    void check_array(node_type* array);

    void check_statements(node_type* statements);

    void check_statement(node_type* statement);
    void check_statement_identifier(node_type* statement);
    void check_statement_write(node_type* statement);
    void check_statement_read(node_type* statement);
    void check_statement_curly_bracket_open(node_type* statement);
    void check_statement_if(node_type* statement);
    void check_statement_while(node_type* statement);

    void check_index(node_type* index);

    void check_exp(node_type* exp);

    void check_exp2(node_type* exp2);
    void check_exp2_parenthesis_open(node_type* exp2);
    void check_exp2_identifier(node_type* exp2);
    void check_exp2_integer(node_type* exp2);
    void check_exp2_minus(node_type* exp2);
    void check_exp2_not(node_type* exp2);

    void check_op_exp(node_type* op_exp);

    void check_op(node_type* op);


public:

    TypeCheck(ParseTree<Parser::TreeData>* parse_tree, std::ostream* error_stream);

    bool operator()();
};

#endif /* TYPE_CHECK_H */

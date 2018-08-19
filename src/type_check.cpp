#include "type_check.h"
#include "exception.h"


FundamentalType TypeCheck::get_data_type(const node_type& node) const {
    const Parser::TreeData& userdata = node.userdata();

    if(userdata.is_token()) return userdata.token().get_data_type();
    return node.get_data_type();
}


void TypeCheck::set_data_type(node_type* node, FundamentalType data_type) const {
    Parser::TreeData& userdata = node->userdata();

    if(userdata.is_token()) userdata.token().set_data_type(data_type);
    else node->set_data_type(data_type);
}


const Token& TypeCheck::get_token(const node_type& node) const {
    return node.userdata().token();
}


Vector<const Token*> TypeCheck::collect_neighbours(const node_type& node, std::size_t hierarchy_levels) const {
    const node_type* root = &node;
    for(std::size_t count = 0; count < hierarchy_levels && root != &(root->parent()); ++count) root = &(root->parent());

    Vector<const node_type*> children{root};
    bool children_changed;

    do {
        children_changed = false;
        Vector<const node_type*> tmp_children;

        for(Vector<const node_type*>::const_iterator children_iterator = children.cbegin(), children_end_iterator = children.cend(); children_iterator != children_end_iterator; ++children_iterator) {
            std::size_t child_count = (*children_iterator)->child_count();

            if(child_count == 0) tmp_children.push_back(*children_iterator);
            else {
                for(std::size_t child_index = 0; child_index < child_count; ++child_index) {
                    tmp_children.push_back(&(*children_iterator)->child(child_index));
                }
                children_changed = true;
            }
        }

        children = tmp_children;
    } while(children_changed);

    Vector<const Token*> token;
    for(Vector<const node_type*>::const_iterator children_iterator = children.cbegin(), children_end_iterator = children.cend(); children_iterator != children_end_iterator; ++children_iterator) {
        if((*children_iterator)->has_userdata() && (*children_iterator)->userdata().is_token()) token.push_back(&(this->get_token(**children_iterator)));
    }

    return token;
}


void TypeCheck::handle_error(node_type* source_node, const char* message, const node_type& affected_node, TypeCheck::ErrorSubMessage sub_message_type, std::size_t hierarchy_levels) {

    this->set_data_type(source_node, FundamentalType::ERROR);
    this->valid = false;

    if(affected_node.has_userdata()) {
        if(affected_node.userdata().is_token()) *this->error_stream << this->get_token(affected_node);
        else *this->error_stream << (affected_node.userdata().variable());
    }
    else *this->error_stream << this->get_data_type(affected_node);

    *this->error_stream << " - " << message << "\nWithin this context: ";

    Vector<const Token*> token(this->collect_neighbours(affected_node, hierarchy_levels));
    *this->error_stream << token[0]->line << ':' << token[0]->column << ": " << this->reconstruct_source(token);

    this->handle_error_sub_message(sub_message_type, affected_node);
}


void TypeCheck::handle_error_sub_message(TypeCheck::ErrorSubMessage sub_message_type, const node_type& node) const {
    switch(sub_message_type) {
    case TypeCheck::ErrorSubMessage::NONE: { /* don't do anything */ break; }
    case TypeCheck::ErrorSubMessage::IDENTIFIER_ALREADY_DEFINED: {

        const node_type& first_definition_node = this->identifier_dictionary_get(node);
        Vector<const Token*> token(this->collect_neighbours(node));

        *this->error_stream << "\nFirst defined here " << this->get_token(first_definition_node) << " - " << this->reconstruct_source(token);
        break;
    }
    case TypeCheck::ErrorSubMessage::INCOMPATIBLE_TYPES_IN_ASSIGNMENT: {

        const node_type& right_node = node.parent().child(3);

        *this->error_stream << "\nNo known conversion from " << this->get_data_type(right_node) << " to " << this->determine_identifier_compound_type(node.parent(), 0);
        break;
    }
    case TypeCheck::ErrorSubMessage::INCOMPATIBLE_TYPES_IN_READ: {
        *this->error_stream << "\nNo known conversion from " << this->determine_identifier_compound_type(node.parent(), 2) << " to " << FundamentalType::INT;
        break;
    }
    case TypeCheck::ErrorSubMessage::NOT_A_PRIMITIVE_TYPE: {

        *this->error_stream << "\nType of " << this->reconstruct_source(this->collect_neighbours(node, 0)) << " is " << this->determine_identifier_compound_type(node, 0);
        break;
    }
    }

    *this->error_stream << "\n\n";
}


String TypeCheck::reconstruct_source(const Vector<const Token*>& token) const {
    if(token.size() == 0) return String();

    String output;
    std::size_t last_token_line = token[0]->line, next_token_earliest_column = token[0]->column;

    for(Vector<const Token*>::const_iterator token_iterator = token.cbegin(), token_end_iterator = token.cend(); token_iterator != token_end_iterator; ++token_iterator) {

        const Token* token = (*token_iterator);

        if(token->line != last_token_line || token->column > next_token_earliest_column) output += ' ';
        String token_string(token->to_string());
        output += token_string;

        last_token_line = token->line;
        next_token_earliest_column = token->column + token_string.size();
    }

    return output;
}


FundamentalType TypeCheck::determine_identifier_compound_type(const node_type& node, std::size_t index) const {
    FundamentalType identifier_type = this->get_data_type(node.child(index));
    FundamentalType index_type = this->get_data_type(node.child(index + 1));

    if(identifier_type == FundamentalType::NONE && index_type == FundamentalType::NONE) return FundamentalType::NONE;
    if(identifier_type == FundamentalType::INT_ARRAY && index_type == FundamentalType::ARRAY) return FundamentalType::INT;
    if(identifier_type == FundamentalType::INT_ARRAY && index_type == FundamentalType::NONE) return FundamentalType::INT_ARRAY;
    if(identifier_type == FundamentalType::INT && index_type == FundamentalType::NONE) return FundamentalType::INT;
    return FundamentalType::ERROR;
}


void TypeCheck::identifier_dictionary_add(const node_type& node) {
    const String* lexem = this->get_token(node).value.information->lexem;
    this->identifier_dictionary.insert(lexem, &node);
}


const TypeCheck::node_type& TypeCheck::identifier_dictionary_get(const node_type& node) const {
    const String* lexem = this->get_token(node).value.information->lexem;
    return *(*this->identifier_dictionary.find(lexem)).second;
}


void TypeCheck::check_prog(node_type* prog) {
    this->check_decls(&prog->child(0));
    this->check_statements(&prog->child(1));
}


void TypeCheck::check_decls(node_type* decls) {
    if(decls->child_count()) {
        this->check_decl(&decls->child(0));
        this->check_decls(&decls->child(2));
    }
}


void TypeCheck::check_decl(node_type* decl) {
    this->check_array(&decl->child(1));

    if (this->get_data_type(decl->child(2)) != FundamentalType::NONE) {
        this->handle_error(decl, "Identifier already defined", decl->child(2), TypeCheck::ErrorSubMessage::IDENTIFIER_ALREADY_DEFINED);
    }
    else {
        switch(this->get_data_type(decl->child(1))) {
        case FundamentalType::ERROR: this->set_data_type(decl, FundamentalType::ERROR); break;
        case FundamentalType::ARRAY: {
            this->identifier_dictionary_add(decl->child(2));
            this->set_data_type(&decl->child(2), FundamentalType::INT_ARRAY);
            break;
        }
        default: {
            this->identifier_dictionary_add(decl->child(2));
            this->set_data_type(&decl->child(2), FundamentalType::INT);
        }
        }
    }
}


void TypeCheck::check_array(node_type* array) {
    if(array->child_count()) {
        if(this->get_token(array->child(1)).value.integer > 0) this->set_data_type(array, FundamentalType::ARRAY);
        else {
            this->handle_error(array, "No valid dimension", array->child(1), TypeCheck::ErrorSubMessage::NONE, 2);
        }
    }
}


void TypeCheck::check_statements(node_type* statements) {
    if(statements->child_count()) {
        this->check_statement(&statements->child(0));
        this->check_statements(&statements->child(2));
    }
}


void TypeCheck::check_statement(node_type* statement) {
    TokenType token_type = this->get_token(statement->child(0)).get_token_type();

    switch(token_type) {
    case TokenType::IDENTIFIER: this->check_statement_identifier(statement); break;
    case TokenType::WRITE: this->check_statement_write(statement); break;
    case TokenType::READ: this->check_statement_read(statement); break;
    case TokenType::CURLY_BRACKET_OPEN: this->check_statement_curly_bracket_open(statement); break;
    case TokenType::IF: this->check_statement_if(statement); break;
    case TokenType::WHILE: this->check_statement_while(statement); break;
    default: throw UnsupportedTokenTypeException("TypeCheck::check_statement(node_type* statement)", token_type);
    }
}


void TypeCheck::check_statement_identifier(node_type* statement) {
    this->check_exp(&statement->child(3));
    this->check_index(&statement->child(1));

    FundamentalType identifier_type = this->get_data_type(statement->child(0));
    FundamentalType index_type = this->get_data_type(statement->child(1));

    if(identifier_type == FundamentalType::NONE) {
        this->handle_error(statement, "Identifier not defined", statement->child(0));
    }
    else if(this->get_data_type(statement->child(3)) != FundamentalType::INT
                || ((identifier_type != FundamentalType::INT || index_type != FundamentalType::NONE) && (identifier_type != FundamentalType::INT_ARRAY || index_type != FundamentalType::ARRAY))) {
        this->handle_error(statement, "Incompatible types in assignment", statement->child(2), TypeCheck::ErrorSubMessage::INCOMPATIBLE_TYPES_IN_ASSIGNMENT);
    }
}


void TypeCheck::check_statement_write(node_type* statement) {
    this->check_exp(&statement->child(2));
}


void TypeCheck::check_statement_read(node_type* statement) {
    this->check_index(&statement->child(3));

    FundamentalType identifier_type = this->get_data_type(statement->child(2));
    FundamentalType index_type = this->get_data_type(statement->child(3));

    if(identifier_type == FundamentalType::NONE) {
        this->handle_error(statement, "Identifier not defined", statement->child(2));
    }
    else if((identifier_type != FundamentalType::INT || index_type != FundamentalType::NONE) && (identifier_type != FundamentalType::INT_ARRAY || index_type != FundamentalType::ARRAY)) {
        this->handle_error(statement, "Incompatible types", statement->child(0), TypeCheck::ErrorSubMessage::INCOMPATIBLE_TYPES_IN_READ);
    }
}


void TypeCheck::check_statement_curly_bracket_open(node_type* statement) {
    this->check_statements(&statement->child(1));
}


void TypeCheck::check_statement_if(node_type* statement) {
    this->check_exp(&statement->child(2));
    this->check_statement(&statement->child(4));
    this->check_statement(&statement->child(6));

    if(this->get_data_type(statement->child(2)) == FundamentalType::ERROR) this->set_data_type(statement, FundamentalType::ERROR);
}


void TypeCheck::check_statement_while(node_type* statement) {
    this->check_exp(&statement->child(2));
    this->check_statement(&statement->child(4));

    if(this->get_data_type(statement->child(2)) == FundamentalType::ERROR) this->set_data_type(statement, FundamentalType::ERROR);
}


void TypeCheck::check_index(node_type* index) {
    if(index->child_count()) {
        this->check_exp(&index->child(1));

        if(this->get_data_type(index->child(1)) == FundamentalType::ERROR) this->set_data_type(index, FundamentalType::ERROR);
        else this->set_data_type(index, FundamentalType::ARRAY);
    }
}


void TypeCheck::check_exp(node_type* exp) {
    this->check_exp2(&exp->child(0));
    this->check_op_exp(&exp->child(1));

    FundamentalType exp2_type = this->get_data_type(exp->child(0));
    FundamentalType op_exp_type = this->get_data_type(exp->child(1));

    if(op_exp_type == FundamentalType::NONE || exp2_type == op_exp_type) this->set_data_type(exp, exp2_type);
    else this->set_data_type(exp, FundamentalType::ERROR);
}


void TypeCheck::check_exp2(node_type* exp2) {
    TokenType token_type = this->get_token(exp2->child(0)).get_token_type();

    switch(token_type) {
    case TokenType::PARENTHESIS_OPEN: this->check_exp2_parenthesis_open(exp2); break;
    case TokenType::IDENTIFIER: this->check_exp2_identifier(exp2); break;
    case TokenType::INTEGER: this->check_exp2_integer(exp2); break;
    case TokenType::MINUS: this->check_exp2_minus(exp2); break;
    case TokenType::NOT: this->check_exp2_not(exp2); break;
    default: throw UnsupportedTokenTypeException("TypeCheck::check_exp2(node_type* exp2)", token_type);
    }
}


void TypeCheck::check_exp2_parenthesis_open(node_type* exp2) {
    this->check_exp(&exp2->child(1));
    this->set_data_type(exp2, this->get_data_type(exp2->child(1)));
}


void TypeCheck::check_exp2_identifier(node_type* exp2) {
    this->check_index(&exp2->child(1));

    FundamentalType identifier_type = this->get_data_type(exp2->child(0));
    FundamentalType index_type = this->get_data_type(exp2->child(1));

    if((identifier_type == FundamentalType::INT && index_type == FundamentalType::NONE) || (identifier_type == FundamentalType::INT_ARRAY && index_type == FundamentalType::ARRAY)) {
        this->set_data_type(exp2, FundamentalType::INT);
    }
    else if(identifier_type == FundamentalType::NONE) this->handle_error(exp2, "Identifier not defined", exp2->child(0), TypeCheck::ErrorSubMessage::NONE, 3);
    else this->handle_error(exp2, "Not a primitive type", *exp2, TypeCheck::ErrorSubMessage::NOT_A_PRIMITIVE_TYPE, 2);
}


void TypeCheck::check_exp2_integer(node_type* exp2) {
    this->set_data_type(exp2, FundamentalType::INT);
}


void TypeCheck::check_exp2_minus(node_type* exp2) {
    this->check_exp2(&exp2->child(1));
    this->set_data_type(exp2, this->get_data_type(exp2->child(1)));
}


void TypeCheck::check_exp2_not(node_type* exp2) {
    this->check_exp2(&exp2->child(1));

    if(this->get_data_type(exp2->child(1)) == FundamentalType::INT) this->set_data_type(exp2, FundamentalType::INT);
    else this->set_data_type(exp2, FundamentalType::ERROR);
}


void TypeCheck::check_op_exp(node_type* op_exp) {
    if(op_exp->child_count()) {
        this->check_op(&op_exp->child(0));
        this->check_exp(&op_exp->child(1));

        this->set_data_type(op_exp, this->get_data_type(op_exp->child(1)));
    }
}


void TypeCheck::check_op(node_type* op) {}


TypeCheck::TypeCheck(ParseTree<Parser::TreeData>* parse_tree, std::ostream* error_stream) : parse_tree(parse_tree), error_stream(error_stream), valid(true) {}


bool TypeCheck::operator()() {
    this->check_prog(&(this->parse_tree->root().child(0)));
    this->error_stream->flush();
    return this->valid;
}

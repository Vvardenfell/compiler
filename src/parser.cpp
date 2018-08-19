#include "parser.h"
#include "exception.h"

Parser::TreeData::~TreeData() {
    if(this->is_token()) this->data.token.~Token();
}


bool  Parser::TreeData::is_token() const {
    return this->type == TreeData::Type::TOKEN;
}


bool  Parser::TreeData::is_variable() const {
    return this->type == TreeData::Type::VARIABLE;
}


Token&  Parser::TreeData::token() {
    return this->data.token;
}


const Token&  Parser::TreeData::token() const {
    return this->data.token;
}


Grammar::Variable  Parser::TreeData::variable() const {
    return this->data.variable;
}



void Parser::init_branch_matrix(const Vector<Grammar::Rule>& rules) {
    for(Vector<Grammar::Rule>::const_iterator rule_iterator = rules.cbegin(), end = rules.cend(); rule_iterator != end; ++rule_iterator) {
        this->init_branch_matrix_row(*rule_iterator);
    }
}


void Parser::init_branch_matrix_row(const Grammar::Rule& rule) {
    const Grammar::Rule::productions_type& productions = rule.productions();
    const Grammar::firsts_type& firsts = rule.firsts();

    Grammar::Rule::productions_type::const_iterator production_iterator = productions.cbegin(), production_end_iterator = productions.cend();
    Grammar::firsts_type::const_iterator first_iterator = firsts.cbegin(), first_end_iterator = firsts.cend();

    for(; production_iterator != production_end_iterator; ++production_iterator, ++first_iterator) {
        this->init_branch_matrix_row(rule.variable(), *production_iterator, *first_iterator);

        if(this->contains_epsilon(*first_iterator)) this->init_branch_matrix_row(rule.variable(), *production_iterator, rule.follows());
    }
}


void Parser::init_branch_matrix_row(Grammar::Variable variable, const Vector<Grammar::Value>& production, const Vector<Grammar::Terminal>& terminals) {
    this->lookup_table.push_back(Vector<Grammar::Value>());
    std::size_t lookup_table_index = this->lookup_table.size() - 1;

    for(Vector<Grammar::Value>::const_reverse_iterator value_iterator = production.rbegin(), value_end_iterator = production.rend(); value_iterator != value_end_iterator; ++value_iterator) {
        if((*value_iterator).is_variable() || (*value_iterator).terminal() != Grammar::Terminal::EPSILON) this->lookup_table[lookup_table_index].push_back(*value_iterator);
    }

    if(lookup_table_index >= Parser::INVALID_RULE_REFERENCE_ID) {
        throw TooManyProductionRulesException("Parser::init_branch_matrix_row(Grammar::Variable, const Vector<Grammar::Value>&, const Vector<Grammar::Terminal>&)", Parser::INVALID_RULE_REFERENCE_ID);
    }

    bool lookup_table_index_reference_written = false;
    for(Vector<Grammar::Terminal>::const_iterator terminal_iterator = terminals.cbegin(), terminal_end_iterator = terminals.cend(); terminal_iterator != terminal_end_iterator; ++terminal_iterator) {
        this->branch_matrix.set(static_cast<std::size_t>(*terminal_iterator), static_cast<std::size_t>(variable), lookup_table_index);
        lookup_table_index_reference_written = true;
    }

    if(!lookup_table_index_reference_written) this->lookup_table.pop_back();
}


bool Parser::contains_epsilon(const Vector<Grammar::Terminal>& firsts) {
    return firsts.find(Grammar::Terminal::EPSILON) != firsts.cend();
}


bool Parser::is_stack_empty() const {
    return this->stack.size() == 0;
}


const Vector<Grammar::Value>& Parser::stack_peek() const {
    return *(this->stack.cend() - 1);
}


const Grammar::Value& Parser::stack_rule_peek() const {
    return *(this->stack_peek().cend() - 1);
}


Grammar::Value Parser::stack_rule_pop() {
    return (*(this->stack.end() - 1)).pop_back();
}


void Parser::cleanup_stack() {
    while(!this->is_stack_empty() && this->stack_peek().size() == 0) {
        this->stack.pop_back();
        this->active_node = &(this->active_node->parent());
    }
}


bool Parser::has_rule(const Grammar::Value& value, TokenType type) const {
    return value.is_variable() && this->branch_matrix.get(static_cast<std::size_t>(type), static_cast<std::size_t>(value.variable())) != Parser::INVALID_RULE_REFERENCE_ID;
}


void Parser::stack_push_rule(Grammar::Variable variable, TokenType type) {
    cell_type lookup_table_index = this->branch_matrix.get(static_cast<std::size_t>(type), static_cast<std::size_t>(variable));
    const Vector<Grammar::Value>& production = this->lookup_table[lookup_table_index];

    if(type != TokenType::EPSILON || production.size() != 0) this->stack.push_back(production);
}


void Parser::handle_unexpected_token(const Token& token, bool force) {
    TokenType token_type = token.get_token_type();
    if(!force && (this->recovery || token_type == TokenType::EPSILON)) return;

    Vector<TokenType> expected = this->gather_expected_token();
    this->write_error_message(token, expected);

    if(token_type != TokenType::OUT_OF_RANGE_INTEGER) {
        if(!this->is_stack_empty()) this->stack_rule_pop();
        this->recovery = true;
    }
}


Vector<TokenType> Parser::gather_expected_token() const {
    Vector<TokenType> expected_token;

    if(this->is_stack_empty()) expected_token.push_back(TokenType::EPSILON);
    else {
        const Grammar::Value value = this->stack_rule_peek();

        if(value.is_terminal()) expected_token.push_back(value.terminal());
        else {
            bool epsilon = false;
            for(std::size_t x = 0; x < static_cast<std::size_t>(TokenType::ENUM_ENTRY_COUNT); ++x) {

                cell_type lookup_table_index = this->branch_matrix.get(x, static_cast<std::size_t>(value.variable()));
                if(lookup_table_index != Parser::INVALID_RULE_REFERENCE_ID) {
                    if(static_cast<TokenType>(x) == TokenType::EPSILON) epsilon = true;
                    else expected_token.push_back(static_cast<TokenType>(x));
                }
            }

            if(epsilon && this->is_eof_expectable()) expected_token.push_back(TokenType::EPSILON);
        }
    }

    return expected_token;
}


bool Parser::is_eof_expectable() const {
    Grammar::Rule::productions_type::const_reverse_iterator stack_iterator = this->stack.rbegin(), stack_end_iterator = this->stack.rend();
    for(; stack_iterator != stack_end_iterator; ++stack_iterator) {

        if(!this->is_epsilon_replaceable(*stack_iterator)) return false;
    }

    return true;
}


bool Parser::is_epsilon_replaceable(const Vector<Grammar::Value>& production_rule) const {
    Vector<Grammar::Value>::const_reverse_iterator production_iterator = production_rule.rbegin(), production_end_iterator = production_rule.rend();
    for(; production_iterator != production_end_iterator; ++production_iterator) {

        const Grammar::Value& value = *production_iterator;
        if(value.is_terminal()) return false;
        else {

            cell_type lookup_table_index = this->branch_matrix.get(static_cast<std::size_t>(TokenType::EPSILON), static_cast<std::size_t>(value.variable()));
            if(lookup_table_index != Parser::INVALID_RULE_REFERENCE_ID) {
                if(!this->is_epsilon_replaceable(this->lookup_table[lookup_table_index])) return false;
            }
            else return false;
        }
    }

    return true;
}

void Parser::write_error_message(const Token& token, const Vector<TokenType>& expected) const {
    if(token.get_token_type() == TokenType::EPSILON) *this->error_stream << "Unexpected end of file\nExpected: ";
    else *this->error_stream << token << " - Unexpected token\nExpected: ";

    for(Vector<TokenType>::const_iterator expected_iterator = expected.cbegin(), expected_end_iterator = expected.cend(); expected_iterator < expected_end_iterator - 1; ++expected_iterator) {
        *this->error_stream << (*expected_iterator) << ", ";
    }

    TokenType last_token_type = expected[expected.size() - 1];
    if (last_token_type == TokenType::EPSILON) *this->error_stream << "EOF";
    else *this->error_stream << last_token_type;

    if(!this->is_stack_empty()) {
        const Grammar::Value& stack_top_value = this->stack_rule_peek();
        if(stack_top_value.is_terminal()) *this->error_stream << '\n';
        else *this->error_stream << " for " << this->stack_rule_peek() << '\n';
    }
    else *this->error_stream << '\n';
}


Parser::Parser(const Vector<Grammar::Rule>& rules, std::ostream* error_stream)
    : tree(), branch_matrix(static_cast<std::size_t>(Grammar::Terminal::ENUM_ENTRY_COUNT), rules.size(), Parser::INVALID_RULE_REFERENCE_ID), lookup_table(), stack()
    , active_node(&this->tree.root()), error_stream(error_stream), recovery(false), valid(true) {

    if(rules.size() == 0) throw NoStartStateException("Parser::Parser(const Vector<Grammar::Rule>&)");

    this->stack.push_back({{rules[0].variable()}});
    this->init_branch_matrix(rules);
}


bool Parser::process(const Token& token) {
    TokenType type = token.get_token_type();

    while(true) {
        this->cleanup_stack();
        if (this->is_stack_empty()) {
            this->handle_unexpected_token(token);
            return this->valid = false;
        }

        const Grammar::Value& stack_value = this->stack_rule_peek();

        if(stack_value.is_terminal() && type == stack_value.terminal()) {
            this->stack_rule_pop();
            this->active_node->create_child(token);
            this->recovery = false;
            return true;
        }
        else if(this->has_rule(stack_value, type)) {
            Grammar::Value value = this->stack_rule_pop();
            this->active_node = &(this->active_node->create_child(value.variable()));
            this->stack_push_rule(value.variable(), type);
        }
        else {
            this->handle_unexpected_token(token);
            return this->valid = false;
        }
    }
}


bool Parser::finalize() {
    const static Token DUMMY_TOKEN(0, 0, TokenType::EPSILON);

    this->error_stream->flush();

    if(this->valid) {
        while(this->process(DUMMY_TOKEN));

        if(this->is_stack_empty()) return this->valid = true;
        else {
            this->handle_unexpected_token(DUMMY_TOKEN, true);
            return false;
        }
    }
    return false;
}


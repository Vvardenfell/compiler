#include "make_code.h"
#include "exception.h"
#include <cstdio>
#include <cinttypes>


const Token& MakeCode::get_token(const node_type& node) const {
    return node.userdata().token();
}


const String& MakeCode::get_lexem(const node_type& node) const {
    return *this->get_token(node).value.information->lexem;
}


long MakeCode::get_integer(const node_type& node) const {
    return this->get_token(node).value.integer;
}


String MakeCode::generate_label() const {
    const static std::size_t LABEL_ID_BITS = 32;
    const static std::size_t BITS_PER_DECIMAL_DIGIT = 3;
    const static std::size_t LABEL_MEMORY_REQUIREMENT = (LABEL_ID_BITS / BITS_PER_DECIMAL_DIGIT) + 2;
    const static std::size_t LABEL_PREFIX_LENGTH = 5;
    static char label_storage[LABEL_PREFIX_LENGTH + LABEL_MEMORY_REQUIREMENT] = "label";

    static std::uint32_t label_id = 0;
    std::snprintf(label_storage + LABEL_PREFIX_LENGTH, LABEL_MEMORY_REQUIREMENT, "%" PRIu32, label_id++);

    if(!label_id) throw LabelsExhaustedException("MakeCode::generate_label() const");

    return String(label_storage);
}


FundamentalType MakeCode::get_data_type(const node_type& node) const {
    const Parser::TreeData& userdata = node.userdata();

    if(userdata.is_token()) return userdata.token().get_data_type();
    return node.get_data_type();
}


void MakeCode::code_prog(const node_type& prog) const {
    this->code_decls(prog.child(0));
    this->code_statements(prog.child(1));
    *this->code_stream << "STP";
}


void MakeCode::code_decls(const node_type& decls) const {
    if(decls.child_count()) {
        this->code_decl(decls.child(0));
        this->code_decls(decls.child(2));
    }
}


void MakeCode::code_decl(const node_type& decl) const {
    *this->code_stream << "DS $" << this->get_lexem(decl.child(2));
    this->code_array(decl.child(1));
}


void MakeCode::code_array(const node_type& array) const {
    if(array.child_count()) *this->code_stream << ' ' << this->get_integer(array.child(1)) << '\n';
    else *this->code_stream << " 1\n";
}


void MakeCode::code_statements(const node_type& statements) const {
    if(statements.child_count()) {
        this->code_statement(statements.child(0));
        this->code_statements(statements.child(2));
    }
    else *this->code_stream << "NOP\n";
}


void MakeCode::code_statement(const node_type& statement) const {
    TokenType token_type = this->get_token(statement.child(0)).get_token_type();

    switch(token_type) {
    case TokenType::IDENTIFIER: this->code_statement_identifier(statement); break;
    case TokenType::WRITE: this->code_statement_write(statement); break;
    case TokenType::READ: this->code_statement_read(statement); break;
    case TokenType::CURLY_BRACKET_OPEN: this->code_statement_curly_bracket_open(statement); break;
    case TokenType::IF: this->code_statement_if(statement); break;
    case TokenType::WHILE: this->code_statement_while(statement); break;
    default: throw UnsupportedTokenTypeException("MakeCode::code_statement(const node_type& statement)", token_type);
    }
}


void MakeCode::code_statement_identifier(const node_type& statement) const {
    this->code_exp(statement.child(3));
    *this->code_stream << "LA $" << this->get_lexem(statement.child(0)) << '\n';
    this->code_index(statement.child(1));
    *this->code_stream << "STR\n";
}


void MakeCode::code_statement_write(const node_type& statement) const {
    this->code_exp(statement.child(2));
    *this->code_stream << "PRI\n";
}


void MakeCode::code_statement_read(const node_type& statement) const {
    *this->code_stream << "REA\n";
    *this->code_stream << "LA $" << this->get_lexem(statement.child(2)) << '\n';
    this->code_index(statement.child(3));
    *this->code_stream << "STR\n";
}


void MakeCode::code_statement_curly_bracket_open(const node_type& statement) const {
    this->code_statements(statement.child(1));
}


void MakeCode::code_statement_if(const node_type& statement) const {
    this->code_exp(statement.child(2));

    String label0(this->generate_label());
    *this->code_stream << "JIN #" << label0 << '\n';

    this->code_statement(statement.child(4));

    String label1(this->generate_label());
    *this->code_stream << "JMP #" << label1 << '\n';
    *this->code_stream << "#" << label0 << '\n';
    *this->code_stream << "NOP\n";

    this->code_statement(statement.child(6));

    *this->code_stream << "#" << label1 << '\n';
    *this->code_stream << "NOP\n";
}


void MakeCode::code_statement_while(const node_type& statement) const {
    String label0(this->generate_label());
    *this->code_stream << "#" << label0 << '\n';
    *this->code_stream << "NOP\n";

    this->code_exp(statement.child(2));

    String label1(this->generate_label());
    *this->code_stream << "JIN #" << label1 << '\n';

    this->code_statement(statement.child(4));

    *this->code_stream << "JMP #" << label0 << '\n';
    *this->code_stream << "#" << label1 << '\n';
    *this->code_stream << "NOP\n";
}


void MakeCode::code_exp(const node_type& exp) const {
    if(this->get_data_type(exp.child(1)) == FundamentalType::NONE) this->code_exp2(exp.child(0));
    else {
        TokenType op_exp_op_terminal_type = this->get_token(exp.child(1).child(0).child(0)).get_token_type();

        if(op_exp_op_terminal_type == TokenType::GREATER_THAN) {
            this->code_op_exp(exp.child(1));
            this->code_exp2(exp.child(0));
            *this->code_stream << "LES\n";
        }
        else {
            this->code_exp2(exp.child(0));
            this->code_op_exp(exp.child(1));
            if(op_exp_op_terminal_type == TokenType::WHATEVER) *this->code_stream << "NOT\n";
        }
    }
}


void MakeCode::code_index(const node_type& index) const {
    if(index.child_count()) {
        this->code_exp(index.child(1));
        *this->code_stream << "ADD\n";
    }
}


void MakeCode::code_exp2(const node_type& exp2) const {
    TokenType token_type = this->get_token(exp2.child(0)).get_token_type();

    switch(token_type) {
    case TokenType::PARENTHESIS_OPEN: this->code_exp2_parenthesis_open(exp2); break;
    case TokenType::IDENTIFIER: this->code_exp2_identifier(exp2); break;
    case TokenType::INTEGER: this->code_exp2_integer(exp2); break;
    case TokenType::MINUS: this->code_exp2_minus(exp2); break;
    case TokenType::NOT: this->code_exp2_not(exp2); break;
    default: throw UnsupportedTokenTypeException("MakeCode::code_exp2(const node_type& exp2)", token_type);
    }
}


void MakeCode::code_exp2_parenthesis_open(const node_type& exp2) const {
    this->code_exp(exp2.child(1));
}


void MakeCode::code_exp2_identifier(const node_type& exp2) const {
    *this->code_stream << "LA $" << this->get_lexem(exp2.child(0)) << '\n';
    this->code_index(exp2.child(1));
    *this->code_stream << "LV\n";
}


void MakeCode::code_exp2_integer(const node_type& exp2) const {
    *this->code_stream << "LC " << this->get_integer(exp2.child(0)) << '\n';
}


void MakeCode::code_exp2_minus(const node_type& exp2) const {
    *this->code_stream << "LC 0\n";
    this->code_exp2(exp2.child(1));
    *this->code_stream << "SUB\n";
}


void MakeCode::code_exp2_not(const node_type& exp2) const {
    this->code_exp2(exp2.child(1));
    *this->code_stream << "NOT\n";
}


void MakeCode::code_op_exp(const node_type& op_exp) const {
    if(op_exp.child_count()) {
        this->code_exp(op_exp.child(1));
        this->code_op(op_exp.child(0));
    }
}


void MakeCode::code_op(const node_type& op) const {
    TokenType token_type = this->get_token(op.child(0)).get_token_type();

    switch(token_type) {
    case TokenType::PLUS: *this->code_stream << "ADD\n"; break;
    case TokenType::MINUS: *this->code_stream << "SUB\n"; break;
    case TokenType::ASTERISK: *this->code_stream << "MUL\n"; break;
    case TokenType::COLON: *this->code_stream << "DIV\n"; break;
    case TokenType::LESS_THAN: *this->code_stream << "LES\n"; break;
    case TokenType::GREATER_THAN: break; // replaced by less than operation
    case TokenType::EQUALITY: *this->code_stream << "EQU\n"; break;
    case TokenType::WHATEVER: *this->code_stream << "EQU\n"; break; // complemented by a not, to invert the result
    case TokenType::LOGICAL_AND: *this->code_stream << "AND\n"; break;
    default: throw UnsupportedTokenTypeException("MakeCode::code_op(const node_type& op)", token_type);
    }
}


MakeCode::MakeCode(const ParseTree<Parser::TreeData>* parse_tree, std::ostream* code_stream) : parse_tree(parse_tree), code_stream(code_stream) {}


void MakeCode::operator()() const {
    this->code_prog(this->parse_tree->root().child(0));
    this->code_stream->flush();
}

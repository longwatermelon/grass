#include "parser.h"
#include <fstream>
#include <sstream>
#include <iostream>


plugin::Parser::Parser(const std::string& plugin_path)
{
    std::ifstream ifs(plugin_path);

    std::stringstream ss;
    std::string line;
    while (std::getline(ifs, line)) ss << line << "\n";

    ifs.close();

    m_lexer = Lexer(ss.str());
    m_current_token = m_lexer.get_next_token();
    m_prev_token = m_current_token;
}


void plugin::Parser::eat(TokenType type)
{
    if (m_current_token.type == type)
    {
        m_prev_token = m_current_token;
        m_current_token = m_lexer.get_next_token();
    }
    else
    {
        std::stringstream ss;
        ss << "Unexpected token '" << m_current_token.value << "' at line " << m_lexer.line_num();
        throw std::runtime_error(ss.str());
    }
}


std::unique_ptr<plugin::Node> plugin::Parser::parse()
{
    std::unique_ptr<Node> root = std::make_unique<Node>(NodeType::COMPOUND);
    root->compound_value.emplace_back(parse_expr());

    while (m_lexer.current_index() < m_lexer.contents().size())
    {
        eat(TokenType::SEMI);

        std::unique_ptr<Node> expr = parse_expr();

        if (!expr.get())
            break;

        root->compound_value.emplace_back(std::move(expr));
    }

   return root; 
}


std::unique_ptr<plugin::Node> plugin::Parser::parse_expr()
{
    switch (m_current_token.type)
    {
        case TokenType::STRING: return parse_string();
        case TokenType::INT: return parse_int();
        case TokenType::ID: return parse_id();
    }

    return 0;
}


std::unique_ptr<plugin::Node> plugin::Parser::parse_string()
{
    std::unique_ptr<Node> string = std::make_unique<Node>(NodeType::STRING);
    string->string_value = m_current_token.value;

    eat(TokenType::STRING);
    return string;
}


std::unique_ptr<plugin::Node> plugin::Parser::parse_int()
{
    std::unique_ptr<Node> integer = std::make_unique<Node>(NodeType::INT);
    integer->int_value = std::stoi(m_current_token.value);

    eat(TokenType::INT);
    return integer;
}


std::unique_ptr<plugin::Node> plugin::Parser::parse_id()
{
    if (m_current_token.value == "set")
        return parse_variable_definition();
    else
        return parse_variable();
}


std::unique_ptr<plugin::Node> plugin::Parser::parse_function_call()
{
    std::unique_ptr<Node> node = std::make_unique<Node>(NodeType::FUNCTION_CALL);
    node->function_call_name = m_prev_token.value;

    eat(TokenType::LPAREN);
    node->function_call_args.emplace_back(parse_expr());

    while (m_current_token.type != TokenType::RPAREN)
    {
        eat(TokenType::COMMA);
        node->function_call_args.emplace_back(parse_expr());
    }

    eat(TokenType::RPAREN);
    return node;
}


std::unique_ptr<plugin::Node> plugin::Parser::parse_variable()
{
    std::string value = m_current_token.value;
    eat(TokenType::ID);

    if (m_current_token.type == TokenType::LPAREN)
    {
        return parse_function_call();
    }

    std::unique_ptr<Node> variable = std::make_unique<Node>(NodeType::VARIABLE);
    variable->variable_name = value;

    return variable;
}


std::unique_ptr<plugin::Node> plugin::Parser::parse_variable_definition()
{
    eat(TokenType::ID);
    std::string name = m_current_token.value;
    eat(TokenType::ID);
    eat(TokenType::EQUALS);

    std::unique_ptr<Node> value = parse_expr();

    std::unique_ptr<Node> variable_def = std::make_unique<Node>(NodeType::VARIABLE_DEFINITION);
    variable_def->variable_definition_name = name;
    variable_def->variable_definition_value = std::move(value);

    return variable_def; 
}


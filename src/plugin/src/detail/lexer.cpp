#include "lexer.h"
#include <iostream>


plugin::Lexer::Lexer(const std::string& contents)
    : m_contents(contents)
{
}


void plugin::Lexer::advance()
{
    if (m_current_index < m_contents.size())
    {
        ++m_current_index;
        m_current_char = m_contents[m_current_index];
    }
}


std::string plugin::Lexer::collect_string()
{
    advance();

    size_t start = m_current_index;

    while (m_current_char != '"' && m_current_char != '\n')
        advance();

    advance();

    return m_contents.substr(start, m_current_index - start - 1);
}


std::string plugin::Lexer::collect_int()
{
    size_t start = m_current_index;

    while (isdigit(m_current_char) && m_current_char != '\n')
        advance();

   return m_contents.substr(start, m_current_index - start); 
}


std::string plugin::Lexer::collect_id()
{
    size_t start = m_current_index;

    while (isalnum(m_current_char) && m_current_char != '\n')
        advance();

    return m_contents.substr(start - 1, m_current_index - start + 1);
}


Token plugin::Lexer::get_next_token()
{
    while (m_current_index < m_contents.size())
    {
        while (isspace(m_current_char) && m_current_char != '\n')
            advance();

        if (m_current_char == '"')
            return { TokenType::STRING, collect_string() };

        if (isdigit(m_current_char))
            return { TokenType::INT, collect_int() }; 

        if (isalnum(m_current_char))
            return { TokenType::ID, collect_id() };

        switch (m_current_char)
        {
            case ';': return advance_w_token({ TokenType::SEMI, ";" }); break;
            case '(': return advance_w_token({ TokenType::LPAREN, "(" }); break;
            case ')': return advance_w_token({ TokenType::RPAREN, ")" }); break;
            case ',': return advance_w_token({ TokenType::COMMA, "," }); break;
            case '=': return advance_w_token({ TokenType::EQUALS, "=" }); break;
        }

        ++m_current_line_num;
        advance();
    }

    return { TokenType::EOF_, "" };
}


Token plugin::Lexer::advance_w_token(Token t)
{
    advance();
    return t;
}


#pragma once
#include "token.h"
#include <string>


namespace plugin
{
    class Lexer
    {
    public:
        Lexer() = default;
        Lexer(const std::string& contents);

        void advance();

        std::string collect_string();
        std::string collect_int();
        std::string collect_id();

        Token get_next_token();

        Token advance_w_token(Token t);

        size_t line_num() { return m_current_line_num + 1; }
        size_t current_index() { return m_current_index; }
        std::string contents() { return m_contents; }

    private:
        char m_current_char{ ' ' };
        size_t m_current_index{ 0 };
        std::string m_contents;
        size_t m_current_line_num{ 0 };
    };
}


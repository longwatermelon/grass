#pragma once
#include "lexer.h"
#include "node.h"
#include <string>


namespace plugin
{
    class Parser
    {
    public:
        Parser(const std::string& plugin_path);

        void eat(TokenType type);

        std::unique_ptr<Node> parse();
        std::unique_ptr<Node> parse_expr();

        std::unique_ptr<Node> parse_string();
        std::unique_ptr<Node> parse_int();
        std::unique_ptr<Node> parse_id();

        std::unique_ptr<Node> parse_function_call();

    private:
        Token m_current_token;
        Token m_prev_token;
        Lexer m_lexer;
    };
}

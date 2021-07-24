#pragma once
#include <string>


enum class TokenType
{
    ID,
    LPAREN,
    RPAREN,
    SEMI,
    STRING,
    INT,
    COMMA,
    EQUALS,
    EOF_ 
};

struct Token
{
    TokenType type;
    std::string value;
};

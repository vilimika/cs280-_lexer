#ifndef TOKEN_STRINGS_H_
#define TOKEN_STRINGS_H_

#include <map>
#include <string>
#include <iostream>


std::map<TokenTypes, std::string> token_names =
{
    {TokenTypes::ID,  "ID"},
    {TokenTypes::ICONST, "ICONST"},
    {TokenTypes::FCONST, "FCONST"},
    {TokenTypes::STRING, "STRING"},
    {TokenTypes::PRINT, "PRINT"},
    {TokenTypes::SET, "SET"},
    {TokenTypes::PLUS, "PLUS"},
    {TokenTypes::MINUS, "MINUS"},
    {TokenTypes::STAR, "STAR"},
    {TokenTypes::COMMA, "COMMA"},
    {TokenTypes::LBR, "LBR"},
    {TokenTypes::RBR, "RBR"},
    {TokenTypes::LSQ, "LSQ"},
    {TokenTypes::RSQ, "RSQ"},
    {TokenTypes::LPAREN, "LPAREN"},
    {TokenTypes::RPAREN, "RPAREN"},
    {TokenTypes::SC, "SC"},
    {TokenTypes::ERR, "ERR"},
    {TokenTypes::DONE, "DONE"}
};
#endif // TOKEN_STRINGS_H

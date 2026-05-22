#pragma once

#include "exception.hpp"
#include "machine-code.hpp"
#include <cstddef>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace translator {
enum TokenType {
  INT,
  EQU,
  SMICOL,
  OUT,
  IDENTIFIER,
  R_BRACKET,
  L_BRACKET,
  R_BRACE,
  L_BRACE,
  COLOMN,
  STRING_LITIRAL,
  INT_LITIRAL
};

inline std::vector<std::function<void()>> generationFucntions;

inline std::map<std::string, TokenType> Token_Tree = {
    {"int", TokenType::INT},     {"=", TokenType::EQU},
    {";", TokenType::SMICOL},    {"print", TokenType::OUT},
    {")", TokenType::R_BRACKET}, {"(", TokenType::L_BRACKET},
    {"}", TokenType::R_BRACE},   {"{", TokenType::L_BRACE},
    {":", TokenType::COLOMN}};

inline bool isSymbol(char c) {
  return c == ';' || c == '=' || c == ')' || c == '(' || c == '}' || c == '{' ||
         c == ';';
}

inline bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

   char * p;
   strtol(s.c_str(), &p, 10);

   return (*p == 0);
}


class Token {
public:
  std::string strToken;
  int token_type;

  Token(const std::string &val) : strToken(val) {
    if (Token_Tree.find(val) != Token_Tree.end()) {
      token_type = Token_Tree[val];
    } else if (val.starts_with('\"') && val.ends_with('\"') || val.starts_with('\'') && val.ends_with('\'')) {
      token_type = TokenType::STRING_LITIRAL;
    } else if (isInteger(val)) {
      token_type = TokenType::INT_LITIRAL;
    } else {
      token_type = TokenType::IDENTIFIER;
    }
  }
};

// typedef std::vector<Token> Token_map;

class Token_map : public std::vector<Token> {

  public:
    size_t current_i = 0;

    Token_map(std::vector<Token> &token_map) : std::vector<Token>(token_map) {}
    Token_map() {}

    Token current() {
      return (*this)[current_i];
    }

    Token advance() {
      current_i++;
      return (*this)[current_i];
    }

    Token back() {
      current_i--;
      return (*this)[current_i];
    }

    Token expect(translator::TokenType expected) {
      current_i++;
      Token actuall = (*this)[current_i];
      if (actuall.token_type != expected) {
        throw CompilerErr("expected token type: " + std::to_string(expected));
      }
      return actuall;
    }

    Token peek(int step) {
      current_i += step;
      Token actuall = (*this)[current_i];
      current_i -= step;
      return actuall;
    }

    bool isAtEnd() {
      return current_i == (*this).size();
    }

};

inline std::ostream &operator<<(std::ostream &os, const Token &t) {
  os << t.strToken;
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const Token_map &tm) {
  for (Token t : tm) {
    os << t << "\n";
  }

  return os;
}

inline Token_map getTokens(std::ifstream &FILE) {
  char c;
  std::string token;
  Token_map tokens;

  while (FILE.get(c)) {
    if (std::isspace(c)) {
      if (!token.empty()) {
        tokens.emplace_back(token);
        token.clear();
      }

      continue;
    }

    if (isSymbol(c)) {

      if (!token.empty()) {
        tokens.emplace_back(token);
        token.clear();
      }

      tokens.emplace_back(std::string(1, c));
      continue;
    }
    token += c;
  }

  if (!token.empty()) {
    tokens.emplace_back(token);
  }
  if (tokens.empty()) {
    // throw CompilerErr(Errors::NO_CONTENT_EXP);
    std::ifstream::failure("No content in file");
  }

  return tokens;
}

inline translator::Token expect(const translator::Token_map &tokens,
                                translator::TokenType expected_type,
                                size_t &i) {
  if (i >= tokens.size()) {
    throw std::out_of_range("Token index out of range");
  }

  if (tokens[i].token_type != expected_type) {
    throw std::runtime_error("Unexpected token: " + tokens[i].strToken +
                             std::to_string(expected_type));
  }

  return tokens[i];
}

} // namespace translator
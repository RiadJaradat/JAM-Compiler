#pragma once

#include "exception.hpp"
#include "machine-code.hpp"
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
  COLOMN
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

class Token {
public:
  std::string strToken;
  int token_type;

  Token(const std::string &val) : strToken(val) {
    if (Token_Tree.find(val) != Token_Tree.end()) {
      token_type = Token_Tree[val];
    } else {
      token_type = TokenType::IDENTIFIER;
    }
  }
};

typedef std::vector<Token> Token_map;

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

inline void parse(const translator::Token_map &tokens, std::map<std::string, size_t> labels, mc::x86_64::machine_code bytes) {
  for (size_t i = 0; i < tokens.size(); i++) {
    const translator::Token &t = tokens[i];

    switch (t.token_type) {
    case translator::TokenType::INT:
      break;
    case translator::TokenType::SMICOL:
      break;
    case translator::TokenType::OUT:
      i++;
      translator::expect(tokens, translator::TokenType::L_BRACKET, i);
      i++;
      translator::expect(tokens, translator::TokenType::R_BRACKET, i);
      i++;
      translator::expect(tokens, translator::TokenType::SMICOL, i);
      i++;

      break;

    case translator::TokenType::IDENTIFIER: {
      std::string ident_name = tokens[i].strToken;
      i++;
      expect(tokens, translator::TokenType::COLOMN, i);

      for (size_t j = i; j < tokens.size(); i++) {

        labels[ident_name] = bytes.size();
        
      }

      break;
    }

    default:
      throw CompilerErr("unexpected token: " + t.strToken);
    }
  }
}

} // namespace translator
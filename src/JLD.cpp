

#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>

#include "exception.hpp"
#include "front_end.hpp"
#include "AST.hpp"

std::ifstream file("input.j");

int main() {

  translator::Token_map tm = translator::getTokens(file);

  while (!tm.isAtEnd()) {

    switch (tm.current().token_type) {
      case translator::TokenType::OUT: {

        tm.expect(translator::TokenType::L_BRACKET);
        translator::Token value = tm.expect(translator::TokenType::STRING_LITIRAL);
        tm.expect(translator::TokenType::R_BRACKET);

        stringExpr* string = new stringExpr(value.strToken);
        printStmt* print = new printStmt(string);
        tm.expect(translator::TokenType::SMICOL);
        tm.advance();
      }
      case translator::TokenType::INT: {
        translator::Token name = tm.expect(translator::TokenType::IDENTIFIER);
        tm.expect(translator::TokenType::EQU);
        translator::Token val = tm.expect(translator::TokenType::INT_LITIRAL);

        intLitiralExpr* intval = new intLitiralExpr(std::stoi(val.strToken));
        intStmt* intsmt = new intStmt(name.strToken, intval);

      }

      default: {
        throw CompilerErr("unexpected token: " + tm.current().strToken + " \n  * at word position: " + std::to_string(tm.current_i));
      }
    }

  }

  

}
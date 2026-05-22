#pragma once

#include <string>

struct Node {
  virtual ~Node() = default;
};

struct Expr : Node {};

struct Stmt : Node {};

struct stringExpr : Expr {
  std::string value;

  stringExpr(std::string &val) : value(val) {}
};

struct printStmt : Stmt {
  Expr *expr;

  printStmt(Expr *e) : expr(e) {}
};

struct intLitiralExpr : Expr {
  int value;

  intLitiralExpr(int val) : value(val) {}
};

struct intStmt : Stmt {
  std::string ident_name;
  Expr *expr;

  intStmt(std::string &ident, Expr *e) : expr(e), ident_name(ident) {}
};
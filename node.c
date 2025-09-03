#include "mamecc.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *nd = malloc(sizeof(Node));
  nd->kind = kind;
  nd->lhs = lhs;
  nd->rhs = rhs;
  return nd;
}

Node *new_node_num(int num) {
  Node *nd = malloc(sizeof(Node));
  nd->kind = ND_NUM;
  nd->lhs = NULL;
  nd->rhs = NULL;
  nd->num = num;
  return nd;
}

Node *new_node_ident(LocalVarList *vl, char *ident) {
  Node *nd = malloc(sizeof(Node));
  nd->kind = ND_IDENT;
  nd->lhs = NULL;
  nd->rhs = NULL;
  LocalVarList *v = is_registered(vl, ident);
  if (!v) v = new_var_ident(vl, ident);
  nd->offset = v->offset;
  return nd;
}

// program = stmt*
void program(Node **ndlist, Token **tk, LocalVarList *vl) {
  int i = 0;
  while (!at_eof(*tk)) {
    ndlist[i] = stmt(tk, vl);
    ++i;
  }
  ndlist[i] = NULL;
}

/* 
  stmt  = expr ";" 
          | "if" "(" expr ")" stmt ("else" stmt)?
          | "while" "(" expr ")" stmt
          | "for" "(" expr? ";" expr? ";" expr? ")" stmt
          | "return" expr ";"
*/
Node *stmt(Token **tk, LocalVarList *vl) {
  if (!at_eof(*tk) && peek(*tk, "if")) {
    advance(tk);
    expect_consume(tk, "(");
    Node *nd_if_expr = expr(tk, vl);
    expect_consume(tk, ")");
    Node *nd_if_stmt = stmt(tk, vl);

    if (!at_eof(*tk) && peek(*tk, "else")) {
      advance(tk);
      Node *nd_else_stmt = stmt(tk, vl);
      return new_node(ND_IF_ELSE, new_node(ND_PHONY, nd_if_expr, nd_if_stmt), nd_else_stmt);
    }

    return new_node(ND_IF, nd_if_expr, nd_if_stmt);
  }
  else if (!at_eof(*tk) && peek(*tk, "while")) {
    advance(tk);
    expect_consume(tk, "(");
    Node *nd_while_expr = expr(tk, vl);
    expect_consume(tk, ")");
    Node *nd_while_stmt = stmt(tk, vl);
    return new_node(ND_WHILE, nd_while_expr, nd_while_stmt);
  }
  else if (!at_eof(*tk) && peek(*tk, "for")) {
    advance(tk);
    expect_consume(tk, "(");

    Node *nd_for_begin = NULL;
    if (!at_eof(*tk) && !peek(*tk, ";")) {
      nd_for_begin = expr(tk, vl);
    }
    expect_consume(tk, ";");

    Node *nd_for_end = NULL;
    if (!at_eof(*tk) && !peek(*tk, ";")) {
      nd_for_end = expr(tk, vl);
    }
    expect_consume(tk, ";");

    Node *nd_for_upd = NULL;
    if (!at_eof(*tk) && !peek(*tk, ")")) {
      nd_for_upd = expr(tk, vl);
    }
    expect_consume(tk, ")");

    Node *nd_for_code = stmt(tk, vl);

    Node *nd_for_cond = new_node(ND_PHONY, nd_for_begin, nd_for_end);
    Node *nd_for_code_upd = new_node(ND_PHONY, nd_for_code, nd_for_upd);

    return new_node(ND_FOR, nd_for_cond, nd_for_code_upd);
  }
  else if (!at_eof(*tk) && peek(*tk, "return")) {
    advance(tk);
    Node *nd = new_node(ND_RETURN, expr(tk, vl), NULL);
    expect_consume(tk, ";");
    return nd;
  }
  else {
    Node *nd = expr(tk, vl);
    expect_consume(tk, ";");
    return nd;
  }
}

// expr = assign
Node *expr(Token **tk, LocalVarList *vl) {
  return assign(tk, vl);
}

// assign = equality ("=" assign)?
Node *assign(Token **tk, LocalVarList *vl) {
  Node *nd = equality(tk, vl);

  if (!at_eof(*tk) && peek(*tk, "=")) {
    advance(tk);
    nd = new_node(ND_ASSIGN, nd, assign(tk, vl));
  }

  return nd;
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality(Token **tk, LocalVarList *vl) {
  Node *nd = relational(tk, vl);

  while (!at_eof(*tk) && (peek(*tk, "==") || peek(*tk, "!="))) {
    char *op = consume(tk);
    if (strncmp(op, "==", 2) == 0) {
      nd = new_node(ND_EQ, nd, relational(tk, vl));
    }
    else {
      nd = new_node(ND_NEQ, nd, relational(tk, vl));
    }
  }

  return nd;
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational(Token **tk, LocalVarList *vl) {
  Node *nd = add(tk, vl);

  while (
    !at_eof(*tk) && 
    (peek(*tk, "<") || peek(*tk, "<=") || peek(*tk, ">") || peek(*tk, ">="))
  ) {
    char *op = consume(tk);
    if (strncmp(op, "<=", 2) == 0) {
      nd = new_node(ND_LE, nd, relational(tk, vl));
    }
    else if (strncmp(op, ">=", 2) == 0) {
      nd = new_node(ND_LE, relational(tk, vl), nd);
    }
    else if (strncmp(op, "<", 1) == 0) {
      nd = new_node(ND_LT, nd, relational(tk, vl));
    }
    else {
      nd = new_node(ND_LT, relational(tk, vl), nd);
    }
  }

  return nd;
}

// add = mul ("+" mul | "-" mul)
Node *add(Token **tk, LocalVarList *vl) {
  Node *nd = mul(tk, vl);

  while (!at_eof(*tk) && (peek(*tk, "+") || peek(*tk, "-"))) {
    char *op = consume(tk);
    if (strncmp(op, "+", 1) == 0) {
      nd = new_node(ND_ADD, nd, mul(tk, vl));
    }
    else {
      nd = new_node(ND_SUB, nd, mul(tk, vl));
    }
  }

  return nd;
}

// mul = unary ("*" unary | "/" unary)*
Node *mul(Token **tk, LocalVarList *vl) {

  Node *nd = unary(tk, vl);

  while (!at_eof(*tk) && (peek(*tk, "*") || peek(*tk, "/"))) {
    char *op = consume(tk);

    if (strncmp(op, "*", 1) == 0) {
      nd = new_node(ND_MUL, nd, unary(tk, vl));
    }
    else {
      nd = new_node(ND_DIV, nd, unary(tk, vl));
    }
  }

  return nd;
}

// unary = ("+" | "-")? primary
Node *unary(Token **tk, LocalVarList *vl) {
  if (!at_eof(*tk) && (peek(*tk, "+") || peek(*tk, "-"))) {
    char *op = consume(tk);
    if (strncmp(op, "+", 1) == 0) {
      return new_node(ND_ADD, new_node_num(0), primary(tk, vl));
    }
    else {
      return new_node(ND_SUB, new_node_num(0), primary(tk, vl));
    }
  }

  return primary(tk, vl);
}

// primary = num | ident | "(" expr ")"
Node *primary(Token **tk, LocalVarList *vl) {
  if (peek_kind(*tk, TK_NUM)) {
    int num = consume_num(tk);
    return new_node_num(num);
  }
  else if (peek_kind(*tk, TK_IDENT)) {
    char *ident = consume_ident(tk);
    return new_node_ident(vl, ident);
  }

  expect_consume(tk, "(");
  Node *nd = expr(tk, vl);
  expect_consume(tk, ")");

  return nd;
}
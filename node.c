#include "mamecc.h"
#include <stddef.h>

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

// expr = equality
Node *expr(Token **tk) {
  return equality(tk);
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality(Token **tk) {
  Node *nd = relational(tk);

  while (!at_eof(*tk) && (peek(*tk, "==") || peek(*tk, "!="))) {
    char *op = consume(tk);
    if (strncmp(op, "==", 2) == 0) {
      nd = new_node(ND_EQ, nd, relational(tk));
    }
    else {
      nd = new_node(ND_NEQ, nd, relational(tk));
    }
  }

  return nd;
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational(Token **tk) {
  Node *nd = add(tk);

  while (
    !at_eof(*tk) && 
    (peek(*tk, "<") || peek(*tk, "<=") || peek(*tk, ">") || peek(*tk, ">="))
  ) {
    char *op = consume(tk);
    if (strncmp(op, "<=", 2) == 0) {
      nd = new_node(ND_LE, nd, relational(tk));
    }
    else if (strncmp(op, ">=", 2) == 0) {
      nd = new_node(ND_LE, relational(tk), nd);
    }
    else if (strncmp(op, "<", 1) == 0) {
      nd = new_node(ND_LT, nd, relational(tk));
    }
    else {
      nd = new_node(ND_LT, relational(tk), nd);
    }
  }

  return nd;
}

// add = mul ("+" mul | "-" mul)
Node *add(Token **tk) {
  Node *nd = mul(tk);

  while (!at_eof(*tk) && (peek(*tk, "+") || peek(*tk, "-"))) {
    char *op = consume(tk);
    if (strncmp(op, "+", 1) == 0) {
      nd = new_node(ND_ADD, nd, mul(tk));
    }
    else {
      nd = new_node(ND_SUB, nd, mul(tk));
    }
  }

  return nd;
}

// mul = unary ("*" unary | "/" unary)*
Node *mul(Token **tk) {

  Node *nd = unary(tk);

  while (!at_eof(*tk) && (peek(*tk, "*") || peek(*tk, "/"))) {
    char *op = consume(tk);

    if (strncmp(op, "*", 1) == 0) {
      nd = new_node(ND_MUL, nd, unary(tk));
    }
    else {
      nd = new_node(ND_DIV, nd, unary(tk));
    }
  }

  return nd;
}

// unary = ("+" | "-")? primary
Node *unary(Token **tk) {
  if (!at_eof(*tk) && (peek(*tk, "+") || peek(*tk, "-"))) {
    char *op = consume(tk);
    if (strncmp(op, "+", 1) == 0) {
      return new_node(ND_ADD, new_node_num(0), primary(tk));
    }
    else {
      return new_node(ND_SUB, new_node_num(0), primary(tk));
    }
  }

  return primary(tk);
}

// primary = num | "(" expr ")"
Node *primary(Token **tk) {
  if (peek_kind(*tk, TK_NUM)) {
    int num = consume_num(tk);
    return new_node_num(num);
  }

  expect_consume(tk, "(");
  Node *nd = expr(tk);
  expect_consume(tk, ")");

  return nd;
}
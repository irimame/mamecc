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

Node *new_node_ident(LocalIdentList **vl, char *ident, LocalIdentKind kind) {
  Node *nd = malloc(sizeof(Node));
  nd->kind = ND_IDENT;
  nd->lhs = NULL;
  nd->rhs = NULL;
  LocalIdentList *v = is_registered(*vl, ident);
  if (!v) v = new_var_ident(vl, ident, kind);
  nd->offset = v->offset;
  return nd;
}

Node *new_node_block(Node **stmt_vec) {
  Node *nd = malloc(sizeof(Node));
  nd->kind = ND_BLOCK;
  nd->lhs = NULL;
  nd->rhs = NULL;
  nd->stmt_vec = stmt_vec;
  return nd;
}

Node *new_node_funccall(char *funcname, Node **param_vec, size_t len_param_vec) {
  Node *nd = malloc(sizeof(Node));
  nd->kind = ND_FUNCCALL;
  nd->lhs = NULL;
  nd->rhs = NULL;
  nd->funcname = funcname;
  nd->param_vec = param_vec;
  nd->len_param_vec = len_param_vec;
  return nd;
}

Node *new_node_funcdef(char *funcname, Node *block, LocalIdentList *lil, char **lcl_arg_list, size_t len_lcl_arg_list) {
  Node *nd = malloc(sizeof(Node));
  nd->kind = ND_FUNCDEF;
  nd->lhs = NULL;
  nd->rhs = NULL;
  nd->funcname = funcname;
  nd->block = block;
  nd->lcl_ident_list = lil;
  nd->lcl_arg_list = lcl_arg_list;
  nd->len_lcl_arg_list = len_lcl_arg_list;
  return nd;
}

// program = funcdef*
void program(Node **ndlist, Token **tk, LocalIdentList **vl) {
  int i = 0;
  while (!at_eof(*tk)) {
    vl[i] = init_varlist();
    ndlist[i] = funcdef(tk, &vl[i]);
    ++i;
  }
  ndlist[i] = NULL;
  vl[i] = NULL;
}

// funcdef = ident "(" (ident ("," ident)*)? ")" block
Node *funcdef(Token **tk, LocalIdentList **vl) {
  char *funcname = consume_ident(tk);
  expect_consume(tk, "(");
  char **lcl_arg_list = (char **)malloc(sizeof(char *) * 100); 
  size_t i = 0;
  while (!at_eof(*tk) && !peek(*tk, ")")) {
    if (i > 0) expect_consume(tk, ",");
    lcl_arg_list[i] = consume_ident(tk);
    ++i;
  }
  lcl_arg_list[i] = NULL;
  expect_consume(tk, ")");
  return new_node_funcdef(funcname, block(tk, vl), *vl, lcl_arg_list, i);
}

// block = "{" stmt* "}"
Node *block(Token **tk, LocalIdentList **vl) {
  expect_consume(tk, "{");
  Node **stmt_vec = (Node **)malloc(sizeof(Node *) * 100); 
  size_t i = 0;
  while (!at_eof(*tk) && !peek(*tk, "}")) {
    stmt_vec[i] = stmt(tk, vl);

    ++i;
  }

  stmt_vec[i] = NULL;
  expect_consume(tk, "}");
  return new_node_block(stmt_vec);
}

/* 
  stmt  = expr ";" 
          | block
          | "if" "(" expr ")" stmt ("else" stmt)?
          | "while" "(" expr ")" stmt
          | "for" "(" expr? ";" expr? ";" expr? ")" stmt
          | "return" expr ";"
  block = "{" stmt* "}"
*/
Node *stmt(Token **tk, LocalIdentList **vl) {
  if (!at_eof(*tk) && peek(*tk, "{")) {
    return block(tk, vl);
  }
  else if (!at_eof(*tk) && peek(*tk, "if")) {
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
Node *expr(Token **tk, LocalIdentList **vl) {
  return assign(tk, vl);
}

// assign = equality ("=" assign)?
Node *assign(Token **tk, LocalIdentList **vl) {
  Node *nd = equality(tk, vl);

  if (!at_eof(*tk) && peek(*tk, "=")) {
    advance(tk);
    nd = new_node(ND_ASSIGN, nd, assign(tk, vl));
  }

  return nd;
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality(Token **tk, LocalIdentList **vl) {
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
Node *relational(Token **tk, LocalIdentList **vl) {
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
Node *add(Token **tk, LocalIdentList **vl) {
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
Node *mul(Token **tk, LocalIdentList **vl) {

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
Node *unary(Token **tk, LocalIdentList **vl) {
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

/* primary  = num 
            | ident ("(" (expr ("," expr)*)? ")")?
            | "(" expr ")"
*/
Node *primary(Token **tk, LocalIdentList **vl) {
  if (peek_kind(*tk, TK_NUM)) {
    int num = consume_num(tk);
    return new_node_num(num);
  }
  else if (peek_kind(*tk, TK_IDENT)) {
    char *ident = consume_ident(tk);
    if (!at_eof(*tk) && peek(*tk, "(")) {
      advance(tk);
      Node **arg_vec = (Node **)malloc(sizeof(Node *) * 100); 
      size_t i = 0;
      while (!at_eof(*tk) && !peek(*tk, ")")) {
        if (i > 0) expect_consume(tk, ",");
        arg_vec[i] = expr(tk, vl);
        ++i;
      }
      arg_vec[i] = NULL;
      expect_consume(tk, ")");
      return new_node_funccall(ident, arg_vec, i);
    }
    return new_node_ident(vl, ident, LCL_VAR);
  }

  expect_consume(tk, "(");
  Node *nd = expr(tk, vl);
  expect_consume(tk, ")");

  return nd;
}
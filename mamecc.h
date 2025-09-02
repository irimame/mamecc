#ifndef _MAMECC_H_
#define _MAMECC_H_

#include <stddef.h>
#include <stdbool.h>

/* Token */
typedef enum {
  TK_NUM,
  TK_SYMBOL,
  TK_IDENT,
  TK_RETURN,
  TK_EOF
} TokenKind;

typedef struct Token Token;
struct Token {
  TokenKind kind;
  char *val;
  size_t len;
  Token *next;
};


/* Node */
typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_EQ,
  ND_NEQ,
  ND_LT,
  ND_LE,
  ND_ASSIGN,
  ND_RETURN,
  ND_NUM,
  ND_IDENT
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int num;
  int offset;
};


/* Varlist */
typedef struct Varlist Varlist;
struct Varlist {
  char *ident;
  int offset;
  Varlist *next;
};


/* Token */
Token *tokenize(char *p);
Token *new_token(Token *cur, TokenKind kind, char *val);
bool at_eof(Token *tk);
bool peek(Token *tk, char *expect);
bool peek_kind(Token *tk, TokenKind kind);
void advance(Token **p_tk);
char *consume(Token **tk);
int consume_num(Token **tk);
char *consume_ident(Token **tk);
void expect_consume(Token **tk, char *expect);
void print_tokens(Token *tk);


/* Node */
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int num);
Node *new_node_ident(Varlist *vl, char *ident);
void program(Node **ndlist, Token **tk, Varlist *vl);
Node *stmt(Token **tk, Varlist *vl);
Node *expr(Token **tk, Varlist *vl);
Node *assign(Token **tk, Varlist *vl);
Node *equality(Token **tk, Varlist *vl);
Node *relational(Token **tk, Varlist *vl);
Node *add(Token **tk, Varlist *vl);
Node *mul(Token **tk, Varlist *vl);
Node *unary(Token **tk, Varlist *vl);
Node *primary(Token **tk, Varlist *vl);


/* codegen */
void node_to_code(Node *nd);
void node_to_code_lhs(Node *nd);


/* varlist */
Varlist *init_varlist();
Varlist *new_var_ident(Varlist *vl, char *ident);
Varlist *is_registered(Varlist *vl, char *ident);
int get_offset(Varlist *vl, char *ident);
Varlist *tail_of_varlist(Varlist *vl);

#endif
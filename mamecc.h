#ifndef _MAMECC_H_
#define _MAMECC_H_

#include <stddef.h>
#include <stdbool.h>

/* Token */
typedef enum {
  TK_NUM,
  TK_SYMBOL,
  TK_EOF
} TokenKind;

typedef struct Token Token;
struct Token {
  TokenKind kind;
  char *val;
  size_t len;
  Token *next;
};

Token *tokenize(char *p);
Token *new_token(Token *cur, TokenKind kind, char *val);
bool at_eof(Token *tk);
bool peek(Token *tk, char *expect);
bool peek_kind(Token *tk, TokenKind kind);
void advance(Token **p_tk);
char *consume(Token **tk);
int consume_num(Token **tk);
void expect_consume(Token **tk, char *expect);


/* Node */
typedef enum {
  ND_NUM,
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_EQ,
  ND_NEQ,
  ND_LT,
  ND_LE
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int num;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int num);
Node *expr(Token **tk);
Node *equality(Token **tk);
Node *relational(Token **tk);
Node *add(Token **tk);
Node *mul(Token **tk);
Node *unary(Token **tk);
Node *primary(Token **tk);
void node_to_code(Node *nd);

#endif
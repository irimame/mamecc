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
  TK_IF,
  TK_ELSE,
  TK_WHILE,
  TK_FOR,
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
  ND_IF_ELSE,
  ND_IF,
  ND_WHILE,
  ND_FOR,
  ND_BLOCK,
  ND_NUM,
  ND_IDENT,
  ND_PHONY
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int num;
  size_t offset;
  Node **stmt_vec;
};


/* LocalVarList */
typedef struct LocalVarList LocalVarList;
struct LocalVarList {
  char *ident;
  size_t offset;
  LocalVarList *next;
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
Node *new_node_ident(LocalVarList *vl, char *ident);
Node *new_node_block(Node **stmt_vec);
void program(Node **ndlist, Token **tk, LocalVarList *vl);
Node *stmt(Token **tk, LocalVarList *vl);
Node *expr(Token **tk, LocalVarList *vl);
Node *assign(Token **tk, LocalVarList *vl);
Node *equality(Token **tk, LocalVarList *vl);
Node *relational(Token **tk, LocalVarList *vl);
Node *add(Token **tk, LocalVarList *vl);
Node *mul(Token **tk, LocalVarList *vl);
Node *unary(Token **tk, LocalVarList *vl);
Node *primary(Token **tk, LocalVarList *vl);


/* codegen */
void node_to_code(Node *nd);
void node_to_code_lhs(Node *nd);


/* LocalVarList */
LocalVarList *init_varlist(void);
LocalVarList *new_var_ident(LocalVarList *vl, char *ident);
LocalVarList *is_registered(LocalVarList *vl, char *ident);
size_t get_offset(LocalVarList *vl, char *ident);
LocalVarList *tail_of_varlist(LocalVarList *vl);
size_t get_num_vars(LocalVarList *vl);

#endif
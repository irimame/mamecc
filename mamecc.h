#ifndef _MAMECC_H_
#define _MAMECC_H_

#include <stddef.h>
#include <stdbool.h>

typedef struct Token Token;
typedef struct Node Node;
typedef struct LocalIdentList LocalIdentList;

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
  ND_FUNCCALL,
  ND_FUNCDEF,
  ND_NUM,
  ND_IDENT,
  ND_PHONY
} NodeKind;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int num;
  size_t offset;
  Node **stmt_vec;
  char *funcname;
  Node **param_vec;
  size_t len_param_vec;
  Node *block;
  LocalIdentList *lcl_ident_list;
  char **lcl_arg_list;
  size_t len_lcl_arg_list;
};


/* LocalIdentList */
typedef enum {
  LCL_ARG,
  LCL_VAR,
  LCL_NULL
} LocalIdentKind;

struct LocalIdentList {
  char *ident;
  size_t offset;
  LocalIdentKind kind;
  LocalIdentList *next;
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
Node *new_node_ident(LocalIdentList **vl, char *ident, LocalIdentKind kind);
Node *new_node_block(Node **stmt_vec);
Node *new_node_funccall(char *funcname, Node **param_vec, size_t len_param_vec);
Node *new_node_funcdef(char *funcname, Node *block, LocalIdentList *lil, char **lcl_arg_list, size_t len_lcl_arg_list);
void program(Node **ndlist, Token **tk, LocalIdentList **vl);
Node *funcdef(Token **tk, LocalIdentList **vl);
Node *block(Token **tk, LocalIdentList **vl);
Node *stmt(Token **tk, LocalIdentList **vl);
Node *expr(Token **tk, LocalIdentList **vl);
Node *assign(Token **tk, LocalIdentList **vl);
Node *equality(Token **tk, LocalIdentList **vl);
Node *relational(Token **tk, LocalIdentList **vl);
Node *add(Token **tk, LocalIdentList **vl);
Node *mul(Token **tk, LocalIdentList **vl);
Node *unary(Token **tk, LocalIdentList **vl);
Node *primary(Token **tk, LocalIdentList **vl);

/* codegen */
void node_to_code(Node *nd, LocalIdentList *lil);
void node_to_code_lhs(Node *nd);


/* LocalIdentList */
LocalIdentList *init_varlist(void);
LocalIdentList *new_var_ident(LocalIdentList **vl, char *ident, LocalIdentKind kind);
LocalIdentList *is_registered(LocalIdentList *vl, char *ident);
size_t get_offset(LocalIdentList *vl, char *ident);
size_t get_num_idents(LocalIdentList *vl, LocalIdentKind kind);

#endif
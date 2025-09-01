#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

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


int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Error: The number of arguments is incorrect\n");
    return 1;
  }

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  /* tokenize */
  Token *token = tokenize(argv[1]);

  /* parse */
  // move to the next of head
  advance(&token);
  Node *node = expr(&token);


  /* codegen */
  node_to_code(node);
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}

Token *tokenize(char *p) {
  Token *token;
  Token head;
  head.next = NULL;
  token = &head;
  Token *cur = token;

  while (*p) {
    if (isspace(*p)) {
      ++p;
      continue;
    }

    if (strncmp(p, "==", 2) == 0) {
      cur = new_token(cur, TK_SYMBOL, "==");
      p += 2;
    }
    else if (strncmp(p, "!=", 2) == 0) {
      cur = new_token(cur, TK_SYMBOL, "!=");
      p += 2;
    }
    else if (strncmp(p, "<=", 2) == 0) {
      cur = new_token(cur, TK_SYMBOL, "<=");
      p += 2;
    }
    else if (strncmp(p, ">=", 2) == 0) {
      cur = new_token(cur, TK_SYMBOL, ">=");
      p += 2;
    }
    else if (*p == '<') {
      cur = new_token(cur, TK_SYMBOL, "<");
      ++p;
    }
    else if (*p == '>') {
      cur = new_token(cur, TK_SYMBOL, ">");
      ++p;
    }
    else if (*p == '+') {
      cur = new_token(cur, TK_SYMBOL, "+");
      ++p;
    }
    else if (*p == '-') {
      cur = new_token(cur, TK_SYMBOL, "-");
      ++p;
    }
    else if (*p == '*') {
      cur = new_token(cur, TK_SYMBOL, "*");
      ++p;
    }
    else if (*p == '/') {
      cur = new_token(cur, TK_SYMBOL, "/");
      ++p;
    }
    else if (*p == '(') {
      cur = new_token(cur, TK_SYMBOL, "(");
      ++p;
    }
    else if (*p == ')') {
      cur = new_token(cur, TK_SYMBOL, ")");
      ++p;
    }
    else if (isdigit(*p)) {
      long num = strtol(p, &p, 10);
      char *numstr = malloc(sizeof(char) * 128);
      sprintf(numstr, "%ld", num);
      cur = new_token(cur, TK_NUM, numstr);
    }
    else {
      fprintf(stderr, "Error: Unexpected char '%c'\n", *p);
    }
  }
  cur = new_token(cur, TK_EOF, "");

  return token;
}

Token *new_token(Token *cur, TokenKind kind, char *val) {
  Token *tk = malloc(sizeof(Token));
  tk->kind = kind;
  tk->val = val;
  tk->len = strlen(val);
  tk->next = NULL;
  cur->next = tk;
  return tk;
}

bool at_eof(Token *tk) {
  return (tk->kind == TK_EOF);
}

bool peek(Token *tk, char *expect) {
  return (strncmp(tk->val, expect, tk->len) == 0);
}

bool peek_kind(Token *tk, TokenKind kind) {
  return (tk->kind == kind);
}

void advance(Token **p_tk) {
  (*p_tk) = (*p_tk)->next;
}

char *consume(Token **tk) {
  char *val = (*tk)->val;
  advance(tk);
  return val;
}

int consume_num(Token **tk) {
  if ((*tk)->kind == TK_NUM) {
    int num = atoi((*tk)->val);
    advance(tk);
    return num;
  } 
  fprintf(stderr, "Error: Expected a number, but not a number\n");
  exit(1);
}

void expect_consume(Token **tk, char *expect) {
  if(strncmp((*tk)->val, expect, strlen(expect)) == 0) {
    advance(tk);
    return;
  } 
  fprintf(stderr, "Error: Unexpected token '%s'\n", (*tk)->val);
  exit(1);
}


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

void node_to_code(Node *nd) {
  if (nd == NULL) return;

  if (nd->kind == ND_NUM) {
    printf("  push %d\n", nd->num);
    return;
  }

  node_to_code(nd->lhs);
  node_to_code(nd->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");
  if (nd->kind == ND_ADD) {
    printf("  add rax, rdi\n");
  }
  else if (nd->kind == ND_SUB) {
    printf("  sub rax, rdi\n");
  }
  else if (nd->kind == ND_MUL) {
    printf("  imul rax, rdi\n");
  }
  else if (nd->kind == ND_DIV) {
    printf("  cqo\n");
    printf("  idiv rdi\n");
  }
  else if (nd->kind == ND_EQ) {
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
  }
  else if (nd->kind == ND_NEQ) {
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
  }
  else if (nd->kind == ND_LT) {
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
  }
  else if (nd->kind == ND_LE) {
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
  }

  printf("  push rax\n");
}
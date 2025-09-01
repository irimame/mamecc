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

void advance(Token **p_tk) {
  (*p_tk) = (*p_tk)->next;
}

int consume_num(Token **tk) {
  if ((*tk)->kind == TK_NUM) {
    int num = atoi((*tk)->val);
    advance(tk);
    return num;
  } 
  fprintf(stderr, "Error: Expected a number, but not a number\n");
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Error: The number of arguments is incorrect\n");
    return 1;
  }

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  Token *token;
  Token head;
  head.next = NULL;
  token = &head;
  Token *cur = token;

  char *p = argv[1];
  while (*p) {
    if (isspace(*p)) {
      ++p;
      continue;
    }

    printf("  mov rax, %ld\n", strtol(p, &p, 10));
    break;
  }

  /* tokenize */
  while (*p) {
    if (isspace(*p)) {
      ++p;
      continue;
    }

    if (*p == '+') {
      cur = new_token(cur, TK_SYMBOL, "+");
      ++p;
    }
    else if (*p == '-') {
      cur = new_token(cur, TK_SYMBOL, "-");
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

  /* codegen */
  // move to the next of head
  advance(&token);

  while (!at_eof(token)) {
    if (peek(token, "+")) {
      advance(&token);
      printf("  add rax, %d\n", consume_num(&token));
    }
    else if (peek(token, "-")) {
      advance(&token);
      printf("  sub rax, %d\n", consume_num(&token));
    }
    else {
      fprintf(stderr, "Error: Unknown kind of token\n");
    }
  }

  printf("  ret\n");
  return 0;
}
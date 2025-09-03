#include "mamecc.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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
    else if (*p == ';') {
      cur = new_token(cur, TK_SYMBOL, ";");
      ++p;
    }
    else if (*p == '=') {
      cur = new_token(cur, TK_SYMBOL, "=");
      ++p;
    }
    else if (*p == '{') {
      cur = new_token(cur, TK_SYMBOL, "{");
      ++p;
    }
    else if (*p == '}') {
      cur = new_token(cur, TK_SYMBOL, "}");
      ++p;
    }
    else if (isalpha(*p)) {
      char *q = p;
      while (isalnum(*q) || *q == '_') ++q;
      char *alnumstr = malloc(sizeof(char) * (q - p + 1));
      strncpy(alnumstr, p, q - p);
      if (strncmp(alnumstr, "return", 6) == 0) {
        cur = new_token(cur, TK_RETURN, "return");
      }
      else if (strncmp(alnumstr, "if", 2) == 0) {
        cur = new_token(cur, TK_IF, "if");
      }
      else if (strncmp(alnumstr, "else", 4) == 0) {
        cur = new_token(cur, TK_ELSE, "else");
      }
      else if (strncmp(alnumstr, "while", 5) == 0) {
        cur = new_token(cur, TK_WHILE, "while");
      }
      else if (strncmp(alnumstr, "for", 3) == 0) {
        cur = new_token(cur, TK_FOR, "for");
      }
      else {
        cur = new_token(cur, TK_IDENT, alnumstr);
      }
      p = q;
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

  // move to the first token
  advance(&token);
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
  return (strncmp(tk->val, expect, strlen(expect)) == 0);
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

char *consume_ident(Token **tk) {
  if ((*tk)->kind == TK_IDENT) {
    char *ident = (*tk)->val;
    advance(tk);
    return ident;
  } 
  fprintf(stderr, "Error: Expected an identifier, but not an identifier\n");
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

void print_tokens(Token *tk) {
  while (!at_eof(tk)) {
    printf("token: %s, len: %ld, type: %d\n", tk->val, tk->len, tk->kind);
    tk = tk -> next;
  }
}
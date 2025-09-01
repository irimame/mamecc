#include "mamecc.h"
#include <stdio.h>

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
  Node *node = expr(&token);

  /* codegen */
  node_to_code(node);
  printf("  pop rax\n");
  printf("  ret\n");

  return 0;
}
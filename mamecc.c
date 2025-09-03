#include "mamecc.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Error: The number of arguments is incorrect\n");
    return 1;
  }

  /* tokenize */
  Token *token = tokenize(argv[1]);

  /* parse */
  LocalVarList *varlist = init_varlist();
  Node *nodelist[100];
  program(nodelist, &token, varlist);

  /* codegen */
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // prologue
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %ld\n", get_num_vars(varlist) * 8);

  size_t i = 0;
  while (nodelist[i]) {
    node_to_code(nodelist[i]);
    ++i;
    printf("  pop rax\n");
  }

  // epilogue
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");

  return 0;
}
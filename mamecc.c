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
  LocalIdentList *local_ident_list[100];
  Node *nodelist[100];
  program(nodelist, &token, local_ident_list);

  /* codegen */
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("\n");

  size_t i = 0;
  while (nodelist[i]) {
    node_to_code(nodelist[i], local_ident_list[i]);
    ++i;
  }



  return 0;
}
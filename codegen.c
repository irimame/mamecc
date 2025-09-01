#include "mamecc.h"
#include <stdio.h>

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
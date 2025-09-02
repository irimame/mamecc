#include "mamecc.h"
#include <stdio.h>
#include <stdlib.h>

void node_to_code(Node *nd) {
  if (nd == NULL) return;

  if (nd->kind == ND_NUM) {
    printf("  push %d\n", nd->num);
    return;
  }

  // a variable in RHS: push the value of the variable
  if (nd->kind == ND_IDENT) {
    printf("  mov rdi, rbp\n");
    printf("  add rdi, %d\n", nd->offset);
    printf("  mov rax, [rdi]\n"); 
    printf("  push rax\n");
    return;
  }

  if (nd->kind == ND_ASSIGN) {
    // a variable in LHS: push the address of the variable
    node_to_code_lhs(nd->lhs);
    node_to_code(nd->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n"); 
    return;
  }

  if (nd->kind == ND_RETURN) {
    node_to_code(nd->lhs);

    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
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

void node_to_code_lhs(Node *nd) {
  if (nd->kind != ND_IDENT) {
    fprintf(stderr, "Error: LHS of assign expr. must contain only variable\n");
    exit(1);
  }
  printf("  mov rax, rbp\n");
  printf("  add rax, %d\n", nd->offset);
  printf("  push rax\n"); 
}
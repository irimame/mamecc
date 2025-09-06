#include "mamecc.h"
#include <stdio.h>
#include <stdlib.h>

static size_t label_num = 0;

void node_to_code(Node *nd, LocalIdentList *lil) {
  if (nd == NULL) return;

  if (nd->kind == ND_NUM) {
    printf("  push %d\n", nd->num);
    return;
  }

  // a variable in RHS: push the value of the variable
  if (nd->kind == ND_IDENT) {
    printf("  mov r12, rbp\n");
    printf("  sub r12, %ld\n", nd->offset);
    printf("  mov rax, [r12]\n"); 
    printf("  push rax\n");
    return;
  }

  if (nd->kind == ND_ASSIGN) {
    // a variable in LHS: push the address of the variable
    node_to_code_lhs(nd->lhs);
    node_to_code(nd->rhs, lil);

    printf("  pop r12\n");
    printf("  pop rax\n");
    printf("  mov [rax], r12\n"); 
    printf("  push r12\n");
    return;
  }

  if (nd->kind == ND_RETURN) {
    node_to_code(nd->lhs, lil);

    // pop & epilogue
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  }

  if (nd->kind == ND_IF) {
    node_to_code(nd->lhs, lil);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lend%ld\n", label_num);
    node_to_code(nd->rhs, lil);
    printf(".Lend%ld:\n", label_num);
    printf("  push rax\n");
    ++label_num;
    return;
  }

  if (nd->kind == ND_IF_ELSE) {
    node_to_code(nd->lhs->lhs, lil);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lelse%ld\n", label_num);
    node_to_code(nd->lhs->rhs, lil);
    printf("  jmp .Lend%ld\n", label_num);
    printf(".Lelse%ld:\n", label_num);
    node_to_code(nd->rhs, lil);
    printf(".Lend%ld:\n", label_num);
    printf("  push rax\n");
    ++label_num;
    return;
  }

  if (nd->kind == ND_WHILE) {
    printf(".Lbegin%ld:\n", label_num);
    node_to_code(nd->lhs, lil);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lend%ld\n", label_num);
    node_to_code(nd->rhs, lil);
    printf("  jmp .Lbegin%ld\n", label_num);
    printf(".Lend%ld:\n", label_num);
    printf("  push rax\n");
    ++label_num;
    return;
  }

  if (nd->kind == ND_FOR) {
    node_to_code(nd->lhs->lhs, lil);
    printf(".Lbegin%ld:\n", label_num);
    node_to_code(nd->lhs->rhs, lil);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lend%ld\n", label_num);
    node_to_code(nd->rhs->lhs, lil);
    node_to_code(nd->rhs->rhs, lil);
    printf("  jmp .Lbegin%ld\n", label_num);
    printf(".Lend%ld:\n", label_num);
    printf("  push rax\n");
    ++label_num;
    return;
  }

  if (nd->kind == ND_BLOCK) {
    size_t i = 0;
    while (nd->stmt_vec[i]) {
      node_to_code(nd->stmt_vec[i], lil);
      printf("  pop rax\n");
      ++i;
    }
    printf("  push rax\n");
    return;
  }

  if (nd->kind == ND_FUNCCALL) {
    // TODO: store parameters 6..n to register/stack

    // alignment check
    //printf("  mov rax, rsp\n");
    //printf("  cqo\n");
    //printf("  mov rdi, 16\n");
    //printf("  div rdi\n");
    //printf("  cmp rdx, 0\n");
    //printf("  jne .Laligned%ld\n", label_num);
    //printf("  sub rsp, 8\n");
    //printf(".Laligned%ld:\n", label_num);

    // alignment restore
    //++label_num;

    // copy parameters 0..5 to registers
    if (nd->len_param_vec > 6) {
      fprintf(stderr, "Error: parameters more than 6 not supported yet!\n");
      exit(1);
    }
    size_t i = 0;
    while (i < nd->len_param_vec) {
      node_to_code(nd->param_vec[i], lil);
      switch (i)
      {
      case 0:
        printf("  pop rdi\n");
        break;
      case 1:
        printf("  pop rsi\n");
        break;
      case 2:
        printf("  pop rcx\n");
        break;
      case 3:
        printf("  pop rdx\n");
        break;
      case 4:
        printf("  pop r8\n");
        break;
      case 5:
        printf("  pop r9\n");
        break;
      }
      ++i;
    }

    // call
    printf("  call %s\n", nd->funcname);

    // push the return value to stack
    printf("  push rax\n");

    // TODO: remove parameters 6..n from stack

    ++label_num;
    return;
  }

  if (nd->kind == ND_FUNCDEF) {
    size_t num_arg = get_num_idents(lil, LCL_ARG);
    size_t num_var = get_num_idents(lil, LCL_VAR);

    // prologue
    printf("%s:\n", nd->funcname);
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %ld\n", (num_arg + num_var) * 8);

    // copy registers to stack
    if (num_arg > 6) {
      fprintf(stderr, "Error: arguments more than 6 not supported yet!\n");
      exit(1);
    }
    if (num_arg != nd->len_lcl_arg_list) {
      fprintf(
        stderr,
        "Error: len of argument list (%ld) is not equal to num of arguments (%ld)\n",
        nd->len_lcl_arg_list, num_arg
      );
      exit(1);
    }
    size_t i = 0;
    while (i < nd->len_lcl_arg_list) {
      printf("  mov rax, rbp\n");
      printf("  sub rax, %ld\n", get_offset(nd->lcl_ident_list, nd->lcl_arg_list[i]));
      //printf("  push rax\n");
      switch (i)
      {
      case 0:
        printf("  mov [rax], rdi\n");
        break;
      case 1:
        printf("  mov [rax], rsi\n");
        break;
      case 2:
        printf("  mov [rax], rcx\n");
        break;
      case 3:
        printf("  mov [rax], rdx\n");
        break;
      case 4:
        printf("  mov [rax], r8\n");
        break;
      case 5:
        printf("  mov [rax], r9\n");
        break;
      }
      ++i;
    }

    node_to_code(nd->block, lil);

    // pop & epilogue
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    printf("\n");
    return;
  }

  node_to_code(nd->lhs, lil);
  node_to_code(nd->rhs, lil);

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
  printf("  sub rax, %ld\n", nd->offset);
  printf("  push rax\n"); 
}
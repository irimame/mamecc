#include <iostream>
#include <cstdlib>
#include "tokenizer.h"
#include "syntax_tree.h"
#include "compilation_engine.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Error: The number of arguments must be 2" << std::endl;
    return 1;
  }

  char *p = argv[1];
  tokenizer tknizer(p);
  syntax_tree stree(tknizer);

  std::cout << ".intel_syntax noprefix" << std::endl;
  std::cout << ".globl main" << std::endl;
  std::cout << "main: " << std::endl;

  stree.execute();

  std::cout << "  pop rax" << std::endl;
  std::cout << "  ret" << std::endl;
  return 0;
}
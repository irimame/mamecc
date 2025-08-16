#include "compilation_engine.h"
#include <iostream>

compilation_engine::compilation_engine(tokenizer tknizer) 
: _tknizer(tknizer) {}

void compilation_engine::compile() {
  std::cout << ".intel_syntax noprefix" << std::endl;
  std::cout << ".globl main" << std::endl;
  std::cout << "main: " << std::endl;
  std::cout << "  mov rax, " << _tknizer.expect_number() << std::endl;

  while (!at_eof()) {
    token tk = _tknizer.peek_token();
    if (tk.val() == "+") {
      _tknizer.advance();
      std::cout << "  add rax, " << _tknizer.expect_number() << std::endl;
      continue;
    }

    if (tk.val() == "-") {
      _tknizer.advance();
      std::cout << "  sub rax, " << _tknizer.expect_number() << std::endl;
      continue;
    }

    std::cerr << "Error: Could not compile" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  std::cout << "  ret" << std::endl;
}

bool compilation_engine::at_eof() const {
  return (this->_tknizer.peek_token().kind() == token_kind::TK_EOF);
}
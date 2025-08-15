#include "compilation_engine.h"
#include <iostream>

compilation_engine::compilation_engine(tokenizer tknizer) 
: _tknizer(tknizer) {}

void compilation_engine::compile() {
  std::cout << ".intel_syntax noprefix" << std::endl;
  std::cout << ".globl main" << std::endl;
  std::cout << "main: " << std::endl;
  std::cout << "  mov rax, " << expect_number() << std::endl;

  while (!at_eof()) {
    if (peek('+')) {
      advance();
      std::cout << "  add rax, " << expect_number() << std::endl;
      continue;
    }

    if (peek('-')) {
      advance();
      std::cout << "  sub rax, " << expect_number() << std::endl;
      continue;
    }

    std::cerr << "Error: Could not compile" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  std::cout << "  ret" << std::endl;
}

bool compilation_engine::peek(char op) {
  token tk = this->_tknizer.peek_token();
  if (tk.kind() != token_kind::TK_SYMBOL || tk.val().at(0) != op)
    return false;
  return true;
}

void compilation_engine::advance() {
  this->_tknizer.advance();
}

void compilation_engine::expect(char op) {
  token tk = this->_tknizer.consume_token();
  if (tk.kind() != token_kind::TK_SYMBOL || tk.val().at(0) != op)
  {
    std::cerr << "Error: '" << op << "' expected" << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

int compilation_engine::expect_number() {
  token tk = this->_tknizer.consume_token();
  if (tk.kind() != token_kind::TK_NUM) {
    std::cerr << "Error: Not a number" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  return std::stoi(tk.val());
}

bool compilation_engine::at_eof() const {
  return (this->_tknizer.peek_token().kind() == token_kind::TK_EOF);
}
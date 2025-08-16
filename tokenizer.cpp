#include "tokenizer.h"
#include <cctype>
#include <iostream>

token::token(token_kind kind, std::string val)
: _kind(kind), _val(val) {}

const token_kind token::kind() const {
  return this->_kind;
}

const std::string& token::val() const {
  return this->_val;
}

tokenizer::tokenizer(char *p)
: token_index(0) {
  tokenize(p);
}

void tokenizer::tokenize(char *p) {
  while (*p) {
    if (isspace(*p)) {
      ++p;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
      token_list.push_back(token(token_kind::TK_SYMBOL, std::string(1, *p++)));
      continue;
    }
    
    if (isdigit(*p)) {
      token_list.push_back(token(token_kind::TK_NUM, std::to_string(std::strtol(p, &p, 10))));
      continue;
    }

    std::cerr << "Error: Could not tokenize" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  token_list.push_back(token(token_kind::TK_EOF, ""));
}

const token& tokenizer::consume_token() {
  return this->token_list.at(this->token_index++);
}

const token& tokenizer::peek_token() const {
  return this->token_list.at(this->token_index);
}

void tokenizer::expect(const std::string& symbol) {
  token tk = consume_token();
  if (tk.val() != symbol) {
    std::cerr << "Error: Expected '" << symbol << "', but got " << tk.val() << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

int tokenizer::expect_number() {
  token tk = consume_token();
  if (tk.kind() != token_kind::TK_NUM) {
    std::cerr << "Error: Not a number" << tk.val() << std::endl;
    std::exit(EXIT_FAILURE);
  }

  return std::stoi(tk.val());
}

void tokenizer::advance() {
  ++this->token_index;
}
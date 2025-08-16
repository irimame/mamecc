#include "syntax_tree.h"
#include <iostream>

node::node(
  node_kind kind, 
  std::shared_ptr<node> lhs, std::shared_ptr<node> rhs,
  int val
)
: _kind(kind), _lhs(lhs), _rhs(rhs), _val(val)
{}

const node_kind node::kind() const {
  return _kind;
}

const int node::val() const {
  return _val;
}

std::shared_ptr<node> node::lhs() {
  return _lhs;
}

std::shared_ptr<node> node::rhs() {
  return _rhs;
}

syntax_tree::syntax_tree(tokenizer tknizer)
: _tknizer(tknizer)
{}

void syntax_tree::execute() {
  root_node = expr();
  gen(root_node);
}

std::shared_ptr<node> syntax_tree::new_node(node_kind kind, std::shared_ptr<node> lhs, std::shared_ptr<node> rhs) {
  std::shared_ptr<node> nd
    = std::make_shared<node>(kind, lhs, rhs);
    return nd;
}

std::shared_ptr<node> syntax_tree::new_node_num(int val) {
  std::shared_ptr<node> nd
    = std::make_shared<node>(node_kind::ND_NUM, nullptr, nullptr, val);
  return nd;
}

// expr = mul ('+' mul | '-' mul)*
std::shared_ptr<node> syntax_tree::expr() {
  std::shared_ptr<node> nd = mul();

  for (;;) {
    token tk = _tknizer.peek_token();
    if (tk.kind() == token_kind::TK_SYMBOL && tk.val() == "+") {
      _tknizer.advance();
      nd = new_node(node_kind::ND_ADD, nd, mul());
    }
    else if (tk.kind() == token_kind::TK_SYMBOL && tk.val() == "-") {
      _tknizer.advance();
      nd = new_node(node_kind::ND_SUB, nd, mul());
    }
    else 
      return nd;
  }
}

// mul = primary ('*' primary | '/' primary)*
std::shared_ptr<node> syntax_tree::mul() {
  std::shared_ptr<node> nd = primary();

  for (;;) {
    token tk = _tknizer.peek_token();
    if (tk.kind() == token_kind::TK_SYMBOL && tk.val() == "*") {
      _tknizer.advance();
      nd = new_node(node_kind::ND_MUL, nd, primary());
    }
    else if (tk.kind() == token_kind::TK_SYMBOL && tk.val() == "/") {
      _tknizer.advance();
      nd = new_node(node_kind::ND_DIV, nd, primary());
    }
    else 
      return nd;
  }
}

// primary = num | '(' expr ')'
std::shared_ptr<node> syntax_tree::primary() {
  token tk = _tknizer.peek_token();
  if (tk.kind() == token_kind::TK_SYMBOL && tk.val() == "(") {
    _tknizer.advance();
    std::shared_ptr<node> nd = expr();
    _tknizer.expect(")");
    return nd;
  }

  _tknizer.advance();
  return new_node_num(std::stoi(tk.val()));
}


void syntax_tree::gen(std::shared_ptr<node> nodeptr) {
  if (nodeptr->kind() == node_kind::ND_NUM) {
    std::cout << "  push " << nodeptr->val() << std::endl;
    return;
  }

  gen(nodeptr->lhs());
  gen(nodeptr->rhs());

  std::cout << "  pop rdi" << std::endl;
  std::cout << "  pop rax" << std::endl;
  switch(nodeptr->kind()) {
    case node_kind::ND_ADD:
      std::cout << "  add rax, rdi" << std::endl;
      break;
    case node_kind::ND_SUB:
      std::cout << "  sub rax, rdi" << std::endl;
      break;
    case node_kind::ND_MUL:
      std::cout << "  imul rax, rdi" << std::endl;
      break;
    case node_kind::ND_DIV:
      std::cout << "  cqo" << std::endl;
      std::cout << "  idiv rdi" << std::endl;
      break;
  }
  std::cout << "  push rax" << std::endl;
}
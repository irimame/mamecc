#ifndef _syntax_tree_h_
#define _syntax_tree_h_

#include <memory>
#include "tokenizer.h"

enum class node_kind {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM
};

class node {
public:
  node(
    node_kind kind, 
    std::shared_ptr<node> lhs, std::shared_ptr<node> rhs, 
    int val = 0
  );
  const node_kind kind() const;
  const int val() const;
  std::shared_ptr<node> lhs();
  std::shared_ptr<node> rhs();

private:
  node_kind _kind;
  std::shared_ptr<node> _lhs;
  std::shared_ptr<node> _rhs;
  int _val;
};

class syntax_tree {
public:
  syntax_tree(tokenizer tknizer);
  void execute();
  std::shared_ptr<node> new_node(
    node_kind kind, std::shared_ptr<node> lhs, std::shared_ptr<node> rhs
  );
  std::shared_ptr<node> new_node_num(int val);
  std::shared_ptr<node> expr();
  std::shared_ptr<node> mul();
  std::shared_ptr<node> unary();
  std::shared_ptr<node> primary();
  void gen(std::shared_ptr<node> nodeptr);

private:
  std::shared_ptr<node> root_node;
  tokenizer _tknizer;
};

#endif
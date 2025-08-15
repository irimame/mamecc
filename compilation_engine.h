#ifndef _compilation_engine_h_
#define _compilation_engine_h_

#include "tokenizer.h"

class compilation_engine {
public:
  compilation_engine(tokenizer tknizer);
  void compile();

private:
  bool peek(char op);
  void advance();
  void expect(char op);
  int expect_number();
  bool at_eof() const;
  tokenizer _tknizer;
};

#endif
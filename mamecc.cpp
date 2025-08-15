#include <iostream>
#include <cstdlib>
#include "tokenizer.h"
#include "compilation_engine.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Error: The number of arguments must be 2" << std::endl;
    return 1;
  }

  char *p = argv[1];
  tokenizer tknizer(p);
  compilation_engine compile_engine(tknizer);
  compile_engine.compile();
  return 0;
}
#ifndef _tokenizer_h_
#define _tokenizer_h_

#include <string>
#include <vector>

enum class token_kind {
  TK_SYMBOL,
  TK_NUM,
  TK_EOF
};

class token {
public:
  token(token_kind kind, std::string val);
  const token_kind kind() const;
  const std::string& val() const;

private:
  token_kind _kind;
  std::string _val;
};

class tokenizer {
public:
  tokenizer(char *input_string);
  const token& consume_token();
  const token& peek_token() const;
  void advance();

private:
  void tokenize(char *input_string);
  std::vector<token> token_list;
  size_t token_index;
};

#endif
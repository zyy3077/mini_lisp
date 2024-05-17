#ifndef PARSE_H
#define PARSE_H
#include <deque>
#include <memory>
#include "./value.h"
#include "./tokenizer.h"

using ValuePtr = std::shared_ptr<Value>;
class Parser {
    std::deque<TokenPtr> tokens;
public:
    Parser(std::deque<TokenPtr> tokens);
    ValuePtr parse();
    ValuePtr parseTails();
};
    
#endif
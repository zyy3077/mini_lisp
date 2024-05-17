#ifndef EVAL_ENV_H
#define EVAL_ENV_H
#include "./value.h"
#include <unordered_map>
#include <string>

using ValuePtr = std::shared_ptr<Value>;

class EvalEnv {
    std::vector<ValuePtr> evalList(ValuePtr expr);
    std::unordered_map<std::string, ValuePtr> symbolMap{};
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);
public:
    EvalEnv();
    ValuePtr eval(ValuePtr expr);
    friend ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env);
    
};

#endif
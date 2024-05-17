#ifndef BUILTINS_H
#define BUILTINS_H
#include <vector>
#include <memory>
#include "./error.h"
#include "./value.h"
#include <unordered_map>

using ValuePtr = std::shared_ptr<Value>;
using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&);

//builtinfunc的具体实现和map在builtin.cpp中
extern const std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue>> builtinfuncs;


#endif
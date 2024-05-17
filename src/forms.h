#ifndef FORMS_H
#define FORMS_H
#include <unordered_map>
#include "./value.h"
#include "./eval_env.h"
#include "./error.h"

using ValuePtr = std::shared_ptr<Value>;
using SpecialFormType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);
extern const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS;


#endif
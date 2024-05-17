#include "./eval_env.h"
#include "./error.h"
#include "./builtins.h"
#include "./value.h"
#include "./forms.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <ranges> 

using namespace std::literals;

EvalEnv::EvalEnv() {
    //初始化时添加内置过程符号表
    symbolMap.insert(BUILTIN_FUNCS.begin(), BUILTIN_FUNCS.end());
}
//把expr转化为vector后的各项求值后插入result
std::vector<ValuePtr> EvalEnv::evalList(ValuePtr expr) {
    std::vector<ValuePtr> result;
    std::ranges::transform(expr->toVector(),
                           std::back_inserter(result),
                           [this](ValuePtr v) { return this->eval(v); });
    return result;
}
//调用内置过程
ValuePtr EvalEnv::apply(ValuePtr proc, std::vector<ValuePtr> args) {
    if (typeid(*proc) == typeid(BuiltinProcValue)) {
        auto builtin = std::dynamic_pointer_cast<BuiltinProcValue>(proc);
        auto func = builtin->getFunc();
        return func(args);
    } else {
        throw LispError("Unimplemented builtin function");
    }
}
//求值
ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (expr->isSeflEvaluating()) {
        return expr;
    } else if (expr->isNil()) {
        throw LispError("Evaluating nil is prohibited.");
    } 

    std::vector<ValuePtr> v = std::move(expr->toVector());
    //expr不是pair时，v为空
    if(v.size()) {
        auto pair = std::dynamic_pointer_cast<PairValue>(expr);
        if (auto name = pair->getCar()->asSymbol()) {
            //特殊形式
            if (SPECIAL_FORMS.find(*name) != SPECIAL_FORMS.end()) {
                return SPECIAL_FORMS.at(*name)(pair->getCdr()->toVector(), *this);
            } else { //内置过程
                ValuePtr proc = this->eval(v[0]);
                std::vector<ValuePtr> args = evalList(pair->getCdr()); //除了符号外，即右半部分
                return this->apply(proc, args); // 最后用 EvalEnv::apply 实现调用
            }
        } else {
            throw LispError("first argument should be symbol");
        }
    } else if (auto name = expr->asSymbol()) { //被define过的符号名
        auto value = symbolMap.find(*name);
        if (value != symbolMap.end()) {
            return value->second;
        } else {
            throw LispError("Variable " + *name + " not defined.");
        }
    } else {
        throw LispError("Unimplemented");
    }
} 
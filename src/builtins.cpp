#include "./builtins.h"
#include <iostream>

ValuePtr add(const std::vector<ValuePtr>& params) {
    int result = 0;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot add a non-numeric value.");
        }
        result += i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}
ValuePtr substract(const std::vector<ValuePtr>& params) {
    if (params.size() == 1) {
        if (!params[0]->isNumber()) {
            throw LispError("Cannot add a non-numeric value.");
        }
        return std::make_shared<NumericValue>(- params[0]->asNumber());
    } else if (params.size() == 2) {
        if (!params[0]->isNumber() || !params[1]->isNumber()) {
            throw LispError("Cannot add a non-numeric value.");
        }
        return std::make_shared<NumericValue>(params[0]->asNumber() - params[1]->asNumber());
    } else {
        throw LispError("too much arguments for substraction.");
    }
}
ValuePtr print(const std::vector<ValuePtr>& params) {
    if (params.size() > 1) throw LispError("too much arguments in print");
    std::cout << params[0]->toString() << '\n';
    return std::make_shared<NilValue>();
}
const std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue>> builtinfuncs = {
    {"+", std::make_shared<BuiltinProcValue>(&add)},
    {"print", std::make_shared<BuiltinProcValue>(&print)},
    {"-", std::make_shared<BuiltinProcValue>(&substract)}
    // 其他内置函数
};
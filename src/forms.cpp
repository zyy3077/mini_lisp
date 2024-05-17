#include "./forms.h"

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 2) {
        throw LispError("wrong argument num for define");
    } else if (auto name = args[0]->asSymbol()) {
        env.symbolMap[*name] = env.eval(args[1]);
        return std::make_shared<NilValue>();
    } else {
        throw LispError("Unimplemented");
    }
};
ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 1) {
        throw LispError("wrong argument num for quote");
    } else {
        return args[0];
    }
}
bool is_false(ValuePtr condition) {
    if (auto boolean = std::dynamic_pointer_cast<BooleanValue>(condition)) {
            if (boolean->getVal() == false) return true;
    }
    return false; 
}
ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 3) {
        throw LispError("wrong argument num for if");
    } else {
        auto condition = env.eval(args[0]);
        //如果condition是#f，求值第二个表达式，否则求值第一个
        if (is_false(condition)) {
            return env.eval(args[2]);
        } else {
            return env.eval(args[1]);
        }
    }
}
ValuePtr andForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() == 0) {
        return std::make_shared<BooleanValue>(true);
    }
    for (auto i : args) {
        auto condition = env.eval(i);
        if (is_false(condition)) {
            return std::make_shared<BooleanValue>(false);
        }
    }
    return env.eval(args[args.size() - 1]);//返回最后一个值
}
ValuePtr orForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() == 0) {
        return std::make_shared<BooleanValue>(false);
    }
    for (auto i : args) {
        auto condition = env.eval(i);
        if(!is_false(condition)) {
            return condition;//返回第一个不为#f的值
        }
    }
    return std::make_shared<BooleanValue>(false);
}

const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS = {
    {"define", defineForm}, 
    {"quote", quoteForm}, 
    {"if", ifForm}, 
    {"and", andForm}, 
    {"or", orForm}
    //其他特殊形式
};

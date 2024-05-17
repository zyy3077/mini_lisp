#include "./forms.h"

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (auto name = args[0]->asSymbol()) {
        env.symbolMap[*name] = env.eval(args[1]);
        return std::make_shared<NilValue>();
    } else {
        throw LispError("Unimplemented");
    }
};
ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() > 1) {
        throw LispError("too much arguments for quote");
    } else {
        return args[0];
    }
}
ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 3) {
        throw LispError("wrong argument num for if");
    } else {
        auto condition = env.eval(args[0]);
        //如果condition是#f，求值第二个表达式，否则求值第一个
        if (auto boolean = std::dynamic_pointer_cast<BooleanValue>(condition)) {
            if (boolean->getVal() == false) return env.eval(args[2]);
            else return env.eval(args[1]);
        } else {
            return env.eval(args[1]);
        }
    }
}

const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS = {
    {"define", defineForm}, 
    {"quote", quoteForm}, 
    {"if", ifForm}
    //其他特殊形式
};

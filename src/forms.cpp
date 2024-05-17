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

const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS = {
    {"define", defineForm}, 
    {"quote", quoteForm}
    //其他特殊形式
};

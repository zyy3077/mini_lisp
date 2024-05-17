#include "./forms.h"

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (auto name = args[0]->asSymbol()) {
        env.symbolMap[*name] = args[1];
        return std::make_shared<NilValue>();
    } else {
        throw LispError("Unimplemented");
    }
};
const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS = {
    {"define", defineForm}
};

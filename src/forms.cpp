#include "./forms.h"
#include <algorithm>
#include <iterator>
#include <ranges> 


ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 1) {
        throw LispError("wrong argument num for quote");
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
        if (condition->isFalse()) {
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
        if (condition->isFalse()) {
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
        if(!condition->isFalse()) {
            return condition;//返回第一个不为#f的值
        }
    }
    return std::make_shared<BooleanValue>(false);
}
ValuePtr labmdaForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    std::vector<std::string> params;
    std::ranges::transform(args[0]->toVector(),
                           std::back_inserter(params),
                           [](ValuePtr v) { return v->toString(); });//args[0]中各项转化为字符串后插入params中
    std::vector<ValuePtr> n_args(args.begin() + 1, args.end());//body
    return std::make_shared<LambdaValue>(params, n_args, env.shared_from_this());
}
ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    std::string name;
    ValuePtr value;
    if (args[0]->asSymbol()) {
        if (args.size() > 2) {
            throw LispError("too much operands for define");
        }
        name = args[0]->asSymbol().value();
        value = env.eval(args[1]);
        env.defineBinding(name, value);
        return std::make_shared<NilValue>();
    } else if (auto pair = std::dynamic_pointer_cast<PairValue>(args[0])) {
        std::vector<ValuePtr> lambdaArgs = {pair->getCdr()};//第一个元素为形参列表
        lambdaArgs.insert(lambdaArgs.end(), args.begin() + 1, args.end());//剩下的元素为表达式中剩下的元素
        name = pair->getCar()->toString();
        value = labmdaForm(lambdaArgs, env);
        env.defineBinding(name, value);
        return std::make_shared<NilValue>();
    } else {
        throw LispError("Unimplemented");
    }
};

const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS = {
    {"define", defineForm}, 
    {"quote", quoteForm}, 
    {"if", ifForm}, 
    {"and", andForm}, 
    {"or", orForm}, 
    {"lambda", labmdaForm}
    //其他特殊形式
};

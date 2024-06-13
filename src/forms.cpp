#include "./forms.h"
#include <algorithm>
#include <iterator>
#include <ranges> 
#include <iostream>

void numCheck(const std::vector<ValuePtr>& params, int expectedNum) {
    if (params.size() != expectedNum) {
        throw LispError("Incorrect number of arguments.");
    }
}
void paramsCheck(const std::vector<ValuePtr>& params, int expectedNum, Type expectedType) {
    numCheck(params, expectedNum);
    for (const auto& param : params) {
        if (param->getType() != expectedType) {
            throw LispError("Incorrect type of argument.");
        }
    }
}


ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    numCheck(args, 1);
    return args[0];
}
ValuePtr quasiquoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    numCheck(args, 1);
    if (typeid(*args[0]) != typeid(PairValue)) return args[0];
    auto pair = std::dynamic_pointer_cast<PairValue>(args[0]);
    ValuePtr car = pair->getCar();
    ValuePtr cdr = pair->getCdr();
    if (typeid(*car) == typeid(SymbolValue) && car->asSymbol() == "unquote") {
        if (auto cdrPair = std::dynamic_pointer_cast<PairValue>(cdr)) {
        return env.eval(cdrPair->getCar());
        } else return env.eval(cdr);
    } 
    car = quasiquoteForm({car}, env);
    cdr = quasiquoteForm({cdr}, env);
    return std::make_shared<PairValue>(car, cdr);    
}
ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    auto condition = env.eval(args[0]);
    if (args.size() == 3) {
        //如果condition是#f，求值第二个表达式，否则求值第一个
        if (condition->isFalse()) {
            return env.eval(args[2]);
        } else {
            return env.eval(args[1]);
        }
    } else if (args.size() == 2) { //实现可以接受忽略 ⟨⟨ 假分支 ⟩⟩ 的条件形式。此时，若 ⟨⟨ 条件 ⟩⟩ 求值为 虚值，则引发未定义行为。建议设置此时的求值结果为空表
        if (condition->isFalse()) {
            return std::make_shared<NilValue>();
        } else {
            return env.eval(args[1]);
        }
    } else {
        throw LispError("2 or 3 arguments expected but " + std::to_string(args.size()) + " were given in \"if\"");
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
    std::vector<ValuePtr> body(args.begin() + 1, args.end());//body
    return std::make_shared<LambdaValue>(params, body, env.shared_from_this());
}
ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    std::string name;
    ValuePtr value;
    if (args.size() < 2) {
        throw LispError("Incorrect number of arguments.");
    }
    if (args[0]->asSymbol()) {
        numCheck(args, 2);
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
        throw LispError("TypeError.");
    }
};
ValuePtr condForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() == 0) throw LispError("arg expected in \"cond\"");
    for (auto it = args.begin(); it != args.end(); ++it) {
        if ((*it)->isList()) {
            auto pair = std::dynamic_pointer_cast<PairValue>(*it);
            if (auto sym = std::dynamic_pointer_cast<SymbolValue>(pair->getCar())) {
                if(sym->asSymbol() == "else") {
                    if (it == args.end() - 1) {
                    auto cdr = std::dynamic_pointer_cast<PairValue>(pair->getCdr());
                    return env.eval(cdr->getCar());
                    } else {
                        throw LispError("\"else\" can only be at the end of \"cond\"");
                    }
                }
            }
            auto cond = env.eval(pair->getCar());
            if (!cond->isFalse()) {
                if (pair->getCdr()->isNil()) return cond;
                auto vec = pair->toVector();
                ValuePtr res = nullptr;
                for (auto i : vec) {
                    res = env.eval(i);
                }
                return res;
            }
        } else {
            throw LispError("pairValue expected in \"cond\"");
        }
    }
    throw LispError("all conditions are false");
}
ValuePtr beginForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    ValuePtr res = nullptr;
    for (auto arg : args) {
        res = env.eval(arg);
    }
    return res;
}
ValuePtr letForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    std::vector<std::string> params;
    std::vector<ValuePtr> arguments;
    std::vector<ValuePtr> body(args.begin() + 1, args.end());
    if (!args[0]->isList()) {
        throw LispError("first argument should be a list");
    }
    auto vec = std::dynamic_pointer_cast<PairValue>(args[0])->toVector(); //{(name, val), (name, val), ...}
    for (auto p : vec) {
        auto v = std::dynamic_pointer_cast<PairValue>(p)->toVector(); //{name, val}
        if (v.size() != 2) {
            throw LispError("a name should be bound to one val");
        }
        params.push_back(v[0]->toString());
        arguments.push_back(env.eval(v[1]));
    }
    auto lambda = std::make_shared<LambdaValue>(params, body, env.shared_from_this());
    return env.apply(lambda, arguments);
}


const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS = {
    {"define", defineForm}, 
    {"quote", quoteForm}, 
    {"if", ifForm}, 
    {"and", andForm}, 
    {"or", orForm}, 
    {"lambda", labmdaForm},
    {"cond", condForm},
    {"begin", beginForm},
    {"let", letForm}, 
    {"quasiquote",quasiquoteForm}, 
    //其他特殊形式
};

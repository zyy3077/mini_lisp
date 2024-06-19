#include "./builtins.h"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cmath>

void checkNum(const std::vector<ValuePtr>& params, int expectedNum) {
    if (params.size() != expectedNum) {
        throw LispError("Incorrect number of arguments.");
    }
}
void checkParams(const std::vector<ValuePtr>& params, int expectedNum, Type expectedType) {
    checkNum(params, expectedNum);
    for (const auto& param : params) {
        if (param->getType() != expectedType) {
            throw LispError("Incorrect type of argument.");
        }
    }
}


ValuePtr add(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkParams(params, params.size(), Type::Number);
    double result = 0;
    for (const auto& i : params) {
        result += i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}
ValuePtr substract(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 2) {
        checkParams(params, 2, Type::Number);
        return std::make_shared<NumericValue>(params[0]->asNumber() - params[1]->asNumber());
    } else if (params.size() == 1) {
        checkParams(params, 1, Type::Number);
        return std::make_shared<NumericValue>(-params[0]->asNumber());
    } else {
        throw LispError("Incorrect number of arguments.");
    }
    
}
ValuePtr multiply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkParams(params, params.size(), Type::Number);
    double result = 1;
    for (const auto& i : params) {
       result *= i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}
ValuePtr divide(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 1) {
        checkParams(params, 1, Type::Number);
        return std::make_shared<NumericValue>(1 / params[0]->asNumber());
    } else if (params.size() == 2) {
        checkParams(params, 2, Type::Number);
        if (params[1]->asNumber() == 0) {
            throw LispError("divided by zero");
        }
        return std::make_shared<NumericValue>(params[0]->asNumber() / params[1]->asNumber());
    } else {
        throw LispError("1 or 2 arguments expected but " + std::to_string(params.size()) + " were given in \"/\"");
    }
}
ValuePtr absolute(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkParams(params, 1, Type::Number);
    auto val = params[0]->asNumber();
    return val >= 0?  std::make_shared<NumericValue>(val) : std::make_shared<NumericValue>(-val);
}
ValuePtr expt(const std::vector<ValuePtr>& params, EvalEnv& env) { //不支持复数
    checkParams(params, 2, Type::Number);
    if (params[0]->asNumber() == 0 && params[1]->asNumber() == 0) {
        throw LispError("0 ^ 0 is undefined");
    } else {
        return std::make_shared<NumericValue>(std::pow(params[0]->asNumber(), params[1]->asNumber()));
    }
}
ValuePtr quotient(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkParams(params, 2, Type::Number);
    double val = params[0]->asNumber() / params[1]->asNumber();
    return std::make_shared<NumericValue>(int(val));
}
ValuePtr modulo(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkParams(params, 2, Type::Number);
    double x = params[0]->asNumber();
    double y = params[1]->asNumber();
    if (y == 0) {
        throw LispError("divided by zero");
    } else if (x * y >= 0) {
        return std::make_shared<NumericValue>(remainder(x, y));
    } else {
        return std::make_shared<NumericValue>(y + remainder(x, y));
    }
}
ValuePtr remain(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkParams(params, 2, Type::Number);
    double x = params[0]->asNumber();
    double y = params[1]->asNumber();
    if (y == 0) {
        throw LispError("divided by zero");
    } else {
        double rem = remainder(x, y);
        if(rem * x < 0) {
            if (x > 0) rem += abs(y);
            else rem -= abs(y);
        }
        return std::make_shared<NumericValue>(rem);
    }
}

ValuePtr equivalent(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkParams(params, 2, Type::Number);
    return std::make_shared<BooleanValue>(params[0]->asNumber() == params[1]->asNumber());
}
ValuePtr smaller(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkParams(params, 2, Type::Number);
    return std::make_shared<BooleanValue>(params[0]->asNumber() < params[1]->asNumber());
}
ValuePtr greater(const std::vector<ValuePtr>& params, EvalEnv& env) {
    // 现在你可以在booleanValue上使用逻辑运算符了
    auto small = std::dynamic_pointer_cast<BooleanValue>(smaller(params, env));
    auto equal = std::dynamic_pointer_cast<BooleanValue>(equivalent(params, env));
    return std::make_shared<BooleanValue>(!(*small || *equal));
}
ValuePtr greater_eq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto small = std::dynamic_pointer_cast<BooleanValue>(smaller(params, env));
    return std::make_shared<BooleanValue>(!(*small));
}
ValuePtr smaller_eq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto great = std::dynamic_pointer_cast<BooleanValue>(greater(params, env));
    return std::make_shared<BooleanValue>(!(*great));
}


ValuePtr print(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkNum(params, 1);
    std::cout << params[0]->toString() << '\n';
    return std::make_shared<NilValue>();
}
ValuePtr newline(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //向 标准输出 输出操作系统定义的换行符序列。
    //返回值：未定义；建议空表。
    checkNum(params, 0);
    std::cout << '\n';
    return std::make_shared<NilValue>();
}
ValuePtr display(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //( display val )
    //若 val 是字符串类型数据，则将字符串内容通过 标准输出 输出；
    //否则输出 val 的外部表示，实现可以在外部表示前添加单引号 '。
    //返回值：未定义；建议空表。
    checkNum(params, 1);
    if (auto string = std::dynamic_pointer_cast<StringValue>(params[0])) {
        std::cout << string->getVal();
    } else {
        std::cout << params[0]->toString();
    }
    return std::make_shared<NilValue>();
}
ValuePtr displayLn(const std::vector<ValuePtr>& params, EvalEnv& env) {
    display(params, env);
    std::cout << '\n';
    return std::make_shared<NilValue>();
}


ValuePtr apply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkNum(params, 2);
    return env.apply(params[0], params[1]->toVector());
}
ValuePtr eval(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkNum(params, 1);
    return env.eval(params[0]);
}
ValuePtr error(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkNum(params, 1);
    throw LispError(params[0]->toString());
}
ValuePtr exitFunc(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 0) {
        std::exit(0);
    }
    checkParams(params, 1 ,Type::Number);
    if (params[0]->asNumber() == int(params[0]->asNumber())) {
        std::exit(int(params[0]->asNumber()));
    } else {
        throw LispError("should be integer");
    }
}

template <typename T>
ValuePtr isType(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkNum(params, 1);
    return std::make_shared<BooleanValue>(typeid(*params[0]) == typeid(T));
}
ValuePtr isInteger(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkNum(params, 1);
    return std::make_shared<BooleanValue>((typeid(*params[0]) == typeid(NumericValue) && params[0]->asNumber() == int(params[0]->asNumber())));
}
ValuePtr isList(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkNum(params, 1);
    return std::make_shared<BooleanValue>(params[0]->isList());
}
ValuePtr isProc(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkNum(params, 1);
    return std::make_shared<BooleanValue>((typeid(*params[0]) == typeid(BuiltinProcValue) || typeid(*params[0]) == typeid(LambdaValue)));
}
ValuePtr isAtom(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //返回值：若 arg 为布尔类型、数类型、字符串类型、符号类型或空表类型的值，则返回 #t；否则返回 #f。
    auto boolean = std::dynamic_pointer_cast<BooleanValue>(isType<BooleanValue>(params, env));
    auto num = std::dynamic_pointer_cast<BooleanValue>(isType<NumericValue>(params, env));
    auto nil = std::dynamic_pointer_cast<BooleanValue>(isType<NilValue>(params, env));
    auto string = std::dynamic_pointer_cast<BooleanValue>(isType<StringValue>(params, env));
    auto symbol = std::dynamic_pointer_cast<BooleanValue>(isType<SymbolValue>(params, env));
    return std::make_shared<BooleanValue>(*boolean || *num || *nil || *string || *symbol);

}

ValuePtr vector2list(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (!params.size()) return std::make_shared<NilValue>();
    std::vector<ValuePtr> n_params;
    n_params.insert(n_params.end(), params.begin() + 1, params.end());
    return std::make_shared<PairValue>(params[0], vector2list(n_params, env));
}
ValuePtr appendFunc(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //将 list 内的元素按顺序拼接为一个新的列表。
    //返回值：拼接后的列表；实参个数为零时返回空表。
    //(append '(1 2 3) '(a b c) '(foo bar baz)) ⇒ '(1 2 3 a b c foo bar baz)
    if (!params.size()) return std::make_shared<NilValue>();
    
    std::vector<ValuePtr> res;
    for (int i = 0; i < params.size(); ++i) {
        if (!params[i]->isList()) {
            throw LispError("list expected in append");
        } else {
            //std::cout<<params[i]->toString()<<'\n';
            auto v = params[i]->toVector();
            res.insert(res.end(), v.begin(), v.end());
        }
    }
    return vector2list(res, env);
}
ValuePtr car(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkParams(params, 1, Type::Pair);
    auto pair = std::dynamic_pointer_cast<PairValue>(params[0]);
    return pair->getCar();
}
ValuePtr cdr(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkParams(params, 1, Type::Pair);
    auto pair = std::dynamic_pointer_cast<PairValue>(params[0]);
    return pair->getCdr();
}
ValuePtr cons(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //返回值：以 first 为左半部分，rest 为右半部分的对子类型数据。
    checkNum(params, 2);
    return std::make_shared<PairValue>(params[0], params[1]);   
}
ValuePtr length(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //返回值：非负整数，list 的元素个数。
    checkNum(params, 1);
    if (!params[0]->isList()) {
        throw LispError("list expected in \"length\"");
    } else {
        auto v = params[0]->toVector();
        return std::make_shared<NumericValue>(v.size());
    }
}
ValuePtr map(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //( map proc list )
    //proc 应能接受一个实参。返回值：一个新列表，其中的每个元素都是 list 中对应位置元素被 proc 作用后的结果。
    checkNum(params, 2);
    std::vector<ValuePtr> result;
    std::ranges::transform(params[1]->toVector(),//将list转化成vector
                           std::back_inserter(result),
                           [&](ValuePtr v) { return env.apply(params[0], std::vector<ValuePtr>{v}); });//vector中每个实参apply proc
    return vector2list(result, env);//转化回list
    
}
ValuePtr filter(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkNum(params, 2);
    std::vector<ValuePtr> result;
    auto vec = params[1]->toVector();
    std::copy_if(vec.begin(), vec.end(), std::back_inserter(result), 
                        [&](ValuePtr v) { return !env.apply(params[0], std::vector<ValuePtr>{v})->isFalse(); });
    return vector2list(result, env);
    
}
ValuePtr reduce(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkNum(params, 2);
    if (!params[1]->isList()) {
        throw LispError("the second argument in \"reduce\" should be a list");
    } else if (typeid(*params[1]) == typeid(NilValue)) {
        throw LispError("the second argument in \"reduce\" cannot be Nil");
    } else if (length(std::vector<ValuePtr>{params[1]}, env)->asNumber() == 1) {
        auto pair = std::dynamic_pointer_cast<PairValue>(params[1]);
        return pair->getCar();
    } else {
         auto pair = std::dynamic_pointer_cast<PairValue>(params[1]);
         auto car = pair->getCar();
         auto cdr = pair->getCdr();
         auto proc = params[0];
         return env.apply(proc, std::vector<ValuePtr>{car, reduce(std::vector<ValuePtr>{params[0], cdr}, env)});
    }

}


ValuePtr isEqual(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkNum(params, 2);
    return std::make_shared<BooleanValue>(params[0]->isEqual(*params[1]));
}
ValuePtr isEq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkNum(params, 2);
    if (typeid(*params[0]) != typeid(*params[1])) return std::make_shared<BooleanValue>(false);
    auto n_params = std::vector<ValuePtr>{params[0]};
    if (!(*std::dynamic_pointer_cast<BooleanValue>(isType<StringValue>(n_params, env))) && *std::dynamic_pointer_cast<BooleanValue>(isAtom(n_params, env))) {
        return std::make_shared<BooleanValue>(params[0]->isEqual(*params[1]));
    } else {
        return std::make_shared<BooleanValue>(params[0] == params[1]);
    }
}
ValuePtr notFunc(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkNum(params, 1);
    return std::make_shared<BooleanValue>(params[0]->isFalse());
}
ValuePtr isOdd(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkParams(params, 1, Type::Number);
    if (!(params[0]->asNumber() == int(params[0]->asNumber()))){
        throw LispError("integer expected");
    } else {
        return std::make_shared<BooleanValue>(int(params[0]->asNumber()) % 2);
    } 
}
ValuePtr isEven(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto odd = std::dynamic_pointer_cast<BooleanValue>(isOdd(params, env));
    return std::make_shared<BooleanValue>(!*odd);
}
ValuePtr isZero(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkParams(params, 1, Type::Number);
    return std::make_shared<BooleanValue>(params[0]->asNumber() == 0); 
}


const std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue>> BUILTIN_FUNCS = {
    {"+", std::make_shared<BuiltinProcValue>(&add)},
    {"print", std::make_shared<BuiltinProcValue>(&print)},
    {"-", std::make_shared<BuiltinProcValue>(&substract)}, 
    {"*", std::make_shared<BuiltinProcValue>(&multiply)}, 
    {">", std::make_shared<BuiltinProcValue>(&greater)}, 
    {"apply", std::make_shared<BuiltinProcValue>(&apply)}, 
    {"display", std::make_shared<BuiltinProcValue>(&display)}, 
    {"displayln", std::make_shared<BuiltinProcValue>(&displayLn)},
    {"error", std::make_shared<BuiltinProcValue>(&error)}, 
    {"eval", std::make_shared<BuiltinProcValue>(&eval)}, 
    {"exit", std::make_shared<BuiltinProcValue>(&exitFunc)}, 
    {"newline", std::make_shared<BuiltinProcValue>(&newline)}, 
    {"atom?", std::make_shared<BuiltinProcValue>(&isAtom)}, 
    {"boolean?", std::make_shared<BuiltinProcValue>(&isType<BooleanValue>)}, 
    {"integer?", std::make_shared<BuiltinProcValue>(&isInteger)}, 
    {"list?", std::make_shared<BuiltinProcValue>(&isList)}, 
    {"number?", std::make_shared<BuiltinProcValue>(&isType<NumericValue>)},
    {"null?", std::make_shared<BuiltinProcValue>(&isType<NilValue>)},
    {"pair?", std::make_shared<BuiltinProcValue>(&isType<PairValue>)},
    {"procedure?", std::make_shared<BuiltinProcValue>(&isProc)},
    {"string?", std::make_shared<BuiltinProcValue>(&isType<StringValue>)},
    {"symbol?", std::make_shared<BuiltinProcValue>(&isType<SymbolValue>)},
    {"append", std::make_shared<BuiltinProcValue>(&appendFunc)},
    {"car", std::make_shared<BuiltinProcValue>(&car)},
    {"cdr", std::make_shared<BuiltinProcValue>(&cdr)},
    {"cons", std::make_shared<BuiltinProcValue>(&cons)},
    {"length", std::make_shared<BuiltinProcValue>(&length)},
    {"list", std::make_shared<BuiltinProcValue>(&vector2list)},
    {"map", std::make_shared<BuiltinProcValue>(&map)},
    {"filter", std::make_shared<BuiltinProcValue>(&filter)},
    {"reduce", std::make_shared<BuiltinProcValue>(&reduce)},
    {"/", std::make_shared<BuiltinProcValue>(&divide)},
    {"abs", std::make_shared<BuiltinProcValue>(&absolute)},
    {"expt", std::make_shared<BuiltinProcValue>(&expt)},//不支持复数
    {"quotient", std::make_shared<BuiltinProcValue>(&quotient)},
    {"modulo", std::make_shared<BuiltinProcValue>(&modulo)},
    {"remainder", std::make_shared<BuiltinProcValue>(&remain)},
    {"eq?", std::make_shared<BuiltinProcValue>(&isEq)},
    {"equal?", std::make_shared<BuiltinProcValue>(&isEqual)},
    {"not", std::make_shared<BuiltinProcValue>(&notFunc)},
    {"<", std::make_shared<BuiltinProcValue>(&smaller)},
    {"<=", std::make_shared<BuiltinProcValue>(&smaller_eq)},
    {"=", std::make_shared<BuiltinProcValue>(&equivalent)},
    {">=", std::make_shared<BuiltinProcValue>(&greater_eq)},
    {"odd?", std::make_shared<BuiltinProcValue>(&isOdd)},
    {"even?", std::make_shared<BuiltinProcValue>(&isEven)},
    {"zero?", std::make_shared<BuiltinProcValue>(&isZero)},
    // 其他内置函数
};
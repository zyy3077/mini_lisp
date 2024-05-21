#include "./builtins.h"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cmath>
void argumentNumError(int x, int params) {
    if (params != x) {
        throw LispError(std::to_string(x) + " arguments expected but " + std::to_string(params) + " were given");
    }
}

ValuePtr add(const std::vector<ValuePtr>& params, EvalEnv& env) {
    double result = 0;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot add a non-numeric value.");
        }
        result += i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}
ValuePtr substract(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 1) {
        if (!params[0]->isNumber()) {
            throw LispError("Cannot substract a non-numeric value.");
        }
        return std::make_shared<NumericValue>(- params[0]->asNumber());
    } else if (params.size() == 2) {
        if (!params[0]->isNumber() || !params[1]->isNumber()) {
            throw LispError("Cannot substract a non-numeric value.");
        }
        return std::make_shared<NumericValue>(params[0]->asNumber() - params[1]->asNumber());
    } else {
        throw LispError("too much arguments for substraction.");
    }
}
ValuePtr print(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(1, params.size());
    std::cout << params[0]->toString() << '\n';
    return std::make_shared<NilValue>();
}
ValuePtr multiply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    double result = 1;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot multiply a non-numeric value.");
        }
        result *= i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}
ValuePtr equivalent(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(2, params.size());
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError("cannot compare between non-numeric values");
    }
    if (params[0]->asNumber() == params[1]->asNumber()){
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr smaller(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(2, params.size());
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError("cannot compare between non-numeric values");
    }
    if (params[0]->asNumber() < params[1]->asNumber()){
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr greater(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(2, params.size());
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError("cannot compare between non-numeric values");
    }
    if (params[0]->asNumber() > params[1]->asNumber()){
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr greater_eq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(2, params.size());
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError("cannot compare between non-numeric values");
    }
    if (params[0]->asNumber() >= params[1]->asNumber()){
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr smaller_eq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(2, params.size());
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError("cannot compare between non-numeric values");
    }
    if (params[0]->asNumber() <= params[1]->asNumber()){
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr apply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //( apply proc list )
    //调用 proc，并将 list 的元素作为调用的实参。
    //返回值：如是调用 proc 得到的返回值。
    argumentNumError(2, params.size());
    return env.apply(params[0], params[1]->toVector());
    
}
ValuePtr display(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //( display val )
    //若 val 是字符串类型数据，则将字符串内容通过 标准输出 输出；
    //否则输出 val 的外部表示，实现可以在外部表示前添加单引号 '。
    //返回值：未定义；建议空表。
    argumentNumError(1, params.size());
    if (auto string = std::dynamic_pointer_cast<StringValue>(params[0])) {
        std::cout << string->getVal() << '\n';
    } else {
        std::cout << '\'' << params[0]->toString() << "\n";
    }
    return std::make_shared<NilValue>();
}
ValuePtr error(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(1, params.size());
    throw LispError(params[0]->toString());
    
}
ValuePtr eval(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //将 expr 作为 Mini-Lisp 程序表达式，对其进行求值。
    //返回值：求值结果。
    argumentNumError(1, params.size());
    return env.eval(params[0]);
}
ValuePtr exitFunc(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(1, params.size());
    if (params[0]->isNumber()) {
        std::exit(int(params[0]->asNumber()));
    } else {
        throw LispError(params[0]->toString() + "is not a number");
    }
}
ValuePtr newline(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //向 标准输出 输出操作系统定义的换行符序列。
    //返回值：未定义；建议空表。
    argumentNumError(0, params.size());
    std::cout << '\n';
    return std::make_shared<NilValue>();
}
ValuePtr isAtom(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //返回值：若 arg 为布尔类型、数类型、字符串类型、符号类型或空表类型的值，则返回 #t；否则返回 #f。
    argumentNumError(1, params.size());
    if (typeid(*params[0]) == typeid(BooleanValue) || typeid(*params[0]) == typeid(NumericValue) ||typeid(*params[0]) == typeid(SymbolValue) || typeid(*params[0]) == typeid(StringValue) || typeid(*params[0]) == typeid(NilValue)) {
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr isBoolean(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(1, params.size());
    if (typeid(*params[0]) == typeid(BooleanValue)) {
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr isInteger(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(1, params.size());
    if (typeid(*params[0]) == typeid(NumericValue) && params[0]->asNumber() == int(params[0]->asNumber())) {
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr isList(const std::vector<ValuePtr>& params, EvalEnv& env) {
     argumentNumError(1, params.size());
     if (params[0]->isList()) {
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr isNum(const std::vector<ValuePtr>& params, EvalEnv& env) {
     argumentNumError(1, params.size());
     if (params[0]->isNumber()) {
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr isNull(const std::vector<ValuePtr>& params, EvalEnv& env) {
     argumentNumError(1, params.size());
     if (typeid(*params[0]) == typeid(NilValue)) {
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr isPair(const std::vector<ValuePtr>& params, EvalEnv& env) {
     argumentNumError(1, params.size());
     if (typeid(*params[0]) == typeid(PairValue)) {
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr isProc(const std::vector<ValuePtr>& params, EvalEnv& env) {
     argumentNumError(1, params.size());
     if (typeid(*params[0]) == typeid(BuiltinProcValue) || typeid(*params[0]) == typeid(LambdaValue)) {
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr isString(const std::vector<ValuePtr>& params, EvalEnv& env) {
     argumentNumError(1, params.size());
     if (typeid(*params[0]) == typeid(StringValue)) {
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr isSymbol(const std::vector<ValuePtr>& params, EvalEnv& env) {
     argumentNumError(1, params.size());
     if (typeid(*params[0]) == typeid(SymbolValue)) {
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
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
            auto v = params[i]->toVector();
            res.insert(res.end(), v.begin(), v.end());
        }
    }
    return vector2list(res, env);
}
ValuePtr car(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(1, params.size());
    if (auto pair = std::dynamic_pointer_cast<PairValue>(params[0])) {
        return pair->getCar();
    } else {
        throw LispError("pair expected in \"car\"");
    }
}
ValuePtr cdr(const std::vector<ValuePtr>& params, EvalEnv& env) {
     argumentNumError(1, params.size());
     if (auto pair = std::dynamic_pointer_cast<PairValue>(params[0])) {
        return pair->getCdr();
    } else {
        throw LispError("pair expected in \"cdr\"");
    }
}
ValuePtr cons(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //返回值：以 first 为左半部分，rest 为右半部分的对子类型数据。
    argumentNumError(2, params.size());
    return std::make_shared<PairValue>(params[0], params[2]);
    
}
ValuePtr length(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //返回值：非负整数，list 的元素个数。
    argumentNumError(1, params.size());
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
    //(map - '(1 2 3)) ⇒ '(-1 -2 -3)
    argumentNumError(2, params.size());
    std::vector<ValuePtr> result;
    std::ranges::transform(params[1]->toVector(),//将list转化成vector
                           std::back_inserter(result),
                           [&](ValuePtr v) { return env.apply(params[0], std::vector<ValuePtr>{v}); });//vector中每个实参apply proc
    return vector2list(result, env);//转化回list
    
}
ValuePtr filter(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //( filter proc list )
    //proc 应能接受一个实参。返回值：一个新列表，其中包含 list 中被 proc 作用后得到非虚值的元素，按原有顺序排列。
    //(filter odd? '(1 2 3 4)) ⇒ '(1 3)
    argumentNumError(2, params.size());
    std::vector<ValuePtr> result;
    auto vec = params[1]->toVector();
    std::copy_if(vec.begin(), vec.end(), std::back_inserter(result), 
                        [&](ValuePtr v) { return !env.apply(params[0], std::vector<ValuePtr>{v})->isFalse(); });
    return vector2list(result, env);
    
}
ValuePtr reduce(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //( reduce proc list )
    //proc 应当接受两个实参。list 不能为空表。
    //返回值：若 ( length list ) 求值为 1，则返回 ( car list )；否则返回 ( proc ( car list ) ( reduce proc ( cdr list ) ) )。
    //(reduce * '(1 2 3 4)) ⇒ 24
    argumentNumError(2, params.size());
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
ValuePtr divide(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 1) {
        if (!params[0]->isNumber()) {
            throw LispError("Cannot divide a non-numeric value.");
        }
        return std::make_shared<NumericValue>(1 / params[0]->asNumber());
    } else if (params.size() == 2) {
        if (!params[0]->isNumber() || !params[1]->isNumber()) {
            throw LispError("Cannot divide a non-numeric value.");
        } else if (params[1]->asNumber() == 0) {
            throw LispError("divided by zero");
        }
        return std::make_shared<NumericValue>(params[0]->asNumber() / params[1]->asNumber());
    } else {
        throw LispError("too much arguments for division.");
    }
}
ValuePtr absolute(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("1 argument expected in \"abs\"");
    } else if (!params[0]->isNumber()) {
        throw LispError("Cannot abs a non-numeric value");
    } else {
        auto val = params[0]->asNumber();
        return val >= 0?  std::make_shared<NumericValue>(val) : std::make_shared<NumericValue>(-val);
    }
}
ValuePtr expt(const std::vector<ValuePtr>& params, EvalEnv& env) { //不支持复数
    argumentNumError(2, params.size());
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError("Cannot expt non-numeric value");
    } else if (params[0]->asNumber() == 0 && params[1]->asNumber() == 0) {
        throw LispError("0 ^ 0 is undefined");
    } else {
        return std::make_shared<NumericValue>(std::pow(params[0]->asNumber(), params[1]->asNumber()));
    }
}
ValuePtr quotient(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(2, params.size());
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError("Cannot quotient non-numeric value");
    }
    double val = params[0]->asNumber() / params[1]->asNumber();
    return std::make_shared<NumericValue>(int(val));
}
ValuePtr modulo(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //( modulo x y )
    //返回值：与 y 具有相同正负性的值 𝑞，满足 0⩽∣𝑞∣<∣𝑦∣ 且 𝑦 整除 ∣𝑞−𝑥∣。
    argumentNumError(2, params.size());
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError("Cannot mod non-numeric value");
    }
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
    argumentNumError(2, params.size());
     if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError("Cannot mod non-numeric value");
    }
    double x = params[0]->asNumber();
    double y = params[1]->asNumber();
    if (y == 0) {
        throw LispError("divided by zero");
    } else {
        return std::make_shared<NumericValue>(remainder(x, y));
    }
}
ValuePtr isEqual(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(2, params.size());
    return std::make_shared<BooleanValue>(params[0]->isEqual(*params[1]));
}
ValuePtr isEq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    //对于符号类型、数类型、布尔类型、空表类型来说，equal? 和 eq? 都是返回值是否相等；
    //而对子类型、字符串类型和过程类型的 eq? 则是表明存储它们的地址（即 std::shared_ptr 本身）是否是相同的。
    argumentNumError(2, params.size());
    if (typeid(*params[0]) != typeid(*params[1])) return std::make_shared<BooleanValue>(false);
    if (typeid(*params[0]) == typeid(SymbolValue) || typeid(*params[0]) == typeid(NumericValue) || typeid(*params[0]) == typeid(BooleanValue) || typeid(*params[0]) == typeid(NilValue)) {
        return std::make_shared<BooleanValue>(params[0]->isEqual(*params[1]));
    } else {
        return std::make_shared<BooleanValue>(params[0] == params[1]);
    }
}
ValuePtr notFunc(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(1, params.size());
    return std::make_shared<BooleanValue>(params[0]->isFalse());
}
ValuePtr isEven(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(1, params.size());
    if (!params[0]->isNumber()) {
        throw LispError("numeric value expected");
    }
    if (!(params[0]->asNumber() == int(params[0]->asNumber()))){
        throw LispError("integer expected");
    } else if (int(params[0]->asNumber()) % 2){
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr isOdd(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(1, params.size());
    if (!params[0]->isNumber()) {
        throw LispError("numeric value expected");
    }
    if (!(params[0]->asNumber() == int(params[0]->asNumber()))){
        throw LispError("integer expected");
    } else if (!(int(params[0]->asNumber()) % 2)){
        return std::make_shared<BooleanValue>(true);
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr isZero(const std::vector<ValuePtr>& params, EvalEnv& env) {
    argumentNumError(1, params.size());
    if (!params[0]->isNumber()) {
        throw LispError("numeric value expected");
    }
    if (params[0]->asNumber() == 0){
         return std::make_shared<BooleanValue>(true); 
    } else {
        return std::make_shared<BooleanValue>(false);
    }
}


const std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue>> BUILTIN_FUNCS = {
    {"+", std::make_shared<BuiltinProcValue>(&add)},
    {"print", std::make_shared<BuiltinProcValue>(&print)},
    {"-", std::make_shared<BuiltinProcValue>(&substract)}, 
    {"*", std::make_shared<BuiltinProcValue>(&multiply)}, 
    {">", std::make_shared<BuiltinProcValue>(&greater)}, 
    {"apply", std::make_shared<BuiltinProcValue>(&apply)}, 
    {"display", std::make_shared<BuiltinProcValue>(&display)}, 
    {"error", std::make_shared<BuiltinProcValue>(&error)}, 
    {"eval", std::make_shared<BuiltinProcValue>(&eval)}, 
    {"exit", std::make_shared<BuiltinProcValue>(&exitFunc)}, 
    {"newline", std::make_shared<BuiltinProcValue>(&newline)}, 
    {"atom?", std::make_shared<BuiltinProcValue>(&isAtom)}, 
    {"boolean?", std::make_shared<BuiltinProcValue>(&isBoolean)}, 
    {"integer?", std::make_shared<BuiltinProcValue>(&isInteger)}, 
    {"list?", std::make_shared<BuiltinProcValue>(&isList)}, 
    {"number?", std::make_shared<BuiltinProcValue>(&isNum)},
    {"null?", std::make_shared<BuiltinProcValue>(&isNull)},
    {"pair?", std::make_shared<BuiltinProcValue>(&isPair)},
    {"procedure?", std::make_shared<BuiltinProcValue>(&isProc)},
    {"string?", std::make_shared<BuiltinProcValue>(&isString)},
    {"symbol?", std::make_shared<BuiltinProcValue>(&isSymbol)},
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
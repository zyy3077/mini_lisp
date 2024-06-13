#include "./value.h"
#include "./error.h"
#include <iomanip>
#include <sstream>
#include <vector>
#include <iostream>

//构造函数
Value::Value(){}
Value::~Value(){}
BooleanValue::BooleanValue(const bool& val): Value(), val{val} {}
NumericValue::NumericValue(const double& val): Value(), val{val} {}
StringValue::StringValue(const std::string& val): Value(), val{val} {}
NilValue::NilValue(): Value() {}
SymbolValue::SymbolValue(const std::string& symbol): Value(), symbol{symbol} {}
PairValue::PairValue(const std::shared_ptr<Value>& left, const std::shared_ptr<Value>& right): Value(), left{left}, right{right} {}
using ValuePtr = std::shared_ptr<Value>;
using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);
BuiltinProcValue::BuiltinProcValue(BuiltinFuncType* func) : Value(), func(func) {}
LambdaValue::LambdaValue(const std::vector<std::string>& params, const std::vector<ValuePtr>& body, std::shared_ptr<EvalEnv> initEnv) : params{params}, body{body}, initEnv{initEnv} {}

//toString函数
std::string BooleanValue::toString() const {
    if(val) return "#t";
    else return "#f";
}
std::string NumericValue::toString() const {
    if(val == int(val)) return std::to_string(int(val));
    else return std::to_string(val);
}
std::string StringValue::toString() const {
    std::ostringstream oss;
    oss << std::quoted(val);
    return oss.str();
}
std::string NilValue::toString() const {
    return "()";
}
std::string SymbolValue::toString() const {
    return symbol;
}
std::string PairValue::toString() const {
    std::string res = left->toString();
    PairValue* rightIsPair = dynamic_cast<PairValue*>(right.get());
    NilValue* rightIsNil = dynamic_cast<NilValue*>(right.get());
    auto nextRight = right;
    while (rightIsPair) {
        res += " " + rightIsPair->left->toString();
        nextRight = rightIsPair->right;
        rightIsPair = dynamic_cast<PairValue*>(nextRight.get());
        rightIsNil = dynamic_cast<NilValue*>(nextRight.get());
    }
    if (rightIsNil) {}//空表不增加输出
    else {
        res += " . " + nextRight->toString();
    }
    return "(" + res + ")";
}
std::string BuiltinProcValue::toString() const {
    return "#<procedure>";
}
std::string LambdaValue::toString() const {
    return "#<procedure>";
}

//is/as函数
bool Value::isList() {
    if (typeid(*this) == typeid(NilValue)) return true;
    if (auto pair = dynamic_cast<PairValue*>(this)) {
        if (pair->getCdr()->isList()) return true;
    }
    return false;
}
bool Value::isNil() {
    if(dynamic_cast<NilValue*>(this)) return true;
    return false;
}
bool Value::isNumber() {
    if(dynamic_cast<NumericValue*>(this)) return true;
    return false;
}
double Value::asNumber() {
    NumericValue* num = dynamic_cast<NumericValue*>(this);
    return num->getVal();
}
bool Value::isSeflEvaluating() {
    if(dynamic_cast<BooleanValue*>(this)) return true;
    if(dynamic_cast<NumericValue*>(this)) return true;
    if(dynamic_cast<StringValue*>(this)) return true;
    if(dynamic_cast<BuiltinProcValue*>(this)) return true;
    return false;
}
std::optional<std::string> Value::asSymbol() {
    SymbolValue* symbol = dynamic_cast<SymbolValue*>(this);
    if (symbol) {
        return symbol->toString();
    } else {
        return std::nullopt;
    }
}
bool Value::isFalse() {
    if (auto boolean = dynamic_cast<BooleanValue*>(this)) {
        if (boolean->getVal() == false) return true;
    }
    return false; 
}

template <typename T>
bool Value::isEqual(const Value& a, const Value& b) const {
    const T* aT = dynamic_cast<const T*>(&a);
    const T* bT = dynamic_cast<const T*>(&b);
    return aT && bT && aT->getVal() == bT->getVal();
}
bool NilValue::isEqual(const Value& other) const {
    const NilValue* otherNil = dynamic_cast<const NilValue*>(&other);
    return otherNil;
}

bool PairValue::isEqual(const Value& other) const {
    const PairValue* otherPair = dynamic_cast<const PairValue*>(&other);
    return otherPair && left->isEqual(*otherPair->left) && right->isEqual(*otherPair->right);
}
bool BuiltinProcValue::isEqual(const Value& other) const {
    const BuiltinProcValue* otherSymbol= dynamic_cast<const BuiltinProcValue*>(&other);
    return otherSymbol && otherSymbol->getFunc() == this->getFunc();
}
bool LambdaValue::isEqual(const Value& other) const {
    const LambdaValue* otherSymbol= dynamic_cast<const LambdaValue*>(&other);
    return otherSymbol && otherSymbol == this;
}

//toVector函数
std::vector<std::shared_ptr<Value>> Value::toVector() {
    std::vector<std::shared_ptr<Value>> vec;
    PairValue* pair = dynamic_cast<PairValue*>(this);
    while (pair) {
        vec.push_back(pair->left);
        PairValue* rightIsPair = dynamic_cast<PairValue*>(pair->right.get());
        NilValue* rightIsNil = dynamic_cast<NilValue*>(pair->right.get());
        if (rightIsPair) {
            pair = rightIsPair;
        } else if (rightIsNil) {
            pair = nullptr;
        } else {
            throw LispError("Value cannot be converted to vector");
        }
    }
    return vec;
}

BuiltinFuncType* BuiltinProcValue::getFunc() const {
    return func;
}


ValuePtr LambdaValue::apply(const std::vector<ValuePtr>& args) {
    //首先是创建一个新的 Lambda 内部求值环境。
    //这个应当包含 LambdaValue::params 数据成员到 args 的一一绑定。
    //然后，将它的上级环境设置为之前保存的 parent。
    //最后，在这个求值环境下对 body 数据成员的表达式逐一求值，返回最后一个即可。
    auto child = initEnv->createChild(params, args);
    ValuePtr res = nullptr;
    for (auto expr : body) {
        res = child->eval(expr);
        //std::cout<<res->toString()<<'\n';
    }
    return res;
}
#include "./value.h"
#include "./error.h"
#include <iomanip>
#include <sstream>
#include <vector>

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
using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&);
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
        res += " . " + right->toString();
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
bool Value::isNil() {
    if(dynamic_cast<NilValue*>(this)) return true;
    return false;
}
bool Value::isNumber() {
    if(dynamic_cast<NumericValue*>(this)) return true;
    return false;
}
int Value::asNumber() {
    NumericValue* num = dynamic_cast<NumericValue*>(this);
    return num->val;
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

std::shared_ptr<Value> PairValue::getCdr() {
    return right;
}
std::shared_ptr<Value> PairValue::getCar() {
    return left;
}
BuiltinFuncType* BuiltinProcValue::getFunc() {
    return func;
}
bool BooleanValue::getVal() {
    return val;
}
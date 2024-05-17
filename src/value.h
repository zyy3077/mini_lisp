#ifndef VALUE_H
#define VALUE_H

#include <memory>
#include <string>
#include <vector>
#include <optional>

class Value {
public:
    Value();
    virtual ~Value();
    virtual std::string toString() const = 0;
    bool isNil();
    bool isNumber();
    bool isSeflEvaluating();
    std::vector<std::shared_ptr<Value>> toVector();
    std::optional<std::string> asSymbol();
    int asNumber();
    
};
using ValuePtr = std::shared_ptr<Value>;
class BooleanValue : public Value {
    bool val;
public:
    BooleanValue(const bool& val);
    std::string toString() const override;
    bool getVal();
};
class NumericValue : public Value {
    double val;
public:
    NumericValue(const double& val);
    std::string toString() const override;
    friend int Value::asNumber();
};
class StringValue : public Value {
    std::string val;
public:
    StringValue(const std::string& val);
    std::string toString() const override;  
};
class NilValue : public Value {
public:
    NilValue();
    std::string toString() const override;
};
class SymbolValue : public Value {
    std::string symbol;
public:
    SymbolValue(const std::string& symbol);
    std::string toString() const override;
};
class PairValue : public Value {
    std::shared_ptr<Value> left;
    std::shared_ptr<Value> right;
public:
    PairValue(const std::shared_ptr<Value>& left, const std::shared_ptr<Value>& right);
    std::string toString() const override;
    friend std::vector<std::shared_ptr<Value>> Value::toVector();
    std::shared_ptr<Value> getCdr();
    std::shared_ptr<Value> getCar();
};
class BuiltinProcValue : public Value {
    using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&);
    BuiltinFuncType* func = nullptr;
public:
    std::string toString() const override;
    BuiltinProcValue(BuiltinFuncType* func);
    BuiltinFuncType* getFunc();
};
class LambdaValue : public Value {
private:
    std::vector<std::string> params;
    std::vector<ValuePtr> body;
    // [...]
public:
    LambdaValue(const std::vector<std::string>& params, const std::vector<ValuePtr>& body);
    std::string toString() const override; // 如前所述，返回 #<procedure> 即可
};

#endif
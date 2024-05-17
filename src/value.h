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
class BooleanValue : public Value {
    bool val;
public:
    BooleanValue();
    BooleanValue(const bool& val);
    std::string toString() const override;
};
class NumericValue : public Value {
    double val;
public:
    NumericValue();
    NumericValue(const double& val);
    std::string toString() const override;
    friend int Value::asNumber();
};
class StringValue : public Value {
    std::string val;
public:
    StringValue();
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
    SymbolValue();
    SymbolValue(const std::string& symbol);
    std::string toString() const override;
};
class PairValue : public Value {
    std::shared_ptr<Value> left;
    std::shared_ptr<Value> right;
public:
    PairValue();
    PairValue(const std::shared_ptr<Value>& left, const std::shared_ptr<Value>& right);
    std::string toString() const override;
    friend std::vector<std::shared_ptr<Value>> Value::toVector();
    std::shared_ptr<Value> getCdr();
    std::shared_ptr<Value> getCar();
};
class BuiltinProcValue : public Value {
    using ValuePtr = std::shared_ptr<Value>;
    using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&);
    BuiltinFuncType* func = nullptr;
public:
    std::string toString() const override;
    BuiltinProcValue();
    BuiltinProcValue(BuiltinFuncType* func);
    BuiltinFuncType* getFunc();
};
#endif
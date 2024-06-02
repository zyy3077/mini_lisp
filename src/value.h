#ifndef VALUE_H
#define VALUE_H

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include "./eval_env.h"
class EvalEnv;

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
    double asNumber();
    bool isList();
    bool isFalse();
     virtual bool isEqual(const Value& other) const = 0;
    
};
using ValuePtr = std::shared_ptr<Value>;
class BooleanValue : public Value {
    bool val;
public:
    BooleanValue(const bool& val);
    std::string toString() const override;
    bool getVal() const;
    bool isEqual(const Value& other) const override;
};
class NumericValue : public Value {
    double val;
public:
    NumericValue(const double& val);
    std::string toString() const override;
    double getVal() const;
    bool isEqual(const Value& other) const override;
};
class StringValue : public Value {
    std::string val;
public:
    StringValue(const std::string& val);
    std::string toString() const override; 
    std::string getVal() const; 
    bool isEqual(const Value& other) const override;
};
class NilValue : public Value {
public:
    NilValue();
    std::string toString() const override;
    bool isEqual(const Value& other) const override;
};
class SymbolValue : public Value {
    std::string symbol;
public:
    SymbolValue(const std::string& symbol);
    std::string toString() const override;
    bool isEqual(const Value& other) const override;
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
    bool isEqual(const Value& other) const override;
    void setCdr(ValuePtr n_right);
    void setCar(ValuePtr n_left);
};
class BuiltinProcValue : public Value {
    using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);
    BuiltinFuncType* func = nullptr;
public:
    std::string toString() const override;
    BuiltinProcValue(BuiltinFuncType* func);
    BuiltinFuncType* getFunc() const;
    bool isEqual(const Value& other) const override;
};
class LambdaValue : public Value {
private:
    std::vector<std::string> params;
    std::vector<ValuePtr> body;
    std::shared_ptr<EvalEnv> initEnv = nullptr;//被定义时的环境
    // [...]
public:
    LambdaValue(const std::vector<std::string>& params, const std::vector<ValuePtr>& body, std::shared_ptr<EvalEnv> initEnv);
    std::string toString() const override; // 如前所述，返回 #<procedure> 即可
    ValuePtr apply(const std::vector<ValuePtr>& args);
    bool isEqual(const Value& other) const override;
};

#endif
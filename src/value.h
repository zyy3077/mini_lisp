#ifndef VALUE_H
#define VALUE_H

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include "./eval_env.h"
class EvalEnv;

enum class Type {
    Number,
    String,
    Boolean,
    Nil,
    Symbol,
    Pair,
    BuiltinProc,
    Lambda,
};

class Value {
public:
    Value();
    virtual ~Value();
    virtual Type getType() const = 0;
    virtual std::string toString() const = 0;
    bool isNil();
    bool isNumber();
    bool isSeflEvaluating();
    bool isList();
    bool isFalse();
    std::optional<std::string> asSymbol();
    double asNumber();
    virtual bool isEqual(const Value& other) const = 0;
    
    template <typename T>
    bool isEqual(const Value& a, const Value& b) const;

    std::vector<std::shared_ptr<Value>> toVector();

};
using ValuePtr = std::shared_ptr<Value>;


class BooleanValue : public Value {
    bool val;
public:
    BooleanValue(const bool& val);
    std::string toString() const override;
    bool getVal() const {
        return val;
    }
    Type getType() const override {
        return Type::Boolean;
    }
    bool isEqual(const Value& other) const {
        return Value::isEqual<BooleanValue>(*this, other);
    }
    operator bool() {
        return val;
    }
};


class NumericValue : public Value {
    double val;
public:
    NumericValue(const double& val);
    Type getType() const override {
        return Type::Number;
    }
    std::string toString() const override;
    double getVal() const {
        return val;
    }
    bool isEqual(const Value& other) const {
        return Value::isEqual<NumericValue>(*this, other);
    }
};
class StringValue : public Value {
    std::string val;
public:
    StringValue(const std::string& val);
    Type getType() const override {
        return Type::String;
    }
    std::string toString() const override; 
    std::string getVal() const {
        return val;
    }
    bool isEqual(const Value& other) const {
        return Value::isEqual<StringValue>(*this, other);
    }
};


class NilValue : public Value {
public:
    NilValue();
    Type getType() const override {
        return Type::Nil;
    }
    std::string toString() const override;
    bool isEqual(const Value& other) const override;
};


class SymbolValue : public Value {
    std::string symbol;
public:
    SymbolValue(const std::string& symbol);
    Type getType() const override {
        return Type::Symbol;
    }
    std::string toString() const override;
    std::string getVal() const {
        return symbol;
    }
    bool isEqual(const Value& other) const {
        return Value::isEqual<SymbolValue>(*this, other);
    }
};


class PairValue : public Value {
    std::shared_ptr<Value> left;
    std::shared_ptr<Value> right;
public:
    PairValue(const std::shared_ptr<Value>& left, const std::shared_ptr<Value>& right);
    Type getType() const override {
        return Type::Pair;
    }
    std::string toString() const override;
    friend std::vector<std::shared_ptr<Value>> Value::toVector();
    std::shared_ptr<Value> getCdr() {
        return right;
    }
    std::shared_ptr<Value> getCar() {
        return left;
    }
    void setCdr(ValuePtr n_right) {
        right = n_right;
    }
    void setCar(ValuePtr n_left) {
        left = n_left;
    }
    bool isEqual(const Value& other) const override;

};


class BuiltinProcValue : public Value {
    using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);
    BuiltinFuncType* func = nullptr;
public:
    BuiltinProcValue(BuiltinFuncType* func);
    Type getType() const override {
        return Type::BuiltinProc;
    }
    std::string toString() const override;
    BuiltinFuncType* getFunc() const;
    bool isEqual(const Value& other) const override;
};


class LambdaValue : public Value {
private:
    std::vector<std::string> params;
    std::vector<ValuePtr> body;
    std::shared_ptr<EvalEnv> initEnv = nullptr;//被定义时的环境
public:    
    Type getType() const override {
        return Type::Lambda;
    }
    LambdaValue(const std::vector<std::string>& params, const std::vector<ValuePtr>& body, std::shared_ptr<EvalEnv> initEnv);
    std::string toString() const override; // 如前所述，返回 #<procedure> 即可
    ValuePtr apply(const std::vector<ValuePtr>& args);
    bool isEqual(const Value& other) const override;
};

#endif
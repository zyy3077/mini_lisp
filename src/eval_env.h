#ifndef EVAL_ENV_H
#define EVAL_ENV_H
#include "./value.h"
#include <unordered_map>
#include <string>
class Value;
using ValuePtr = std::shared_ptr<Value>;

class EvalEnv : public std::enable_shared_from_this<EvalEnv>{
    std::vector<ValuePtr> evalList(ValuePtr expr);
    std::unordered_map<std::string, ValuePtr> symbolMap{};
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);
    std::shared_ptr<EvalEnv> parent = nullptr;
    EvalEnv();
public:
    std::shared_ptr<EvalEnv> createChild(const std::vector<std::string>& params, const std::vector<ValuePtr>& args);
    static std::shared_ptr<EvalEnv> createGlobal() {
        return std::shared_ptr<EvalEnv>(new EvalEnv());
    }//确保 EvalEnv 的实例总是被 std::shared_ptr 管理
    ValuePtr eval(ValuePtr expr);
    ValuePtr lookupBinding(const std::string& name);//通过本层级的搜索和向上追溯来找到正确的变量定义
    void defineBinding(const std::string& name, ValuePtr value);
};

#endif
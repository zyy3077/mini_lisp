#include <iostream>
#include <string>
#include "./value.h"
#include "./tokenizer.h"
#include "./parse.h"
#include "./eval_env.h"


#include "rjsj_test.hpp"
struct TestCtx {
    std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();
    std::string eval(std::string input) {
        auto tokens = Tokenizer::tokenize(input);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        auto result = env->eval(std::move(value));
        return result->toString();
    }
};
int main() {
    //RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv6);
    auto env = EvalEnv::createGlobal();
    while (true) {
        try {
            std::cout << ">>> " ;
            std::string line;
            std::getline(std::cin, line);
            if (std::cin.eof()) {
                std::exit(0);
            }
            auto tokens = Tokenizer::tokenize(line);
            // for (auto& token : tokens) {
            //     std::cout << *token << std::endl;
            // }
            Parser parser(std::move(tokens)); // TokenPtr 不支持复制
            auto value = parser.parse();
            auto result = env->eval(std::move(value));
            std::cout << result->toString() << std::endl; // 输出外部表示
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
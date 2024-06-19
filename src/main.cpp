#include <iostream>
#include <string>
#include "./value.h"
#include "./tokenizer.h"
#include "./parse.h"
#include "./eval_env.h"
#include <fstream>
#include "./error.h"
#include "rjsj_test.hpp"
struct TestCtx {
    std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();
    std::string eval(std::string input) {
        auto tokens = std::move(Tokenizer::tokenize(input));
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        auto result = env->eval(std::move(value));
        return result->toString();
    }
};
int checkBracket(std::deque<TokenPtr>& tokens) {
    std::deque<char> stack;
    for (auto& token : tokens) {
        if (token->getType() == TokenType::LEFT_PAREN) {
            stack.push_back('(');
        } else if(token->getType() == TokenType::RIGHT_PAREN) {
            if (stack.empty()) return 2; //2代表右括号多了
            stack.pop_back();
        }
    }
    if (stack.empty()) return 0; //0代表括号匹配
    return 1; //1代表左括号多了
}

std::deque<std::deque<TokenPtr>> splitExpressions(std::deque<TokenPtr>& tokens) {
    std::deque<std::deque<TokenPtr>> expressions;
    std::deque<TokenPtr> currentExpression;
    for (auto& token : tokens) { 
        currentExpression.push_back(std::move(token));
        if (checkBracket(currentExpression) == 0) { //完整表达式
            expressions.push_back(std::move(currentExpression));
            currentExpression.clear();
        }
    }
    return expressions;
}
void standardInput(std::shared_ptr<EvalEnv> env) {
    std::string line;
    while (true) {
        try {
            std::cout << ">>> " ;
            std::getline(std::cin, line);
            if (std::cin.eof()) {
                std::cin.clear(); 
                continue;
            }
            auto current_tokens = Tokenizer::tokenize(line);
            auto tokens = std::move(current_tokens);
            while (checkBracket(tokens) != 0) {
                if (checkBracket(tokens) == 2) { //右括号多了
                    throw SyntaxError("too much \')\'");
                } else { //左括号多了
                    std::getline(std::cin, line);
                    if (std::cin.eof()) {
                        std::cin.clear(); 
                        continue;
                    }
                    current_tokens = Tokenizer::tokenize(line);
                    for (auto& token : current_tokens) {
                        tokens.push_back(std::move(token));
                    }
                }
            }
            auto expressions = std::move(splitExpressions(tokens));
            for (auto& expression : expressions) {
                Parser parser(std::move(expression)); //含有一个token的deque
                auto value = parser.parse(); //一个ValuePtr的deque
                auto result = env->eval(std::move(value));
                std::cout << result->toString() << std::endl; // 输出外部表示                    
            }
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
void fileInput(std::ifstream& file, std::shared_ptr<EvalEnv> env) {
    std::string line;
     while (std::getline(file, line)) {
        if (line == "") continue; //空行
        try {
            auto current_tokens = Tokenizer::tokenize(line);
            auto tokens = std::move(current_tokens);
            while (checkBracket(tokens) != 0) {
                if (checkBracket(tokens) == 2) { //右括号多了
                    throw SyntaxError("too much \')\'");
                } else { //左括号多了
                    if (std::getline(file, line)) {
                        current_tokens = Tokenizer::tokenize(line);
                        for (auto& token : current_tokens) {
                            tokens.push_back(std::move(token));
                        }
                    } else {
                        throw SyntaxError("dismatched brackets");
                    }
                }
            }
            auto expressions = std::move(splitExpressions(tokens));
            for (auto& expression : expressions) {
                Parser parser(std::move(expression)); //含有一个token的deque
                auto value = parser.parse(); //一个ValuePtr的deque
                auto result = env->eval(std::move(value));                   
            }
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    //RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv5Extra, Lv6, Lv7, Lv7Lib, Sicp);
    auto env = EvalEnv::createGlobal();
    std::ifstream file;
    int mode = 1;

    // Check if a file path was provided
    if (argc >= 2) {
        file.open(argv[1]);
        if (file) mode = 2; // Switch to file input mode
         else {
            std::cerr << "Error: Could not open file " << argv[1] << "\n";
            return 1;
        }
    }

    if (mode == 1) standardInput(env);
    else fileInput(file, env);

    return 0;
}
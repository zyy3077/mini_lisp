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
        auto tokens = Tokenizer::tokenize(input);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        auto result = env->eval(std::move(value));
        return result->toString();
    }
};
bool checkBracket(const std::string& text) {
    const char* cstr = text.c_str();
    std::deque<char> stack;
    for (int i = 0; i < strlen(cstr); ++i) {
        if (cstr[i] == '(') {
            stack.push_back('(');
        } else if(cstr[i] == ')') {
            if (stack.empty()) return false;
            stack.pop_back();
        }
    }
    if (stack.empty()) return true;
    return false;
}

int main(int argc, char* argv[]) {
    //RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv5Extra, Lv6, Lv7, Lv7Lib, Sicp);
    auto env = EvalEnv::createGlobal();
    std::string line;
    int mode = 1; // Default to standard input mode
    std::ifstream file;

    // Check if a file path was provided
    if (argc >= 2) {
        file.open(argv[1]);
        if (file) {
            mode = 2; // Switch to file input mode
        } else {
            std::cerr << "Error: Could not open file " << argv[1] << "\n";
            return 1;
        }
    }

    if (mode == 1) {
        // Standard input mode
        while (true) {
            try {
                std::string text;
                std::cout << ">>> " ;
                do {
                    std::getline(std::cin, line);
                    text += line;
                } while (!checkBracket(text)); //读入直到括号匹配再tokenize
                auto tokens = Tokenizer::tokenize(text);
                Parser parser(std::move(tokens)); //含有一个token的deque
                auto value = parser.parse(); //一个ValuePtr的deque
                auto result = env->eval(std::move(value));
                std::cout << result->toString() << std::endl; // 输出外部表示
            } catch (std::runtime_error& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    } else if (mode == 2) {
        // File input mode
        while (std::getline(file, line)) {
            if (line == "") continue;
            try {
                std::string text = line;
                while (!checkBracket(text)) { //读入直到括号匹配
                    if (std::getline(file, line)) {
                        text +=line;
                    } else {
                        throw SyntaxError("dismatched brackets");
                    }
                }
                
                auto tokens = Tokenizer::tokenize(text);
                Parser parser(std::move(tokens));
                auto value = parser.parse();
                auto result = env->eval(std::move(value));
            } catch (std::runtime_error& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }

    return 0;
}

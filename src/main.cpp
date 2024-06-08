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
int checkBracket(const std::string& text) {
    const char* cstr = text.c_str();
    std::deque<char> stack;
    for (int i = 0; i < strlen(cstr); ++i) {
        if (cstr[i] == '(') {
            stack.push_back('(');
        } else if(cstr[i] == ')') {
            if (stack.empty()) return 2; //2代表右括号多了
            stack.pop_back();
        }
    }
    if (stack.empty()) return 0; //0代表括号匹配
    return 1; //1代表左括号多了
}

void neglectComment(std::string& line) {
    size_t pos = line.find(';'); //忽略注释中的括号，把;后的内容删掉
    if (pos != std::string::npos) {
        line = line.substr(0, pos);
    }
}

std::vector<std::string> splitExpressions(const std::string& line) {
    std::vector<std::string> expressions;
    std::string currentExpression;
    for (char c : line) { 
        currentExpression += c;
        if (currentExpression == " ") {
            currentExpression = "";
            continue;
        }
        if (checkBracket(currentExpression) == 0) { //完整表达式
            expressions.push_back(currentExpression);
            currentExpression = "";
        }
    }
    return expressions;
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
                std::cout << ">>> " ;
                std::getline(std::cin, line);
                if (std::cin.eof()) {
                    std::cin.clear(); 
                    continue;
                }
                neglectComment(line);
                std::string text = line;
                while (checkBracket(text)) {
                    if (checkBracket(text) == 2) {
                        throw SyntaxError("too much \')\'");
                    }
                    std::getline(std::cin, line);
                    if (std::cin.eof()) continue;
                    neglectComment(line);
                    text += line;
                } //读入直到括号匹配再tokenize
                std::vector<std::string> expressions = splitExpressions(text); //对每个表达式求值
                if (text == "") continue;
                for (const std::string& expression : expressions) {
                    auto tokens = Tokenizer::tokenize(expression);
                    Parser parser(std::move(tokens)); //含有一个token的deque
                    auto value = parser.parse(); //一个ValuePtr的deque
                    auto result = env->eval(std::move(value));
                    std::cout << result->toString() << std::endl; // 输出外部表示
                }
            } catch (std::runtime_error& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    } else if (mode == 2) {
        // File input mode
        while (std::getline(file, line)) {
            if (line == "") continue; //空行
            size_t pos = line.find(';'); //忽略注释中的括号
            if (pos != std::string::npos) {
                line = line.substr(0, pos);
            }
            try {
                std::string text = line;
                while (checkBracket(text)) { //读入直到括号匹配，多行读入
                    if (checkBracket(text) == 2) {
                        throw SyntaxError("too much \')\'");
                    }
                    if (std::getline(file, line)) {
                        size_t pos = line.find(';');
                        if (pos != std::string::npos) {
                        line = line.substr(0, pos);
                        }
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

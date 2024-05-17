#include "./parse.h"
#include "./error.h"
#include <iostream>
Parser::Parser(std::deque<TokenPtr> tokens): tokens(std::move(tokens)) {}

ValuePtr Parser::parse(){
    if (tokens.empty()) throw SyntaxError("missing token");
    auto token = std::move(tokens.front());
    tokens.pop_front();
    if (token->getType() == TokenType::NUMERIC_LITERAL) {
        auto value = static_cast<NumericLiteralToken&>(*token).getValue();
        return std::make_shared<NumericValue>(value);
    }
    else if (token->getType() == TokenType::BOOLEAN_LITERAL) {
        auto value = static_cast<BooleanLiteralToken&>(*token).getValue();
        return std::make_shared<BooleanValue>(value);
    }
    else if (token->getType() == TokenType::STRING_LITERAL) {
        auto value = static_cast<StringLiteralToken&>(*token).getValue();
        return std::make_shared<StringValue>(value);
    }
    else if (token->getType() == TokenType::IDENTIFIER) {
        auto name = static_cast<IdentifierToken&>(*token).getName();
        return std::make_shared<SymbolValue>(name);
    }
    else if (token->getType() == TokenType::LEFT_PAREN) {
        return parseTails();
    }
    else if (token->getType() == TokenType::QUOTE) {
        return std::make_shared<PairValue>(
            std::make_shared<SymbolValue>("quote"),
            std::make_shared<PairValue>(
                this->parse(),
                std::make_shared<NilValue>()
            )
        );
    }
    else if (token->getType() == TokenType::QUASIQUOTE) {
        return std::make_shared<PairValue>(
            std::make_shared<SymbolValue>("quasiquote"),
            std::make_shared<PairValue>(
                this->parse(),
                std::make_shared<NilValue>()
            )
        );
    }
    else if (token->getType() == TokenType::UNQUOTE) {
        return std::make_shared<PairValue>(
            std::make_shared<SymbolValue>("unquote"),
            std::make_shared<PairValue>(
                this->parse(),
                std::make_shared<NilValue>()
            )
        );
    }
    throw SyntaxError("Unimplemented");
}

ValuePtr Parser::parseTails() {
    if (tokens.empty()) throw SyntaxError("missing token");
    if (tokens.front()->getType() == TokenType::RIGHT_PAREN) {
        tokens.pop_front();
        return std::make_shared<NilValue>();
    }
    auto car = this->parse();
    if (tokens.front()->getType() == TokenType::DOT) {
        tokens.pop_front();
        auto cdr = this->parse();
        //std::cout << cdr->toString();
        if (tokens.empty()) throw SyntaxError("missing )");
        tokens.pop_front();//再弹出一个词法标记，它应当是 ')';
        return std::make_shared<PairValue>(car, cdr);
    } else {
      auto cdr = this->parseTails();
      return std::make_shared<PairValue>(car, cdr);
    }
}

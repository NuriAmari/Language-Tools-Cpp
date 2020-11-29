//
// Created by nuria on 2020-07-06.
//

#include "Token.h"

bool operator==(const Token& lhs, const Token& rhs) {
    return lhs.m_lexme == rhs.m_lexme && lhs.m_type == rhs.m_type;
}

Token::Token(std::string type, int priority) {
    m_type = type;
    m_priority = priority;
}

void Token::setLexme(std::string lexme) {
    m_lexme = lexme;
}


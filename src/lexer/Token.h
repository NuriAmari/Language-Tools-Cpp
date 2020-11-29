//
// Created by nuria on 2020-07-06.
//

#ifndef CPP_TOKEN_H
#define CPP_TOKEN_H
#include <string>
#include <boost/container_hash/hash.hpp>

class Token {
public:
    std::string m_lexme;
    std::string m_type;
    int m_priority;

    Token(std::string type, int priority);
    void setLexme(std::string lexme);

    friend bool operator==(const Token& lhs, const Token& rhs);
};


namespace std {
    template <>
    struct hash<Token>
    {
        std::size_t operator()(const Token& token) const
        {
            std::size_t seed = 0;
            boost::hash_combine(seed, token.m_lexme);
            boost::hash_combine(seed, token.m_priority);
            boost::hash_combine(seed, token.m_type);
            return seed;
        }
    };
}


#endif //CPP_TOKEN_H

//
// Created by Nuri Amari on 2020-07-04.
//

#ifndef CPP_STATE_H
#define CPP_STATE_H
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>
#include "Token.h"

class NFAState {
public:
    bool m_accepting;
    std::unordered_map<char, std::unordered_set<NFAState*>> m_transitions;
    std::unordered_set<Token> m_tokens;

    void addTransition(const char transitionChar, NFAState* target) {
        if (m_transitions.find(transitionChar) != m_transitions.end()) {
            m_transitions.at(transitionChar).insert(target);
        } else {
            m_transitions.insert({transitionChar, std::unordered_set<NFAState*>{target}});
        }
    }

    template <class It>
    void addTokens(It begin, It end) {
        m_tokens.insert(begin, end);
    }
    void addTokens(const Token& token) {
       m_tokens.insert(token);
    }

    NFAState(bool accepting = false):m_accepting{accepting} { }

    template <class It>
    NFAState(bool accepting, It first, It last):m_accepting{accepting} {
        m_tokens.insert(first, last);
    }
};

class DFAState {
public:
    bool m_accepting;
    std::unordered_map<char, DFAState*> m_transitions;
    std::unordered_set<Token> m_tokens;

    void addTransition(const char transitionChar, DFAState* target) {
        if (m_transitions.find(transitionChar) != m_transitions.end()) {
            throw "Transition already set for DFA state";
        }
        m_transitions.insert({transitionChar, target});
    }

    template<class It>
    void addTokens(It first, It last) {
        m_tokens.insert(first, last);
    }
    void addTokens(Token token) {
        m_tokens.insert(token);
    }

    Token resolveToken() {
        if (m_tokens.empty()) {
            throw "No tokens found";
        }
        int highestPriorty = 0;
        Token tokenToReturn = *(m_tokens.begin());
        for (auto& token : m_tokens) {
            if (token.m_priority > highestPriorty) {
                highestPriorty = token.m_priority;
                tokenToReturn = token;
            }
        }
        return tokenToReturn;
    }


    DFAState(bool accepting = false):m_accepting{accepting} {}

    template<class It>
    DFAState(bool accepting, It first, It last):m_accepting{accepting} {
        m_tokens.insert(first, last);
    }
};

#endif //CPP_STATE_H

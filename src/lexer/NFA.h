//
// Created by Nuri Amari on 2020-07-04.
//

#ifndef CPP_NFA_H
#define CPP_NFA_H
#include <deque>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iterator>
#include <memory>
#include <utility>
#include "State.h"

class NFA {
    friend class Atom;
    friend class Union;
    friend class Concat;
    friend class Kleenestar;
    friend class DFA;

    NFAState* m_startState = nullptr;
    NFAState* m_endState = nullptr;
    std::unordered_set<std::unique_ptr<NFAState>> m_states;
    std::unordered_set<char> m_alphabet;

public:
    NFA() = default;
    NFA(std::unique_ptr<NFAState>&& state);
    NFA(std::unique_ptr<NFAState>&& startState, std::unique_ptr<NFAState>&& endState);
    NFA(const NFA& other);
    NFA(NFA&& other);
    NFA& operator=(const NFA& other);
    NFA& operator=(NFA&& other);

    template <class It>
    void addTokens(It begin, It end);

    void addTokens(const Token& token);
};

class Atom : public NFA {
public:
    Atom(const char charToRecognize);
};

class Union : public NFA {
public:
    Union(std::vector<NFA>&& operands);
};

class Concat : public NFA {
public:
    Concat(std::vector<NFA>&& operands);
};

class Kleenestar : public NFA {
public:
    Kleenestar(NFA operand);
};


#endif //CPP_NFA_H

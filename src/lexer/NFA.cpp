//
// Created by Nuri Amari on 2020-07-04.
//

#include "NFA.h"

NFA::NFA(std::unique_ptr<NFAState>&& startState, std::unique_ptr<NFAState>&& endState) {
    m_startState = startState.get();
    m_endState = endState.get();
    m_states.insert(std::move(startState));
    m_states.insert(std::move(endState));
}

NFA::NFA(std::unique_ptr<NFAState>&& state) {
    m_startState = state.get();
    m_endState = state.get();
    m_states.insert(std::move(state));
}

NFA::NFA(const NFA& other) {
    *this = other;
}

NFA::NFA(NFA&& other) {
    *this = std::move(other);
}

NFA& NFA::operator=(const NFA& other) {
    // given a state in the other, returns the corresponding state in the new NFA
    std::unordered_map<NFAState*, NFAState*> stateCopyMapping;

    // copy the states
    for (const std::unique_ptr<NFAState>& state : other.m_states) {
        std::unique_ptr<NFAState> currCopy = std::make_unique<NFAState>(state.get()->m_accepting);
        stateCopyMapping.insert({state.get(), currCopy.get()});
        m_states.insert(std::move(currCopy));
    }

    // copy the transitions
    for (const std::unique_ptr<NFAState>& state : other.m_states) {
        for (const std::pair<char, std::unordered_set<NFAState*>>& transition : state->m_transitions) {
            NFAState* startState = stateCopyMapping.at(state.get());
            for (NFAState* endStateToCopy : transition.second) {
                NFAState* endState = stateCopyMapping.at(endStateToCopy);
                startState->addTransition(transition.first, endState);
            }
        }
    }

    m_startState = stateCopyMapping.at(other.m_startState);
    m_endState = stateCopyMapping.at(other.m_endState);
    m_alphabet = other.m_alphabet;
    return *this;
}

NFA& NFA::operator=(NFA&& other) {
    m_startState = other.m_startState;
    m_endState = other.m_endState;
    m_states = std::move(other.m_states);
    m_alphabet = std::move(other.m_alphabet);
    return *this;
}

template <class It>
void NFA::addTokens(It begin, It end) {
    m_endState->addTokens(begin, end);
}

void NFA::addTokens(const Token& token) {
    m_endState->addTokens(token);
}

Atom::Atom(const char charToRecognize) : NFA(std::make_unique<NFAState>(), std::make_unique<NFAState>(true)) {
    m_startState->addTransition(charToRecognize, m_endState);
    m_alphabet.insert(charToRecognize);
}

Union::Union(std::vector<NFA>&& operands): NFA(std::make_unique<NFAState>(), std::make_unique<NFAState>(true)) {
    if (operands.size() < 1) {
        throw "Union expects at least one operator";
    }
    for (NFA& operand : operands) {
        m_alphabet.insert(operand.m_alphabet.begin(), operand.m_alphabet.end());
        m_startState->addTransition('\0', operand.m_startState);
        m_states.merge(std::move(operand.m_states));
        operand.m_states.clear();
        operand.m_endState->m_accepting = false;
        operand.m_endState->addTransition('\0', m_endState);
    }
}

Concat::Concat(std::vector<NFA>&& operands) : NFA(std::make_unique<NFAState>(), std::make_unique<NFAState>(true)) {
    if (operands.size() < 1) {
        throw "Concat expects at least one operator";
    }
    NFA &frontOfChain = operands.front();
    NFA &backOfChain = operands.back();

    // connect start state to start of chain
    m_startState->addTransition('\0', frontOfChain.m_startState);
    for (int i = 0; i < operands.size() - 1; i++) {
        // copy the alphabet
        m_alphabet.insert(operands.at(i).m_alphabet.begin(), operands.at(i).m_alphabet.end());
        // connect pieces of the chain together
        operands.at(i).m_endState->addTransition('\0', operands.at(i + 1).m_startState);
        operands.at(i).m_endState->m_accepting = false;
        m_states.merge(std::move(operands.at(i).m_states));
        operands.at(i).m_states.clear();
    }
    m_alphabet.insert(backOfChain.m_alphabet.begin(), backOfChain.m_alphabet.end());
    // connect end of the chain to end state
    m_states.merge(std::move(backOfChain.m_states));
    backOfChain.m_states.clear();
    backOfChain.m_endState->m_accepting = false;
    backOfChain.m_endState->addTransition('\0', m_endState);
}

Kleenestar::Kleenestar(NFA operand): NFA(std::make_unique<NFAState>(true)) {
    m_alphabet = std::move(operand.m_alphabet);
    operand.m_endState->m_accepting = false;
    m_startState->addTransition('\0', operand.m_startState);
    operand.m_endState->addTransition('\0', m_startState);
    m_states.merge(std::move(operand.m_states));
    operand.m_states.clear();
}


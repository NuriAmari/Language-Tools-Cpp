//
// Created by Nuri Amari on 2020-07-04.
//

#include "DFA.h"

std::unordered_set<NFAState*> DFA::findEpsilonClosure(const std::unordered_set<NFAState*>& currClosure) {
    std::unordered_set<NFAState*> retval = currClosure;
    DFA::expandToEpsilonClosure(retval);
    return retval;
}

void DFA::expandToEpsilonClosure(std::unordered_set<NFAState *>& currClosure) {
    std::unordered_set<NFAState*> statesLeftToExplore = currClosure;
    while (!statesLeftToExplore.empty()) {
        for (auto it = statesLeftToExplore.begin(); it != statesLeftToExplore.end();) {
            currClosure.insert(*it);
            if ((*it)->m_transitions.find('\0') != (*it)->m_transitions.end()) {
                for (NFAState *targetState : (*it)->m_transitions.at('\0')) {
                    if (currClosure.find(targetState) == currClosure.end()) {
                        statesLeftToExplore.insert(targetState);
                    }
                }
            }
            it = statesLeftToExplore.erase(it);
        }
    }
}

DFA::DFA(const NFA& nfaToConvert) {
    std::unordered_map<std::unordered_set<NFAState*>, std::unique_ptr<DFAState>, UnorderedSetHasher<NFAState*>> nfaStatesToDfaStateMap;
    std::unordered_set<NFAState*> startClosure {nfaToConvert.m_startState};
    DFA::expandToEpsilonClosure(startClosure);
    std::unordered_set<Token> startTokens;
    bool startStateAccepting = false;

    for (NFAState* state : startClosure) {
        startStateAccepting = startStateAccepting || state->m_accepting;
        startTokens.insert(state->m_tokens.begin(), state->m_tokens.end());
    }

    std::unique_ptr<DFAState> startState = std::make_unique<DFAState>(startStateAccepting, startTokens.begin(), startTokens.end());
    m_startState = startState.get();
    nfaStatesToDfaStateMap.insert({startClosure, std::move(startState)});

    std::deque<std::unordered_set<NFAState*>> stateQ {startClosure};
    while (!stateQ.empty()) {
        // we call it a DFA state, but it is really a set of NFA states corresponding to an DFA state
        std::unordered_set<NFAState*> currDfaState = stateQ.front();
        std::unordered_set<NFAState*> currDfaStateEpsilonClosure = DFA::findEpsilonClosure(currDfaState);
        stateQ.pop_front();

        for (const char transitionChar : nfaToConvert.m_alphabet) {
            std::unordered_set<NFAState*> transitionCharClosure;

            for (const NFAState* epsilonNeighbour : currDfaStateEpsilonClosure) {
                if (epsilonNeighbour->m_transitions.find(transitionChar) != epsilonNeighbour->m_transitions.end()) {
                    for (NFAState* transitionNeighour : epsilonNeighbour->m_transitions.at(transitionChar)) {
                        transitionCharClosure.insert(transitionNeighour);
                    }
                }
            }

            DFA::expandToEpsilonClosure(transitionCharClosure);

            if (!transitionCharClosure.empty()) {
                // check if we already created this DFA state
                if (nfaStatesToDfaStateMap.find(transitionCharClosure) == nfaStatesToDfaStateMap.end()) {
                    stateQ.push_back(transitionCharClosure);
                    bool newStateAccepting = false;
                    std::unordered_set<Token> newStateTokens;
                    for (const NFAState* state : transitionCharClosure) {
                        newStateAccepting = newStateAccepting || state->m_accepting;
                        newStateTokens.insert(state->m_tokens.begin(), state->m_tokens.end());
                    }
                    nfaStatesToDfaStateMap.insert({transitionCharClosure, std::make_unique<DFAState>(newStateAccepting, newStateTokens.begin(), newStateTokens.end())});
                }

                nfaStatesToDfaStateMap.at(currDfaState).get()->addTransition(transitionChar, nfaStatesToDfaStateMap.at(transitionCharClosure).get());
            }
        }
    }

    for (std::pair<std::unordered_set<NFAState*>,std::unique_ptr<DFAState>&> mapping : nfaStatesToDfaStateMap) {
        m_states.insert(std::move(mapping.second));
    }
}

DFA::DFA(DFA& other) {
    *this = other;
}

DFA::DFA(DFA&& other) {
    *this = std::move(other);
}

DFA& DFA::operator=(DFA& other) {
    // given a state in the other, returns the corresponding state in the new NFA
    std::unordered_map<DFAState*, DFAState*> stateCopyMapping;

    // copy the states!
    for (const std::unique_ptr<DFAState>& state : other.m_states) {
        std::unique_ptr<DFAState> currCopy = std::make_unique<DFAState>(state->m_accepting);
        stateCopyMapping.insert({state.get(), currCopy.get()});
        m_states.insert(std::move(currCopy));
    }

    // copy the transitions!
    for (const std::unique_ptr<DFAState>& state : other.m_states) {
        for (const std::pair<char, DFAState*>& transition : state->m_transitions) {
            DFAState* startState = stateCopyMapping.at(state.get());
            DFAState* endState = stateCopyMapping.at(transition.second);
            if (startState->m_transitions.find(transition.first) != startState->m_transitions.end()) {
                throw "Copying this DFA went horribly wrong";
            }
            startState->addTransition(transition.first, endState);
        }
    }
    return *this;
}
DFA& DFA::operator=(DFA&& other) {
    m_startState = other.m_startState;
    m_states = std::move(other.m_states);
    return *this;
}

bool DFA::match(const std::string& inputStr) {
    DFAState* currState = m_startState;
    for (const char transitionChar : inputStr) {
        if (currState->m_transitions.find(transitionChar) == currState->m_transitions.end()) {
            return false;
        }
        currState = currState->m_transitions.at(transitionChar);
    }
    return currState->m_accepting;
}

std::vector<Token> DFA::tokenize(const std::string& inputStr) {
    if (inputStr.empty()) {
        return std::vector<Token>{};
    }

    bool passedAcceptState = false;
    int lastCharacterAccepted = -1;
    int lastCharacterTokenized = -1;
    DFAState* lastAcceptState = nullptr;
    std::string lastAcceptLexme;

    std::vector<Token> result;

    DFAState* currState = m_startState;
    std::string currLexme{""};
    for (int i = 0; i <= inputStr.size(); i++) {
        bool transitionFailure = false;
        // if at end of string
        if (i == inputStr.size()) {
            // if entire string has been accepted
            if (lastCharacterTokenized == i-1) {
                break;
            } else {
                transitionFailure = true;
            }
        } else {
            char transitionChar = inputStr.at(i);
            currLexme.push_back(transitionChar);
            transitionFailure = currState->m_transitions.find(transitionChar) == currState->m_transitions.end();
        }

        if (!transitionFailure) {
            currState = currState->m_transitions.at(inputStr.at(i));
            if (currState->m_accepting) {
                lastCharacterAccepted = i;
                lastAcceptState = currState;
                lastAcceptLexme = currLexme;
                passedAcceptState = true;
            }
        } else if (passedAcceptState) {
            i = lastCharacterAccepted;
            lastCharacterTokenized = lastCharacterAccepted;
            result.push_back(lastAcceptState->resolveToken());
            result.back().setLexme(lastAcceptLexme);
            lastAcceptLexme = "";
            currLexme = "";
            currState = m_startState;
            passedAcceptState = false;
        } else {
            std::cout << "FAIL: " << lastCharacterAccepted << std::endl;
            break;
        }
    }

    return result;
}

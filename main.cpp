#include <iostream>
#include <string>
#include <vector>
#include "lexer/DFA.h"
#include "lexer/NFA.h"

int main() {

    NFA abc = Concat(std::vector<NFA>{Atom('a'), Atom('b'), Atom('c')});

    DFA test = DFA(abc);

    std::vector<Token> tokens = test.tokenize("abcabcabcabcabcabc");

    for (auto token : tokens) {
        std::cout << token.m_type << " : " << token.m_lexme << ", " << std::endl;
    }
    return 0;
}

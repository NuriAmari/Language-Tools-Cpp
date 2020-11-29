//
// Created by Nuri Amari on 2020-09-17.
//

#include "gtest/gtest.h"
#include "lexer/DFA.h"
#include "lexer/NFA.h"

class LexerFixture : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        std::vector<NFA> alphabet;
        for (char letter = 'a'; letter <= 'z'; ++letter) {
            alphabet.push_back(Atom(letter));
        }
        DFA alphabetDFA = DFA(Union(std::move(alphabet)));
    }

    virtual void TearDown() {

    }
};

TEST_F(LexerFixture, SingleCharacterTokenization) {
   EXPECT_EQ(1,1) ;
}

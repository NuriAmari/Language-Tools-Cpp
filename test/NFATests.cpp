//
// Created by Nuri Amari on 2020-07-05.
//

#include <string>
#include <unordered_map>
#include "gtest/gtest.h"
#include "lexer/DFA.h"
#include "lexer/NFA.h"

class AtomFixture : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        a = new DFA{Atom{'a'}};
    }

    virtual void TearDown() {
        delete a;
    }

    DFA*a ;
};

TEST_F(AtomFixture, SingleCharacterMatch) {
    EXPECT_EQ(a->match("a"), 1);
}

TEST_F(AtomFixture, SingleCharacterMissmatch) {
    for (char i = 'b'; i <= 'z'; i++) {
        EXPECT_EQ(a->match(std::string{i}), 0);
    }
}

TEST_F(AtomFixture, EmptyStringMissmatch) {
    EXPECT_EQ(a->match(""), 0);
}

class ConcatFixture : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        for (char i = 'A'; i <= 'z'; i++) {
            alphabet.insert({i, new Atom{i}});
        }
    }
    virtual  void TearDown() {
        for (char i = 'A'; i <= 'z'; i++) {
            delete alphabet.at(i);
        }
    }
    std::unordered_map<char, Atom*> alphabet;
};

TEST_F(ConcatFixture, SingleCharConcatMatch) {
    Concat a{std::vector<NFA>{*alphabet.at('a')}};
    DFA aDFA = {a};
    EXPECT_EQ(aDFA.match("a"), 1);
}

TEST_F(ConcatFixture, SingleCharConcatMismatch) {
    Concat a{std::vector<NFA>{*alphabet.at('a')}};
    DFA aDFA = {a};
    EXPECT_EQ(aDFA.match("b"), 0);
}

TEST_F(ConcatFixture, StandardConcatMatch) {
    Concat abcd {std::vector<NFA>{*alphabet.at('a'), *alphabet.at('b'), *alphabet.at('c'), *alphabet.at('d')}};
    DFA abcdDFA = {abcd};
    EXPECT_EQ(abcdDFA.match("abcd"), 1);
}

TEST_F(ConcatFixture, StandardConcatMismatch) {
    Concat abcd {std::vector<NFA>{*alphabet.at('a'), *alphabet.at('b'), *alphabet.at('c'), *alphabet.at('d')}};
    DFA abcdDFA = {abcd};
    EXPECT_EQ(abcdDFA.match("abc"), 0);
    EXPECT_EQ(abcdDFA.match("abcde"), 0);
    EXPECT_EQ(abcdDFA.match("abcde"), 0);
    EXPECT_EQ(abcdDFA.match("aabcd"), 0);
}

class UnionFixture : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        for (char i = 'A'; i <= 'z'; i++) {
            alphabet.insert({i, new Atom{i}});
        }
    }
    virtual  void TearDown() {
        for (char i = 'A'; i <= 'z'; i++) {
            delete alphabet.at(i);
        }
    }
    std::unordered_map<char, Atom*> alphabet;
};

TEST_F(UnionFixture, SingleCharacterUnionMatch) {
    Union abcd {std::vector<NFA>{*alphabet.at('a'), *alphabet.at('b'), *alphabet.at('c'), *alphabet.at('d')}};
    DFA abcdDFA = {abcd};
    EXPECT_EQ(abcdDFA.match("a"), 1);
    EXPECT_EQ(abcdDFA.match("b"), 1);
    EXPECT_EQ(abcdDFA.match("c"), 1);
    EXPECT_EQ(abcdDFA.match("d"), 1);
}

TEST_F(UnionFixture, SingleCharacterUnionMismatch) {
    Union abcd {std::vector<NFA>{*alphabet.at('a'), *alphabet.at('b'), *alphabet.at('c'), *alphabet.at('d')}};
    DFA abcdDFA = {abcd};
    EXPECT_EQ(abcdDFA.match("ab"), 0);
    EXPECT_EQ(abcdDFA.match(""), 0);
    EXPECT_EQ(abcdDFA.match("bc"), 0);
    EXPECT_EQ(abcdDFA.match("abcd"), 0);
}

class KleenestarFixture : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        for (char i = 'A'; i <= 'z'; i++) {
            alphabet.insert({i, new Atom{i}});
        }
    }
    virtual  void TearDown() {
        for (char i = 'A'; i <= 'z'; i++) {
            delete alphabet.at(i);
        }
    }
    std::unordered_map<char, Atom*> alphabet;
};

TEST_F(KleenestarFixture, SingleCharacterKleeneMatch) {
    DFA aStar{Kleenestar{*alphabet.at('a')}};
    EXPECT_EQ(aStar.match(""), 1);
    EXPECT_EQ(aStar.match("a"), 1);
    EXPECT_EQ(aStar.match("aaaaaaaaaaaaaaaaaaaaaaa"), 1);
}

TEST_F(KleenestarFixture, SingleCharacterKleeneMismatch) {
    DFA aStar{Kleenestar{*alphabet.at('a')}};
    EXPECT_EQ(aStar.match("b"),0);
    EXPECT_EQ(aStar.match("aaaaaaaaaaaaaaab"),0);
}

class GeneralFixture : public ::testing::Test {
protected:
    virtual void SetUp()
    {
    }
    virtual  void TearDown() {
    }
};

TEST_F(GeneralFixture, UnionOfConcatMatch) {
    DFA abcOrDefOrGhi {Union{std::vector<NFA>{Concat{std::vector<NFA>{Atom{'a'}, Atom{'b'}, Atom{'c'}}}, Concat{std::vector<NFA>{Atom{'d'}, Atom{'e'}, Atom{'f'}}},Concat{std::vector<NFA>{Atom{'g'}, Atom{'h'}, Atom{'i'}}}}}};
    EXPECT_TRUE(abcOrDefOrGhi.match("abc"));
    EXPECT_TRUE(abcOrDefOrGhi.match("def"));
    EXPECT_TRUE(abcOrDefOrGhi.match("ghi"));
    EXPECT_FALSE(abcOrDefOrGhi.match(""));
    EXPECT_FALSE(abcOrDefOrGhi.match("abcd"));
    EXPECT_FALSE(abcOrDefOrGhi.match("ab"));
}

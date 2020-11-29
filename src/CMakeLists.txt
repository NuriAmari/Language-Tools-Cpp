project(Language-Tools)

set(LEXER_SOURCE
        lexer/DFA.cpp
        lexer/DFA.h
        lexer/NFA.h
        lexer/NFA.cpp
        lexer/State.h
        lexer/Utils.h
        lexer/Token.cpp lexer/Token.h parser/CFG.cpp parser/CFG.h parser/Symbol.cpp parser/Symbol.h)

add_library(Language-Tools STATIC ${LEXER_SOURCE})
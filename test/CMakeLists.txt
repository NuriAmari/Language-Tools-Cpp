project(Tests)

add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(LaraTests NFATests.cpp LexerTests.cpp)
target_link_libraries(LaraTests gtest gtest_main)
target_link_libraries(LaraTests Language-Tools)
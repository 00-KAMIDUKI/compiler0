cmake_minimum_required(VERSION 3.27)

project(compiler1)

set(CMAKE_CXX_STANDARD 20)

add_executable(regex2nfa regex2nfa.cc)

add_executable(nfa2dfa nfa2dfa.cc)

add_executable(dfa dfa.cc)

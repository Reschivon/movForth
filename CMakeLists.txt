cmake_minimum_required(VERSION 3.17)
project(movForth)

set(CMAKE_CXX_STANDARD 20)

add_executable(movForth
        main.cpp
        Interpret/Interpreter.cpp
        Word/Word.cpp
        Interpret/Stack.h
        Interpret/WordGenerator.cpp
        )
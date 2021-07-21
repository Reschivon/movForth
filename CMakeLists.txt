cmake_minimum_required(VERSION 3.17)
project(movForth)

set(CMAKE_CXX_STANDARD 17)

add_executable(movForth
        main.cpp
        sources/Interpreter.cpp
        headers/WordGenerator.h
        headers/Input.h
        sources/Word.cpp
        headers/Stack.h
        headers/Types.h
        )
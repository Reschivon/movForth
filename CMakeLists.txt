cmake_minimum_required(VERSION 3.17)
project(movForth)

set(CMAKE_CXX_STANDARD 17)

add_executable(movForth
        main.cpp
        sources/Interpretation/Interpreter.cpp
        headers/Interpretation/WordGenerator.h
        headers/Interpretation/Input.h
        sources/Interpretation/Word.cpp
        headers/Interpretation/Stack.h
        headers/Interpretation/Types.h
        sources/Symbolic/SymbolicPass.cpp
        headers/Symbolic/SymbolicPass.h
        sources/Symbolic/Translation.cpp
        headers/Util.h
        sources/Symbolic/Structures.cpp
        headers/Print.h
        sources/Print.cpp)
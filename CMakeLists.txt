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
        headers/Symbolic/Structures.h
        sources/Cyclic/SymbolicPass.cpp
        headers/Symbolic/SymbolicPass.h
        sources/Cyclic/Translation.cpp
        headers/Symbolic/Translation.h
        headers/Util.h
        sources/Cyclic/Structures.cpp
        headers/Print.h
        )
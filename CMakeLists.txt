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
        headers/Cyclic/Structures.h
        sources/Cyclic/DictConversion.cpp
        headers/Cyclic/DictConversion.h
        sources/Cyclic/Translation.cpp
        headers/Cyclic/Translation.h
        headers/Util.h
        sources/Cyclic/Structures.cpp)
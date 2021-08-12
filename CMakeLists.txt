cmake_minimum_required(VERSION 3.16)
project(movForth)

set(CMAKE_CXX_STANDARD 20)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

message(STATUS "Include_directories ${LLVM_INCLUDE_DIRS}")

message(STATUS "Libs at ${llvm_libs}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(llvm_libs support core irreader)

add_executable(movForth
        main.cpp

        sources/Print.cpp

        sources/Interpretation/iWord.cpp
        sources/Interpretation/iData.cpp
        sources/Interpretation/Interpreter.cpp

        sources/symbolic/sWord.cpp

        sources/Symbolic/Instruction.cpp

        sources/Symbolic/BasicBlock.cpp

        sources/symbolic/symbolicPass.cpp
        sources/Symbolic/SymbolicPass_TranslateToBasicBlocks.cpp
        sources/Symbolic/SymbolicPass_WordStackGraph.cpp
        sources/Symbolic/SymbolicPass_ComputeMatchingPairs.cpp
        sources/Symbolic/SymbolicPass_BBStackGraph.cpp

        #headers/Generation/IRGenerator.h

        headers/Interpretation/iData.h sources/Interpretation/iData.cpp)

target_link_libraries(movForth ${llvm_libs})

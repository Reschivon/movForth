cmake_minimum_required(VERSION 3.16)
project(movForth)

# set(CMAKE_CXX_COMPILER  "/usr/bin/g++-7")
# set(CMAKE_CXX_FLAGS "-std=c++1z")

set(CMAKE_CXX_STANDARD 17)

set(_IMPORT_PREFIX "/usr/lib/llvm-3.8")

# set(LLVM_CMAKE_DIR "/usr/share/llvm-3.8")

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "llvm cmake dir ${LLVM_CMAKE_DIR}")

message(STATUS "Include_directories ${LLVM_INCLUDE_DIRS}")

message(STATUS "Libs at ${llvm_libs}")

find_package(LLVM REQUIRED CONFIG)


include_directories(${LLVM_INCLUDE_DIRS})
#separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(llvm_libs support core irreader)

add_executable(movForth
        main.cpp

        sources/Print.cpp

        sources/Interpretation/iWord.cpp
        sources/Interpretation/iData.cpp
        sources/Interpretation/Interpreter.cpp

        sources/Symbolic/sWord.cpp

        sources/Symbolic/Instruction.cpp

        sources/Symbolic/Block.cpp

        sources/Symbolic/Pass.cpp
        sources/Symbolic/Pass_TranslateToBasicBlocks.cpp
        sources/Symbolic/Pass_BBStackGraph.cpp
        sources/Symbolic/Pass_ComputeMatchingPairs.cpp
        sources/Symbolic/Pass_WordStackGraph.cpp

        sources/Generation/IRGenerator.cpp

        headers/Interpretation/iData.h
        sources/Interpretation/iData.cpp

        sources/SystemExec.cpp)



target_link_libraries(movForth ${llvm_libs})

SET(LLVM_ENABLE_DUMP ON)
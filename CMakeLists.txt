
cmake_minimum_required(VERSION 3.16)

project(movForth)

set(CMAKE_CXX_STANDARD 17)
set(LLVM_ENABLE_NEW_PASS_MANAGER OFF)

# finding libz may be a bit buggy on some systems;
# cmake expects to find it in /lib/ but it may be in /lib/x86_64-linux-gnu/
# if you have installed libz-dev but cmake still fails,
# you can try sym linking it or setting ZLIB_LIBRARY to the location of the .so
find_package(ZLIB REQUIRED)

add_executable(movForth

        main.cpp

        sources/Print.cpp
        sources/SystemExec.cpp

        sources/Interpretation/iWord.cpp
        sources/Interpretation/iData.cpp
        sources/Interpretation/Interpreter.cpp
        sources/Interpretation/WordGenerator.cpp
        sources/Interpretation/ForthWord.cpp
        sources/Interpretation/Stack.cpp
        sources/Interpretation/IP.cpp
        sources/Interpretation/Primitive.cpp

        sources/Symbolic/sWord.cpp
        sources/Symbolic/Instruction.cpp
        sources/Symbolic/Block.cpp
        sources/Symbolic/Pass.cpp
        sources/Symbolic/Pass_ToBasicBlocks.cpp
        sources/Symbolic/Pass_BlockStackGraph.cpp
        sources/Symbolic/Pass_MatchingPairs.cpp
        sources/Symbolic/Pass_WordStackGraph.cpp

        sources/Generation/IRGenerator.cpp
        sources/Generation/FBuilder.cpp
        sources/Generation/ObjectGenerator.cpp
        )


find_package(LLVM REQUIRED CONFIG PATHS)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")


target_include_directories(movForth PUBLIC ${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs
        support core irreader native passes codegen
)

message(STATUS "Linking LLVM libraries ${llvm_libs}")
target_link_libraries(movForth PUBLIC ${llvm_libs})

SET(LLVM_ENABLE_DUMP ON)


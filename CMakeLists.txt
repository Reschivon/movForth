
cmake_minimum_required(VERSION 3.16)

project(movForth)

set(CMAKE_CXX_STANDARD 17)

# Now build our tools
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

        #        DirtyPass/Inliner.cpp
        #        DirtyPass/IPO.h
        #        DirtyPass/InlineSimple.cpp
        #        DirtyPass/LegacyPassManager.cpp
        )

SET(LLVM13 "/home/reschivon/Hard-Drive/clang+llvm-12.0.1-x86_64-linux-gnu-ubuntu-")

# add_library(LLVM "${LLVM13}")
find_package(LLVM REQUIRED CONFIG PATHS)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Find the libraries that correspond to the LLVM components
# that we wish to use
include(${CMAKE_CURRENT_SOURCE_DIR}/TargetGenComponents.cmake)
llvm_map_components_to_libnames(llvm_libs
        support core irreader native passes codegen
        ${TARGET_COMPONENTS}
)

# execute_process(COMMAND llvm-config --libs all OUTPUT_VARIABLE llvm_libraries)

# Link against LLVM libraries
message(STATUS "Linking LLVM libraries ${llvm_libs}")
target_link_libraries(movForth ${llvm_libs})

SET(LLVM_ENABLE_DUMP ON)



cmake_minimum_required(VERSION 3.16)

project(movForth)

set(CMAKE_CXX_STANDARD 17)


find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Now build our tools
add_executable(movForth

        main.cpp

        sources/Print.cpp
        sources/SystemExec.cpp

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
        sources/Generation/FBuilder.cpp

        )

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader native passes)

# Link against LLVM libraries
message(STATUS "Linking LLVM libraries ${llvm_libs}")
target_link_libraries(movForth ${llvm_libs})

SET(LLVM_ENABLE_DUMP ON)

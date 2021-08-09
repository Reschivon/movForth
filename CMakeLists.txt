cmake_minimum_required(VERSION 3.16)
project(movForth)

set(CMAKE_CXX_STANDARD 20)

find_package(LLVM REQUIRED CONFIG PATHS "D:/llvm-bin/")

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

message(STATUS "Include_directories ${LLVM_INCLUDE_DIRS}")

#set(llvm_libs "D:/llvm-bin/lib")

message(STATUS "Libs at ${llvm_libs}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(llvm_libs support core irreader)

add_executable(movForth
        main.cpp

        #headers/Print.h
        sources/Print.cpp

        #headers/Interpretation/WordGenerator.h
        #headers/Interpretation/Input.h
        #headers/Interpretation/iData.h
        #headers/Interpretation/Stack.h

        sources/Interpretation/iWord.cpp
        sources/Interpretation/Interpreter.cpp


        #headers/PrimitiveEffects.h
        #headers/PrimitiveEnums.h

        #headers/Symbolic/Effects.h

        sources/symbolic/sWord.cpp

        sources/Interpretation/iData.cpp

        #headers/Symbolic/Instruction.h
        sources/Symbolic/Instruction.cpp

        #headers/Symbolic/BasicBlock.h
        sources/Symbolic/BasicBlock.cpp

        sources/symbolic/symbolicPass.cpp
        sources/Symbolic/SymbolicPass_BlockEntries.cpp
        sources/Symbolic/SymbolicPass_StackGraph.cpp
        sources/Symbolic/SymbolicPass_RetrievePushPopEffects.cpp
        sources/Symbolic/SymbolicPass_BBCyclicPass.cpp

        #headers/Generation/IRGenerator.h

        )

target_link_libraries(movForth ${llvm_libs})

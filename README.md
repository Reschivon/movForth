# MovForth

A Forth to binary compiler using LLVM.

## Motivation
MovForth was motivated by the lack of integration between Forth and modern technologies. It demonstrates that Forth can be compiled and made to run very fast -- in fact, using the same optimization passes and code generators as Clang, Rust, and Swift.


## Installation
- Clone the repository
- Have LLVM findable in your include paths (tested with llvm-10)
- Build as CMake project

## Concept
MovForth achieves LLVM IR generation in three steps
1. The immediate portion of Forth source is run, and a dictionary is generated
2. Data flow and control flow graphs are built for the words in the dictionary. This process deduces the number of parameters and returns for each word, as well as allocating unique registers for each element passed on the stack.
3. The graphs from the last step are trivially converted to LLVM Basic Blocks and registers. Optimization passes are run and machine code for a target of choice is produced.

## API
There are two ways to run movForth; through the command line utility and programmatically through C++. The command line utility is not finished.

You can find an example usage of the C++ API in `main.cpp`. The source file read by this example is `boot.fs`.
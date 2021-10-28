# MovForth

![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/Reschivon/movForth?display_name=release&include_prereleases&label=release)   ![GitHub](https://img.shields.io/github/license/Reschivon/movForth?label=license)

MovForth compiles Forth source code to executable binaries. Using LLVM IR as an intermediate target, it is an experiment in adapting Forth for modern compiler libraries and modern architectures.

- Bottom-up compilation; no dictionary or stack remains in final executable
- Compile time evaluation for immediate words allows programmers to use meta-compilation without fear
- Imtermediate form LLVM IR allows compilation to pretty much any architecture
- Modern LLVM optimization passes are run on forth code

You can find Forth source and its corresponding compiled forms in `Examples/.`

## Installation
- Clone the repository
- Have LLVM findable in your include paths (tested with llvm-10)
- Build as CMake project (version 3 or above should do)

Alternatively, you can check `Releases` for precompiled binaries.

In the likely scenario that you encounter a bug or crash, feel free to create an issue.

## Concept
MovForth achieves LLVM IR generation in three steps

1. The immediate (compile-time) portion of Forth source is run, and a dictionary is generated
2. Data flow and control flow graphs are built for the words in the dictionary. This process computes the number of parameters and returns for each word, and allocates unique registers for each element passed on the stack.
3. The graphs from the previous step are used to generate LLVM Basic Blocks and registers. Optimization passes are run and machine code for a target of choice is produced.

**Future features:**
- Dynamic linking so you can have an interactive Forth with certain compiled words
- Interfacing with libraries that follow the C ABI

## Supported Primitives
MovForth is powerful becasue operations that are immediate in a traditional interpreted Forth are run during compile time. Therefore, meta-compilation words like `IF`, `DO`, and `COLON` incur no runtime penalty.

Words available during immediate mode:

```+ - * / SWAP ROT DUP DROP . = SHOW ' , SEE [ ] IMMEDIATE ALLOT @ ~ BRANCH BRANCHIF LITERAL HERE CREATE```

Words available during run time (memory words are on the todo list):

```+ - * / BRANCH BRANCHIF . DUP DROP = LITERAL```

## API
There are two ways to run movForth; through the command line utility and programmatically through C++. The command line utility is not finished.

You can find an example usage of the C++ API in `main.cpp`. The source file read by this example is `boot.fs`.

## Contribute
I'm all for the extra help! I will do my best to respond in a timely manner, though I may not always be available.

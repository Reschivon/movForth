# MovForth

A Forth to binary compiler made with LLVM.

MovForth was created to see if Forth can be compiled, optimized, and executed like modern languages. No longer is Forth confined to the interpreter! No longer must Forthers contend with cycles spent purely on interpretation! No longer must Forth "compilers" be tangled monoliths that target only one architecture!

MovForth translates Forth source into LLVM IR, an intermediate language used by modern compilers such as Clang, Rust, Swift, and GHC. This means Forth can be optimized to the same efficiency and target the same variety of architectures as popular "performant" lanaguages.

You can find Forth source and its corresponding IR file in `Examples/.`

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
3. The graphs from the previous step are trivially converted to LLVM Basic Blocks and registers. Optimization passes are run and machine code for a target of choice is produced.

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

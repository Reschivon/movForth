# MovForth

![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/Reschivon/movForth?display_name=release&include_prereleases&label=release)   ![GitHub](https://img.shields.io/github/license/Reschivon/movForth?label=license)



MovForth compiles Forth source code to executable binaries. Using LLVM IR as an intermediate target, it is an experiment in adapting Forth for modern compiler libraries and modern architectures.

#### **[Forth 2020 Presentation Recording](https://youtu.be/CgxwbeLWSZM?t=9963)**

- Bottom-up compilation; no dictionary or interpreter remains in final executable
- Compile time evaluation for immediate words allows programmers to use meta-compilation without fear
- Compiling to LLVM IR allows for compilation to pretty much [any architecture](https://en.wikipedia.org/wiki/LLVM#Back_ends)
- Modern LLVM optimization passes used on Forth code

You can find Forth source and its corresponding compiled forms in `Examples/`

## Installation

- Have LLVM installed (LLVM-10 and above). For Ubuntu/Debian-based distros, simply run:

```
sudo apt install llvm
```

- Clone the repository

```
git clone https://github.com/Reschivon/movForth
```

- Build as CMake project
```
cd movForth
mkdir build && cd build
cmake ..
make -j4
```

- Optionally, you can install `clang++`. MovForth uses clang++ to link the generated assembly file with stdlibc++ and produce a binary. If clang++ is not installed, you can link and assemble the .S file manually.

Alternatively, you can check `Releases` for precompiled binaries, but it is highly unlikely I will remember to update them.

## Running

The `main.cpp` file compiles into a simple command line utility for movForth. The syntax is:

`movForth [source file]`

An example usage, assuming you have a cmake build directory named `build`, is:
```
build/movForth Examples/fibonacci/source.fs
```
Do check out the `Examples/` folder. In the likely scenario that you encounter a bug or crash, feel free to create an issue.


## Concept
MovForth achieves LLVM IR generation in three steps

1. The immediate (compile-time) portion of Forth source is run, and a dictionary is generated
2. Data flow and control flow graphs are built for the words in the dictionary. This process computes the number of parameters and returns for each word, and allocates unique registers for each element passed on the stack.
3. The graphs from the previous step are used to generate LLVM Basic Blocks and registers. Optimization passes are run and machine code for a target of choice is produced.

**Future features:**
- Benchmarks
- Standardizing movForth to be as close to "regular" Forth as possible
- Dynamic linking so you can have an interactive Forth with certain compiled words
- Interfacing with libraries that follow the C ABI

## Supported Primitives
MovForth is powerful because operations that are immediate in a traditional interpreted Forth are run during compile time. Therefore, meta-compilation words like `IF`, `DO`, and `COLON` incur no runtime penalty.

Words available during immediate mode:

```< = + - * / SWAP ROT DUP DROP . SHOW ' , SEE [ ] IMMEDIATE ALLOT @ ! BRANCH BRANCHIF LITERAL HERE CREATE```

Words available during runtime:

```< = + - * / BRANCH BRANCHIF . DUP DROP LITERAL ! @ MALLOC ```

Locals can be used in both compile and runtime, and compile to the
exact same machine code as their stack-based counterpart.
They use the word `to` for assignment. 

Push 42 to local named "life" (creation of locals is implicit):

`42 to life`

Push the value stored in local "life" to stack:

`life`

## Contribute
I welcome all tips, comments, or help! I'll do my best to respond in a timely manner.

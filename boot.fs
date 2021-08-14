
\ Compilation steps:
\
\ First, this file is interpreted similar to a typical Forth program. This way,
\ user-defined compiling actions actually do execute during compilation time
\
\ Next, in main.cpp, we instruct the compiler to compile a certain word in the
\ dictionary. This is important, becasue some lower level words manipulate
\ interpreter state and cannot be compiled as as standalone word.
\
\
\        Dependency Tree:
\ Then, the compiler builds a dependency tree for the selected word. The word is
\ the root node and all of its offspring are derived from its definition.
\ This happens recursively until a primitive, which has no dependecies, is reached
\
\ As this tree is built, the compiler collapses data stored in the definition
\ (like the number that proceeds a LITERAL) into the word that owns it
\
\ When all offspring of a certain node have been built, the compiler searches
\ the node(or word)'s definition for BRANCH and BRANCHIF. The compiler uses this
\ information to make a control flow graph. Each word/node has its own control
\ flow graph
\
\ A sequence of words that doesn't include BRANCH or BRANCHIF
\ is a basic block. This is the same defintion that LLVM uses.
\
\
\        Stack Graph:
\ This compiler compiles forth to LLVM IR. Then LLVM does the hard work of
\ optimizing and writing assembly. But LLVM uses SSA, which is a register-based
\ format, and Forth is a stack-based. So a stack graph is needed to symbolically
\ link the output of one word to the input of another.
\
\ Imagine a sequence of words, each with as many input nodes as it
\ takes from the stack and as many output nodes as it pushes to the
\ stack. The stack graph algorithm creates an edge between every pair of input\output
\ nodes and assigns a name to the edge. An edge is a register.
\
\ The stack graph is printed out on the console during compilation.
\
\ Registers are named X-Y,
\    where X is the index of the basic block it is consumed in,
\    and Y is an index that is unique to the register within its basic block
\
\
\        Stack Graph with Control Flow
\ The compiler supports not just structured control flow but any CFG
\ that can be represented by BRANCH and BRANCHIF. Even if two basic blocks
\ merge into the same final BB but push different register indexes,
\ the registers that cross control flow edges are guaranteed to be the same
\ for all merging basic blocks.

\ (I made a very elegant solution to this here. It piggybacks on the control
\ flow-less stack graph DFS without needing passes of its own)

\ This assumes that BBs that merge together push the same number of
\ stack elements. Otherwise, this would be a very, very hard problem.


: ['] immediate ' ;

: [,] immediate , ;

: negative -1 * ;

: jump>
    literal branch ,
    here
    0 , ;

: jumpif>
    literal branchif ,
    here
    0 , ;

: >land
    dup
    here - negative
    swap ! ;

: land<
    here ;

: <jump
    literal branch ,
    here - , ;

: <jumpif
    literal branchif ,
    here - , ;

: if immediate
    jumpif> ;

: else immediate
    jump>
    swap
    >land ;

: then immediate
    >land ;

: while immediate
    land< ;

: repeat immediate
    <jump ;

: repeatif immediate
    <jumpif ;

: test 1 if 69 69 else 420 420 then ;

see

0 test

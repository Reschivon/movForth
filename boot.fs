
\ Compilation steps:
\
\ First, this file is interpreted as a typical Forth program. This way,
\ user-defined compiling actions actually do execute during compilation time
\
\ Next, in main.cpp, we instruct the compiler to compile a chosen word in the
\ dictionary ("test" in this case)
\
\        Dependency Tree:
\ Then, the compiler builds a dependency tree for the selected word. The word forms
\ the root node and all of its offspring nodes are determined from its definition.
\ This happens recursively until a primitive word, which has no dependecies, is reached
\
\ As this tree is built, the compiler collapses number data stored in the definition
\ (like the number that proceeds a LITERAL) into the word that owns it
\
\ When all offspring of a certain node have been built, the compiler searches
\ the node/word's definition for BRANCH and BRANCHIF. The compiler uses this
\ information to make a control flow graph. Each word/node has its own control
\ flow graph
\
\ A sequence of words that don't include BRANCH or BRANCHIF
\ is a basic block. This is the same defintion that LLVM uses.
\
\
\        Stack Graph:
\ This compiler compiles forth to LLVM IR. But LLVM uses SSA, which is a register-based
\ format, and Forth is a stack-based. So a stack graph is needed to
\ link the output of one word to the input of another.
\
\ Imagine a sequence of words, each with as many input nodes as it
\ takes elements from the stack and as many output nodes as elements that it pushes to the
\ stack. The stack graph algorithm creates an edge between every pair of input/output
\ nodes and assigns a name to the edge. An edge becomes a register in IR.
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
\ that can be represented by BRANCH and BRANCHIF. If two basic blocks
\ merge into the same final BB, they will not push differently named registers,
\ In other words, the registers that cross control flow edges are guaranteed to be the same
\ for all merging basic blocks.

\ (I made a very elegant solution to this here. It piggybacks on the control
\ flow DFS pass so it doesn't need its own pass)


\ BBs that merge together *must* push the same number of
\ stack elements. Making this assumption makes the code much easier to write.




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

: gen 177013 ;

\ the word to be compiled.
\ : main 0 if 69 69 else 420 420 then . . ;


: main gen . ;

main

see

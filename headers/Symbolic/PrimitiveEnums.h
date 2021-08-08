

#ifndef MOVFORTH_PRIMITIVEENUMS_H
#define MOVFORTH_PRIMITIVEENUMS_H

namespace mov{
    enum primitive_words{
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        SWAP,
        ROT,
        DUP,
        DROP,
        EMIT,
        SHOW,
        TICK,
        COMMA,
        SEE,
        TOIMMEDIATE,
        TOCOMPILE,
        IMMEDIATE,
        FETCH,
        STORE,
        BRANCH,
        BRANCHIF,
        LITERAL,
        HERE,
        CREATE,
        EXIT,
        OTHER
    };
}
#endif //MOVFORTH_PRIMITIVEENUMS_H

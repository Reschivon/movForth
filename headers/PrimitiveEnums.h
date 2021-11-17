

#ifndef MOVFORTH_PRIMITIVEENUMS_H
#define MOVFORTH_PRIMITIVEENUMS_H

namespace mov{
    enum primitive_words{
        ADD         =0,
        SUBTRACT    =1,
        MULTIPLY    =2,
        DIVIDE      =3,
        SWAP        =4,
        ROT         =5,
        DUP         =6,
        DROP        =7,
        EMIT        =8,
        SHOW        =9,
        TICK        =10,
        COMMA       =11,
        SEE         =12,
        TOIMMEDIATE =13,
        TOCOMPILE   =14,
        IMMEDIATE   =15,
        ALLOT       =16,
        FETCH       =17,
        STORE       =18,
        BRANCH      =19,
        BRANCHIF    =20,
        LITERAL     =21,
        HERE        =22,
        CREATE      =23,
        EXIT        =24,
        OTHER       =25,
        EQUALS      =26,
        MALLOC      =27,
        FREE    =28
    };
}
#endif //MOVFORTH_PRIMITIVEENUMS_H



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
        FETCH       =16,
        STORE       =17,
        BRANCH      =18,
        BRANCHIF    =19,
        LITERAL     =20,
        HERE        =21,
        CREATE      =22,
        EXIT        =23,
        OTHER       =24
    };
}
#endif //MOVFORTH_PRIMITIVEENUMS_H

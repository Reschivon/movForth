: ['] immediate ' ;

: [,] immediate , ;

: payload 11 . ;

: negative -1 * ;

: jump>
    here
    literal branch ,
            0      , ;

: jumpif>
    here
    literal branchif ,
            0        , ;

: >land
    dup
    here - negative
    swap ! ;

: land< immediate
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
    >land
    jump> ;

: then immediate
    >land ;

: while immediate
    land< ;

: repeat immediate
    <jump ;

: repeatif immediate
    <jumpif ;

: test if 69 . 69 . else 420 . 420 . then ;

see

0 test
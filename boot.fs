: ['] immediate ' ;

: [,] immediate , ;

: payload 11 . ;

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

: test if 69 69 else 420 then ;

see

0 test
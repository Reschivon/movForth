: ['] immediate ' ;

: [,] immediate , ;

: payload 11 . ;

: negative -1 * ;

: jump>
    literal branch ,
    here 0 , ;

: jumpif>
    literal branchif ,
    here 0 , ;

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

: then immediate
    >land ;

: while immediate
    land< ;

: loop immediate
    <jump ;

: loopif immediate
    <jumpif ;

: r while 69 . 69 . 0 loopif 420 . ;

see

1 r


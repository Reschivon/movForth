\ Implementation of Bubble Sort
\ Lotta boilerplate here, because there's no mechanism
\ to `include` files (yet). The locals here compile
\ to the same code as an equivalent stack-using program


\ Control flow utilities

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

: land< \ equivalent of MARK
    here ;

: <jumpif \ equivalent of RESOLVE
    literal branchif ,
    here - , ;

: while immediate
    land< ;

: repeatif immediate
    literal literal , \ compile `0 =` into word being defined
    0 ,
    literal = ,
    <jumpif ;

: if immediate
 jumpif> ;

: else immediate
 jump>
 swap
 >land ;

: then immediate
 >land ;

\ Array utilities

: cells 8 * ; \ movForth uses 64 bit only for now

: get-value \ pointer index -- value
    cells + @ ;

: set-index \ pointer index value --
    rot rot
    cells +
     ! ;

: swap \ pointer index1 index2 --
    to index2 to index1 to pointer

    pointer index1 get-value to num1
    pointer index2 get-value to num2

    pointer index1 num2 set-index
    pointer index2 num1 set-index
;

: print-array \ pointer length --
    to length to pointer
    0 to index
    while
        pointer index get-value .
        index 1 + to index
    index length < repeatif
;


: random \ seed -- seed
    1309 * 13849 + 65535 &
;

: populate \ length -- pointer
    to length
    42 to seed
    length 8 * malloc to pointer

    0 to i
    while
        seed random to seed
        pointer i seed set-index
        i 1 + to i
    i length < repeatif

    pointer
;


: main
    20000 to length
    length populate to array

    0 to i
    while
        0 to j
        while
            array j 1 + get-value
            array j get-value
            < if
                array j j 1 + swap
            then

            j 1 + to j
        j length i - 1 - < repeatif

        i 1 + to i
    i length 1 - < repeatif

    array length print-array

    array free
;


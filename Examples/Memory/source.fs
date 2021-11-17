: cells 4 * ;

: main
    2 cells malloc

    dup 99 !
    dup 1 cells + 100 !

    dup @ .
    dup 1 cells + @ .

    free
;
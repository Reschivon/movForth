\ Some simple array utilities

: cells 8 * ; \ movForth uses 64 bit only for now

: get-index \ pointer index -- value
    cells + @ ;

: set-index \ pointer index value --
    rot rot
    cells +
     ! ;

: main
    2 cells malloc to array

    array 0 99 set-index
    array 1 100 set-index

    array 0 get-index .
    array 1 get-index .

    array free
;
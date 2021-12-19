

: land<
    here ;


: <jumpif
    literal branchif ,
    here - , ;

: <jump
    literal branch ,
    here - , ;

: while immediate
    land< ;

: until0 immediate
    literal literal , \ compile `1 -` into word being defined
    1 ,
    literal - ,

    literal dup ,     \ compile `dup` into word being defined

    literal literal , \ compile `0 =` into word being defined
    0 ,
    literal = ,

    <jumpif
;

: repeatif immediate
    literal literal , \ compile `0 =` into word being defined
    0 ,
    literal = ,

    <jumpif
;

: main
    1 1
    40 while
        rot rot     \ send the two fib numbers to top
        dup rot +   \ add the fib numbers and destroy the smaller one
        dup .       \ print the first fib number
        rot         \ put index back on top
    until0

    drop drop drop
;


\ main \ you can run it during interpretation too



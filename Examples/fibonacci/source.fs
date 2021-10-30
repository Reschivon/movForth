

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


: show-under
    
;

: fib
    
;

: main
    1 1
    1000000 while
        rot rot 
        dup rot +
        rot rot dup . rot
        rot   
    until0
    
    drop drop drop
;

main



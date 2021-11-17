
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

: if immediate
 jumpif> ;

: else immediate
 jump>
 swap
 >land ;

: then immediate
 >land ;

: land< \ equivalent of MARK?
    here ;

: <jumpif \ equivalent of RESOLVE?
    literal branchif ,
    here - , ;

: while immediate
    land< ;

: repeatif immediate
    literal literal , \ compile `0 =` into word being defined
    0 ,
    literal = ,
    <jumpif ;

: 2dup
    swap dup
    rot dup
;

: 2swap
    rot rot
    ;

: gep \(array* index -- )
    +
;

: gep+1 \(array* index -- )
    + 1 +
;



: swapif \ (array* index -- )
    2dup 2dup       \(array* index array* index array* index)
    2dup gep        \(array* index array* index gep)
    2swap gep+1 @   \
    swap < if \ not less than
        2dup gep @


: main
    12 while
        dup .
        1 - 
    dup repeatif
    drop
;



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

: main
    12 while
        dup .
        1 - 
    dup repeatif
    drop
;

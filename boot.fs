: ['] immediate ' ;

: [,] immediate , ;

: payload 11 . ;

: inject immediate
    ['] literal [,]
    ['] payload [,]
    ['] , [,]
;

: postpone immediate
    literal literal
    ' ,
    literal ,
;

: 1+ postpone payload 1 + ;

21 1+ .

: loop 33 . branch -3 ;

see


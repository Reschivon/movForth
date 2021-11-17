: cells 4 * ;

: [,] immediate , ;

: variable
    1 cells malloc
    create            \ token after variable is variable name
    literal literal , \ compile literal to the variable
    literal [,]     \ compile the malloc address
;

: main
    variable dog
;

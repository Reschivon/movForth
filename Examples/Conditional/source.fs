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

: if immediate
 jumpif> ;

: else immediate
 jump>
 swap
 >land ;

: then immediate
 >land ;

: gen32 32 32 ;

: gen42 42 42 ;

: print . ;

: conditional if gen32 else gen42 then print print ;

: main 1 conditional ;

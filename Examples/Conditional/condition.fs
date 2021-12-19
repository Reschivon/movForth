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

: conditional if 32 32 else 42 42 then . . ;

: main 1 conditional ;

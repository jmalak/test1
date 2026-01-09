.func strtold wcstold _ustrtold
.funcw wcstold 
#include <stdlib.h>
long double strtold( const char *ptr, char **endptr );
.ixfunc2 '&Conversion' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
long double wcstold( const wchar_t *ptr, wchar_t **endptr );
.ixfunc2 '&Conversion' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
long double _ustrtold( const wchar_t *ptr, wchar_t **endptr );
.ixfunc2 '&Conversion' &ufunc
.do end
.funcend
.*
.im strtoflt.gml
.*
.see begin
.seelist strtold strtod strtof atof
.see end
.*
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main( void )
{
    long double pi;
.exmp break
    pi = strtold( "3.141592653589793", NULL );
    printf( "pi=%17.15Lf\n",pi );
}
.exmp end
.class ISO C99
.system

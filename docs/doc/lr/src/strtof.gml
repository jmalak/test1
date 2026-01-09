.func strtof wcstof _ustrtof
.funcw wcstof
#include <stdlib.h>
float strtof( const char *ptr, char **endptr );
.ixfunc2 '&Conversion' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
float wcstof( const wchar_t *ptr, wchar_t **endptr );
.ixfunc2 '&Conversion' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
float _ustrtof( const wchar_t *ptr, wchar_t **endptr );
.ixfunc2 '&Conversion' &ufunc
.do end
.funcend
.*
.im strtoflt.gml
.*
.see begin
.seelist strtof strtod strtold atof
.see end
.*
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main( void )
{
    float pi;
.exmp break
    pi = strtof( "3.141592653589793", NULL );
    printf( "pi=%17.6f\n",pi );
}
.exmp end
.class ISO C99
.system

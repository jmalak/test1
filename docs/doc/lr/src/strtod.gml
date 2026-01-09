.func strtod wcstod _ustrtod
.funcw wcstod 
#include <stdlib.h>
double strtod( const char *ptr, char **endptr );
.ixfunc2 '&Conversion' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
double wcstod( const wchar_t *ptr, wchar_t **endptr );
.ixfunc2 '&Conversion' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
double _ustrtod( const wchar_t *ptr, wchar_t **endptr );
.ixfunc2 '&Conversion' &ufunc
.do end
.funcend
.*
.im strtoflt.gml
.*
.see begin
.seelist strtod strtof strtold atof
.see end
.*
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main( void )
{
    double pi;
.exmp break
    pi = strtod( "3.141592653589793", NULL );
    printf( "pi=%17.15f\n",pi );
}
.exmp end
.class ISO C90
.system

.* Extracted from vi manual, passing of named arguments to a macro.
.*
.dm tstmac begin
.ty >&*1< >&*2<
.dm tstmac end
.*
.dm macpar begin
.ty >&*parm< >&*1<
.dm macpar end
.*
.tstmac two args
.tstmac 'n fg bg' tilecolor
.*
.macpar *parm=macro parameters
.macpar *parm='n fg bg' tilecolor
.macpar *parm='don't' overdo it

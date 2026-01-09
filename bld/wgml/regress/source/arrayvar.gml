.se av=0
.se av()='*1*'
&av. &av(1). &av(2-1). &av(2).
.br
.se sl()=2
.se sl()=5
.se sl()=9
&sl &sl(0). &sl(1) &sl(4).
&sl(*).
.br
.se foo=55
.* Expands to nothing -- no subscripts
&foo(*)
.se foo()=22
&foo(*)
.*
.* Now try various ways of listing all
.* the subscripts
.se cw()=3
.se cw(4)=7
.se cw()=2
.se cw(-3)=-2
.se cw(-1)=1
&cw(*)
.br
&cw(*-).
.br
&cw(*+)

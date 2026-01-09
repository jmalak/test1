.* SCRIPT IF and GOTO label testcase.
.*
.* First a simple loop with a few iterations.
.sr i=0
...loop .sr i = &i + 1
&i
.if &i lt 10 .go loop
.br
.* Now a slightly more interesting loop counter.
.sr i = -20
...loop2 .sr i = &i + 5
(&i)
.if &i lt 22 .go loop2
.br
.* Same as the first loop, but using a relative GOTO.
.* Line-oriented GOTO currently only implemented for macros.
.dm do_ten begin
.sr i=0
.sr i=&i+1
&i
.if &i < 10 .go -2
.dm do_ten end
.do_ten
.br
.* This ought to work but not implemented.
.* So skip over it.
.go not_impl
.sr i=0
.sr i=&i+1
&i
.if &i < 10 .go -2
.br
...not_impl

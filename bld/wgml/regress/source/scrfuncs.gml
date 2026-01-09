.* Hex to ASCII conversion function: &x'
.* Input may but does not have to be terminated with a period.
.* A string of 8-bit hex numbers may be provided.
H&x'65.x &x'414243616263
&x'414243616263 He&x'78.
.br
.* These aren't valid escape sequences (except for the last one)
.* and their arguments must be copied to the output.
&x'40z &x'41? &x'42! &x'43; &x'44.*
.br
.* The hex FD character is internally used as an escape. We still
.* must be able to emit it if the user asks for it.
&x'fD.
.br
.* &x' and &X' are equivalents.
&x'41.&X'42.
.br
.*
.*
.* Width function: &w'
.* Returns the width of operand in CPI (Characters Per Inch) units.
.* The operand may be a text string or a symbol.
.* &w' and &W' are equivalents.
&w'text. &W'text
.br
.se sym='spacey symbol'
&w'&sym &W'&sym.
.br
.*
.*
.* Existence function: &e'
.* Returns 1 if operand is a currently defined symbol,
.* or 0 otherwise. If the operand does not start with
.* an ampersand, it's not a defined symbol.
.* &e' and &E' are equivalents.
&e'&sym &e'sym
&E'&nosym. &E'&sym
.br
.*
.*
.* Uppercase function: &u'
.* Converts the operand text to upper case. The operand
.* can be a symbol or a text string.
.* &u' and &U' are equivalents.
&u'MiXeDcAsE.&U'12ABc4
.br
&u'&sym. &U'&nosym.
.br

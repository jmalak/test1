.* Inspired by wccerrs document. Control word separator inside delimiters,
.* fun with changing the CW separator, and stray quotes.
.* Also exercises the order in which symbol substitution and control word
.* separators are processed.
.*
.* Quote effectively vanishes, '.' is recognized/ignored.
.'
.* Quote disables separator scanning, semicolon is part of symbol.
.'sesym4=good3;'
.*
.* The dollar character is valid as part of a symbol name.
.se pfx=good1
.se pfx$=good2
.* CW separator is ';' not a valid symbol name character and
.* splits the line, leaving the opening single quote in place.
.* The remaining .' ends up doing nothing.
.sr sym1='&pfx;.'
.cw $
.* CW separator is now '$' and ';' is normal character.
.sr sym2='&pfx;.'
.* The '$' is a valid symbol name character unlike ';'. Since
.* substitution is done first, it will be consumed during
.* symbol substitution *before* being recognized as a separator.
.sr sym3='&pfx$.'
.sr sym5='&nopfx$.'
.* Undefine the pfx$ symbol
.cw ;$.se pfx$ off;.cw $
.* Now sym6 cannot be substituted and ends up as '&pfx
.se sym6='&pfx$.'
&sym1.
&sym2.
&sym3.
.br
.se pfx=good4
&sym4.
&sym5.
.* sym6 will now pick up the updated pfx symbol
&sym6.
.br
.* The lines starting with a '.' vanish without a trace in the output.
This
.'
 is some
.
text. There is
.* Symbols are substituted before control words are processed. Hence
.* a control word itself may be a result of substitution.
.se b1=b
.se b2=r
.&b1.&b2.
a break.

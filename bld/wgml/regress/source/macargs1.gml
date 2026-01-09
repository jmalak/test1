.* Exercise macro argument passing, especially delimiters.
.* Note that the &* local shows the 'raw' text, while &*1 etc.
.* hold arguments after processing delimiters and splitting
.* individual words.
.* Uses SCRIPT only, no GML.

.dm tstmac begin
.ty 0>&*< 1>&*1< 2>&*2<
.dm tstmac end

.tstmac spaced string
.tstmac 'single quote'
.tstmac "double quote"
.tstmac `back quote`
.tstmac 'backx quote'x
.tstmac x'frontx quote'
.tstmac 'bsp quote '
.tstmac ' fsp quote'
.tstmac 'first quote
.tstmac quote last'
.tstmac 'quote'middle'
.* TODO -- not correctly processed
.*.tstmac tabbed	string

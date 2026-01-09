.* Test &amp and &gml symbols that need special processing.
.* Need to test that everything that should get substituted
.* actually does, but we can still end up with '&amp' in
.* the resulting text.
.*
.ju off
This is a GML tag&gml. &gml.GDOC.
The &amp.amp. symbol is quite handy&gml. Without it,
putting '&amp.' and '&gml.' characters in the document
would be hard.
.*
.br
&gml.&gml.&amp.&amp.&gml.&amp.

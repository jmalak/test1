.* Problem case extracted from wd manual. A *= passed as a macro
.* argument might look like a fake local variable.
.*
.dm begnote begin
:DL.
.dm begnote end
.*
.dm endnote begin
:eDL.
.dm endnote end
.*
.dm note begin
:DT.&*
:DD.
.dm note end
.*
.dm mnote begin
.note &*
.dm mnote end
.*
What follows is a note.
.begnote
.mnote *=
That's a binary "*=" operator in the debugger.
.endnote
This is normal text again.

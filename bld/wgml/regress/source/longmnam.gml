.* "Long names" test. Long macro names are accepted, but only the first
.* 8 characters are considered. Thus defining 'longname2' overrides 'longname'
.* and references to either 'longname' or 'longname2' both call the same
.* macro. The 'longnam1' macro is distinct because it differs within the first
.* eight characters.
.* WGML 4.0 does not diagnose this issue.
.* 
.dm longname begin
.ty Hello, world!
.dm longname end
.*
.dm longname2 begin
.ty Hello, another world!
.dm longname2 end
.*
.dm longnam1 begin
.ty Hello, yet another world!
.dm longnam1 end
.*
.longname
.longname2
.longnam1

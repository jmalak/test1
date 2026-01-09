.* Example from the C Library Reference
.* Uses interesting functions as well as control word
.* separator (;) in GML tags.
.* vv-- the example below not done right!
.*.se func='some&amp.'
.'se func='some;'
.se _func=''

.dm class begin
.if &'length(&func.) ne 0 .do begin
.   :set symbol="func" value="&'translate("&func.",' ',';')".
.   (&func.) <-- translated
.do end
.if &'length(&_func.) ne 0 .do begin
. . .se func='&'translate("&_func.",' ',';')'
. . .ty _func yes
.do end
.dm class end

.class foo
.br
.********************************
.* Example from the C++ Class Library Reference
.* Combines the null control word (dot followed by
.* one or more spaces) with a GML tag. The SCRIPT
.* control word separator must not be recognized
.* in the SCRIPT tag.
.dm sf4 begin
&*.
.dm sf4 end
.dm esf begin
.dm esf end
.*
.dm ssf begin
.   :set symbol=function_s value="&function._s".
.   :set symbol="func_s" value=";.sf4 &function_s.;.esf ".
.dm ssf end
.*
.se function='foo_func'
.ssf
&func_s.

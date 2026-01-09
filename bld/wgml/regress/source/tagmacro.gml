.* Testcase distilled from the whelp variant of the
.* C Language Reference. 
.*
.* The I1 tag is overridden to use the ZI1 macro, but
.* the ZI1 macro is only invoked when the :I1 tag is used
.* in document text. When used from the .mtst macro, ZI1
.* does not get invoked.
.*
:set symbol='rskw'      value='&'d2c(236)'.
.*
.dm ctxkw1 begin
&rskw.&*.&rskw.
.dm ctxkw1 end
.*
:cmt. **** :I1 ****
.dm ZI1 begin
.if '&*id.' eq '' .do begin
:I1.&*.
.do end
.el .do begin
:I1 id='&*id.'.&*.
.do end
.ctxkw1 &*.
.dm ZI1 end
.*
.gt I1 add ZI1 att cont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY
.ga * PG ANY
.*
.gt ZI1 add ZI1 att cont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY
.ga * PG ANY
.***
.dm mtst begin
:I1.test1
.dm mtst end
.*
.dm ztst begin
:ZI1.test3
.dm ztst end
.*
.* This will *not* invoke ZI1 macro
.mtst
.* But this will invoke ZI1 macro as expected.
:I1.test2
.* And this will also invoke ZI1 macro because the ZI1 tag
.* is separate from the I1 tag.
.ztst
text

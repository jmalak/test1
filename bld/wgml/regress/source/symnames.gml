.* Define simple string symbols.
.* The length of the symbol name has unexpected effects.
.se subst = 'orig'
.se langsuffup = 'C'
.se langsuffx = 'X'
.se langsuff = 'c'
.*
.* The following two get substituted
&subst. &subst
.* The second gets substituted, first does not
my&subst_suff my&subst._suff
.br
.* None of these get substituted
&subst_to_OBJ &langsuff_to_obj &langsuffx_to_obj
.br
.* Both get substituted!!
&langsuffup_to_OBJ this&langsuffuptoo
.br
.* The second gets substituted, first does not
other&subst#suf# other&subst!suf!
.br
.* The second gets substituted, first does not
another&substitute another&subst?itute
.br
.* All three get substituted
a&subst,itute a&subst/itute a&subst\itute
.br
.* Substituted
".&subst"

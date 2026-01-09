.* Test SCRIPT symbol substitution
.*
.* Example adapted from IBM SCRIPT/VS documentation.
.se a  = '&J'
.se b  = 'K'
.se JK = 'Timothy'
.se J  = 'Nope'
.* The following produces 'Timothy'. The &a. is substituted first,
.* transforming the line to &J&b; substitution continues and
.* replaces &b, resulting in &JK. The line is processed again, resulting
.* in 'Timothy'. Note that even if the symbol J is defined, it will
.* never be used because each substitution pass continues at the first
.* character not yet processed (rather than restarting from the beginning
.* of just-replaced text). 
&a.&b
.* The following produces '2K' because &L'&a. is processed
.* first, resulting in '2', and &b is substituted next, adding 'K'.
&L'&a.&b

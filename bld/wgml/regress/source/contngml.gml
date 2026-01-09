.* Testcase for custom 'cont' GML tag extracted from vi manual.
.* Trouble when user erroneously attempts to pass a semicolon
.* as argument.

.dm @cont begin
.ct &*.
.dm @cont end
.gt cont add @cont

And then a semicolon
:cont.;
however, that didn't work.

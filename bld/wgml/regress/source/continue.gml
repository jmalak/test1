.* Test the .CT (continue) control word. Special trickery is needed
.* when a semicolon should be the continuing character, since the
.* semicolon is normally a control word separator.

.dm contsemi begin
.cw !
.ct ;
.cw ;
.dm contsemi end

.dm cont begin
.ct &*.
.dm cont end

this text conti
.cont nues
.contsemi
with a semicolon

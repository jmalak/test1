.* SCRIPT comments
.*
.*This gets ignored; this too; .* ignores entire line
.'*This is a comment as well; all of it.
.CM does not suppress control word separators.
.* The following line is not all a comment.
.cmcomment here; not a comment
.* But if we suppress separator recognition, it will be.
.'cm comment here; this is a comment too; even this
.* .CM can be the result of substitution, too
.se ignore '.cm
&ignore.also a comment
&ignore. even this is a comment
.cm multiple comments;.cmon a line;.* are also possible
.* Even a .* comment indicator can be substituted
.se star '.*
&star. Even this is a comment.
..* Also a comment.
..*Even with no space.
..'* Comment too.
..'*A comment.
.* .'.* But this is an error!
.*
.if 1 eq 0 .do begin
Condition false -- ignored.
.do end
.***************************
.*  .* style comments do not upset
.*  .if/.el pairing
.*  whereas .cm does
.***************************
.el .do begin
.* Comment, ignore.
.do end

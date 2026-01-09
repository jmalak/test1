.* Replicates problem in vi manual where '&$AMP.Delete' was
.* incorrectly substituted with existing &delete symbol. The
.* behavior of &$amp needs to mirror &amp.
.*
.se delete = 'not this'
.*
:XMP.
menu float0
    menuitem "&$AMP.Open" edit %1
    menuitem ""
    menuitem "&$AMP.Change" keyadd cr
    menuitem "&$AMP.Delete" keyadd dr
    menuitem "&$AMP.Yank" keyadd yr
    menuitem ""
    menuitem "&$AMP.Fgrep" fgrep "%1"
    meniutem "&$AMP.Tag" tag %1
endmenu
:eXMP.

#pmake: build os_os2 lcl cpu_386 support
#
# Create OS2V2HLP.EXE
# This is for debugging 32-bit programs with a 16-bit trap file
# on OS/2 2.x.
#
host_cpu = 386
host_os  = os2

!include cproj.mif
!include deftarg.mif
!include defrule.mif

w : .symbolic os2v2hlp.exe
        @%null

.asm : ../asm

os2v2hlp.exe : os2v2hlp.obj
        $(linker) system os2v2 lx op dosseg, map name $^@ f os2v2hlp.obj &
                imp DOS32WRITE DOSCALLS.282,DOS32DEBUG DOSCALLS.317

# makefile for tstrtnt1.c - test that DLL threading
# starts as expected.

all: tstdll1.dll tstexe1.exe

tstdll1.dll : tstrtnt1.c
        wcl386 -zq -wx -i$(%WATCOM)\h\nt -d2 -bd -bt=nt -bm -fm -DQA_MAKE_DLL -l=nt_dll -fe=tstdll1.dll -"option offset=0x01000000" tstrtnt1.c
        wlib -q -o=tstdll1.lib tstdll1.dll

tstexe1.exe : tstrtnt1.c tstdll1.dll
        wcl386 -zq -wx -i$(%WATCOM)\h\nt -bt=nt -bm -fm -DQA_MAKE_EXE -l=nt -fe=tstexe1.exe tstrtnt1.c tstdll1.lib

clean: .symbolic
        @rm -f tstdll1.dll tstdll1.lib tstexe1.exe
	@rm -f *.obj *.map *.err

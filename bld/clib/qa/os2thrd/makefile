# makefile for test1.c - test that DLL threading
# starts as expected.

all: tdll32_1.dll texe32_1.exe texe32_2.exe tdll32_3.dll texe32_3a.exe texe32_3b.exe

tdll32_1.dll : test1.c
        wcl386 -zq -wx -d1 -i$(%WATCOM)\h\os2 -fm -bm -bt=os2 -bd -DQA_MAKE_DLL -l=os2v2_dll -fe=tdll32_1.dll test1.c
        wlib -q -o=tdll32_1.lib tdll32_1.dll

texe32_1.exe : test1.c tdll32_1.dll
        wcl386 -zq -wx -d1 -i$(%WATCOM)\h\os2 -fm -bm -bt=os2 -DQA_MAKE_EXE -l=os2v2 -fe=texe32_1.exe test1.c tdll32_1.lib

texe32_2.exe : test2.c
        wcl386 -zq -wx -d1 -i$(%WATCOM)\h\os2 -fm -bm -bt=os2 -l=os2v2 -fe=texe32_2.exe test2.c

tdll32_3.dll : test3.c
        wcl386 -zq -wx -d1 -i$(%WATCOM)\h\os2 -fm -bm -bt=os2 -bd -s -DQA_MAKE_DLL -l=os2v2_dll -fe=$@ test3.c
        wlib -q -o=tdll32_3.lib $@

texe32_3a.exe : test3.c tdll32_3.dll
        wcl386 -zq -wx -d1 -i$(%WATCOM)\h\os2 -fm -bm -bt=os2 -DQA_MAKE_EXE -DQA_DLL_LINK -l=os2v2 -fe=$@ test3.c tdll32_3.lib

texe32_3b.exe : test3.c
        wcl386 -zq -wx -d1 -i$(%WATCOM)\h\os2 -fm -bm -bt=os2 -DQA_MAKE_EXE -DQA_DLL_LOAD -l=os2v2 -fe=$@ test3.c

clean: .symbolic
        @rm -f *.exe *.dll *.lib
        @rm -f *.obj *.map *.err

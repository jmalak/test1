# WGML regression testing
#
# In general, the output is expected to match WGML 4.0.

!include source/gmltest.mif

# Find the right WGML
!ifdef __MSDOS__
!   define wgml ..\dos386\wgml.exe
!else ifdef __OS2__
 !  define wgml ..\os2386\wgml.exe
!else ifdef __NT__
!   define wgml ..\nt386\wgml.exe
!else ifdef __LINUX__
!   define wgml ../linux386/wgml.exe
!else
!  error unsupported host OS
!endif

# The open bracket '(' has a special meaning in *nix shells. Have to
# compensate.
!ifdef __UNIX__
B = \(
!else
B = (
!endif

# Use a separate tiny GML library. The output devices, notably PostScript,
# have changed in the past and may change again. That is likely to affect
# the PostScript output and would result in spurious differences with the
# known good output files.
gml_lib = syslib
gml_inc = source

.extensions:
.extensions: .erc       # Differences, should be empty (console)
.extensions: .era       # Ditto (TASA device)
.extensions: .erp       # Ditto (PostScript device)
.extensions: .out       # WGML console output (.TY)
.extensions: .txt       # WGML ASCII text (ASA) output
.extensions: .ps        # WGML PostScript output
.extensions: .gml       # Source GML file

# Run source through WGML, produce .out file from console output
.gml: source
.gml.out:
    $(wgml) -q -l $(gml_lib) -i $(gml_inc) $* $B wscript dev tasa > $@

# Run source through WGML, format for TASA device, produce .txt output.
.gml: source
.gml.txt:
    $(wgml) -q -l $(gml_lib) -i $(gml_inc) $* $B wscript dev tasa output $@

# Run source through WGML, format for PostScript device; assumed to
# produce a $*.ps file. Force date/time via datetime.opt to get
# reproducible output files that don't change with each run.
.gml: source
.gml.ps:
    $(wgml) -q -l $(gml_lib) -i $(gml_inc) $* $B wscript dev ps file datetime


# Compare actual (.out) and known good (.chk) output,
# produce .er? file with differences. Note that .er? files
# are left behind if the test fails.
#
# There are three variants for console output (.out/.erc), TASA device
# output (.txt/.era), and PostScript device output (.ps/.erp).
# The resulting .er? file will be an empty file on success and will
# contain the differences on failure.
# NB: Must compare whitespace, too! No -b switch.

.out.erc: .precious
    @if not exist source/$*.out missing_source_$*.out
    -diff source/$*.out $*.out > $*.erc
    diff $*.erc <<
<<
    @echo === Test $* OK ===

.txt.era: .precious
    @if not exist source/$*.txt missing_source_$*.txt
    -diff source/$*.txt $*.txt > $*.era
    diff $*.era <<
<<
    @echo === Test $* OK ===

.ps.erp: .precious
    @if not exist source/$*.ps missing_source_$*.ps
    -diff source/$*.ps $*.ps > $*.erp
    diff $*.erp <<
<<
    @echo === Test $* OK ===


test : .symbolic start_test $(test_targ)
    @echo === SUCCESS ===
#    %append $(log_file) PASS $(%__CWD__)


# Clean output files; everything else depends on them and will
# be re-generated.
start_test : .symbolic
    @if exist *.out rm *.out
    @if exist *.txt rm *.txt
    @if exist *.ps  rm *.ps


clean : .symbolic
    @if exist *.er? rm *.er?
    @if exist *.out rm *.out
    @if exist *.txt rm *.txt
    @if exist *.ps  rm *.ps
    @if exist *.lis rm *.lis


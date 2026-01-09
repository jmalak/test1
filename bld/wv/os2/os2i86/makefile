#pmake: os_os2 cpu_i86
#
# Open Watcom Debugger makefile for OS/2
#

host_OS  = os2
host_CPU = i86

syshdir  =

# Workaround for a possible compiler bug, should
# be investigated/fixed/removed. Causes wv.exe to
# crash during startup in the madman module.
extra_c_flags_i64 = -od

#add '16' suffix to avoid conflicts
tail_char_os2 = 16

sysobjs =            &
        os2init.obj  &
        os2lkup.obj  &
        os2chk.obj   &
        os2cmd.obj   &
        os2filio.obj &
        os2scrn.obj

!include ../../master.mif

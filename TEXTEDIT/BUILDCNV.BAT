@echo off
echo Building TXTWRITE.CNV...
gcc -o txtwrite.exe txtwrite.c
echo Building TXTRTF.CNV...
gcc -o txtrtf.exe rtfactn.c rtfreadr.c
upx txtwrite.exe txtrtf.exe
ren TXTWRITE.EXE TXTWRITE.CNV
ren TXTRTF.EXE TXTRTF.CNV

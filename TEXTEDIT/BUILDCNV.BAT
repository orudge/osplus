@echo off
echo Building TXTWRITE.CNV...
gcc -o txtwrite.exe src/convert/txtwrite.c
echo Building TXTRTF.CNV...
gcc -o txtrtf.exe src/convert/rtfactn.c src/convert/rtfreadr.c
upx txtwrite.exe txtrtf.exe
move txtwrite.exe bin\%1\txtwrite.cnv
move txtrtf.exe bin\%1\txtrtf.cnv

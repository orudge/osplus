@echo off
upx -9 bin/realdos/ospedit.exe bin/djgppsnd/ospedit.exe bin/djgpp/ospedit.exe
upx -9 bin/realdos/txtrtf.cnv bin/realdos/txtwrite.cnv
upx -9 --compress-resources=0 bin/win32/ospedit.exe bin/win32/txtrtf.cnv bin/win32/txtwrite.cnv
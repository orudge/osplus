@echo off
upx --best --8086 bin/realdos/ospedit.exe bin/realdos/txtrtf.cnv bin/realdos/txtwrite.cnv
upx --best bin/djgppsnd/ospedit.exe bin/djgpp/ospedit.exe
upx --best --compress-resources=0 bin/win32/ospedit.exe bin/win32/txtrtf.cnv bin/win32/txtwrite.cnv
upx --best --compress-resources=0 bin/w32msvc/ospedit.exe bin/win32/txtrtf.cnv bin/win32/txtwrite.cnv
upx --best --compress-resources=0 bin/mingw32/ospedit.exe bin/win32/txtrtf.cnv bin/win32/txtwrite.cnv

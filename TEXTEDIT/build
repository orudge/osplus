#/usr/bin/sh
echo "Building OSPlus Editor with sound..."
gcc -o ospedit aboutosp.cpp aboutdlg.cpp ospedit1.cpp ospedit2.cpp ospedit3.cpp sound.c -I/root/tvision/include -lrhtv -lalleg -lalleg_unsharable -lm -lXxf86dga -lXxf86vm -lXext -lvga -lesd -D__LINUX__ -L/usr/X11R6/lib
echo "Building OSPlus Editor without sound..."
gcc -o ospedtns aboutosp.cpp aboutdlg.cpp ospedit1.cpp ospedit2.cpp ospedit3.cpp sound.c libc.c -I/root/tvision/include -lrhtv -D__LINUX__ -DLINUX_NO_SOUND_SUPPORT
echo "Building TXTWRITE.CNV..."
gcc -o txtwrite.cnv txtwrite.c -D__LINUX__
echo "Building TXTRTF.CNV..."
gcc -o txtrtf.cnv rtfactn.c rtfreadr.c -D__LINUX__

OSPLUS DISK IMAGER SOURCE CODE NOTES
====================================

This code requires Borland C++ to compile. I believe it will work
with Borland C++ 3.0 or higher, but I have only compiled and tested
it with Borland C++ 4.51. Any version of BC above 4.0 should work.
See the /README.TXT file in the CVS repository for information on
where to obtain Turbo Vision.

I have used the code in RAWRITE by Mark Becker. Thanks to him for
creating the code. I have also incorporated disk reading into Disk
Imager, using the code from RAWREAD, by Hans Lermen.

It is recommended you use the Borland C++ IDE for development, but a
MAKEFILE has been provided anyway. You must modify BCPATH= in the
makefile.

Finally, the code produced by the IDE seems to be smaller than the
code produced by the makefile. I'm guessing that this is because
the IDE's Generate Makefile command didn't include all the
command-line parameters I specified in the options dialog.

Happy hacking!

Owen Rudge, 26th May 2001
Last Update: 2nd September 2001
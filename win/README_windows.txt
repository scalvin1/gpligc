GPLIGC/OGIE for windows

Author:
    Hannes Krueger

Bug reports / Support / Forum
    http://sourceforge.net/projects/gpligc/support

Mailing list
    There is a gpligc-announce mailing list, which will inform you about any updates
    (extreme low traffic)
    https://lists.sourceforge.net/lists/listinfo/gpligc-announce
    This mailing list is also used by the author to estimate the number of users

For detailed information about installation and usage
read the manual (GPLIGC_Manual.pdf)

Installation:
    Before the installation can be performed using the
    install_windows.pl
    script, Perl has to be installed. If you dont have Perl
    installed, visit
    http://www.strawberryperl.com
    to download and install the free Perl distribution.

    Subsequently, the installation of GPLIGC/Ogie can be
    carried out by running the install_windows.pl script.

    IMPORTANT!!!
    Dont run the install_windows.pl script from within
    the zip-archive. Unpack the archive first!

    For running GPLIGC the Perl Tk module is needed
    (installation details are given in the GPLIGC_Manual.pdf)

    OGIE can be used from the commandline or from within GPLIGC

    more details can be found in the GPLIGC_Manual.pdf


GPLIGC/OGIE is covered by the GPLv3 license,
which is included in the Manual (and file COPYING)
This zip package does not contain the full source-code.
The sources are available at the GPLIGC website.

The executable files of this software (ogie, etc.) are compiled
using MinGW / MSYS (www.mingw.org).


This package contains third party software:

libglut-0.dll
	see freeglut.sourceforge.net
	freeglut is a completely OpenSourced alternative to
	the OpenGL Utility Toolkit (GLUT) library.
	freeglut is released under the X-Consortium license.
libjpeg-9.dll
	Ogie is based in part on the work of the Independent
	JPEG Group. Ogie uses the jpeg-library functions, therefore
	a compiled (using mingw) version of this library is
	included. For more information see:
	http://www.ijg.org/
wgnuplot.exe
	gnuplot 4.2.6 executable file for use with GPLIGC.
	copyright/license details for gnuplot can be found in the file
	Copyright-gnuplot
	more details/info/and sourcecode can be found at
	https://sourceforge.net/projects/gnuplot/files/gnuplot/4.2.6/
	www.gnuplot.info

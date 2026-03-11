$Id: README.txt,v 1.4 2016/06/17 09:21:17 alex Exp alex $

August 2021

                                   CSOFT
                                   =====


Hi!  This is my general-purpose C library (LIBGPL) consisting of
networking, regular expression, string manipulation, and assorted
other functions. The code is implemented as "packages", usually
with a "<prefix>_util.h" header file and a "<prefix>_util.c" code
file, where <prefix> is a mnemonic prefix used for the package's
function names.

A great deal of the code (e.g., the basic TCP/IP networking code)
has been in use, in one form or another, for over 30 years.  In the
early to mid-1990's, after reading Bertrand Meyers' OBJECT-ORIENTED
SOFTWARE CONSTRUCTION (1st Edition)
( http://www.geonius.com/writing/other/oops.html ), I reworked most
of the APIs and internals to implement a simple object-based approach
to using the packages.

More documentation about the software as a whole can be found at:

	http://www.geonius.com/software/

The web pages for the individual packages basically echo the source
file prologs.

If you have any questions or find any problems, please let me know.
The software is free to use without asking; you can leave my name
in, take my name out, modify the software, whatever.  I appreciate
suggestions on how to improve or enhance the sofware - even
constructive criticism - but the likelihood of my implementing
suggested changes depends on the time I have available to do so
and on the number of programs I have that would be affected by
the changes.

				Alex Measday
				calexm5y@gmail.com
				http://www.geonius.com/


CONTENTS
--------

The archive file ("csoft.zip" or "csoft.tgz") consists of the following
directories/software:

    include		- The "<prefix>_util.h" headers for LIBGPL.
    include/palm	- PalmOS-specific headers.
    include/vms		- VAX/VMS-specific headers.

    bsd-xdr		- XDR library for platforms that need it.

    libgpl		- The "<prefix>_util.c" source for LIBGPL.

    anise		- A simple HTTP/FTP server based on LIBGPL.

    tools		- Miscellaneous test programs that use LIBGPL.


INSTALLATION
------------

Choose the correct distribution file for your platform:

    csoft.tgz = all text files have LF-terminated lines.

    csoft.zip - all text files have CR/LF-terminated lines.

[This paragraph is partly historical; Visual Studio 2019 seems to behave
better than earlier versions.]  Be sure and use "csoft.zip" on Windows;
otherwise Visual C++ will croak on the solution and project files.
(The contents of the two archive files are identical except for the
line termination characters in text files.  Advanced users can work
with either archive file by setting the appropriate line-termination
options when extracting the files.)  (Later versions of Visual Studio
seem to be unable to load solution and project files that don't have
the UTF-8 byte-order marker at the beginning of the file.  Add the BOM
in if you encounter this problem.  The 3-character UTF-8 BOM is 0xEF,
0xBB, 0xFF.)

Extract the archive into a directory you've set up.  If your platform
(e.g., UNIX or LINUX) has an XDR/RPC library, get rid of CSOFT's
"include/rpc" and "libxdr" directories.


BUILD
-----

On Windows, open the appropriate solution file.  The test programs are
Win32 console applications, so screen output is abysmally slow.  (My
current build and test environments are Windows 7 and Windows 10 with
Visual Studio 2019, Community Edition.)  The solution files are in the
top-level directory; build them in the following order:

    csoft.sln		- The Base (bsd-xdr, libgpl, tools, anise)

    ... whatever other solutions you wish to build ...

The generated object, library, and executable files are stored in
"DebugWin32" or "ReleaseWin32" subdirectories of the top-level
directories "obj", "lib", and "bin", respectively.

On other platforms, the different directories have Make and build files.
The "gold standards" are the "Makefile.linux" files, which build almost
all of the software.  Other platforms' Make files are derived from the
Linux Make files and add or remove files that don't apply to a platform;
e.g., removing the System V IPC-related files in "libgpl" or adding the
PalmOS/NDS-specific standard I/O files in "libgpl".

    Makefile.linux	- I've used Redhat 6.2, 7.3, Fedora Core 2,
			  and, currently, FC15 and Linux Mint 20.1.

    Makefile.cygwin	- I'm using both Cygwin (Windows 7) and Cygwin64
			  (Windows 10).  I added "-ltirpc" to the link
			  command line for programs that need the RPC/XDR
			  library.

    Makefile.android	- I've used Android 5.1 and Android 10.  I use
			  the Android Native Development Kit (NDX) under
			  Linux.  My NDK GNU Make files include
			  "android-ndk-make-setup.txt" from the top-level
			  CSOFT include directory; this file has detailed
			  information about setting up your build environment.

    Makefile.palm68k	- PalmOS using "prc-tools-remix"; the old
			  "prc-tools" package no longer worked on
			  FC15.  I build "prc-tools-remix" in the
			  same-named subdirectory of "/opt" and
			  install the cross-platform build tools
			  in /usr/local or /usr/share or wherever.
			  My PalmOS GNU Make files include
			  "palmos-prc-make-setup.txt" found in the
			  top-level, CSOFT include directory; this
			  file has detailed information about setting
			  up your build environment.

    Makefile.nds	- Nintendo DS using devkitARM.  My programs
			  actually work on the NDS.

Inactive build files (meaning I no longer have access to these platforms),
most of which are not included in the "csoft.tgz/.zip" distribution:

    Makefile.solaris	- If you've got GCC, you can comment out Sun's
			  compiler.  (Or comment it in depending on *my*
			  current configuration when I zipped the file!)

    Makefile.freebsd	- Every so often, I build, but don't test, CSOFT
			  in my web hosting service's FreeBSD shell account.
			  I haven't done so in many years.

    Makefile.tcc	- Tiny C Compiler ( https://bellard.org/tcc/ )
			  by Fabrice Bellard.  The compiler had a problem
			  with bytes or shorts in structs, which caused
			  problems in my CORBA stuff.  Aside from that,
			  it is an amazing compiler!

    Makefile.vxworks	- Not used since 1996, when my software ran on
			  rack-mounted Motorola 68030/68040 single-board
			  computers with application-specific IC-based
			  hardware boards.

    vaxc.com		- DCL script that builds the library or programs
			  under VAX/VMS.  I used to have access to an old
			  VAXstation which croaked during a power outage
			  in Fall 2004; before then, I was able to build
			  and test the software, including the CORBA stuff.
			  I haven't had access to workstations running
			  newer versions of VMS and DEC C.

Go into the subdirectories and run "make -f Makefile.<platform>" or
"@vaxc.com".  For Nintendo DS, PalmOS, VAX/VMS, and Windows, you'll
need to build LIBXDR.

Regarding LIBGPL's regular expression parser...  On Linux, I let the
Makefile generate "rex_util_y.c" using Bison and sed.  On the other
platforms, most of which don't have Bison or sed installed, the library
is built using the Linux-pre-generated "rex_util_y.c" file.  In 2016,
I replaced the aging "rex_util_y.c" with a newer file generated by a
more up-to-date version of Bison.  If you have problems compiling
"rex_util_y.c", try replacing it with "rex_util_y.c.old".

LIBGPL's string manipulation package, "str_util.c", has a number of
common, non-ANSI string functions that are found on some platforms,
but not all.  These functions are protected by HAVE_STRxyz macros
defined in "include/pragmatics.h".  Some of these functions have
been neither compiled nor tested since I haven't used them yet on
the platforms that don't have them.  Sorry!

On UNIX/LINUX platforms, I usually construct a parallel directory of symbolic
links using "lndir" and build in that directory; for example:

    ~/src/include	- Source directories.
    ~/src/libgpl
    ...

    ~/obj/include	- Object directories containing links to source files.
    ~/obj/libgpl
    ~/obj/...

Under VMS, programs that expect a UNIX-like, argc/argv command line must be
defined in DCL as foreign commands.


Nintendo DS
===========

My command-line programs make use of libgpl/StdIONDS.c as the main program,
which then calls my programs' "main" routines.  See anise/Makefile.nds or
tools/Makefile.nds for the preprocessor magic that makes this possible.

(I'll expand this section when time and inclination permit!)


PalmOS
======

    NOTE:  Many of the EPOCH3 tools make use of my CORBA middleware
    implementation.  Early in 2006, the EPOCH3-specific IDL grew so large
    that one of the code sections overflowed and the CORBA-based EPOCH3
    tools would no longer build under PalmOS.  Base CORBA clients such as
    colior still build successfully.  Because of the code section overflow
    and the global variable problem described below, I haven't done any
    more PalmOS development since early 2006.  (Plus, as of August 2007,
    I'm still using an old Palm M105, mostly to read E-books I download
    from Project Gutenberg!)

PalmOS ... aarrgghh!  I built the software using the latest version of
prc-tools under Fedora Core 2.  Because of the Palm's restrictions on the
size of code and data regions, I had to resort to explicitly allocating
different LIBGPL/LIBEPL packages to separate, GCC object-code sections.
Throughout the header files and in some of the source files, you'll see
this allocation:

    OCD ("<prefix>_util")

The "OCD" macro is defined in "pragmatics.h" for the PalmOS platform.
What a pain!  To make matters worse, the Makefile has to actually
build each program twice, the first time to determine which sections
are actually needed and the second to time to build it without the
unused sections.

I build the programs as command-line StdIO applications that can be
run from the "Preferences->Network->Options->View Log" screen.
A couple of the programs I have actually downloaded and run on my
Palm M105, linked by a serial cable to my pppd-running Linux box.
Mostly, though, I use the Palm OS Emulator (POSE).

I've run into a problem with global variables and static variables.
Although disassembled object files show the variables being correctly
allocated and initialized, the linker seems to be mapping groups of
variables to the same memory location.  I've searched the news groups
and on-line forums for answers without success.  The start-up flags
for the programs show that the programs *do* have access to global
variables, but things are just not working correctly.


#*******************************************************************************
#
#    Environment variable PALM_OS_OPTIONS is passed to GCC
#    in the palm68k Makefiles.
#
#    -DHAVE_FLPBUFFERATOF=0
#        PalmOS 3.5 doesn't have the FlpBuffer* floating-point conversion
#        functions needed for GCC users.
#
#    -DPRE_VERSION_4
#        PalmOS 4 and later declare SioMain()'s ARGV argument as "const".
#        LIBGPL's SioMain.c uses this macro to avoid compilation errors.
#
#    -DI_DEFAULT_HANDLER=HostFPrintF
#    "-DI_DEFAULT_PARAMS=HostLogFile(),"
#        These definitions override the definitions in "pragmatics.h" and
#        cause debug output to be written to the PALM emulator's (POSE)
#        default log file.
#
#*******************************************************************************

setenv PALMOS_OPTIONS '-palmos3.5 -DHAVE_FLPBUFFERATOF=0 -DPRE_VERSION_4 -DI_DEFAULT_HANDLER=HostFPrintF "-DI_DEFAULT_PARAMS=HostLogFile(),"'
#setenv PALMOS_OPTIONS '-palmos4 -DI_DEFAULT_HANDLER=HostFPrintF "-DI_DEFAULT_PARAMS=HostLogFile(),"'

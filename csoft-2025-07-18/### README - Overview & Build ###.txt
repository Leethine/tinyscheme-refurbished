$Id: ###\040README\040-\040Overview\040&\040Build\040###.txt,v 1.1 2021/09/29 05:41:06 alex Exp alex $
********************************************************************************

June 2023

Alex Measday
http://www.geonius.com/software/
calexm5y@gmail.com


              ===============================================
              = CSOFT - Quick Overview & Build Instructions =
              ===============================================

Hi!  This is my "general-purpose C library" (LIBGPL) consisting of
networking, regular expression, string manipulation, and assorted
other functions. The code is implemented as "packages", usually
with a "<prefix>_util.h" header file and a "<prefix>_util.c" code
file for each "package", where <prefix> is a mnemonic prefix used
for the package's function names.  Plus some utility programs.

After scanning this document, see the README.txt file for more detailed
information.  I was motivated to write this semi-quick overview by the
realization that it was not at all clear how to build my software.

                Specifically, in 2018 or 2019, I switched
                from a local copy of Sun's 1988 XDR code to
                the more up-to-date BSD-XDR code.  At some
                point, I finally read through its README
                file to the end and discovered that the
                developer(s) had used my TALKNET program
                during testing (cool!) and that there had
                been some trouble building my software
                (bummer!).

                https://code.google.com/archive/p/bsd-xdr/

To prevent a TAR bomb, the source code in the TAR and ZIP archives
is rooted in a directory "csoft-yyyy-mm-dd", where the last three
fields are the year, month, and day of the archive files creation.
The instructions below for the different platforms work regardless
of the root directory name.  I personally rename the directory to
"source", a convention I've used since the late 1980s.  (Perhaps
I chose that name to match the "full-word"ness of "include"?)

The CSOFT software proper consists of 3 modules ("modules" in an
informal sense), built in the following order:

   .-------------------------------------------------------------------.
   |                                                                   |
   |  (1) the BSD-XDR library (if needed by the development platform,  |
   |      e.g., Windows, Android, and PalmOS),                         |
   |  (2) my LIBGPL library, and                                       |
   |  (3) my command-line utility programs, including ANISE.           |
   |                                                                   |
   `-------------------------------------------------------------------'

The source code and build files are found in the following directories:

           .---------------------------------------------------.
           |                                                   |
           |  bsd-xdr/lib/    - the XDR library, libxdr.a      |
           |  bsd-xdr/rpc/    - the <rpc/*.h> header files     |
           |  include/        - CSOFT *.h header files         |
           |  libgpl/         - the CSOFT library, libgpl.a    |
           |  tools/          - command-line utility programs  |
           |  anise/          - HTTP/FTP server program        |
           |  cornet/         - CORTEX client program          |
           |                                                   |
           `---------------------------------------------------'

The order of building the two libraries, "libxdr.a" and "libgpl.a",
is not important as their only dependencies are static header files.
LIBGPL must be built before the utility programs and ANISE.  The
Visual Studio solution file builds everything in the correct order.
For the other platforms, I manually build the different parts in
order; the "makefile.recurse" file is an as-yet-incomplete attempt
at building everything in order for the other platforms.


===========
= Windows =
===========

In the main CSOFT directory,

    (1) Double-click "csoft.sln", a Visual Studio 2019 solution file.

VS 2019 will start up and load the solution and project files.

    (2) Choose your configuration type: "DebugWin32", "ReleaseWin32",
        "Debugx64", or "Releasex64".

    (3) "Build All".

The complete set of software will be built in the correct order: "libxdr.a",
"libgpl.a", the command-line tools, and "anise.exe".

Object files are stored in one of the following subdirectories depending
on the chosen configuration type:

    obj\DebugWin32\
    obj\ReleaseWin32\
    obj\Debugx64\
    obj\Releasex64\

Static library files are likewise stored in one of the the following
subdirectories:

    lib\DebugWin32\
    lib\ReleaseWin32\
    lib\Debugx64\
    lib\Releasex64\

Command-line executables are stored in:

    bin\DebugWin32\
    bin\ReleaseWin32\
    bin\Debugx64\
    bin\Releasex64\

As you can see, the DebugWin32 et al versions coexist in the obj, lib,
and bin directories.

I first built the x64 versions of the software in late 2022.  I cleaned
up a lot of the compilation warnings, but a few still remain.  I've only
performed perfunctory testing of some of the tool programs.  As described
in the next paragraph, most of my command-line work is done in Cygwin,
not the Power Shell or Windows Command Prompt.

I'm not a dyed-in-the-wool Windows programmer, so the solution builds static
libraries and console applications.  (I've been doing so since Visual C++ 6.)
I used to use some of the utilities in the Windows command prompt until I
discovered Cygwin.  (In August 2021, I did have the Visual Studio-built ANISE
serving out two websites on my LAN and I exercised several of the utilities.)

The Visual Studio project files for the tool programs are automatically
generated; see the "tools/" Ancillary Files section at the end of this file.


================================
= Linux, BSD, Cygwin, and Unix =
================================

My software has been built and used under a good number of Unices and a
small sample of Linux distributions over the years.  Currently, I build
under Linux Mint, Cygwin, and FreeBSD, the latter in my web host's shell
account.  The respective Makefiles invoke the default C compiler (MAKE's
CC variable), but use GCC command-line options accepted by both GCC and
CLANG.  For other Unices or with different compilers, the Makefiles may
require some tweaking.

The software has the following directory structure:

    include/*		-- "*.h" header files.
    libgpl/*		-- "*.c" and related source files plus Makefiles.
    tools/*		-- "*.c" source files plus Makefiles.
    anise/*		-- "*.c" source files plus Makefiles.
    cornet/*		-- "*.c" source files plus Makefiles.

"tools" is a collection of command-line utilities.  "anise" is a single,
command-line, HTTP/FTP server daemon.  "cornet" is a command-line CORTEX
client.

To build the software:

    (1) Build LIBGPL with the appropriate "libgpl/Makefile".
    (2) Build TOOLS with the appropriate "tools/Makefile".
    (3) Build ANISE with the appropriate "anise/Makefile".
    (4) Build CORNET with the appropriate "cornet/Makefile".

(LIBGPL must be built first; TOOLS, ANISE, and CORNET can be built in any
order.  LIBXDR is NOT needed because the library is generally part of the
distributions; but see Note 2 below.)

I usually use the "lndir" command to create a parallel object directory
for building the software:

    % cd ~				-- If "csoft-yyyy-mm-dd" is in ~.
    % mkdir obj
    % cd obj
    % lndir ../csoft-yyyy-mm-dd		-- Create parallel object tree.
        ... Creates ../csoft-yyyy-mm-dd/include link tree
        ... Creates ../csoft-yyyy-mm-dd/libgpl link tree
        ... etc.
    %

(I cross-compile my software for other platforms such as the Nintendo DS,
so I further subdivide the object directory into various combinations of
target OS and architecture.  Consequently, my LNDIR command actually goes
up two levels: "lndir ../../csoft-yyyy-mm-dd".)

And build with (under Cygwin, for example):

    % cd ~/obj/libgpl
    % ln -s Makefile.cygwin Makefile
    % make
        ...		-- Static libgpl.a is built.
    % cd ../tools
    % ln -s Makefile.cygwin Makefile
    % make
        ...		-- Command-line utilities are built.
    % cd ../anise
    % ln -s Makefile.cygwin Makefile
    % make
        ...		-- ANISE daemon is built.
    % cd ../cornet
    % ln -s Makefile.cygwin Makefile
    % make
        ...		-- CORNET client is built.
    %

(Note that, under CYGWIN, executable programs have a ".exe" extension.)

Building under Linux and BSD is the same, except that "Makefile.linux"
and "Makefile.bsd", respectively, are used instead of "Makefile.cygwin"

I added BSD "Makefile.bsd"s in June 2023 and tested them in my web host's
FreeBSD 12.3 shell account.  These files are intended for BSD's default
MAKE.  If GNU MAKE is installed, it and the "Makefile.linux"s can also
be used as well.

        NOTE 1:  In my web host's FreeBSD account, LNDIR was not installed
        and I downloaded an old "lndir.sh" shell script which incorrectly
        creates multiple symbolic links for the different parts of file
        names with embedded spaces, i.e. (and I mean i.e.!), this very
        "### README - ..." file.  I now delete this file before LNDIRing
        the source tree in my web host account.

        NOTE 2:  Under Linux, the RPC/XDR library either is part of "glib"
        or is the Transport-Independent RPC library ("tirpc").  Cygwin
        only has the latter.  There is some GNUMAKE trickery in the Linux
        Makefiles to detect which library to use; said trickery has only
        been tested on two versions of Linux Mint Xfce, one using "glib"
        and the newer one using "tirpc".  The latter requires adding
        "-ltirpc" to the link command.  Also, Cygwin stores the TI-RPC
        header files in a different directory ("/usr/include/rpc")
        than Linux ("/usr/include/tirpc/rpc").  Perhaps I'll work on
        a unified Linux/Cygwin Makefile someday ...


===========
= Android =
===========

I currently build the Android versions of the CSOFT libraries and programs
on a Linux Mint 20.3 PC, using Android Native Development Kit r25c installed
in "/opt" and my own Makefiles.  (The NDK includes the cross-compilation
toolchains needed for the target device architectures.)

        NOTE 3:  I use GNU MAKE to build the Android versions of CSOFT,
        not the NDK's own build systems (CMAKE, etc.).  The configuration
        of the cross-compilation MAKE variables is done in CSOFT's
        "include/android-ndk-make-setup.txt", which is included by all
        of CSOFT's Android Makefiles.  See that file for information
        about setting environment variables to control the target
        architectures and API levels.

        That setup file is for the NDK's LLVM-based toolchains.  If you
        need to use one of the earlier, GCC-based toolchains, read the
        prologue in and use "include/android-ndk-gcc-make-setup.txt";
        the CSOFT Makefiles will work with either toolchain since both
        setup files define the same set of MAKE variables.

I downloaded Android NDK r25c, unzipped it into /opt, and created a symbolic
link (remember to "sudo" if necessary):

    % cd /opt
    % unzip android-ndk-r25c-linux-x86_64.zip
    % ln -s android-ndk-r25c android-ndk

producing:

    /opt/android-ndk-r25c/
    /opt/android-ndk -> android-ndk-r25c

Once the NDK is installed, building CSOFT for Android is very similar
to building it for Linux/Cygwin, except that you must build the bsd-xdr
library.  The Linux/Cygwin section above described the CSOFT object
directory as "obj/" with the source code directories underneath.  In
actuality, I have an intermediate directory level containing a
subdirectory for each target I'm building on the host PC.  Each target
directory has a full tree of symbolic links to the entire CSOFT source
tree.  For example, my PC has the following object directory structure:

    obj/
        obj/ndk-aarch64/			- Build for Android device.
            obj/ndk-aarch64/bsd-xdr/lib/	- links to XDR library.
            obj/ndk-aarch64/include/		- links to CSOFT header files.
            obj/ndk-aarch64/libgpl/		- links to LIBGPL library.
            ..., etc., etc.
        obj/nds/				- Build for Nintendo DS.
            obj/nds/bsd-xdr/lib/		- links to XDR library.
            obj/nds/include/			- links to CSOFT header files.
            obj/nds/libgpl/			- links to LIBGPL library.
            ..., etc., etc.
        obj/palm68k/				- Build for PalmPilot.
            obj/palm68k/bsd-xdr/lib/		- links to XDR library.
            obj/palm68k/include/		- links to CSOFT header files.
            obj/palm68k/libgpl/			- links to LIBGPL library.
            ..., etc., etc.
        obj/x86_64linux/			- Built for Host OS.
            obj/x86_64linux/include/		- links to CSOFT header files.
            ..., etc., etc.

(On my Windows PCs, I use "obj/x86cygwin" and "obj/x86_64cygwin"
for Cygwin32 and Cygwin64, respectively.)

As with Linux and Cygwin, create the symbolic link tree in the object
directory (NOTE that the "source" directory is two levels up):

    % cd ~				-- if "csoft-yyyy-mm-dd" is in ~.
    % mkdir obj
    % mkdir obj/ndk-aarch64
    % cd obj/ndk-aarch64
    % lndir ../../csoft-yyyy-mm-dd	-- Create parallel object tree.
        ... Creates ../../csoft-yyyy-mm-dd/bsd-xdr link tree
        ... Creates ../../csoft-yyyy-mm-dd/include link tree
        ... Creates ../../csoft-yyyy-mm-dd/libgpl link tree
        ... etc.
    %

And then build with (NOTE that BSD-XDR's library is built in its
"bsd-xdr/lib" subdirectory):

    % cd ~/obj/ndk-aarch64/bsd-xdr/lib
    % ln -s Makefile.ndk Makefile
    % make
        ...			-- Static libxdr.a is built.
    % cd ../../libgpl
    % ln -s Makefile.ndk Makefile
    % make
        ...			-- Static libgpl.a is built.
    % cd ../tools
    % ln -s Makefile.ndk Makefile
    % make
        ...			-- Command-line utilities are built.
    % cd ../anise
    % ln -s Makefile.ndk Makefile
    % make
        ...			-- ANISE daemon is built.
    % cd ../cornet
    % ln -s Makefile.ndk Makefile
    % make
        ...			-- CORNET client is built.
    %

The Android software is built!  I copy the executables (tools and ANISE)
to the standard "Downloads" directory on my tablet and on my phone.

The programs successfully run in the Termux (https://termux.com/)
"Android terminal emulator and Linux environment app" on my tablet.
I highly recommend Termux and *do* get it from F-Droid, not the
Google Play Store.

Once Termux is installed, (i) the executables must be moved from the
"Downloads" directory to your Termux home directory and (ii) you then
need to "chmod 755" the files to make them executable.  (Android won't
let you do the latter in the "Downloads" directory.)  I'll write more
about this later, I hope.

If you install the needed development tools (Termux's "clang" and
"binutils-is-llvm" packages, the latter needed for AR and RANLIB),
you can also compile and link my software on your device using
CSOFT's Linux Makefiles ("Makefile.linux", not "Makefile.ndk").
It is not necessary to build BSD-XDR as the Termux development
environment already includes Linux's TI-RPC/XDR library.

With the "Termux:Boot" addon, you can start up your own services when
the device is booted.  In fact, I run an instance of my ANISE web
server in the background on my cellphone and tablet to serve out
copies of two of my own websites stored on the devices' SD cards.
(To be clear, that's one instance serving both websites on a device,
not one website on the cellphone and the other website on the tablet.)
I do this because the major Android web browsers do not support the
"file://" protocol; a couple of web browsers offer extremely limited
and therefore effectively unusable support for accessing HTML files
on the SD card.  With ANISE, I just point any browser at
"http://localhost:<port>/" to access my websites, whether or not
I'm connected to the internet.


===============
= Nintendo DS =
===============

... Remaining to be written ...

But, until then, the Nintendo DS version of CSOFT is built using
the same steps as the Android version, except that the Makefiles
are "Makefile.nds":

    % ln -s Makefile.nds Makefile

The Nintendo DS executable programs have a ".nds" extension.

Off the top of my head, you'll need devkitPro's NDS cross-compilation
release, including the FAT and DSWIFI libraries.


==============
= PalmOS 68K =
==============

... Remaining to be written ...

But, until then, the PalmOS version of CSOFT is built using the
same steps as the Android version, except that the Makefiles are
"Makefile.palm68k":

    % ln -s Makefile.palm68k Makefile

As is done for Android, the BSD-XDR library must be built.  The
PalmOS executable programs have a ".prc" extension.

I use the "prc-tools-remix" cross-compilation toolkit; its website also
has the different versions of the PalmOS Software Development Kit (SDK)
that you'll need.  I use the PalmOS Emulator (POSE) to test the programs
under Windows.  It's kind of slow and tedious to download and test the
programs on my PalmPilot m105, plus POSE has networking, screenshots,
logging, etc.  (And now it's extremely slow, in fact impossible, as
the digitizer has given up the ghost on the m105!)

Some of the CSOFT programs work and some don't in POSE, which is odd.
Programs I built and more rigorously tested a couple of years ago are
now, after a rebuild, crashing immediately on start-up.  For example,
the PRIMAL primality testing program worked correctly two years ago
and now crashes.  But the other numeric programs, SQUINT and FRIZE,
worked correctly then and work correctly now.  Time to dive back into
remote debugging of the programs in POSE.


                       =============================
                       = Ancillary File Generation =
                       =============================

Some of the CSOFT software depends on automatically generated,
intermediate source files.  The tools used to generate these files
are not available on all build platforms, so CSOFT makes use of
prebuilt versions of these files across all platforms.

For example, the regular expression, REX_UTIL package depends on
a BISON-generated C parser, "rex_util_y.c".  BISON is typically
not available in Windows Visual Studio or under VAX/VMS.  (Yes,
I realize BISON can be installed in these environments, but it
is not typically pre-installed as part of the host development
suite.)  Consequently, CSOFT includes a pre-generated "rex_util_y.c"
file.  All of the build files (UNIX Makefiles/Visual Studio project
files/VMS DCL scripts) use this configuration-managed C file and not a
generated-on-the-fly C file.  When changes to the parser are necessary,
then, after completing and testing the changes to the input ".y" file
under Linux, I update the configured ".c" file with the new version.

Another example is CSOFT's CORBA library and programs which depend
on header and C files automatically generated from CORBA IDL files
by two Python scripts.  Again, these header and C files are kept
under configuration and, when changes to the scripts are necessary
to enhance the header and/or C files for CSOFT's purposes, I manually
update the configured files after completing and testing the changes.

More generally, the UNIX Makefiles have targets for generating new
versions of ancillary header and C files:

                               <item>.h.new
                               <item>.c.new

For example:

    rex_util_y.c.new:			# Generate new C parser.
        ...
    <CORBA-package>_idl.h.new:		# Generate new CORBA header.
        ...
    <CORBA-package>_idl.c.new"		# Generate new CORBA C file.
        ...


==============================
= "include/" Ancillary Files =
==============================

The following files are generated by targets in "include/Makefile":

    rex_util_help.h.new			# RE help text for programs.
    gimx_idl.h.new			# Base CORBA header file.
    <prefix>_idl.h.new			# Additional CORBA header examples.
    ...

The regular expression help header is generated from a text file,
"rex_util_help.txt"

The CORBA headers are generated from a collection of IDL files in the root
"idl" directory using a Python script, "include/idl2h.py".  In addition to
the header files, generating these files also produces post-processing
Python script fragments used later to generate the C files in LIBGPL.
CSOFT includes a copy of the base post-processing fragment, "gimx_post.py",
which is easily regenerated by "make gimx_idl.h.new".


=============================
= "libgpl/" Ancillary Files =
=============================

The following files are generated by targets in "libgpl/Makefile.<unix>":

    rex_util_y.c.new			# RE parser C file.
    rex_tokens.h.new			# (Not Used)
    gimx_idl.c.new			# Base CORBA C file.
    <prefix>_idl.c.new			# Additional CORBA C examples.
    ...

The regular expression parser is generated by BISON from the input
"rex_util_y.y" file.  The parser tokens header is not currently
used.

The CORBA C files, "<prefix>_idl.c.new", are generated from the
"include/<prefix>_idl.h" files (NOT the ".h.new" files!) using a
Python script, "libgpl/idlh2c.py", and the post-processing Python
script fragments, "include/<prefix>_post.py".


===========================
= "tools/" Ancillary Files =
===========================

The following file is generated by a target in "tools/Makefile.<unix>":

    rex_henry_spencer_tests.h.new	# RE test suite.

The input file is "include/rex_henry_spencer_tests.txt" and the output
file is an array of strings used by the TREX program.

The following files are generated by targets in "tools/Makefile.linux"
or collectively by target "vcxproj":

    <program>.vcxproj.new		# Windows Visual Studio project file.

The Visual Studio project files for the one-C-file programs are
generated from a template file, "tools/vcxproj_template_tools.txt",
and a name-to-GUID map file, "include/vcxproj_guid_map.txt".  These
generation rules utilize GNU MAKE-specific functionality.  If GNU
MAKE is available on a non-Linux UNIX build platform, these files
can be generated by a simple "gmake -f Makefile.linux ..." command,
where "gmake" is whatever command invokes GNU MAKE as opposed to
the host's default MAKE command.

Global changes to the project files and the adding of new programs
require changes to the template and GUID map files, respectively.

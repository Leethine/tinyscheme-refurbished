/* $Id: pragmatics.h,v 1.25 2023/05/24 16:36:33 alex Exp alex $ */
/*******************************************************************************

    pragmatics.h

    Compiler, Operating System, and Debug Logging Particulars.

*******************************************************************************/

#ifndef  PRAGMATICS_H		/* Has the file been INCLUDE'd already? */
#define  PRAGMATICS_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#if defined(HAVE_CONFIG_H) && HAVE_CONFIG_H
#    include  "config.h"		/* Auto-generated configuration file. */
#endif


/*******************************************************************************
    C is nuts.
*******************************************************************************/

	/* GCC issues a warning when code uses a "char" as an array subscript.
	   (Other compilers probably do as well.)  This is a problem on
	   platforms which treat a plain "char" as a signed number; "char"s
	   above 0x7f would then be negative subscripts.  Also, arithmetic
	   with signed plain "char"s will probably produce unwanted results.
	   And it's not just your code that may be vulnerable.  The isxyz()
	   functions/macros in "ctypes.h" may, depending on a platform's
	   implementation, be indexing arrays with the signed plain "char"s
	   passed to them.  A solid solution seems to be to always cast the
	   plain "char" as an "unsigned char" and to then cast the "unsigned
	   char" to the "int" that the isxyz() functions/macros expect.  This
	   solution is rather verbose, so here's a C Preprocessor macro to
	   simplify it.  Usage:  ... isxdigit (INT_UCHAR (c)) ... */

#define  INT_UCHAR(c)  ((int) ((unsigned char) (c)))

/*******************************************************************************
    Platform peculiarities.
*******************************************************************************/

#ifndef HAVE_STRLCAT
#    define  HAVE_STRLCAT  0
#endif
#ifndef HAVE_STRLCPY
#    define  HAVE_STRLCPY  0
#endif

/* Android oddities.  (The IEEE-754 floating-point support really depends
   on the target CPU, not Android itself.  However, all the currently
   supported processors have IEEE FP support.  Old ARM processors *did*
   have mixed-endian floating-point representation, but the newer ARM
   processors are little-endian.) */

#ifdef __ANDROID__
#    define  HAVE_GETSERVBYNAME  0
#    define  HAVE_IN_PORT_T  0		/* Network port number. */
#    define  XDRREC_CADDR_T  void *	/* BSD-XDR's readit()/writeit() functions. */
#endif

/* Cygwin oddities. */

#ifdef __CYGWIN__
#    define  HAVE_FLOCK  1
#    define  HAVE_STRNCASESTR  0
#    define  HAVE_STRNSTR  1
#    define  HAVE_STRLWR  1
#    define  HAVE_STRNLWR  0
#    define  HAVE_STRUPR  1
#    define  HAVE_STRNUPR  0
#endif

/* NDS oddities. */

#ifdef NDS
#    define  HAVE_ARPA_INET_H  0	/* Empty file; use <netinet/in.h>. */
#    define  HAVE_BYTESWAP_H  0
#    define  HAVE_CUSERID  0
#    define  HAVE_ENDIAN_H  0
#    define  HAVE_DUP  0
#    define  HAVE_GETHOSTBYADDR  0
#    define  HAVE_GETLOGIN  0
#    define  HAVE_GETSERVBYNAME  0
#    define  HAVE_H_ADDR  0		/* Field name in "hostent" structure. */
#    define  HAVE_IN_ADDR_T  0		/* Binary IPV4 address type. */
#    define  HAVE_IN_PORT_T  0		/* Network port number. */
#    define  HAVE_INTTYPES_H  0
#    define  HAVE_IOCTL_H  0		/* Declared in <sys/socket.h>. */
#    define  HAVE_LSTAT  0
#    define  HAVE_MEMDUP  0
#    define  HAVE_NETWORK_PORT_0  1
#    define  HAVE_RMDIR  0
#    define  HAVE_SLEEP  0
#    define  HAVE_SOCKLEN_T  0
#    define  HAVE_STDDEF  0
#    define  HAVE_STDERR  0
#    define  HAVE_STRNLEN  0
#    define  HAVE_STRNLWR  0
#    define  HAVE_STRNUPR  0
#    define  HAVE_WCS  0
#    define  HAVE_WORKING_FORK  0
#    define  MAX_LISTEN_BACKLOG  1
#    ifndef SSIZE_MAX
#        define  SSIZE_MAX  INT_MAX	/* Type of ssize_t defined in GCC ".../include/sys/_types.h". */
#    endif
#    define  XDRREC_CADDR_T  void *	/* BSD-XDR's readit()/writeit() functions. */
#    include  <nds.h>			/* NDS definitions. */
#endif

/* PalmOS oddities. */

#ifdef __palmos__
#    define  HASH_UTIL_STATISTICS  0	/* Exclude function in "hash_util.c". */
#    define  HAVE_64_BIT_INTEGERS  1
#    define  HAVE_ATOF  0		/* Implemented in "std_util.c". */
#    define  HAVE_ARPA_INET_H  0	/* "sdk/include/Core/System/Unix/arpa_inet.h" */
#    define  HAVE_BYTESWAP_H  0
#    define  HAVE_CADDR_T  0
#    define  HAVE_ENDIAN_H  0
#    define  HAVE_ETC_SERVICES  0	/* Until implemented with PalmOS DB. */
#    define  HAVE_FEOF  0
#    define  feof(a1)  (-1)		/* Substitute value indicating EOF. */
#    define  HAVE_FREOPEN  0
#    define  HAVE_FSEEK  0
#    define  HAVE_GETCWD  0
#    define  HAVE_GMTIME  0
#    define  HAVE_IN_ADDR_T  0		/* Binary IPV4 address type. */
#    define  HAVE_IN_PORT_T  0		/* Network port number. */
#    define  HAVE_INTTYPES_H  0
#    define  HAVE_IOCTL  0
#    define  HAVE_LOCALTIME  0
#    define  HAVE_LSEEK  0
#    define  HAVE_MEMDUP  0
#    define  HAVE_MKTIME  0
#    define  HAVE_MODF  0
#    define  HAVE_PERCENT_Z_FORMAT  0
#    define  HAVE_POPEN  0
#    define  HAVE_PRINTF_FORMAT_FLOAT  0
#    define  HAVE_REWIND  0
#    define  rewind(a1)			/* No return value. */
#    define  HAVE_SIGNAL  0
#    define  HAVE_SOCKLEN_T  0
#    define  HAVE_SSCANF  0
#    define  HAVE_SSIZE_T  0
#    define  HAVE_STAT_H  0
#    define  HAVE_STDINT_H  1
#    define  HAVE_STRCASESTR  0
#    define  HAVE_STRCSPN  0
#    define  HAVE_STRDUP  0
#    define  HAVE_STRERROR  0
#    define  HAVE_STRFTIME  0
#    define  HAVE_STRLWR  0
#    define  HAVE_STRNCASESTR  0
#    define  HAVE_STRNDUP  0
#    define  HAVE_STRNLEN  0
#    define  HAVE_STRNLWR  0
#    define  HAVE_STRNUPR  0
#    define  HAVE_STRRCHR  0
#    define  HAVE_STRSPN  0
#    define  HAVE_STRTOD  0		/* Implemented in "std_util.c". */
#    define  HAVE_STRTOK  0
#    define  HAVE_STRTOK_R  0
#    define  HAVE_STRTOLD  0
#    define  HAVE_STRUPR  0
#    define  HAVE_TIME_H  0
#    define  HAVE_TIMESPEC  0
#    define  HAVE_WORKING_FORK  0
#    define  HAVE_WCS  0
#    define  MAX_LISTEN_BACKLOG  1
#    undef  NULL
#    define  NULL  ((void *) 0)
#    ifndef SECTIONS_BY_MODULE
#        define  SECTIONS_BY_MODULE
#    endif
	/* Do NOT use the PalmOS SDK's "unix_stdarg.h".  It does NOT match
	   the PRC-TOOLS GCC's convention for stacking arguments (at least
	   on the m68k architecture).  Given the address of an argument
	   on the stack, "unix_stdarg.h" adds sizeof() the argument's data
	   type to get the address of the next argument.  If a one-byte
	   "short enum" is at offset 4 on the stack, "unix_stdarg.h" thinks
	   the next argument will be at offset 5.  However, m68k-palmos-gcc
	   aligns arguments on 2-byte boundaries, so, in the case of the short
	   enum at offset 4, the compiler adds one byte of padding at offset 5
	   and places the next argument at offset 6.  The #define immediately
	   after this comment predefines the guard macro for "unix_stdarg.h"
	   so that its contents are skipped. */
#    define  __stdarg_h			/* Exclude Palm's "unix_stdarg.h". */
	/* "/usr/share/prc-tools/include/ctype.h" does not define isascii(3). */
#    define  isascii(c) \
	(('\0' <= (c)) && ((c) <= '\0177'))
#    define  HAVE_STDBOOL_H  0		/* Conflicts with "PalmTypes.h". */
#    define  _BSDTYPES_DEFINED		/* BSD-XDR's types defined in
					   "m68k-palmos/include/sys/types.h"
					   and in Palm's "sys_types.h". */
#    define  XDRREC_CADDR_T  void *	/* BSD-XDR's readit()/writeit() functions. */
#    include  <HostControl.h>		/* POSE debug/logging functions. */
#endif

/* VMS or VAX-C oddities.  I don't have access to a DEC-C compiler, but I do
   have a copy of its header files.  So, I'm trying to make informed guesses
   as to the differences between the VAX-C and DEC-C compilers. */

#ifdef vms
#    include  <descrip.h>		/* Descriptor definitions. */
#    define  ASSIGN(desc, string)	\
      { desc.dsc$w_length = (string == NULL) ? 0 : strlen (string) ;	\
        desc.dsc$b_dtype = DSC$K_DTYPE_T ;				\
        desc.dsc$b_class = DSC$K_CLASS_S ;				\
        desc.dsc$a_pointer = (string == NULL) ? "" : string ;		\
  }
#    define  HAVE_CUSERID  1
#    define  HAVE_IOCTL  0
#    define  HAVE_LSTAT  0
#    define  HAVE_RMDIR  0
#    define  HAVE_SOCKLEN_T  0
#    define  HAVE_WORKING_FORK  0
#    define  PROTOTYPES  1
#    define  STDC_HEADERS  1
#    ifdef vaxc
#        define  HAVE_64_BIT_INTEGERS  0	/* VAX = 0, Alpha = 1 */
#        define  HAVE_BYTESWAP_H  0
#        define  HAVE_ENDIAN_H  0
#        define  HAVE_GETSERVBYNAME  0
#        define  HAVE_GETTIMEOFDAY  0
#        define  HAVE_IEEEFP  0		/* IEEE floating-point? */
#        define  HAVE_IN_ADDR_T  0	/* Binary IPV4 address type. */
#        define  HAVE_INTTYPES_H  0
#        define  HAVE_MEMDUP  0
#        define  HAVE_POPEN  0		/* Although I do have this available. */
#        define  HAVE_SSIZE_T  0
#        define  HAVE_STRCASECMP  0
#        define  HAVE_STRCASESTR  0
#        define  HAVE_STRDUP  0
#        define  HAVE_STRLWR  0
#        define  HAVE_STRNCASESTR  0
#        define  HAVE_STRNDUP  0
#        define  HAVE_STRNLEN  0
#        define  HAVE_STRNLWR  0
#        define  HAVE_STRNUPR  0
#        define  HAVE_STRTOK_R  0
#        define  HAVE_STRUPR  0
#        define  HAVE_TIMESPEC  0	/* POSIX high-resolution time. */
#        define  HAVE_WCS  0
#        define  XDRREC_CADDR_T  void *	/* BSD-XDR's readit()/writeit() functions. */
#    else
#        define  HAVE_WCSDUP  0
#    endif
#endif

/* Windows oddities. */

#ifdef _WIN32
#    ifndef _CRT_NONSTDC_NO_DEPRECATE	/* No warnings of non-standard functions. */
#        define  _CRT_NONSTDC_NO_DEPRECATE
#    endif
#    ifndef _CRT_SECURE_NO_WARNINGS	/* No warnings of non-secure standard functions. */
#        define  _CRT_SECURE_NO_WARNINGS
#    endif
#    ifndef WIN32_LEAN_AND_MEAN
		/* Prevent "windows.h" from including "winsock.h", which
		   would cause conflicts with "winsock2.h".  Enter the 3
		   header file names into your favorite search engine to
		   find out more about this longstanding problem. */

#        define  WIN32_LEAN_AND_MEAN
#    endif
#    define  chdir  _chdir
#    define  getcwd(buffer,maxlen)  _getcwd ((buffer), (int) (maxlen))
#    define  mkdir  _mkdir
#    define  rmdir  _rmdir
#    define  strcasecmp  _stricmp
#    define  strncasecmp  _strnicmp
#    define  HAVE_BYTESWAP_H  0
#    define  HAVE_CADDR_T  0
#    define  HAVE_CUSERID  0
#    define  HAVE_ENDIAN_H  0
#    define  HAVE_GETLOGIN  0
#    define  HAVE_GETTIMEOFDAY  0
#    define  HAVE_IN_ADDR_T  0		/* Binary IPV4 address type. */
#    define  HAVE_IN_PORT_T  0		/* Network port number. */
#    define  HAVE_INTTYPES_H  0
#    define  HAVE_LSTAT  0
#    define  HAVE_MEMDUP  0
#    define  HAVE_POPEN  1		/* But of limited usefulness! */
#    define  popen  _popen
#    define  pclose  _pclose
#    define  HAVE_SIGNAL  0
#    define  HAVE_STDINT_H  (_MSC_VER >= 1600)	/* Introduced in VS 2010. */
#    define  HAVE_STRCASESTR  0
#    define  HAVE_STRNCASESTR  0
#    define  HAVE_STRNDUP  0
#    define  HAVE_STRNLWR  0
#    define  HAVE_STRNUPR  0
#    define  HAVE_STRTOK_R  0
#    define  HAVE_TIMESPEC  (_MSC_VER >= 1900)	/* Introduced in VS 2015. */
#    define  HAVE_WORKING_FORK  0
#    define  PROTOTYPES  1
#    define  STDC_HEADERS  1
#    define  XDRREC_CADDR_T  void *	/* BSD-XDR's readit()/writeit() functions. */
	/* Windows' "Basic sized types" header file, <BaseTsd.h>, defines
	   a zillion integer/pointer data types appropriately for both 32-
	   and 64-bit builds, including SSIZE_T.  For my own reference,
	   the file is found on my Windows 10 PC in this folder:
	   "C:\Program Files (x86)\Windows Kits\10\Include\10...\shared\". */
#    include  <BaseTsd.h>		/* Windows "Basic sized types". */
    typedef  SSIZE_T  ssize_t ;
#    ifdef MAXSSIZE_T
#        define  SSIZE_MAX  MAXSSIZE_T
#    else
#        define  SSIZE_MAX  MAXLONG_PTR
#    endif
#    define  HAVE_SSIZE_T  1
#    define  DATA_MODEL_64_BIT  LLP64
#endif

/* Windows CE oddities.  (Didn't pursue this very far.) */

#ifdef _WIN32_WCE
#    define  strcasecmp  _stricmp
#    define  strncasecmp  _strnicmp
#    define  HAVE_BYTESWAP_H  0
#    define  HAVE_CADDR_T  0
#    define  HAVE_ENDIAN_H  0
#    define  HAVE_GETENV  0
#    define  HAVE_GETTIMEOFDAY  0
#    define  HAVE_IN_ADDR_T  0		/* Binary IPV4 address type. */
#    define  HAVE_INTTYPES_H  0
#    define  HAVE_POPEN  0
#    define  HAVE_SIGNAL  0
#    define  HAVE_SSIZE_T  0
#    define  HAVE_STRERROR  0
#    define  HAVE_TIMESPEC  0		/* POSIX high-resolution time. */
#    define  HAVE_WORKING_FORK  0
#    define  PROTOTYPES  1
#    define  STDC_HEADERS  1
#endif

/* strcasestr() is a GNU extension, so don't include the "str_util.h/c"
   declaration/implementation if _GNU_SOURCE is #define'd.  strncasestr()
   is NOT a GNU extension, so leave HAVE_STRNCASESTR undefined.  (NOTE
   that strcasestr() is guarded in GCC's "<string.h> by GCC-internal
   macro __USE_GNU, which is set by GCC in "<features.h>" based on the
   user's #define of _GNU_SOURCE -- #define the latter, not the former!) */

#ifndef HAVE_STRCASESTR
#    ifdef _GNU_SOURCE
#        define  HAVE_STRCASESTR  1
#    else
#        define  HAVE_STRCASESTR  0
#    endif
#endif

/*******************************************************************************
    Compiler peculiarities.
*******************************************************************************/

/* Guess if the Standard C header files are available. */

#ifndef STDC_HEADERS
#    if defined(__STDC__) || defined(__cplusplus)
#        define  STDC_HEADERS  1
#    else
#        define  STDC_HEADERS  0
#    endif
#endif

#ifndef HAVE_STDARG_H
#    if STDC_HEADERS
#        define  HAVE_STDARG_H  1
#    else
#        define  HAVE_STDARG_H  0
#    endif
#endif

/* Are namespaces supported? */

#if defined(__cplusplus) && !defined(OS_ALTERNATIVE_STL_NAMES)
#    define  HAVE_NAMESPACES  1
#else
#    define  HAVE_NAMESPACES  0
#endif

/* Are function prototypes available? */

#if !defined(PROTOTYPES) && !defined(__PROTOTYPES)
#    if defined(__STDC__) || defined(__cplusplus)
#        define  PROTOTYPES  1
#    else
#        define  PROTOTYPES  0
#    endif
#endif

#if PROTOTYPES
#    define  P_(s)  s
#else
#    define  P_(s)  ()
#endif

#ifndef __PROTOTYPES
#    define  __PROTOTYPES  PROTOTYPES
#endif

/* Are const and volatile suppported by the compiler? */

#if !defined(const) && !defined(__STDC__) && !defined(__cplusplus)
#    define  const
#endif

#if !defined(volatile) && !defined(__STDC__) && !defined(__cplusplus)
#    define  volatile
#endif

/* Put functions/constants in different object code sections? */

#ifndef OCD
#    ifdef SECTIONS_BY_MODULE
#        define  OCD(m)  __attribute__ ((__section__ (m)))
#    else
#        define  OCD(m)
#    endif
#endif

/*******************************************************************************
    Data types of interest.
*******************************************************************************/

/* Define the C99 _Bool data type if <stdbool.h> is not available. */

#if defined(__GNUC__) && !defined(HAVE_STDBOOL_H)
#    define  HAVE_STDBOOL_H  1
#endif

#if defined(HAVE_STDBOOL_H) && HAVE_STDBOOL_H
#    include  <stdbool.h>		/* C99 boolean type and values. */
#elif defined(__palmos__)
	/* Sometime between successfully compiling my code in March 2016 and
	   unsucessfully trying again in December 2018, something was updated
	   in Cygwin that caused a conflict in the enumerated values true and
	   false defined both in the GCC 68K compiler's "stdbool.h" and the
	   PalmOS SDK's "PalmTypes.h".  Use the SDK's definitions. */
#    define  __STDBOOL_H__  1	/* Pretend "stdbool.h" has been included ... */
#    include  <PalmTypes.h>	/* ... and use "PalmTypes.h" instead. */
#    define  _Bool  Boolean
#    define  bool  _Bool
#elif defined(_WIN32) && defined(__cplusplus)
					/* In "yvals.h" via "limits.h" below. */
#elif !defined(_Bool) && (!defined(HAVE__BOOL) || !HAVE__BOOL)
#    define  _Bool  int
#    if !defined(__cplusplus) || defined(OS_ALTERNATIVE_STL_NAMES)
#        define  OS_OMIT_BOOL  0
#        define  bool  _Bool
#        ifndef true
#            define  true  1
#        endif
#        ifndef false
#            define  false  0
#        endif
#    endif
#endif

/* Character types and such. */

#if defined(__vax__)
#    include  <socket.h>			/* "caddr_t" definition. */
#elif !defined(HAVE_CADDR_T) || HAVE_CADDR_T
#    include  <sys/types.h>			/* System type definitions. */
#else
    typedef  char  *caddr_t ;
#endif

/* Integer data types of specific bit widths.  NOTE that the intN_t types
   are *exact-width* integers; these may not be available on all platforms.
   In the C99 standard, these types are defined in <stdint.h>, which
   is included by <inttypes.h>; some platforms have the UNIX standard
   <inttypes.h> only. */

#if defined(NDS)
#    include  <nds/ndstypes.h>			/* System type definitions. */
#elif defined(HAVE_STDINT_H) && HAVE_STDINT_H
#    include  <stdint.h>			/* C99 integer types. */
#elif !defined(HAVE_INTTYPES_H) || HAVE_INTTYPES_H
#    include  <inttypes.h>			/* C99 integer types. */
#else
#    if defined(vaxc)
        typedef  char  int8_t			/* VAX-C doesn't allow "signed char". */
#    else
        typedef  signed  char  int8_t ;
#    endif
    typedef  unsigned  char  uint8_t ;
    typedef  short  int16_t ;			/* Assume short is 16 bits. */
    typedef  unsigned  short  uint16_t ;
    typedef  int  int32_t ;			/* Assume int is 32 bits. */
    typedef  unsigned  int  uint32_t ;
#    if !defined(HAVE_64_BIT_INTEGERS) || HAVE_64_BIT_INTEGERS
        typedef  long  long  int64_t ;
        typedef  unsigned  long  long  uint64_t ;
#    endif
#endif

/* BSD defines the u_intN_t types, but they are deprecated in "sys/types.h". */

#if !defined(HAVE_U_INTN_T) || !HAVE_U_INTN_T
    typedef  uint8_t  u_int8_t ;
    typedef  uint16_t  u_int16_t ;
    typedef  uint32_t  u_int32_t ;
#    if !defined(HAVE_64_BIT_INTEGERS) || HAVE_64_BIT_INTEGERS
        typedef  uint64_t  u_int64_t ;
#    endif
#endif

/* Define IEEEFP if platform has IEEE floating-point representation;
   this symbol is used in the XDR xdr_float() function. */

#if !defined(HAVE_IEEEFP) || HAVE_IEEEFP
#    define  IEEEFP
#endif

#if defined(HAVE_IN_ADDR_T) && !HAVE_IN_ADDR_T
    typedef  uint32_t  in_addr_t ;		/* Binary IPV4 address type. */
#    undef  HAVE_IN_ADDR_T
#    define  HAVE_IN_ADDR_T  1
#endif

#if defined(HAVE_SSIZE_T) && !HAVE_SSIZE_T
    typedef  long  ssize_t ;
#    undef  HAVE_SSIZE_T
#    define  HAVE_SSIZE_T  1
#    define  SSIZE_MAX  LONG_MAX
#endif

#include  <limits.h>			/* Maximum/minimum value definitions. */
#if defined(HAVE_SIZE_MAX) && !HAVE_SIZE_MAX
					/* If size_t maximum is not defined;
					   assumes twos-complement numbers. */
#    define  SIZE_MAX  ~((size_t) 0)
#endif

/*******************************************************************************
    (I originally wrote this section for Visual Studio 64-bit compilation,
    but, in the process, I discovered online the existence of Windows'
    "BaseTsd.h" header file, which solved my particular problem.  This
    header is "#include"d above under "Windows oddities".  I'll keep
    this section here for now and maybe refine or delete it later.)

    I/O formats for (s)size_t when %z is not supported.  In this case:

        (1) #define  HAVE_PERCENT_Z_FORMAT  0

    and, if the base type for "size_t" is NOT "unsigned long":

        (2) #define  SIZE_T_BASE_SUFFIX  ULL

    If the base type is neither UL (default) or ULL, then a new subsection
    needs to be added here for the convention used by the compiler.
*******************************************************************************/

#if defined(HAVE_PERCENT_Z_FORMAT) && !HAVE_PERCENT_Z_FORMAT
					/* No "%zd" or "%zu" formats;
					   use long and unsigned long
					   casts and formats. */

#    if (SIZE_T_BASE_SUFFIX == ULL)
					/* If size_t is "unsigned long long";
					   e.g., LLP64-model compilation. */
        typedef  unsigned long long  size_t_base_type ;
        typedef  long long  ssize_t_base_type ;

#        ifndef SIZE_T_C
#            define  SIZE_T_C(literal)  literal##ULL
#        endif
#        ifndef PRIuSIZE_T
#            define  PRIuSIZE_T  "llu"
#        endif
#        ifndef SCNuSIZE_T
#            define  SCNuSIZE_T  PRIuSIZE_T
#        endif
#        ifndef SSIZE_T_C
#            define  SSIZE_T_C(literal)  literal##LL
#        endif
#        ifndef PRIdSSIZE_T
#            define  PRIdSSIZE_T  "lld"
#        endif
#        ifndef SCNdSSIZE_T
#            define  SCNdSSIZE_T  PRIdSSIZE_T
#        endif

#    else
					/* If size_t is "unsigned long";
					   e.g., LP64-model compilation. */
        typedef  unsigned long  size_t_base_type ;
        typedef  long  ssize_t_base_type ;

#        ifndef SIZE_T_C
#            define  SIZE_T_C(literal)  literal##UL
#        endif
#        ifndef PRIuSIZE_T
#            define  PRIuSIZE_T  "lu"
#        endif
#        ifndef SCNuSIZE_T
#            define  SCNuSIZE_T  PRIuSIZE_T
#        endif
#        ifndef SSIZE_T_C
#            define  SSIZE_T_C(literal)  literal##L
#        endif
#        ifndef PRIdSSIZE_T
#            define  PRIdSSIZE_T  "ld"
#        endif
#        ifndef SCNdSSIZE_T
#            define  SCNdSSIZE_T  PRIdSSIZE_T
#        endif

#    endif

#else
					/* The "%zd" or "%zu" formats are
					   supported; define the macros
					   above appropriately. */
        typedef  size_t  size_t_base_type ;
        typedef  ssize_t  ssize_t_base_type ;

#    ifndef SIZE_T_C
#        define  SIZE_T_C(literal)  literal##UL
#    endif
#    ifndef PRIuSIZE_T
#        define  PRIuSIZE_T  "zu"
#    endif
#    ifndef SCNuSIZE_T
#        define  SCNuSIZE_T  PRIuSIZE_T
#    endif
#    ifndef SSIZE_T_C
#        define  SSIZE_T_C(literal)  literal##L
#    endif
#    ifndef PRIdSSIZE_T
#        define  PRIdSSIZE_T  "zd"
#    endif
#    ifndef SCNdSSIZE_T
#        define  SCNdSSIZE_T  PRIdSSIZE_T
#    endif

#endif

/*******************************************************************************
    Operating system peculiarities.
*******************************************************************************/

/* Errno! */

#if defined(__palmos__)
					/* "sys_socket.h" declares "errno" ...
					   ... and includes "sys_errno.h". */
#    include  <Unix/sys_socket.h>	/* UNIX socket definitions. */
#    define  EOVERFLOW  ERANGE		/* Use ERANGE for numeric overflow. */
    typedef  Err  errno_t ;		/* "Err" type defined in "PalmTypes.h". */
#    include  "strerror.h"		/* strerror(3) function. */
#elif defined(HAVE_ERRNO_H) && !HAVE_ERRNO_H
    typedef  int  errno_t ;
#    include  "strerror.h"		/* strerror(3) function. */
#else
#    include  <errno.h>			/* System error definitions. */
    typedef  int  errno_t ;
#endif

#define  PUSH_VAR(type, variable)  { type  save_variable = (variable) ;
#define  POP_VAR(variable)  (variable) = save_variable ; }

#define  SET_ERRNO(error)  (errno = (errno_t) (error))
#define  PUSH_ERRNO  { errno_t  save_errno = (errno_t) errno ;
#define  POP_ERRNO     SET_ERRNO (save_errno) ; }

/* Supply the ANSI strerror(3) function on systems that don't support it. */

#if defined(vms)
#    define  STRERROR(code)  strerror ((code), vaxc$errno)
#    include  <stsdef.h>		/* VMS status code structure. */
#else
#    define  STRERROR  strerror
#endif

/* Pathnames and such. */

#if defined(HAVE_SYS_PARAM_H) && HAVE_SYS_PARAM_H
#    include  <sys/param.h>		/* System parameters. */
#    define  PATH_MAX  MAXPATHLEN
#endif

#ifndef PATH_MAX
#    if defined(vms)
#        include  <nam.h>		/* RMS name block (NAM) definitions. */
#        define  PATH_MAX  NAM$C_MAXRSS
#    elif defined(_WIN32)
#        include  <windows.h>		/* Windows definitions. */
#        define  PATH_MAX  MAX_PATH
#    else
#        define  PATH_MAX  256
#    endif
#endif

#if defined(VXWORKS) || defined(_WIN32)
#    define  MKDIR(path,mode)  mkdir ((path))
#else
#    define  MKDIR(path,mode)  mkdir ((path), (mode))
#endif

/* Signals. */

#if !defined(HAVE_SIGNAL_H) || HAVE_SIGNAL_H
#    if !defined(HAVE_SIGNAL) || HAVE_SIGNAL
#        include  <signal.h>		/* Signal definitions. */
#        define  HAVE_SIGNAL  1
#    endif
#endif

/* fork(2) and popen(3) - assume they're available if not told otherwise. */

#ifndef HAVE_WORKING_FORK
#    define  HAVE_WORKING_FORK  1
#endif

#ifndef HAVE_POPEN
#    define  HAVE_POPEN  1
#endif

/* PalmOS standard I/O applications are built using PalmOS's "StdIOPalm.c"
   (a copy of which is found in my LIBGPL) and LIBGPL's "SioMain.c".  To
   put a program's main() function in the "sio_appl" code section used by
   the above files, add an external declaration of main() that places it
   in the "sio_appl" code section.  I originally did this in the program
   files themselves, but I forgot to in some more recent programs I wrote
   and I had a great deal of trouble trying to figure out why the old
   programs would build and the new programs wouldn't.  The presence of
   this declaration doesn't affect non-main() source code; e.g., LIGPL's
   source files. */

#if defined(__palmos__) && (!defined(PALMOS_STDIO_MAIN) || PALMOS_STDIO_MAIN)
    extern  int  main P_((int argc, char *argv[]))  OCD ("sio_appl") ;
#endif

/*******************************************************************************
    Odds and ends.
*******************************************************************************/

/* Length-limited counterparts to the strto...() functions.  If these
   functions ever become standard or are present on a particular platform,
   just fix these "#define"s as necessary.  The macro definitions are
   used in "sto_util.h" and LIBGPL's "sto_util.c" */

#define  HAVE_STRNTO  0
#if defined(HAVE_STRNTO) && !HAVE_STRNTO
#    define  HAVE_ANTOF  0
#    define  HAVE_ANTOI  0
#    define  HAVE_ANTOL  0
#    define  HAVE_ANTOLL  0
#    define  HAVE_STRNTOL  0
#    define  HAVE_STRNTOUL  0
#    define  HAVE_STRNTOLL  0
#    define  HAVE_STRNTOULL  0
#    define  HAVE_STRNTOF  0
#    define  HAVE_STRNTOD  0
#    define  HAVE_STRNTOLD  0
#endif

/*******************************************************************************
    Debug and error logging - definitions of the LGI and LGE macros.
*******************************************************************************/

#include  "iel_util.h"			/* Information and error logging. */


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */

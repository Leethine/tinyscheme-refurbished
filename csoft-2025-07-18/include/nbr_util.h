/* $Id: nbr_util.h,v 1.9 2023/02/26 06:30:09 alex Exp $ */
/*******************************************************************************

    nbr_util.h

    Number Utilities.

*******************************************************************************/

#ifndef  NBR_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  NBR_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    Define relevant data types and their I/O format specifiers.
*******************************************************************************/

/* Factors (factor_t) - NOTE that this is not just a type for factors,
   but the type used for internal calculations involving factors. */

#ifndef FACTOR_T
#    define  FACTOR_T  unsigned long
#endif

typedef  FACTOR_T  factor_t ;

#ifndef FACTOR_T_C
#    define  FACTOR_T_C(literal)  literal##UL
#endif

#ifndef FACTOR_T_MAX
#    define  FACTOR_T_MAX  ~(FACTOR_T_C(0))
#endif

#ifndef STRTO_FACTOR_T
#    define  STRTO_FACTOR_T(s,p,r)  strtoul(s,p,r)
#endif

#ifndef PRIuFACTOR_T
#    define  PRIuFACTOR_T  "lu"
#endif

#ifndef PRIxFACTOR_T
#    define  PRIxFACTOR_T  "lx"
#endif

#ifndef PRIXFACTOR_T
#    define  PRIXFACTOR_T  "lX"
#endif

#ifndef SCNuFACTOR_T
#    define  SCNuFACTOR_T  PRIuFACTOR_T
#endif


/* Greatest Common Divisors (GCD_t) - NOTE that this is not just a type for
   GCDs, but the type used for internal calculations involving GCDs. */

#ifndef GCD_T
#    define  GCD_T  unsigned long
#endif

typedef  GCD_T  GCD_t ;

#ifndef GCD_T_C
#    define  GCD_T_C(literal)  literal##UL
#endif

#ifndef GCD_T_MAX
#    define  GCD_T_MAX  ~(GCD_T_C(0))
#endif

#ifndef STRTO_GCD_T
#    define  STRTO_GCD_T(s,p,r)  strtoul(s,p,r)
#endif

#ifndef PRIuGCD_T
#    define  PRIuGCD_T  "lu"
#endif

#ifndef PRIxGCD_T
#    define  PRIxGCD_T  "lx"
#endif

#ifndef PRIXGCD_T
#    define  PRIXGCD_T  "lX"
#endif

#ifndef SCNuGCD_T
#    define  SCNuGCD_T  PRIuGCD_T
#endif


/* Primes (prime_t) - NOTE that this is not just a type for prime numbers,
   but the type used for internal calculations involving prime numbers. */

#ifndef PRIME_T
#    define  PRIME_T  unsigned long
#endif

typedef  PRIME_T  prime_t ;

#ifndef PRIME_T_C
#    define  PRIME_T_C(literal)  literal##UL
#endif

#ifndef PRIME_T_MAX
#    ifdef ULONG_MAX
#        define  PRIME_T_MAX  ULONG_MAX
#    else
#        define  PRIME_T_MAX  ~(PRIME_T_C(0))
#    endif
#endif

#ifndef PRIME_T_ATLEAST_BITS
	/* This parameter is only used by the PRIMAL and PRIMATE programs.
	   The programs each have pre-computed tables of the 1,000 largest
	   32-bit primes and the 1,000 largest 64-bit primes, so they need
	   to know whether or not to include the tables at compile time.
	   For example, if prime_t is 48 bits wide, the "last 32 bit primes"
	   table can be included, but not the "last 64 bit primes" table.
	   There is no way for the C Preprocessor to know the size of
	   prime_t so, for the purposes of PRIMAL and PRIMATE, I compare
	   PRIME_T_MAX to the maximum representable, unsigned 32- and
	   64-bit integers.  Applications should NOT depend on this
	   parameter being the exact bit width of prime_t. */
#    if (PRIME_T_MAX <= 0xFFFF)
#        define  PRIME_T_ATLEAST_BITS  16
#    elif (PRIME_T_MAX <= 0xFFFFFFFFUL)
#        define  PRIME_T_ATLEAST_BITS  32
#    elif (PRIME_T_MAX <= 0xFFFFFFFFFFFFFFFFUL)
#        define  PRIME_T_ATLEAST_BITS  64
#    else
#        error "PRIME_T_ATLEAST_BITS cannot be determined; see \"nbr_util.h\"."
#    endif
#endif

#ifndef STRTO_PRIME_T
#    define  STRTO_PRIME_T(s,p,r)  strtoul(s,p,r)
#endif

#ifndef PRIuPRIME_T
#    define  PRIuPRIME_T  "lu"
#endif

#ifndef PRIxPRIME_T
#    define  PRIxPRIME_T  "lx"
#endif

#ifndef PRIXPRIME_T
#    define  PRIXPRIME_T  "lX"
#endif

#ifndef SCNuPRIME_T
#    define  SCNuPRIME_T  PRIuPRIME_T
#endif


/* Square Roots (root_t) - NOTE that this is not just a type for square roots,
   but the type used for internal calculations involving square roots. */

#ifndef ROOT_T
#    define  ROOT_T  unsigned long
#endif

typedef  ROOT_T  root_t ;

#ifndef ROOT_T_C
#    define  ROOT_T_C(literal)  literal##UL
#endif

#ifndef ROOT_T_MAX
#    define  ROOT_T_MAX  ~(ROOT_T_C(0))
#endif

#ifndef STRTO_ROOT_T
#    define  STRTO_ROOT_T(s,p,r)  strtoul(s,p,r)
#endif

#ifndef PRIuROOT_T
#    define  PRIuROOT_T  "lu"
#endif

#ifndef PRIxROOT_T
#    define  PRIxROOT_T  "lx"
#endif

#ifndef PRIXROOT_T
#    define  PRIXROOT_T  "lX"
#endif

#ifndef SCNuROOT_T
#    define  SCNuROOT_T  PRIuROOT_T
#endif

/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

					/* GCD function prototype. */
typedef  GCD_t  (*GCDFunc) P_((GCD_t x,
                               GCD_t y,
                               size_t *numIterations)) ;

					/* Prime number function prototype. */
typedef  bool  (*IsPrimeFunc) P_((prime_t number,
                                  void *configuration)) ;

	/* If you pass a NULL context pointer to nbrUnsignedSQRT(),
	   the function function will compute and save the values
	   internally on the first call.  If you pass in a pointer
	   to an application-allocated context structure (useful in
	   a multi-threaded environment) and the precision field is 0,
	   nbrUnsignedSQRT() will compute the the values based on
	   ULONG_MAX and store them in your structure; thereafter,
	   the function won't need to recompute the values.  If the
	   precision is non-zero on the first call, nbrUnsignedSQRT()
	   will use the values you provide; this might be useful if
	   you want to play around with different precisions. */

typedef  struct  UnsignedSqrtContext {
    bool  oddPrecision ;	/* Is precision odd or even? */
    size_t  precision ;		/* # of significant bits in unsigned integer. */
    root_t  estimate ;		/* Most significant bit of maximum square root. */
}  UnsignedSqrtContext ;

					/* Integer square root function prototype. */
typedef  root_t  (*UnsignedSqrtFunc) P_((root_t number,
                                         UnsignedSqrtContext *context)) ;

					/* Global debug switch (1/0 = yes/no). */
extern  int  nbr_util_debug  OCD ("nbr_util") ;

/*******************************************************************************
    Public functions (nbr_util.c)
*******************************************************************************/

extern  const  uint16_t  *nbrEarlyPrimes P_((size_t *numPrimes))
    OCD ("nbr_util") ;

extern  size_t  nbrFactorsOf P_((factor_t number,
                                 size_t maxFactors,
                                 factor_t *factors))
    OCD ("nbr_util") ;

extern  GCD_t  nbrBinaryGCD P_((GCD_t x,
                                GCD_t y,
                                size_t *numSteps))
    OCD ("nbr_util") ;

extern  GCD_t  nbrModuloGCD P_((GCD_t x,
                                GCD_t y,
                                size_t *numSteps))
    OCD ("nbr_util") ;

extern  GCD_t  nbrSubtractGCD P_((GCD_t x,
                                  GCD_t y,
                                  size_t *numSteps))
    OCD ("nbr_util") ;

/*******************************************************************************
    Public functions (nbr_util_primes.c)
*******************************************************************************/

extern  bool  nbrIsPrime P_((prime_t number,
                             void *configuration))
    OCD ("nbr_util") ;

extern  prime_t  nbrNextPrime P_((prime_t number,
                                  void *configuration))
    OCD ("nbr_util") ;

extern  prime_t  nbrNextPrimeF P_((prime_t number,
                                   IsPrimeFunc isPrimeF,
                                   void *configuration))
    OCD ("nbr_util") ;


#if !defined(NBR_SKIP_2_3_HARDCODED) || !NBR_SKIP_2_3_HARDCODED

extern  void  *nbrPrimeConfigCreate P_((prime_t skipThroughPrime,
                                        unsigned char *differences,
                                        bool isDynamic))
    OCD ("nbr_util") ;

extern  errno_t  nbrPrimeConfigDestroy P_((void *configuration))
    OCD ("nbr_util") ;

extern  unsigned  char  *nbrPrimeSeqDiffs P_((prime_t skipThroughPrime,
                                              prime_t *startDivisor,
                                              size_t *seqLength))
    OCD ("nbr_util") ;

extern  size_t  nbrPrimeSeqDiffsLength P_((prime_t skipThroughNumber,
                                           prime_t *skipThroughPrime,
                                           prime_t *startDivisor))
    OCD ("nbr_util") ;

extern  prime_t  nbrPrimeSeqDiffsMax P_((prime_t skipThroughPrime))
    OCD ("nbr_util") ;

extern  prime_t  nbrPrimeSeqDiffsRange P_((prime_t skipThroughPrime))
    OCD ("nbr_util") ;

#else

#    define  nbrPrimeConfigCreate(a1,a2,a3)  (NULL)
#    define  nbrPrimeConfigDestroy(a1)  ((errno_t) 0)
#    define  nbrPrimeSeqDiffs(a1,a2,a3)  (NULL)
#    define  nbrPrimeSeqDiffsLength(a1,a2,a3)  ((size_t) 0)
#    define  nbrPrimeSeqDiffsMax(a1)  PRIME_T_C(0)
#    define  nbrPrimeSeqDiffsRange(a1)  PRIME_T_C(0)

#endif	/* NBR_SKIP_2_3_HARDCODED */

/*******************************************************************************
    Public functions (nbr_util_roots.c)
*******************************************************************************/

#if !defined(HAVE_BINARY_INTEGERS) || HAVE_BINARY_INTEGERS
extern  root_t  nbrUnsignedSQRT P_((root_t number,
                                    UnsignedSqrtContext *context))
    OCD ("nbr_util") ;
#endif


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */

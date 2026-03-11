/* $Id: bmw_util.c,v 1.12 2021/02/19 22:13:14 alex Exp $ */
/*******************************************************************************

File:

    bmw_util.c


Author:    Alex Measday


Purpose:

    The BMW utilities provide a simple means of measuring the performance
    of an arbitrary task, where the measure is the number of <something>
    performed per second.  For example, the following program measures
    the performance of standard output in number of lines per second:

        #include  <stdio.h>		-- Standard I/O definitions.
        #include  <stdlib.h>		-- Standard C Library definitions.
        #include  "bmw_util.h"		-- Benchmarking definitions.

        int  main (int argc, char *argv[])
        {
            BmwClock  clock ;
            size_t  i, numLines ;

            numLines = (size_t) strtoul (argv[1], NULL, 0) ;

            bmwStart (&clock) ;
            for (i = 0 ;  i < numLines ;  i++)
                printf ("Hello, World!\n") ;
            bmwStop (&clock) ;

            printf ("%g lines per second.\n",
                    bmwRate (&clock, numLines)) ;
        }


Notes:

    These functions are reentrant under VxWorks.


Procedures:

    bmwElapsed() - returns the elapsed time for a timer as a floating-point
        number of seconds.
    bmwElapsedTV() - returns the elapsed time for a timer as a UNIX timeval
        structure (seconds and microseconds).
    bmwRate() - computes the rate (# of X per second) for a timer.
    bmwStart() - starts a timer.
    bmwStop() - stops a timer.

*******************************************************************************/


#ifdef CPU_TIME
#    include  <sys/resource.h>		/* Process resource definitions. */
#    define  tvTOD  bmwCPUTime
#endif
#include  "tv_util.h"			/* "timeval" manipulation functions. */
#include  "bmw_util.h"			/* Benchmarking functions. */


/*******************************************************************************
    Private Functions.
*******************************************************************************/

#ifdef CPU_TIME
static  struct  timeval  bmwCPUTime (
#    if PROTOTYPES
        void
#    endif
    ) ;
#endif

#ifdef CPU_TIME
/*******************************************************************************

Procedure:

    bmwCPUTime ()


Purpose:

    Function bmwCPUTime() returns the cumulative amount of CPU time (in
    user and system space) used by the process.


    Invocation:

        cpuTime = bmwCPUTime () ;

    where

        <cpuTime>	- O
            returns the CPU time used by the process.

*******************************************************************************/


static  struct  timeval  bmwCPUTime (

#    if PROTOTYPES
        void)
#    else
        )
#    endif

{    /* Local variables. */
    struct  rusage  usage ;



/* Get the current usage statistics. */

    getrusage (RUSAGE_SELF, &usage) ;

/* Return the sum of the user CPU time and the system CPU time. */

/*    return (tvAdd (usage.ru_utime, usage.ru_stime)) ; */
    return (usage.ru_utime) ;

}
#endif

/*******************************************************************************

Procedure:

    bmwElapsed ()


Purpose:

    Function bmwElapsed() returns a benchmarking timer's elapsed time as a
    floating-point number of seconds.  If the timer has been stopped by a
    call to bmwStop(), the time returned by bmwElapsed() is the number of
    seconds between the start time and the stop time.  If the timer hasn't
    been stopped, the time returned is the difference between the start time
    and the current time of day.


    Invocation:

        elapsedTime = bmwElapsed (&timer) ;

    where

        <timer>		- I
            is the address of the timer used for this benchmark.
        <elapsedTime>	- O
            returns the floating-point number of seconds of elapsed time.

*******************************************************************************/


double  bmwElapsed (

#    if PROTOTYPES
        BmwClock  *timer)
#    else
        timer)

        BmwClock  *timer ;
#    endif

{    /* Local variables. */
    struct  timeval  elapsedTime ;



/* If the timer hasn't been stopped yet, then compute the current elapsed
   time.  If the timer has been stopped, then compute the time elapsed
   between the start time and stop time. */

    if ((timer->stopTime.tv_sec == 0) &&
        (timer->stopTime.tv_usec == 0))
        elapsedTime = tvSubtract (tvTOD (), timer->startTime) ;
    else
        elapsedTime = tvSubtract (timer->stopTime, timer->startTime) ;

    return (tvFloat (elapsedTime)) ;

}

/*******************************************************************************

Procedure:

    bmwElapsedTV ()


Purpose:

    Function bmwElapsedTV() returns a benchmarking timer's elapsed time in a
    UNIX timeval structure containing the number of seconds and microseconds.
    If the timer has been stopped by a call to bmwStop(), the time returned
    by bmwElapsedTV() is the difference between the start time and the stop
    time.  If the timer hasn't been stopped, the time returned is the
    difference between the start time and the current time of day.


    Invocation:

        elapsedTime = bmwElapsedTV (&timer) ;

    where

        <timer>		- I
            is the address of the timer used for this benchmark.
        <elapsedTime>	- O
            returns a UNIX timeval structure containing the number of seconds
            and microseconds of elapsed time.

*******************************************************************************/


struct  timeval  bmwElapsedTV (

#    if PROTOTYPES
        BmwClock  *timer)
#    else
        timer)

        BmwClock  *timer ;
#    endif

{

/* If the timer hasn't been stopped yet, then compute the current elapsed
   time.  If the timer has been stopped, then compute the time elapsed
   between the start time and stop time. */

    if ((timer->stopTime.tv_sec == 0) &&
        (timer->stopTime.tv_usec == 0))
        return (tvSubtract (tvTOD (), timer->startTime)) ;
    else
        return (tvSubtract (timer->stopTime, timer->startTime)) ;

}

/*******************************************************************************

Procedure:

    bmwRate ()


Purpose:

    Function bmwRate() returns the rate at which a caller-specified number
    of items were processed.  The function simply divides the number of
    items by the timer's elapsed time, giving the number of items processed
    per second.


    Invocation:

        itemsPerSecond = bmwRate (&timer, numItems) ;

    where

        <timer>			- I
            is the address of the timer used for this benchmark.
        <numItems>		- I
            is the count of that which is being measured.
        <itemsPerSecond>	- O
            returns the number of items divided by the elapsed time
            in seconds.  To avoid a divide-by-zero error, zero is
            returned if the elapsed time is zero.

*******************************************************************************/


double  bmwRate (

#    if PROTOTYPES
        BmwClock  *timer,
        size_t  numItems)
#    else
        timer, numItems)

        BmwClock  *timer ;
        size_t  numItems ;
#    endif

{    /* Local variables. */
    double  elapsed = bmwElapsed (timer) ;

    return ((elapsed == 0.0) ? 0.0 : (double) numItems / elapsed) ;

}

/*******************************************************************************

Procedure:

    bmwStart ()


Purpose:

    Function bmwStart() initializes and starts a benchmarking timer.


    Invocation:

        bmwStart (&timer) ;

    where

        <timer>		- O
            is the address of a timer to be used for this benchmark.

*******************************************************************************/


void  bmwStart (

#    if PROTOTYPES
        BmwClock  *timer)
#    else
        timer)

        BmwClock  *timer ;
#    endif

{
    timer->startTime = tvTOD () ;		/* Current time. */
    timer->stopTime.tv_sec = 0 ;		/* Reset stop time. */
    timer->stopTime.tv_usec = 0 ;
}

/*******************************************************************************

Procedure:

    bmwStop ()


Purpose:

    Function bmwStop() stops a benchmarking timer.


    Invocation:

        bmwStop (&timer) ;

    where

        <timer>		- O
            is the address of the timer used for this benchmark.

*******************************************************************************/


void  bmwStop (

#    if PROTOTYPES
        BmwClock  *timer)
#    else
        timer)

        BmwClock  *timer ;
#    endif

{
    timer->stopTime = tvTOD () ;		/* Current time. */
}

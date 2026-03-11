#include  <stdio.h>
#include  <stdlib.h>
#include  "gq_util.h"

        int  main (int argc, char *argv[])
        {
            GeneralQ  queue ;

            gq_util_debug = 1 ;

            gqCreate (5, &queue) ;

            printf ("Double-ended queue:\n") ;

            gqAddFront (queue, "A") ;
            gqAddRear (queue, "B") ;
            gqAddFront (queue, "C") ;
            gqAddRear (queue, "D") ;
            gqAddFront (queue, "E") ;
            gqAddRear (queue, "F") ;

            while (gqCount (queue) > 0) {
                printf ("%s", (char *) gqRemoveFront (queue)) ;
            }
            printf ("\n") ;

            printf ("Regular FIFO queue:\n") ;

            gqAddRear (queue, "A") ;
            gqAddRear (queue, "B") ;
            gqAddRear (queue, "C") ;
            gqAddRear (queue, "D") ;
            gqAddRear (queue, "E") ;
            gqAddRear (queue, "F") ;

            while (gqCount (queue) > 0) {
                printf ("%s", (char *) gqRemoveFront (queue)) ;
            }
            printf ("\n") ;

            printf ("LIFO stack-like queue:\n") ;

            gqPush (queue, "A") ;
            gqPush (queue, "B") ;
            gqPush (queue, "C") ;
            gqPush (queue, "D") ;
            gqPush (queue, "E") ;
            gqPush (queue, "F") ;

            while (gqCount (queue) > 0) {
                printf ("%s", (char *) gqPop (queue)) ;
            }
            printf ("\n") ;

            gqDestroy (queue) ;

            exit (0) ;

        }

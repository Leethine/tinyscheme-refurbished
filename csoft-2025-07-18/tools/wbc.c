/* $Id: wbc.c,v 1.2 2022/10/26 13:27:14 alex Exp alex $ */
/*******************************************************************************

Program:

    wbc.c

    WordBrain Count.


Author:    Alex Measday


Purpose:

    WBC counts the total number of possible chains of N cells that can be
    formed on an X-by-Y rectangular grid.  At each of the X*Y positions
    in the grid, WBC determines how many N-length chains can be constructed
    beginning at that position.

    The program was inspired by MAG Interactive's WordBrain game for phones
    and tablets:

              https://www.maginteractive.com/games/wordbrain/

    Alphabetic tiles spelling certain words are jumbled on a square playing
    board.  Adjacent tiles must be connected in sequence to spell out a word.
    For example, "BRAIN" is formed by starting at a "B", sliding to an
    adjacent "R", sliding to an "A" adjacent to the "R", and sliding to
    an "I" and "N" in turn.  Adjacent tiles can be connected up, down, left,
    right, and diagonally.  A tile already used in a word cannot be reused.
    When a word is formed, its tiles disappear from the board and the tiles
    above drop down to fill the space.  The player is only given the lengths
    of the words to be found, although hints can be used to reveal actual
    letters in the words.

    The game is fun and I got pretty far, I think well into the 7x7 games.
    However, the need sometimes to find two-letter words became tedious:
    at each of the 49 positions, try all of the possible two letter words.
    It was frustrating and I eventually moved on to other games.

    Since then, I've always wondered how many possible two-letter words
    are there on a full 7x7 board.  So, a few years later, I wrote WBC.
    The program does make a couple of assumptions.  First, the board
    is full, which is only true before any words are formed.  Second,
    a chain of N arbitrary tiles always forms a word.  Obviously, this
    is not the case in the game: you must form valid words.  WBC, on the
    other hand, will count "THE" beginning at the "T", "HET" beginning
    at the "H", and "EHT" beginning at "E".

    As it turns out, there are 312 possible two-letter words in a 7x7 puzzle.
    Here are the counts for all the possible lengths in a 3x4 puzzle (with a
    tweak in the formatting!):

        % wbc -board 3x4 1-13
        Board: 3x4      Length: 1       # of possible words:    12
        Board: 3x4      Length: 2       # of possible words:    58
        Board: 3x4      Length: 3       # of possible words:   256
        Board: 3x4      Length: 4       # of possible words:   946
        Board: 3x4      Length: 5       # of possible words:  2952
        Board: 3x4      Length: 6       # of possible words:  7808
        Board: 3x4      Length: 7       # of possible words: 17348
        Board: 3x4      Length: 8       # of possible words: 31604
        Board: 3x4      Length: 9       # of possible words: 45424
        Board: 3x4      Length: 10      # of possible words: 47312
        Board: 3x4      Length: 11      # of possible words: 30848
        Board: 3x4      Length: 12      # of possible words:  9356
        Board: 3x4      Length: 13      # of possible words:     0
        %

    I initially thought it might be possible to populate a board with
    two alternating letters which spelled a word both forward and
    backword, and thus give a board which achieved the maximum number
    of two-letter words.  (I was wrong!)  I implemented the "-two"
    option to generate all possible two-letter combinations in which
    one and only one letter is a vowel.  Pairs are generated in which
    the second of a pair is the first in reverse.  For example:

        ab  ba
        ac  ca
        ad  da
        af  fa
        ...
        on  no
        ...

    "On" and "no" were the only "true" word pair, not counting slightly
    slangy words such as "ah"/"ha", "am"/"ma", etc.  When I actually
    wrote a board down on paper with alternating "N"s and "O"s, however,
    I saw my mistake.  If you begin each row with the same letter, "N"
    or "O", then you can't form vertical words because they're the same
    letters.  If you alternate the beginning letter of each row, then
    you can't form diagonal words for the same reason.  Oh, well ...

        N O N O           N O N O
        N O N O    vs.    O N O N
        N O N O           N O N O


    Invocation:

        % wbc [-debug] [-help]
              [-board <rows>[x<columns>]]
              [-two]
              [<minLength>[-<maxLength>]]

    where

        "-debug"
            enables debug, if any to standard output.
        "-help"
            displays help information.
        "-board <rows>[x<columns>]"
            specifies the dimensions of the rectangular board.  If the number
            of columns is not specified, the board is square.  The default
            board size is 7x7.
        "-two"
            generates pairs of two-letter words and their reverse.
        "<minLength>[-<maxLength>]"
            specifies the chain length or a range of chain lengths.  The
            default length is 2.  If a range is specified, the counts for
            each length are separately calculated and printed.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "opt_util.h"			/* Option scanning definitions. */


static  int  debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  debug


/* Used for both dimensions (number of rows and columns on a board) and
   positions (row and column of a tile on the board). */

typedef  struct  RowColumn {
    long  row ;
    long  col ;
}  RowColumn ;


/*******************************************************************************
    Private Functions
*******************************************************************************/

static  int  chainsOf (
#    if PROTOTYPES
        RowColumn  position,
        RowColumn  dimensions,
        long  length,
        const  char  *visited
#    endif
    ) ;

/*******************************************************************************
    Main Program.
*******************************************************************************/

int  main (

#    if PROTOTYPES
        int  argc,
        char  *argv[])
#    else
        argc, argv)

        int  argc ;
        char  *argv[] ;
#    endif

{    /* Local variables. */
    bool  generatePairs, help ;
    char  *argument, *s ;
    int  errflg, option ;
    long  count, i, j, length, maxLength, minLength ;
    OptContext  context ;
    RowColumn  dimensions, position ;

    static  const  char  *optionList[] = {	/* Command line options. */
        "{debug}", "{help}",
        "{board:}", "{two}", NULL
    } ;




    aperror_print = 1 ;


/*******************************************************************************
    Scan the command line options.
*******************************************************************************/

    debug = 0 ;  help = false ;
    generatePairs = false ;
    minLength = maxLength = 2 ;
    dimensions.row = dimensions.col = 7 ;

    opt_init (argc, argv, NULL, optionList, &context) ;
    errflg = 0 ;

    while ((option = opt_get (context, &argument)) && !errflg) {

        switch (option) {
        case 1:			/* "-debug" */
            debug = 1 ;
            break ;
        case 2:			/* "-help" */
            help = true ;
            break ;
        case 3:			/* "-board <rows>[x<columns>]" */
            dimensions.row = (long) strtoul (argument, &s, 0) ;
            if (*s == '\0') {
                dimensions.col = dimensions.row ;
            } else if ((*s == 'x') || (*s == 'X')) {
                dimensions.col = (long) strtoul (++s, &s, 0) ;
            }
            if (*s != '\0')  errflg++ ;
            break ;
        case 4:			/* "-two" */
            generatePairs = true ;
            break ;
        case NONOPT:		/* "<minLength>[-<maxLength>]" */
            minLength = (long) strtoul (argument, &s, 0) ;
            if (*s == '\0') {
                maxLength = minLength ;
            } else if (*s == '-') {
                maxLength = (long) strtoul (++s, &s, 0) ;
            }
            if (*s != '\0')  errflg++ ;
            break ;
        case OPTERR:
            errflg++ ;  break ;
        default :  break ;
        }

    }

    opt_term (context) ;


/*******************************************************************************
    Help text.
*******************************************************************************/

    if (errflg || help) {
        FILE  *helpout = (help) ? stdout : stderr ;
        fprintf (helpout, "WordBrain Count\n") ;
        fprintf (helpout, "Usage:  wbc [-debug] [-help]\n") ;
        fprintf (helpout, "            [-board <rows>[x<columns>]]\n") ;
        fprintf (helpout, "            [-two]\n") ;
        fprintf (helpout, "            [<minLength>[-<maxLength]]\n") ;
        exit (0) ;
    }

/******************************************************************************
    If the "-two" option was given, generate pairs of two letter words
    that are the reverse of each other.
*******************************************************************************/

    if (generatePairs) {

        static  const  char  *consonants = "bcdfghjklmnpqrstvwxyz" ;
        static  const  char  *vowels = "aeiouy" ;

        for (i = 0 ;  i < (long) strlen (vowels) ;  i++) {
            int  letter_1 = vowels[i] & 0x7F ;
            for (j = 0 ;  j < (long) strlen (consonants) ;  j++) {
                int  letter_2 = consonants[j] & 0x7F ;
                if (letter_1 == letter_2)  continue ;	/* Skip "yy". */
                printf ("%c%c  %c%c\n",
                        letter_1, letter_2,
                        letter_2, letter_1) ;
            }
        }

        exit (0) ;

    }

/*******************************************************************************
    Count the number of possible chains of length N on the complete board.
*******************************************************************************/

    for (length = minLength ;  length <= maxLength ;  length++) {

        count = 0 ;

        for (i = 1 ;  i <= dimensions.row ;  i++) {
            for (j = 1 ;  j <= dimensions.col ;  j++) {
                position.row = i ;
                position.col = j ;
                count += chainsOf (position, dimensions, length, NULL) ;
            }
        }

        printf ("Board: %ldx%ld\tLength: %ld\t# of possible words: %ld\n",
                dimensions.row, dimensions.col, length, count) ;

    }


    exit (0) ;

}

/*******************************************************************************

Procedure:

    chainsOf ()

    Count the Number of Chains of Length N from a Given Position.


Purpose:

    Function chainsOf() counts the number of chains of length N that can
    be constructed beginning at a given position.  Positions already in
    use (visited) cannot be reused in a chain.  The function is recursive:

        chainsOf (position, length, visited) {
            if (position already visited)  return (0)
            if (length is 1)  return (1)	! End of chain?
            Mark position as "visited".
            count = 0
            for (each adjacent position) {
                count += chainsOf (adjacent position, length - 1, visited)
            }
            return (count)
        }

    The map of visited positions is logically passed by value: chainsOf()
    makes a copy of the map so that the caller's map is not modified.


    Invocation:

        count = chainsOf (position, dimensions, length, visited) ;

    where

        <position>	- I
            is position (row, column) on the board at which to begin
            constructing chains of length N.
        <dimensions>	- I
            are the size (number of rows and columns) of the board.
        <length>	- I
            is the length, N, of the chains to be constructed.
        <visited>	- I
            is a map of the positions already visited by the recursive
            algorithm.  Visited positions cannot be used when constructing
            new chains.  This argument is logically passed by value; the
            function will modify a copy of the argument during its processing.
        <count>		- O
            returns the count of possible chains that can be constructed
            beginning at the specified position.

*******************************************************************************/


static  int  chainsOf (

#    if PROTOTYPES
        RowColumn  position,
        RowColumn  dimensions,
        long  length,
        const  char  *visited)
#    else
        position, dimensions, length, visited)

        RowColumn  position ;
        RowColumn  dimensions ;
        long  length ;
        const  char  *visited ;
#    endif

{    /* Local variables. */
    char  buffer[128] ;
    long  count, i, j ;
    RowColumn  next ;

#define  AREA_OF(d)  ((d).row * (d).col)
#define  COPY_OF(v,d)  (memcpy (malloc (AREA_OF(d)), (v), AREA_OF(d)))
#define  ZERO_OF(v,d)  (memset (malloc (AREA_OF(d)), 0, AREA_OF(d)))
#define  VISITED(v,d,p)  (((char *)(v))[(((((p).row-1) * (d).col)) + ((p).col-1))])

    if (debug) {
        LGI "(chainsOf) Called ...\n") ;
        LGI "(chainsOf)     Position:   %ld, %ld\n",
            position.row, position.col) ;
        LGI "(chainsOf)     Dimensions: %ldx%ld\n",
            dimensions.row, dimensions.col) ;
        LGI "(chainsOf)     Length:     %ld\n", length) ;
        LGI "(chainsOf)     Visited:    %p\n", visited) ;

        for (i = 1 ;  i <= dimensions.row ;  i++) {
            strcpy (buffer, "") ;
            for (j = 1 ;  j <= dimensions.col ;  j++) {
                next.row = i ;  next.col = j ;
                if ((visited != NULL) && VISITED (visited, dimensions, next))
                    strcat (buffer, "X") ;
                else
                    strcat (buffer, "-") ;
            }
            LGI "(chainsOf)     %s\n", buffer) ;
        }
    }

    if (length == 0)
        return (0) ;			/* Past end of chain? */
    if ((position.row < 1) || (dimensions.row < position.row))
        return (0) ;			/* Off board? */
    if ((position.col < 1) || (dimensions.col < position.col))
        return (0) ;			/* Off board? */
    if ((visited != NULL) && (VISITED (visited, dimensions, position)))
        return (0) ;			/* Already visited in chain? */
    if (length ==1)
        return (1) ;			/* End of chain? */

    if (visited == NULL)		/* First call: allocate blank board. */
        visited = ZERO_OF (visited, dimensions) ;
    else				/* "Pass-by-value" argument, so copy. */
        visited = COPY_OF (visited, dimensions) ;

    VISITED (visited, dimensions, position) = 1 ;
    count = 0 ;

    for (i = 0 ;  i < 3 ;  i++) {

        next.row = position.row - 1 + i ;

        for (j = 0 ;  j < 3 ;  j++) {

            next.col = position.col - 1 + j ;

            LGI "(chainsOf) Recursing on Length-1: %ld  Row: %ld  Col: %ld\n",
                length - 1, next.row, next.col) ;

            count += chainsOf (next, dimensions, length - 1, visited) ;

        }

    }

    free ((char *) visited) ;

    return (count) ;

}

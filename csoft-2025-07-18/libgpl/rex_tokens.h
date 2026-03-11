/* $Id: rex_tokens.h,v 1.1 2021/04/22 02:40:55 alex Exp $ */
/*******************************************************************************

    Parser Lookup Table for Tokens

    - Manually updated from the "rex_tokens.h.new" file generated
      by the Makefile.

    - Included at the bottom of the "rex_util_y.y" grammar file;
      it needs to be included after the parser-generator's "#define"s
      of the numeric values of the tokens.

    - A sample declaration of the lookup table would be:

            #include  "nnl_util.h"	-- Name-to-Number Lookup utilities.
            ...
            static  const  NNLTable  tokenLUT = {
            #    include  "rex_tokens.h"
                { -1, NULL }
            } ;

    - I did this because I could--I haven't actually found a use for it yet!

*******************************************************************************/

    { (long) ASTERISK, "ASTERISK" },
    { (long) CARET, "CARET" },
    { (long) COLON, "COLON" },
    { (long) COMMA, "COMMA" },
    { (long) CTYPE, "CTYPE" },
    { (long) DIGIT, "DIGIT" },
    { (long) DOLLAR_SIGN, "DOLLAR_SIGN" },
    { (long) _ERROR, "_ERROR" },
    { (long) HYPHEN, "HYPHEN" },
    { (long) LEFT_ANGLE, "LEFT_ANGLE" },
    { (long) LEFT_BRACE, "LEFT_BRACE" },
    { (long) LEFT_BRACKET, "LEFT_BRACKET" },
    { (long) LEFT_PAREN, "LEFT_PAREN" },
    { (long) PERIOD, "PERIOD" },
    { (long) PLUS, "PLUS" },
    { (long) RIGHT_ANGLE, "RIGHT_ANGLE" },
    { (long) RIGHT_BRACE, "RIGHT_BRACE" },
    { (long) RIGHT_BRACKET, "RIGHT_BRACKET" },
    { (long) RIGHT_PAREN, "RIGHT_PAREN" },
    { (long) QUESTION_MARK, "QUESTION_MARK" },
    { (long) SINGLE_CHARACTER, "SINGLE_CHARACTER" },
    { (long) STRING, "STRING" },
    { (long) VERTICAL_BAR, "VERTICAL_BAR" },

/* get_token.c (acdc) - copyleft I.M.Dunkin 1990.
 *
 * 15 Sep 90   IMD         Initial coding.
 *
 */

/*
Implementation of the System V function strtok() for Primos.

    Summary:

        char *get_token(string1, string2);     Finds token in string1

        char *string1;                      String containing token(s)
        char *string2;                      Set of delimiter characters

    Description:

        First call, searches for first token in strin1, skipping
        leading delimiters, returns pointer to first token.

        To read subsequent tokens, call get_token with NULL as string1.
        Returns pointer to next token.

    Notes:

        o   Set of delimiters may be changed from call to call.

        o   get_token() modifies string1 by inserting nulls.

        o   If no more tokens, NULL is returned.

        o   All tokens are null terminated.

IMD 25 Oct 90
*/
#include <string.h>
#include <stdio.h>

static char *saved = NULL;

#ifdef __STDC__
char *get_token(char *s, char *c)
#else
char *get_token(s, c)
char *s, *c;
#endif
{
    register char *p, *result;

    /* If a first call, we save the string.. */
    if (s)
        saved = s;

    /* If saved is null, we are past end of string.. */
    if (!saved)
        return(NULL);

    /* skip leading delimiters, find end of first token.. */
    while ((p = strpbrk(saved, c)) == saved)
        saved++;

    /* strpbrk() returns null if no match, otherwise an address.. */
    if (p)
    {
        *p = 0;
        /* Store start of this token.. */
        result = saved;
    }
    else
        result = p;

    /* Set start for next time.  If not null this is next postion.. */
    if ((saved = p))
        saved++;

    /* And return the result.. */
    return(result);
}


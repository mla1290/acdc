/* get_token.c (acdc) - copyleft Mike Arnautov 2003.
 *
 * 01 Mar 03   MLA         Initial coding.
 *
 */

#include <stdio.h>
#include <string.h>

#ifdef __STDC__
char *get_token(char **string, char *delims)
#else
char *get_token(string, delims)
char **string;
char *delims;
#endif
{
   char *cptr;
   char *token = *string;

   if (! *token)
      return (NULL);

   while ((cptr = strpbrk(token, delims)) == token)
      (token)++;

   if (cptr)
      *cptr++ = 0;

   *string = cptr;
   return(token);
}

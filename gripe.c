/* gripe.c (acdc) - copyleft Mike Arnautov 1990-2003.
 *
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 22 Nov 90   MLA           Don't print null file name.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#include <stdlib.h>

#include "const.h"
#include "line.h"
#include "source.h"

#ifdef __STDC__
void gripe(char *error_token, char *error_message)
#else
void gripe(error_token, error_message)
char *error_token;
char *error_message;
#endif
{
   if (listing == FALSE)
   {
      char *rptr = raw_line;
      while (*rptr == ' ' || *rptr == '\t') rptr++;
      *(rptr + strlen (rptr) - 1) = '\0';
      (void) fprintf (stderr, "*ERROR* in \"%s\"\n", rptr);
   }
   if (*pathname[level] != '\0')
      (void) fprintf (stderr, "%s, line %d: ", pathname[level], 
         line_count[level]);
   if (*error_token != '\0')
      (void) fprintf (stderr, "%s - ", error_token);
   (void) fprintf (stderr, "%s\n\n", error_message);
   exit(ERROR);
}

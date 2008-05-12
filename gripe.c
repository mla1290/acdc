/* gripe.c (acdc) - copyleft Mike Arnautov 1990-2008.
 *
 * 15 Mar 08   MLA           Version 12 changes.
 * 06 Mar 03   Stuart Munro  Include string.h.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 22 Nov 90   MLA           Don't print null file name.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include <stdlib.h>
#include <string.h>

#include "acdc.h"
#include "const.h"
#include "line.h"
#include "source.h"
#include "text.h"

#ifdef __STDC__
void gripe(char *error_token, char *error_message)
#else
void gripe(error_token, error_message)
char *error_token;
char *error_message;
#endif
{
   char *rptr = raw_line;
   while (*rptr == ' ' || *rptr == '\t') rptr++;
   *(rptr + strlen (rptr) - 1) = '\0';
   
   (void) fprintf (stderr, "*ERROR* ");
   if (stage != -1)     /* I.e. not called from organise() */
   {
      (void) fprintf (stderr, "in \"%s\"\n", rptr);
      if (*pathname[level] != '\0')
         (void) fprintf (stderr, "%s, line %d: ", pathname[level], 
            line_count[level]);
   }
   if (error_token && *error_token != '\0')
      (void) fprintf (stderr, "%s - ", error_token);
   (void) fprintf (stderr, "%s\n\n", error_message);
   exit (ERROR);
}

/*====================================================================*/

#ifdef __STDC__
void deprecate( const char *directive, int from, int major)
#else
void deprecate(directive, from, major)
char *directive;
int from;
int major;
#endif
{
   if (style >= from)
   fprintf (stderr, "%s, line %d: deprecated %s directive %s\n", 
      pathname[level], line_count[level], 
         major ? "major" : "minor", directive);
}

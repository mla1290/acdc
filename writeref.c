/* writeref.c (acdc) - copyleft Mike Arnautov 1990-2010.
 *
 * 06 Mar 03   Stuart Munro  Include stdlib.h.
 * 10 Mar 02   MLA           Initial coding.
 *
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include <stdio.h>
#include <stdlib.h>

#include "source.h"
#include "output.h"

char xref_path [MAXLINE + 1];
#ifdef __STDC__

void write_ref (
   char *tag,
   char *token)
   
#else

void write_ref (tag, token)
   char *tag;
   char *token;
   
#endif

{
   if (xref_file == NULL)
   {
      if ((xref_file = fopen (xref_path, "w")) == NULL)
      {
         fprintf (stderr, "Sorry, cannot open cross-reference file %s.\n", 
            xref_path);
         exit (1);
      }
   }
   fprintf (xref_file, "%24s   %s   %6d %s\n",
      token, tag, line_count [level], pathname [level]);
}

/* clsfile.c (acdc) - copyleft Mike Arnautov 1990-2011.
 *
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 29 Dec 90   MLA           Initial coding.
 *
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include <stdio.h>
#ifndef PRIMOS
#include <errno.h>
#endif /* PRIMOS */
#include "acdc.h"

#ifdef __STDC__
void clsfile (FILE *outfile, char *text)
#else
void clsfile (outfile, text)
FILE *outfile;
char *text;
#endif
{
   if (ferror (outfile))
   {
      perror ("Fatal!");
      gripe (text, "Unable to complete file.");
   }
   fclose (outfile);
   return;
}

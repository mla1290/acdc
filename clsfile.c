/* clsfile.c (acdc) - copyleft Mike Arnautov 1990-2004.
 *
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 29 Dec 90   MLA           Initial coding.
 *
 */

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
      (void) perror ("Fatal!");
      (void) gripe (text, "Unable to complete file.");
   }
   (void) fclose (outfile);
   return;
}

/* openout.c (acdc) - copyleft Mike Arnautov 2002 - 2008.
 *
 * 03 Aug 09   MLA           Extended copyleft notice to all non-adv550 games.
 * 06 Mar 03   Stuart Munro  Remove unused variables.
 * 04 Mar 03   MLA           Changed copyleft code to be unconditional.
 * 18 Nov 01   MLA           Added the copyleft code for adv660, adv770.
 * 30 Sep 00   MLA           Simplified directory serach.
 * 07 Aug 99   MLA           Initial coding.
 *
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include <stdio.h>
#include <string.h>

#include "acdc.h"
#include "source.h"
#include "game.h"

#ifdef __STDC__
FILE *openout (
   char *file,
   char *mode)
#else
FILE *openout (file, mode)
   char *file;
   char *mode;
#endif
{
   char *brk;
   char full_name [128];
   FILE *outfile;
   
   if (*source_stem == '\0')
      sprintf (full_name, "C%c%s", SEP, file);
   else
      sprintf (full_name, "%sC%c%s", source_stem, SEP, file);

   if ((outfile = fopen (full_name, mode)) == NULL)
   {      
      if (*source_stem == '\0')
         strcpy (full_name, file);
      else
         sprintf (full_name, "%s%s", source_stem, file);

      if ((outfile = fopen (full_name, mode)) == NULL)
      {
         fprintf (stderr, "%s -- unable to open file!\n", full_name);
         exit (1);
      }
   }

   if (*title && *author && strcmp (title, "adv550") != 0)
   {
      brk = file + strlen (file) - 2;
      if (*brk == '.' && (*(brk + 1) == 'c' || *(brk + 1) == 'h'))
            fprintf (outfile, "/* %s: %s - copyleft %s, %s. */\n\n", file,
               title, author, datbuf);
   }

   return (outfile);
}

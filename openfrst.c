/* openfrst.c (acdc) - copyright Mike Arnautov 1990-2015.
 * Licensed under the Modified BSD Licence (see the supplied LICENCE file).
 *
 * 03 Jan 15   MLA           BUG: Fixed source file specified by pathname.
 * 11 May 13   MLA           Bug: Fixed in-line name generation.
 * 14 Jul 09   MLA           Fixed gcc --pedantic warnings.
 * 12 Mar 08   MLA           Varsion 12 changes.
 * 24 Jan 03   MLA           Allowed the .acode variant of the suffix.
 * 10 Mar 02   MLA           Moved opening of xref file to writeref.c
 * 31 Dec 01   MLA           Added cross-referencing code.
 * 17 Nov 01   MLA           Bug: Fixed pathname code.
 * 06 Nov 01   MLA           bug: Display correctly what file not found.
 * 30 Sep 00   MLA           Removed searching for adv_src.
 * 07 Aug 99   MLA           Initial coding.
 *
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __50SERIES
#include <errno.h>
#endif

#include "output.h"
#include "source.h"
#include "line.h"

char source_stem [MAXLINE + 1];
char source_file [80];

#ifdef __STDC__
void openfrst (char *file_spec)
#else
void openfrst (file_spec)
   char *file_spec;
#endif
{
   int i;
   char *nameptr = file_spec;
   char *sfxptr = NULL;
   char fullname [128];
      
   if ((nameptr = strrchr (file_spec, SEP)) != NULL)
   {
      nameptr ++;
      strncpy (source_stem, file_spec,  nameptr - file_spec);
   }
   else
   {
      nameptr = file_spec;
      *source_stem = '\0';
   }      
   strncpy (source_file, nameptr,  sizeof (source_file) - 5);  
      
   sfxptr = strrchr (source_file, '.');
   if (sfxptr == NULL || 
      (strcmp (sfxptr + 1, "acd") != 0 && strcmp (sfxptr + 1, "acode") != 0))
         strcat (source_file, ".acd");
   
   sprintf (fullname, "%s%s", source_stem, source_file);
   if ((*infile = fopen (fullname, "r")) == NULL)
   {
      fprintf (stderr, "Sorry, cannot find source file %s.\n", fullname);
      exit (1);
   }
   strcpy (pathname [0], fullname);
   level = 0;

   strncpy (xref_path, fullname, strlen (fullname) - 3);
   strcat (xref_path, "xrf");

   file_count = 1;
   line_status = EOL;
   total_lines = 0;
   text_lines = 0;
   text_count = 0;
   switch_count = 0;
   inline_count = 0;
   for (i=0; i<10; i++)
      line_count[i] = 0;
   return;

}

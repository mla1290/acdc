/* opnsrc.c (acdc) - copyleft Mike Arnautov 1990-2011.
 *
 * 22 Jul 09   MLA           Changed check for the quiet mode.
 * 15 Mar 08   MLA           Version 12 changes.
 * 19 May 07   MLA           Added "quiet".
 * 08 Mar 94   MLA           Made INCLUDEs relative to umbrella source.
 * 23 Dec 90   MLA           Check for .acd files first!
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#ifndef PRIMOS
#include <errno.h>
#endif
#include "acdc.h"
#include "const.h"
#include "line.h"
#include "source.h"

#ifdef __STDC__
void opnsrc(char *name, int fatal)
#else
void opnsrc(name, fatal)
char *name;
int fatal;
#endif
{
   char *cstring;
   int lev;
   char *path;   
   int len;

   path = &pathname [++level] [0];

/* Construct the source file name. */

   if (*source_stem == '\0')
      strcpy (path, name);
   else
      sprintf (path, "%s%s", source_stem, name);
   cstring = path;
   recase (LOWERCASE, cstring);
   len = strlen (path);
   if (len < 4 || strcmp (cstring + len - 4,".acd") != 0) 
      strcat (path, ".acd");
   else
      cstring = NULL;

/* Try opening the source file. */

   if ((infile [level] = fopen (path, "r")) == NULL)
   {
      if (cstring == NULL || (infile [level] = fopen (path, "r")) == NULL) 
      {
         level--;
         if (fatal == 0)
            return;
         perror (path);
         gripe (path, "Unable to open source file.");
      }
   }
   file_count++;
   line_count [level] = 0;   
   if ((quiet & 1) == 0 && stage) 
   {
      lev = level;
      while (lev-- > 0) 
         printf ("   ");
      printf ("%s\n",path + strlen (source_stem));
   }
   return;
}

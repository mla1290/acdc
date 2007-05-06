/* opnsrc.c (acdc) - copyleft Mike Arnautov 1990-2007.
 *
 * 08 Mar 94   MLA           Made INCLUDEs relative to umbrella source.
 * 23 Dec 90   MLA           Check for .acd files first!
 * 15 Sep 90   MLA           Initial coding.
 *
 */

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
void opnsrc(char *name)
#else
void opnsrc(name)
char *name;
#endif
{
   char *string;
   int lev;
   char *path;   
   int len;

   path = &pathname [++level] [0];

/* Construct the source file name. */

   if (*source_stem == '\0')
      (void) strcpy (path, name);
   else
      (void) sprintf (path, "%s%s", source_stem, name);
   string = path;
   (void) recase (LOWERCASE, string);
   len = strlen (path);
   if (len < 4 || strcmp (string + len - 4,".acd") != 0) 
      (void) strcat (path, ".acd");
   else
      string = NULL;

/* Try opening the source file. */

   if ((infile [level] = fopen (path, "r")) == NULL)
   {
      if (string == NULL)
         goto failed;
      if ((infile [level] = fopen (path, "r")) == NULL) 
         goto failed;
   }
   file_count++;
   line_count [level] = 0;   
   if (listing == FALSE) 
   {
      lev = level;
      while (lev-- > 0) 
         (void) printf ("   ");
      (void) printf ("%s\n",path + strlen (source_stem));
   }
   return;

failed:
   level--;
   (void) perror (path);
   (void) gripe (path, "Unable to open source file.");
}

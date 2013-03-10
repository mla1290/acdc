/* getline.c (acdc) - copyleft Mike Arnautov 1990-2011.
 *
 * 11 Jan 10   MLA           Renamed getline() to nextline() to avoid a
 *                           new gcc header clash.
 * 15 Mar 08   MLA           Version 12 changes.
 * 09 Mar 03   MLA           Replaced trace with debug.
 * 25 Feb 01   MLA           Don't count comment or blank lines towards total.
 * 03 Dec 00   MLA           Allowed '#' as a comment delimiter.                
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 20 Mar 94   MLA           Added source tracing.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include <string.h>

#include "const.h"
#include "line.h"
#include "source.h"
#include "output.h"

#ifdef __STDC__
int nextline (int key)
#else
int nextline (key)
int key;
#endif
{
   line_ptr = line;

next_line:
   if (line_status == EOL)
   {
      while (level >= 0)
      {
         if (fgets (line, MAXLINE, infile [level]) != NULL) break;
         fclose (infile [level]);
         infile [level] = NULL;
         if (--level < 0)
         {
            line_status = EOF;
            return (EOF);
         }
      }
      line_count [level]++;
      if (debug > 1)
      {
         int len;
         len = strlen (line) - 1;
         *(line + len) = '\0';
         fprintf (code_file, "/* %s */\n", line);
         *(line + len) = '\n';
      }
      if (*line_ptr == '*' || *line_ptr == '#')
         goto next_line;   /* Ignore comment lines */
      if (*line_ptr == '\n' && key == IGNORE_BLANK) goto next_line;
      total_lines++;
   }
   line_status = BOL;
   return (OK);
}
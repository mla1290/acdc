/* getline.c (acdc) - copyleft Mike Arnautov 1990-2004.
 *
 * 09 Mar 03   MLA           Replaced trace with debug.
 * 25 Feb 01   MLA           Don't count comment or blank lines towards total.
 * 03 Dec 00   MLA           Allowed '#' as a comment delimiter.                
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 20 Mar 94   MLA           Added source tracing.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#include <string.h>

#include "const.h"
#include "line.h"
#include "source.h"
#include "output.h"

#ifdef __STDC__
int getline (int key)
#else
int getline (key)
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
         (void) fclose (infile [level]);
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
      if (listing) (void) printf ("%d:%d %s", level, line_count [level], line);
      if (*line_ptr == '*' || *line_ptr == '#')
         goto next_line;   /* Ignore comment lines */
      if (*line_ptr == '\n' && key == IGNORE_BLANK) goto next_line;
      total_lines++;
   }
   line_status = BOL;
   return (OK);
}

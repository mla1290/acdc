/* storchar.c (acdc) - copyleft Mike Arnautov 1990-2003.
 *
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#include "text.h"

#ifdef __STDC__
void storchar (int ch)
#else
void storchar (ch)
int ch;
#endif
{
#ifndef STRIP
   if (plain_text)
#endif
      fputc(ch, text_file);
#ifndef STRIP
   else
   {
      int mask;
      if (*(++dbname_ptr) == '\0')
         dbname_ptr = dbname;
      mask = (next_addr >> 4) & 127;
      if (mask == 0) 
         mask = next_addr & 127;
      if (mask == 0)
         mask = 'X';
      fputc(ch ^ *dbname_ptr ^ mask, text_file);
   }
#endif
   next_addr++;
   return;
}

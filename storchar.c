/* storchar.c (acdc) - copyleft Mike Arnautov 1990-2005.
 *
 * 09 Mar 03   MLA           Store as binary or ASCII data depending on 
 *                           value of memory. 
 * 03 Mar 03   MLA           Store as ASCII data.
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
/* PRIVATE */
   if (plain_text)
/* ENDPRIVATE */
      fputc(ch, text_file);
/* PRIVATE */
   else
   {
      int mask;

      if (*(++version_ptr) == '\0')
         version_ptr = version;
      mask = (next_addr >> 4) & 127;
      if (mask == 0) 
         mask = next_addr & 127;
      if (mask == 0)
         mask = 'X';
      mask = (17 * mask + 13) & 127;
      if (memory == 3)
      {
         fprintf (text_file, ",%d", ch ^ *version_ptr ^ mask);
         if ((next_addr + 1) % 16 == 0)
            fputc ('\n', text_file);
      }
      else
         fputc(ch ^ *version_ptr ^ mask, text_file);
   }
/* ENDPRIVATE */
   next_addr++;
   return;
}

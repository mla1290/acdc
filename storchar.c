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
/* PRIVATE */
   if (plain_text)
/* ENDPRIVATE */
      fputc(ch, text_file);
/* PRIVATE */
   else
   {
      int mask;

      if (*(++title_ptr) == '\0')
         title_ptr = title;
      mask = (next_addr >> 4) & 127;
      if (mask == 0) 
         mask = next_addr & 127;
      if (mask == 0)
         mask = 'X';
      mask = (17 * mask + 13) & 127;
      fprintf (text_file, ",%d", ch ^ *title_ptr ^ mask);
      if ((next_addr + 1) % 16 == 0)
         fputc ('\n', text_file);
/*      fputc(ch ^ *title_ptr ^ mask, text_file); */
   }
/* ENDPRIVATE */
   next_addr++;
   return;
}

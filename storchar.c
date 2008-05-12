/* storchar.c (acdc) - copyleft Mike Arnautov 1990-2008.
 *
 * 09 Mar 03   MLA           Store as binary or ASCII data depending on 
 *                           value of memory. 
 * 03 Mar 03   MLA           Store as ASCII data.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include "text.h"
#include "game.h"

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

      if (*(++gameid_ptr) == '\0')
         gameid_ptr = gameid;
      mask = (next_addr >> 4) & 127;
      if (mask == 0) 
         mask = next_addr & 127;
      if (mask == 0)
         mask = 'X';
      mask = (17 * mask + 13) & 127;
      if (memory == 3)
      {
         fprintf (text_file, ",%d", ch ^ *gameid_ptr ^ mask);
         if ((next_addr + 1) % 16 == 0)
            fputc ('\n', text_file);
      }
      else
         fputc(ch ^ *gameid_ptr ^ mask, text_file);
   }
/* ENDPRIVATE */
   next_addr++;
   return;
}

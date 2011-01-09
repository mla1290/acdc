/* storchar.c (acdc) - copyleft Mike Arnautov 1990-2011.
 *
 * 31 Jul 09   MLA           BUG: text file screwed up in plain mode!
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
intch;
#endif
{
/*   ch &= 255; */
   if (plain_text)
   {
      if (memory == 3)
         fprintf (text_file,"%s%d", 
            (next_addr + 1) % 16 == 0 ? ",\n" : ",", ch);
      else
         fputc(ch, text_file);
   }
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
         fprintf (text_file, "%s%d", 
            (next_addr + 1) % 16 == 0 ? ",\n" : ",", ch ^ *gameid_ptr ^ mask);
      else
         fputc(ch ^ *gameid_ptr ^ mask, text_file);
   }
   next_addr++;
   return;
}

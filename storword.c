/* storword.c (acdc) - copyright Mike Arnautov 1990-2015.
 * Licensed under the Modified BSD Licence (see the supplied LICENCE file).
 *
 * 15 Mar 08   MLA           Version 12 changes.
 * 14 Feb 04   MLA           Converted longs to ints.
 * 11 Mar 03   MLA           Suppressed "vocabulary buffer enlarged" message.
 * 14 Sep 00   MLA           Replace'_'s with NBSP.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 30 Dec 90   MLA           Use voc_buf for intermediate vocab storage.
 * 15 Sep 90   MLA           Initial coding.
 *
 */
 
#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include "acdc.h"
#include "symbol.h"
#include "text.h"
#include "const.h"

#ifdef __STDC__
struct node *storword(char *cstring, int type, int addr)
#else
void storword(cstring, type, addr)
char *cstring;
int type;
int addr;
#endif
{
   char *old_voc_ptr;
   struct node *np;

   vocab_count++;
   np = addsymb (VOCAB, cstring, type, -1);
   np -> word_addr = addr;
   np -> voc_addr = next_vocaddr;

   if (voc_ptr >= voc_top)
   {
      voc_buf_len += 1024;      /* Extend by one Kbyte */
      old_voc_ptr = voc_buf_ptr;
      if ((voc_buf_ptr = (char *) realloc (voc_buf_ptr, voc_buf_len)) == NULL)
         gripe ("","Unable to extend vocabulary buffer.");
      voc_ptr = voc_buf_ptr + (voc_ptr - old_voc_ptr);
      voc_top = voc_buf_ptr + voc_buf_len - 20;
   }

   while (*cstring != '\0')
   {
      next_vocaddr++;
      if (*cstring == '_')
      {
         *voc_ptr++ = NBSP;
         cstring++;
      }
      else
         *voc_ptr++ = *cstring++;
   }
   next_vocaddr++;
   *voc_ptr++ = '\0';

   return (np);
}

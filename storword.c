/* storword.c (acdc) - copyleft Mike Arnautov 1990-2002.
 *
 * 14 Sep 00   MLA           Replace'_'s with NBSP.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 30 Dec 90   MLA           Use voc_buf for intermediate vocab storage.
 * 15 Sep 90   MLA           Initial coding.
 *
 */
 
#include "acdc.h"
#include "symbol.h"
#include "text.h"
#include "const.h"

#ifdef __STDC__
void storword(char *string, int type, int refno, long addr)
#else
void storword(string, type, refno, addr)
char *string;
int type;
int refno;
long addr;
#endif
{
   char *old_voc_ptr;
   struct node *np;
   extern void *realloc();

   vocab_count++;
   np = addsymb (VOCAB, string, type, refno);
   np -> body.vocab.word_addr = addr;
   np -> body.vocab.voc_addr = next_vocaddr;

   if (voc_ptr >= voc_top)
   {
      voc_buf_len += 1024;      /* Extend by one Kbyte */
      old_voc_ptr = voc_buf_ptr;
      if ((voc_buf_ptr = (char *) realloc (voc_buf_ptr, voc_buf_len)) == NULL)
         (void) gripe ("","Unable to extend vocabulary buffer.");
      voc_ptr = voc_buf_ptr + (voc_ptr - old_voc_ptr);
      voc_top = voc_buf_ptr + voc_buf_len - 20;
      (void) printf ("Vocabulary buffer size increased to %d.\n", voc_buf_len);
   }

   while (*string != '\0')
   {
      next_vocaddr++;
      if (*string == '_')
      {
         *voc_ptr++ = NBSP;
         string++;
      }
      else
         *voc_ptr++ = *string++;
   }
   next_vocaddr++;
   *voc_ptr++ = '\0';

   return;
}

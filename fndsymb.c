/* fndsymb.c (acdc) - copyleft Mike Arnautov 1990-2007.
 *
 * 07 Jan 03   MLA           Use btree instead of tsearch.
 * 17 Oct 01   MLA           Improved compatibility with Platt's A-code.
 * 28 Jul 99   MLA           Removed superfluous setjmp declaration.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 16 Mar 91   MLA           Unified symbol and vocab searches.
 * 22 Nov 90   MLA           Own TSEARCH -some systems don't have it.
 * 15 Sep 90   MLA           Initial coding.
 *
 */
 
#include <string.h>
#include <setjmp.h>

#include "acdc.h"
#include "symbol.h"
#include "const.h"
#include "text.h"
#include "btree.h"

int refno;
int type;
jmp_buf abort_env;
struct node *result;

#ifdef __STDC__
struct node *fndsymb (int root_type, char *word)
#else
struct node *fndsymb (root_type, word)
int root_type;
char *word;
#endif
{
   int real_root;
   void checksymb ();

   if (root_type == SYMBOL_OR_CONSTANT)
      real_root = SYMBOL;
   else
      real_root = root_type;

   result = btfind (real_root, word);
   if (result == NULL && real_root == SYMBOL)
   {
      result = btfind (VOCAB, word);
      if (result == NULL)
      {
         if (root_type == SYMBOL)
            (void) gripe (word, "Symbol not found.");
         return (NULL);
      }
      refno = result -> refno;
      type = result -> type;
      result = NULL;
      if (setjmp (abort_env) == 0)
         btspan (SYMBOL, checksymb);
   }
   if (result != NULL)
   {
      if (real_root == SYMBOL)
         (result -> used_count)++;
      return (result);
   }
   else
      return (NULL);
}

#ifdef __STDC__
void checksymb (struct node *npp)
#else
void checksymb (npp)
struct node *npp;
#endif
{
      if (npp -> refno == refno && npp -> type == type)
      {
         result = npp;
         longjmp (abort_env, 1);
      }

   return;
}

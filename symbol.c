/* symbol.c (acdc) - copyleft Mike Arnautov 1990-2013.
 *
 * 06 Apr 10   MLA           Resticted identifier names.
 * 15 Mar 08   MLA           Version 12 changes.
 * 07 May 07   Stuart Munro  bug: declare chksymb before fndsymb.
 * 15 Jan 05   MLA           Added AUTOSYMBOL handling.
 * 07 Jan 03   MLA           Use btree instead of tsearch.
 * 17 Oct 01   MLA           Improved compatibility with Platt's A-code.
 * 24 Jul 01   MLA           Added ANSI C declaration.
 * 28 Jul 99   MLA           Removed superfluous setjmp declaration.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 16 Mar 91   MLA           Unified symbol and vocab searches.
 * 22 Nov 90   MLA           Own TSEARCH -some systems don't have it.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include <string.h>
#include <ctype.h>
#include <setjmp.h>

#include "acdc.h"
#include "game.h"
#include "symbol.h"
#include "const.h"
#include "btree.h"
#include "major.h"

#ifdef __STDC__
struct node *addsymb (int btroot, char *name, int type, int refno)
#else
struct node *addsymb (btroot, name, type, refno)
   int btroot;
   char *name;
   int type;
   int refno;
#endif
{
   struct node *np;
   int len;
   int auto_flag = btroot & 64;

   btroot &= ROOT_MASK;
   
   if (style > 1)
   {
      int bad_char = 1;
      if (isalpha (*name) || strchr (".?!", *name))
      {
         char *c = name + 1;
         bad_char = 0;
         while (*c)
         {
            if (isalnum (*c) || strchr (".?!-_'/&", *c))
               c++;
            else
            {
               bad_char = 1;
               break;
            }
         }
      }
      if (bad_char)
         gripe (name, "Not a legal name.");
   }
   
   if ((np = (struct node *) malloc (sizeof (struct node))) == NULL)
      gripe (name, "Unable to allocate memory.");

   len = strlen (name);
   if (style < 10 && len > 12)
   {
      len = 12;
      *(name + 12) = '\0';
   }
   if ((np -> name = (char *) malloc (len + 1)) == NULL)
      gripe (name, "Unable to allocate symbol name storage.");
   
   strcpy (np -> name, name);
   np -> type = type;
   np -> state_count = (type == VAR) ? 1 : 0;
   np -> used_count = 0;
   np -> auto_flag = auto_flag;
   np -> text_addr[0] = -1;
   np -> text_addr[1] = -1;
   np -> text_addr[2] = -1;
   np -> name_addr = -1;
   np -> proc_count = 0;
   np -> proc_done = 0;
   np -> arg_count = -1;
   np -> symbol = NULL;
   np -> refno = refno;

   if (btadd (btroot, np) == 0)
      gripe (name, "Symbol already defined.");
   return (np);
}

/*======================================================================*/

#ifdef __STDC__
struct node *fndsymb (int btroot, char *fname)
#else
struct node *fndsymb (btroot, fname)
int btroot;
char *fname;
#endif
{
   struct node *np;
   char nbuf [160];
   char *name = nbuf;
   int maxlen = sizeof (nbuf) - 1;
   int testing = (btroot & 32);
   
   if (style < 10 && strlen (fname) > 12)
      maxlen = 12;
   strncpy (nbuf, fname, maxlen);
   *(name + maxlen) = '\0';

   btroot &= ROOT_MASK;

   if (*name == '-' || *name == '+' || *name == '!' || *name == '=')
      name++;
   np = btfind (btroot, name);
   if (np == NULL && btroot == SYMBOL)
   {
      np = btfind (VOCAB, name);
      if (np == NULL)
      {
         if (testing) return (NULL);
         gripe (name, "Symbol not found.");
      }
      np = np -> symbol;
   }

/*   if (np && stage && btroot == SYMBOL)
         (np -> used_count)++;
*/
   return (np);
}

/************************************************************************/


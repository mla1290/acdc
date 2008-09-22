/* symbol.c (acdc) - copyleft Mike Arnautov 1990-2008.
 *
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
#include <setjmp.h>

#include "acdc.h"
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
         
   if ((np = (struct node *) malloc (sizeof (struct node))) == NULL)
      (void) gripe (name, "Unable to allocate memory.");

   len = strlen (name);
   if ((np -> name = (char *) malloc (len + 1)) == NULL)
      (void) gripe (name, "Unable to allocate symbol name storage.");
   
   (void) strcpy (np -> name, name);
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
      (void) gripe (name, "Symbol already defined.");
   return (np);
}

/*======================================================================*/

#ifdef __STDC__
struct node *fndsymb (int btroot, char *name)
#else
struct node *fndsymb (btroot, name)
int btroot;
char *name;
#endif
{
   struct node *np;
   
   int testing = (btroot & 32);
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
         (void) gripe (name, "Symbol not found.");
      }
      np = np -> symbol;
   }

/*   if (np && stage && btroot == SYMBOL)
         (np -> used_count)++;
*/
   return (np);
}

/************************************************************************/

/* getdesc.c (acdc) - copyleft Mike Arnautov 1990-2007.
 *
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 12 Dec 91   MLA           Added third arg to gettxt ().
 * 30 Sep 90   MLA           Intialise states!!!
 * 15 Sep 90   MLA           Initial coding.
 *
 */
 
#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include "acdc.h"
#include "const.h"
#include "line.h"
#include "symbol.h"
#include "text.h"

#ifdef __STDC__
void getdesc (struct node *np)
#else
void getdesc (np)
struct node *np;
#endif
{
   int description;
   int max_states;
   int states;

   description = 1;
   max_states = 0;
   states = 0;

   while (description > 0)
   {
      np -> body.text.text_addr [description - 1] = next_addr;
      description = gettxt (description, &states, FALSE, NULL);
      if (max_states < states) max_states = states;
   }
   np -> state_count = max_states;
   return;
}

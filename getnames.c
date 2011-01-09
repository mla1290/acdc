/* getnames.c (acdc) - copyleft Mike Arnautov 1990-2011.
 *
 * 08 Jul 09   MLA           bug: Reinstate style 1 support.
 * 15 Mar 08   MLA           Version 12 changes.
 * 14 Feb 04   MLA           Converted longs to ints.
 * 14 Jul 02   MLA           BUG: Report primary names too!
 * 13 Jan 02   MLA           Added xref.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 16 Mar 91   MLA           Suppress used count for . prefix.
 * 30 Dec 90   MLA           Use voc_buf for intermediate vocab storage.
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
#include "major.h"
#include "output.h"
#include "source.h"
#include "text.h"

#ifdef __STDC__
struct node *getnames (int major_type, struct node *gp)
#else
struct node *getnames (major_type, gp)
int major_type;
struct node *gp;
#endif
{
   int index;
   int prefix;
   int current_type;
   int real_type;
   int refno;
   int used;
   int last_addr;
   char *tag;
   struct node *np;
   struct node *vp;
   
   if (*tp [1] == '=')
      gripe (tp [1], "No preceding word to equate to!");
   index = 0;
   current_type = real_type = major_type;
   if (major_type == NOUN || major_type == ADJECTIVE || 
       major_type == PREPOSITION)  
          current_type = major_type = VERB;
   
   if ((np = gp) != NULL)
   {
      index = 1;
      refno = gp -> refno;
      last_addr = gp -> word_addr;
   }

   while (tp [++index] != NULL)
   {
      prefix = *tp [index];
      used = (prefix == '.');
      if (prefix == '+' ||prefix == '-' || prefix == '=' || prefix == '!')
         tp [index]++;
      else
         prefix = (major_type == LOC) ? '-' : '+';

      if (index == 1)
      {
         if ((np = fndsymb (TESTSYMBOL, tp [index])) == NULL)
            np = addsymb (SYMBOL, tp [index], current_type, -1);
         else if (np -> type > PROC)
            np -> type = major_type;
         if (used)
            np -> used_count = 1;
         
         if (xref)
         {
            if (real_type == LOC)
               tag = "LOC";
            else if (real_type == OBJ)
               tag = " OBJ ";
            else if (real_type == SYNONYM)
               tag = "SYNON";
            else if (real_type == VERB)
               tag = " VRB ";
            else if (real_type == NOUN)
               tag = "NOUN  ";
            else if (real_type == ADJECTIVE)
               tag = " ADJ ";
            else if (real_type == PREPOSITION)
               tag = "PREP ";
            write_ref (tag, tp [index]);
         }
         
         if (prefix != '-')
               np -> name_addr = next_vocaddr;
         else
            return (np);
      }

      if (xref) 
      {
         if (real_type == LOC)
            tag = "place";
         else if (real_type == OBJ)
            tag = " obj ";
         else if (real_type == SYNONYM)
            tag = "synon";
         else if (real_type == VERB)
            tag = " vrb ";
         else if (real_type == NOUN)
            tag = "noun ";
         else if (real_type == ADJECTIVE)
            tag = " adj ";
         else if (real_type == PREPOSITION)
            tag = "prep ";
         write_ref (tag, tp [index]);
      }

      if (prefix == '!') tp [index]--;
      current_type = SYNONYM;            /* For the next loop! */

      if (prefix == '=')
         vp = storword (tp [index], major_type, last_addr);
      else
      {
         last_addr = next_vocaddr;
         vp = storword (tp [index], major_type, next_vocaddr);
      }
      vp -> symbol = np;
   }

   return (np);
}

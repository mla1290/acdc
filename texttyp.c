/* texttyp.c (acdc) - copyleft Mike Arnautov 1990-2009.
 *
 * 25 Mar 08   MLA               Split off from domajor.c
 *
 */
 
#include <stdio.h>

#include "line.h"
#include "text.h"

#ifdef __STDC__
int texttyp (void)
#else
int texttyp ()
#endif
{
   int type = 0;
   int frag = 0;
   int index;
   
   while (1)
   {
      int typ = 0;
      if (tp [1])
      {
         if (strcmp (tp [1], "fragment") == 0) typ = FRAGMENT_TEXT;               
         if (strcmp (tp [1], "cycle") == 0 ||
             strcmp (tp [1], "cyclic") == 0) typ = CYCLIC_TEXT;
         if (strcmp (tp [1], "assigned") == 0) typ = ASSIGNED_TEXT;
         if (strcmp (tp [1], "random") == 0) typ = RANDOM_TEXT;
         if (strcmp (tp [1], "increment") == 0) typ = INCREMENTING_TEXT;
         if (typ)
         {
            if (type && (typ & MORPHING_TEXT))
               (void) gripe (NULL, "Incompatible text typifiers.");
            if (typ & FRAGMENT_TEXT)
               frag = 1;
            else
               type = typ;
            index = 1;
            while (tp [index] = tp [index + 1]) /* Yes, I do mean an assignement! */
               index++;
            typ = 0;            
            continue;
         }
         else if (tp [2])
            (void) gripe (tp [1], "Illegal typifier.");
         break;
      }
      break;
   }
   return (type + FRAGMENT_TEXT * frag);
}

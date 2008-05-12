/* texttyp.c (acdc) - copyleft Mike Arnautov 1990-2008.
 *
 * 25 Mar 08   MLA               Split off from domajor.c
 *
 */
 
#include <stdio.h>

#include "line.h"

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
         if (strcmp (tp [1], "fragment") == 0) typ = 5;               
         if (strcmp (tp [1], "cycle") == 0 ||
             strcmp (tp [1], "cyclic") == 0) typ = 3;
         if (strcmp (tp [1], "assigned") == 0) typ = 4;
         if (strcmp (tp [1], "random") == 0) typ = 1;
         if (strcmp (tp [1], "increment") == 0) typ = 2;
         if (typ)
         {
            if (type && typ != 5)
               (void) gripe (NULL, "Incompatible text typifiers.");
            if (typ == 5)
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
   return (type + 16 * frag);
}

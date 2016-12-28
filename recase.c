/* recase.c (acdc) - copyright Mike Arnautov 1990-2016.
 * Licensed under GPL, version 3 or later (see the supplied LICENCE file).
 *
 * 03 Mar 16   MLA           Removed non-ANSI C support.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 06 Mar 91   MLA           Provide own tolower/upper, to guard against
 *                           implementations which make no checks.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#include "const.h"

#define TOLOWER(x) (x >= 'A' && x <= 'Z') ? x += 'a' - 'A' : x
#define TOUPPER(x) (x >= 'a' && x <= 'z') ? x += 'A' - 'a' : x

void recase(int key, char *cstring)
{
   if (cstring && *cstring)
   {
      if (key == LOWERCASE)
         *cstring = TOLOWER(*cstring);
      else
         *cstring = TOUPPER(*cstring);

      if (key == CAPITALISE) key = LOWERCASE;

      while (*(++cstring) != '\0')
      {
         if (key == UPPERCASE)
            *cstring = TOUPPER(*cstring);
         else
            *cstring = TOLOWER(*cstring);
      }
   }
   return;
}

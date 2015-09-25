/* recase.c (acdc) - copyright Mike Arnautov 1990-2015.
 * Licensed under the Modified BSD Licence (see the supplied LICENCE file).
 *
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 06 Mar 91   MLA           Provide own tolower/upper, to guard against
 *                           implementations which make no checks.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include "const.h"

#define TOLOWER(x) (x >= 'A' && x <= 'Z') ? x += 'a' - 'A' : x
#define TOUPPER(x) (x >= 'a' && x <= 'z') ? x += 'A' - 'a' : x

#ifdef __STDC__
void recase(int key, char *cstring)
#else
void recase(key, cstring)
int key;
char *cstring;
#endif
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

/* recase.c (acdc) - copyleft Mike Arnautov 1990-2007.
 *
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 06 Mar 91   MLA           Provide own tolower/upper, to guard against
 *                           implementations which make no checks.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#include "const.h"

#define TOLOWER(x) (x >= 'A' && x <= 'Z') ? x += 'a' - 'A' : x
#define TOUPPER(x) (x >= 'a' && x <= 'z') ? x += 'A' - 'a' : x

#ifdef __STDC__
void recase(int key, char *string)
#else
void recase(key, string)
int key;
char *string;
#endif
{
   if (key == LOWERCASE)
      *string = TOLOWER(*string);
   else
      *string = TOUPPER(*string);

   if (key == CAPITALISE) key = LOWERCASE;

   while (*(++string) != '\0')
   {
      if (key == UPPERCASE)
         *string = TOUPPER(*string);
      else
         *string = TOLOWER(*string);
   }
   return;
}

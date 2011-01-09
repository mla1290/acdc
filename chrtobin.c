/* chrtobin.c (acdc) - copyleft Mike Arnautov 1990-2011.
 *
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include "acdc.h"

#ifdef __STDC__
int chrtobin (char *cstring)
#else
int chrtobin (cstring)
char *cstring;
#endif
{
   int sign;
   int base;
   int value;
   char *raw_cstring;

   sign = 1;
   base = 10;
   value = 0;

   raw_cstring = cstring;
   if (*cstring == '\0')
      gripe (cstring,"Null number.");
   if (*cstring == '-')
      sign = -1;
   if (*cstring == '-' || *cstring == '+')
      cstring++;
   if (*cstring == '\0')
      gripe (raw_cstring,"A naked sign is not a number!");
   if (*cstring == '0')
      base = 8;
   while (*cstring != '\0')
   {
      if (*cstring < '0' || *cstring >= '0' + base)
         gripe (raw_cstring,"Not an octal or decimal number.");
      value = base * value + (*cstring++) - '0';
   }
   return (sign * value);
}

/* chrtobin.c (acdc) - copyleft Mike Arnautov 1990-2005.
 *
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#include "acdc.h"

#ifdef __STDC__
int chrtobin (char *string)
#else
int chrtobin (string)
char *string;
#endif
{
   int sign;
   int base;
   int value;
   char *raw_string;

   sign = 1;
   base = 10;
   value = 0;

   raw_string = string;
   if (*string == '\0')
      (void) gripe (string,"Null number.");
   if (*string == '-')
      sign = -1;
   if (*string == '-' || *string == '+')
      string++;
   if (*string == '\0')
      (void) gripe (raw_string,"A naked sign is not a number!");
   if (*string == '0')
      base = 8;
   while (*string != '\0')
   {
      if (*string < '0' || *string >= '0' + base)
         (void) gripe (raw_string,"Not an octal or decimal number.");
      value = base * value + (*string++) - '0';
   }
   return (sign * value);
}

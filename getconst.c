/* getconst.c (acdc) - copyleft Mike Arnautov 1990-2002.
 *
 * 10 Jan 02   MLA           Disallow LOCAL in constants.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#include <ctype.h>

#include "acdc.h"
#include "symbol.h"
#include "const.h"
#include "major.h"
#include "param.h"

#ifdef __STDC__
int getconst (
   char *string)
#else
int getconst (string)
   char *string;
#endif
{
   char *string_ptr;
   char token [161];
   char *token_ptr;
   int numbase;
   char oper;
   int value;
   int next_value;
   int digit;
   struct node *np;

   value = 0;
   string_ptr = string;
   oper = '+';

   if (*string_ptr == '+' || *string_ptr == '-')
      oper = *string_ptr++;

next_token:
   if (*string_ptr == '0')
      numbase = 8;
   else if (isdigit (*string_ptr))
      numbase = 10;
   else if (isalpha (*string_ptr))
      numbase = 0;
   else if (*string_ptr == '@' && isalpha (*(++string_ptr)))
      numbase = -1;
   else
      (void) gripe (string,"Not reducible to a constant value.");

   next_value = 0;
   if (numbase > 0)
      while (TRUE)
      {
         digit = *string_ptr - '0';
         if (digit < 0 || digit >= numbase)
            break;
         next_value = digit + next_value * numbase;
         string_ptr++;
      }
   else
   {
      token_ptr = token;
      while (isalnum (*string_ptr) || *string_ptr == '_' || *string_ptr == '.'
         || *string_ptr == '!' || *string_ptr == '?')
            *token_ptr++ = *string_ptr++;
      *token_ptr = '\0';
      if (fndparam (token) != -1)
         gripe (token, "cannot use local symbols in constant expressions!");
      np = fndsymb (SYMBOL, token);
      next_value = (numbase == 0) ? np -> refno : np -> state_count;
   }

   if (oper == '+')
      value += next_value;
   else
      value -= next_value;

   if (*string_ptr == '+' || *string_ptr == '-')
   {
      oper = *string_ptr++;
      goto next_token;
   }
   else if (*string_ptr != '\0')
      (void) gripe (string, "Bad operator in constant expression.");

   return (value);
}

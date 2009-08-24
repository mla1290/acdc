/* getconst.c (acdc) - copyleft Mike Arnautov 1990-2009.
 *
 * 10 Jan 02   MLA           Disallow LOCAL in constants.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include <ctype.h>

#include "acdc.h"
#include "symbol.h"
#include "const.h"
#include "major.h"
#include "param.h"

#ifdef __STDC__
int getconst (
   char *cstring)
#else
int getconst (cstring)
   char *cstring;
#endif
{
   char *cstring_ptr;
   char token [161];
   char *token_ptr;
   int numbase;
   char oper;
   int value;
   int next_value;
   int digit;
   struct node *np;

   value = 0;
   cstring_ptr = cstring;
   oper = '+';

   if (*cstring_ptr == '+' || *cstring_ptr == '-')
      oper = *cstring_ptr++;

next_token:
   if (*cstring_ptr == '0')
      numbase = 8;
   else if (isdigit (*cstring_ptr))
      numbase = 10;
   else if (isalpha (*cstring_ptr))
      numbase = 0;
   else if (*cstring_ptr == '@' && isalpha (*(++cstring_ptr)))
      numbase = -1;
   else
      gripe (cstring,"Not reducible to a constant value.");

   next_value = 0;
   if (numbase > 0)
      while (TRUE)
      {
         digit = *cstring_ptr - '0';
         if (digit < 0 || digit >= numbase)
            break;
         next_value = digit + next_value * numbase;
         cstring_ptr++;
      }
   else
   {
      token_ptr = token;
      while (isalnum (*cstring_ptr) || *cstring_ptr == '_' || *cstring_ptr == '.'
         || *cstring_ptr == '!' || *cstring_ptr == '?')
            *token_ptr++ = *cstring_ptr++;
      *token_ptr = '\0';
      if (fndparam (token) != -1)
         gripe (token, "cannot use local symbols in constant expressions!");
      np = fndsymb (SYMBOL, token);
      next_value = (numbase == 0) ? np -> refno : np -> state_count;
      (np -> used_count)++;
   }

   if (oper == '+')
      value += next_value;
   else
      value -= next_value;

   if (*cstring_ptr == '+' || *cstring_ptr == '-')
   {
      oper = *cstring_ptr++;
      goto next_token;
   }
   else if (*cstring_ptr != '\0')
      gripe (cstring, "Bad operator in constant expression.");

   return (value);
}

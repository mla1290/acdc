/* parse.c (acdc) - copyleft Mike Arnautov 1990-2004.
 *
 * 19 Aug 04   MLA           Added FREE_ARG.
 * 03 Mar 03   MLA           Use new-style get_token().
 * 06 Nov 01   MLA           Fiddle FULLDISP for Platt-style A-code.
 * 17 Oct 01   MLA           Improved compatibility with Platt's A-code.
 * 03 Dec 00   MLA           Allowed '#' as a comment delimiter.
 * 09 Aug 99   MLA           Allowed for \r in sources imported from Windows.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 26 Dec 90   MLA           Support for new CONSTANT etc syntax.
 * 15 Sep 90   MLA           Initial coding.
 *
 */
 
#include <string.h>

#include "acdc.h"
#include "const.h"
#include "line.h"
#include "symbol.h"
#include "major.h"
#include "text.h"

#ifdef __STDC__
struct node *parse(int type)
#else
struct node *parse(type)
int type;
#endif
{
   int index;
   struct node *np;
   extern char *get_token();
   char *cptr = line;
   int minargs;
   int direct_call = 0;

   (void) strcpy (raw_line, line);
   recase(LOWERCASE, line);
   tp [1] = NULL;
   if ((tp[0] = get_token(&cptr, " ,\n\0")) == NULL)
      (void) gripe ("","Null directive???");
   if (type != NONE)
   {
      if ((np = fndsymb(type, tp[0])) == NULL)
      {
         if (type != MINOR || 
            (np = fndsymb(SYMBOL_OR_CONSTANT, tp[0])) == NULL)
               return (NULL);
         direct_call = 1;
      }
      index = 0;
      if (direct_call == 0 && np -> body.directive.max_args == REST)
      {
         cptr = raw_line + (cptr - line);
         cptr += strspn (cptr, " ,\n\0");
         tp [1] = cptr;
         cptr += strlen(cptr) - 1;
         if (*cptr == '\n') *cptr = '\0';
         if (! *tp[1])
            (void) gripe (tp[0], "Missing \"rest of line\" argument.");
         return (np);
      }
   }
   while (cptr && (tp[++index] = get_token(&cptr," ,\n\0\r")) != NULL)
   {
      if (style == 1)
      {
         if (strcmp (tp[index], "object") == 0) 
            tp[index] = "objflag";
         else if (strcmp (tp[index], "place") == 0) 
            tp[index] = "placeflag";
         else if (strcmp (tp[index], "verb") == 0) 
           tp[index] = "verbflag";
         else if (strcmp (tp[index], "variable") == 0) 
            tp[index] = "varflag";
         else if (strcmp (tp[index], "fulldisp") == 0) 
            tp[index] = "fulldisplay";
      }
      if (index > ANY_NUMBER)
          (void) gripe ("","*Far* too many arguments!");
      if (! *tp[index] || *tp[index] == '{' || *tp[index] == '#')
      {
         tp[index] = NULL;
         break;
      }
   }

   if (type == NONE)
      return NULL;
   if (direct_call == 0)
   {
      index--;
      minargs = np -> body.directive.min_args;
      if (minargs >= FREE_ARG)
         minargs -= FREE_ARG - 1;
      if (index < minargs)
         (void) gripe (tp[0],"Not enough arguments.");
      if (index > np -> body.directive.max_args)
         (void) gripe (tp[0],"Too many arguments.");
   }
   return (np);
}

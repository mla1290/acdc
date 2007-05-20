/* param.c (acdc) - copyleft Mike Arnautov 1990-2007.
 *
 * 06 Mar 03   Stuart Munro  Include stdlib.h; declare gripe().
 * 13 Jan 02   MLA           Give up -- pass "hidden" type args.
 * 12 Jan 02   MLA           bug: construct own parameter names.
 * 10 Jan 02   MLA           BUG: sundry fixes.
 * 30 Dec 01   MLA           Initial coding.
 *
 */
 
#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include <string.h>
#include <stdlib.h>

#include "output.h"
#include "param.h"

struct param_list *param_root = NULL;

#ifdef __STDC__
void gripe (char *token, char *message);

void addparam (
   int   arg,
   char *name)
   
#else
void gripe ();

void addparam (arg, name)
int   arg;
char *name;

#endif /* __STDC__ */

{
   struct param_list *ppl = param_root;
   struct param_list *lppl = NULL;

   while (ppl && ppl -> in_use)
   {
      if (strcmp (name, ppl -> name) == 0)
         gripe (name, "Duplicate call parameter.");
      lppl = ppl;
      ppl = ppl -> next;
   }
   if (ppl == NULL)
   {
      if ((ppl = (struct param_list *)malloc 
         (sizeof (struct param_list))) == NULL)
            gripe (name, "Unable to allocate parameter node.");
      if (lppl)
         lppl -> next = ppl;
      else
         param_root = ppl;
      ppl -> next = NULL;
   }
   ppl -> in_use = 1;
   strncpy (ppl -> name, name, 32);
   ppl -> arg = arg;
   (ppl -> name)[31] = '\0';

   return;
}

#ifdef __STDC__

int fndparam (
   char *name)
   
#else

int fndparam (name)
char *name;

#endif /* __STDC__ */

{
   int lrefno = 0;
   struct param_list *ppl = param_root;

   while (ppl && ppl -> in_use)
   {
      if (strcmp (ppl -> name, name) == 0)
         return (lrefno);
      ppl = ppl -> next;
      lrefno++;
   }
   return (-1);
}

#ifdef __STDC__

int iniparam (
   int varsize)

#else

int iniparam (varsize)
int varsize;

#endif /* __STDC__ */

{
   int locals = 0;
   struct param_list *ppl = param_root;
   
   while (ppl && ppl -> in_use)
   {
      locals++;
      ppl = ppl -> next;
   }
   if (locals)
   {
      (void) fprintf (code_file, "   int lval [%d];\n", locals);
      (void) fprintf (code_file, "   short lbts [%d];\n", locals * varsize);
   }

   ppl = param_root;
   locals = 0;
   while (ppl && ppl -> in_use)
   {
      (void) fprintf (code_file, "   lval[%d] = ", locals);
      if (ppl -> arg)
         (void) fprintf (code_file, "par%d;\n", locals);
      else
         (void) fprintf (code_file, "0;\n");
      (void) fprintf (code_file, "   lbts [%d] = ", locals * varsize);
      if (ppl -> arg)
         (void) fprintf (code_file, "typ%d;\n", locals);
      else
         (void) fprintf (code_file, "0;\n");
      ppl = ppl -> next;
      locals++;
   }
   return (locals);
}   

void zapparam (void)
{
   struct param_list *ppl = param_root;
   
   while (ppl && ppl -> in_use)
   {
      ppl -> in_use = 0;
      ppl = ppl -> next;
   }
}


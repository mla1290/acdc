/* addsymb.c (acdc) - copyleft Mike Arnautov 1990-2005.
 *
 * 07 Jan 03   MLA           Use btree instead of tsearch.
 * 24 Jul 01   MLA           Added ANSI C declaration.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#include <string.h>

#include "acdc.h"
#include "symbol.h"
#include "btree.h"

#ifdef __STDC__
struct node *addsymb (
   int node_type,
   char *word,
   int type,
   int refno)
#else
struct node *addsymb (node_type, word, type, refno)
   int node_type;
   char *word;
   int type;
   int refno;
#endif
{
   struct node *node_ptr;
   int len;
   extern void *malloc ();

   if ((node_ptr = (struct node *) malloc (sizeof (struct node))) == NULL)
      (void) gripe (word, "Unable to allocate memory.");

   len = strlen (word);
   if ((node_ptr -> name = (char *) malloc (len + 1)) == NULL)
      (void) gripe (word, "Unable to allocate storage.");
   (void) strcpy (node_ptr -> name, word);
   node_ptr -> type = type;
   node_ptr -> refno = refno;
   node_ptr -> body.text.text_addr[0] = -1;
   node_ptr -> body.text.text_addr[1] = -1;
   node_ptr -> body.text.text_addr[2] = -1;
   node_ptr -> body.text.name_addr = -1;
   node_ptr -> used_count = 0;
   node_ptr -> head = NULL;
   node_ptr -> tail = NULL;
   if (btadd (node_type, node_ptr) == 0)
      (void) gripe (word, "Symbol already defined.");
   return (node_ptr);
}

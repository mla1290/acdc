/* addsymb.c (acdc) - copyleft Mike Arnautov 1990-2002.
 *
 * 24 Jul 01   MLA           Added ANSI C declaration.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#include <string.h>

#include "acdc.h"
#include "symbol.h"

#ifdef __STDC__
struct node *addsymb (
   int root_type,
   char *word,
   int type,
   int refno)
#else
struct node *addsymb (root_type, word, type, refno)
   int root_type;
   char *word;
   int type;
   int refno;
#endif
{
   struct node *node_ptr;
   struct node *ret_ptr;
   int add_compare ();
   int len;
   extern void *malloc ();
   extern void *tsearch ();

   if ((node_ptr = (struct node *) malloc (sizeof (struct node))) == NULL)
      (void) gripe (word, "Unable to allocate memory.");

   len = strlen (word) + 1;
   if ((node_ptr -> name = (char *) malloc (len)) == NULL)
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
   ret_ptr = *(struct node **)
      tsearch ((char *) node_ptr, &root [root_type], add_compare);
   if (node_ptr != ret_ptr)
      (void) gripe (word, "Symbol already defined.");
   return (ret_ptr);
}

#ifdef __STDC__
int add_compare (
   struct node *node1,
   struct node *node2)
#else
int add_compare (node1, node2)
   struct node *node1;
   struct node *node2;
#endif
{
   char *n1;
   char *n2;

   if (*(n1 = node1 -> name) == '!')
      n1++;
   if (*(n2 = node2 -> name) == '!')
      n2++;

   return (strcmp (n1, n2));
}


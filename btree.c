/* btree.c (acdc) - copyleft Mike Arnautov, 2001-2004.
 *
 * Balanced tree handling on a linear stack.
 *
 * 06 Mar 03   Stuart Munro      Fix non-ASCII btfind args declaration;
 *                               include stdlib.h; remove unused variables.
 * 07 Jan 03   MLA               Adapted for use by acdc.
 * 24 Mar 01   MLA               Initial coding.
 */
 
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "text.h"
#include "const.h"
#include "symbol.h"
#include "btree.h"

#define BT_LSIB   -1
#define BT_UP      1
#define BT_RSIB    1
#define BT_BAL     3
#define BT_PTR     4
#define BT_STP  4096

/*====================================================================*/

#ifdef __STDC__
static void sing_rot (int *root, int parent, int child, int dir)
#else /* ! __STDC__ */
static void sing_rot (root, parent, child, dir)
int *root;
int parent;
int child;
int dir;
#endif /* __STDC__ */
{
   int gparent = *(root + parent + BT_UP);
   int gchild = *(root + child + BT_UP - dir);
   
   *(root + parent + BT_UP + dir) = *(root + child + BT_UP - dir);
   *(root + child + BT_UP - dir) = parent;
   *(root + parent + BT_BAL) -= *(root + child + BT_BAL);
   *(root + child + BT_BAL) = -(*(root + parent + BT_BAL));
   if (gparent > 0)
      *(root + gparent + BT_UP +
         (*(root + gparent + BT_UP + BT_LSIB) == parent ? BT_LSIB : BT_RSIB)) =
            child;
   else
      *(root + 1) = child;
   *(root + child + BT_UP) = gparent;
   *(root + parent + BT_UP) = child;
   if (gchild)
      *(root + gchild + BT_UP) = parent;
}

/*--------------------------------------------------------------------*/

#ifdef __STDC__
static void dub_rot (int *root, int parent, int child, int dir)
#else /* ! __STDC__ */
static void dub_rot (root, parent, child, dir)
int *root;
int parent;
int child;
int dir;
#endif /* __STDC__ */
{
   int gparent = *(root + parent + BT_UP);
   int gchild = *(root + child + BT_UP - dir);

   *(root + child  + BT_UP - dir) = *(root + gchild + BT_UP + dir);
   if (*(root + gchild + BT_UP + dir))
      *(root + *(root + gchild + BT_UP + dir) + BT_UP) = child;
   *(root + parent + BT_UP + dir) = *(root + gchild + BT_UP - dir);
   if (*(root + gchild + BT_UP - dir))
      *(root + *(root + gchild + BT_UP - dir) + BT_UP) = parent;
   *(root + gchild + BT_UP + dir) = child;
   *(root + gchild + BT_UP - dir) = parent;

   if (*(root + gchild + BT_BAL) == *(root + child + BT_BAL))
      *(root + child + BT_BAL) *= -1;
   else
      *(root + child + BT_BAL) = 0;
   if (*(root + gchild + BT_BAL) == *(root + parent + BT_BAL))
      *(root + parent + BT_BAL) *= -1;
   else
      *(root + parent + BT_BAL) = 0;
   *(root + gchild + BT_BAL) = 0;

   *(root + child + BT_UP) = gchild;
   *(root + parent + BT_UP) = gchild;
   *(root + gchild + BT_UP) = gparent;
   if (gparent > 0)
      *(root + gparent + BT_UP +
         (*(root + gparent + BT_UP + BT_LSIB) == parent ? BT_LSIB : BT_RSIB)) =
            gchild;
   else
      *(root + 1) = gchild;
}

/*====================================================================*/

#ifdef __STDC__
int btcmpa (struct node *itemptr, struct node *nodeptr)
#else /* ! __STDC__ */
int btcmpa (itemptr, nodeptr)
struct node *itemptr;
struct node *nodeptr;
#endif /* __STDC__ */
{
   char *n1;
   char *n2;
   int result;

   if (*(n1 = itemptr -> name) == '!')
      n1++;
   if (*(n2 = nodeptr -> name) == '!')
      n2++;

   result = strcmp (n1, n2);
   if (result)
      return (result > 0 ? 1 : -1);
   else
      return (0);
}

/*--------------------------------------------------------------------*/

#ifdef __STDC__
int btcmpf (char *word, struct node *nodeptr)
#else /* ! __STDC__ */
int btcmpf (word, nodeptr)
char *word;
struct node *nodeptr;
#endif /* __STDC__ */
{
   char *name;
   int result;

   if (*(name = nodeptr -> name) == '!')
      name++;
   result =  style == 1 ? strncmp (word, name, 12) : strcmp (word, name);
   if (result)
      return (result > 0 ? 1 : -1);
   else
      return (0);
}

/*====================================================================*/

#ifdef __STDC__
int *btinit (int *root)
#else /* ! __STDC__ */
int *btinit (root)
int *root;
#endif /* __STDC__ */
{
   if (root == NULL)
   {
      if ((root = (int *)malloc(BT_STP * sizeof(int))) == NULL)
         return (NULL);
      *root = 3;
      *(root + 1) = 0;
      *(root + 2) = BT_STP;
   }
   else
   {
      if ((root = (int *)realloc 
         (root, (*(root + 2) + BT_STP) * sizeof(int))) == NULL)
            return (NULL);
      *(root + 2) += BT_STP;
   }
   return (root);
}

/*--------------------------------------------------------------------*/

#ifdef BT_DEBUG

#ifdef __STDC__
void btshow (int *root, struct node *nodeptr)
#else /* ! __STDC__ */
void btshow (root, text)
int *root;
char *text;
#endif /* __STDC__ */
{
   int *iptr;

   printf ("Show: %s, free: %hd, root: %hd\n", text, *root, *(root + 1));
   iptr = root + 3;
   while (iptr < root + *root)
   {
      fprintf (stderr, "Offset %d: Up %hd, L %hd, R %hd, B %hd, T: %s\n",
         iptr - root, *(iptr + BT_UP), *(iptr + BT_UP + BT_LSIB), 
            *(iptr + BT_UP + BT_RSIB), *(iptr + BT_BAL), 
               nodeptr -> name);
      iptr += BT_PTR + 1 + (sizeof (int *) - 1) / sizeof (int);
   }
}

#endif /* BT_DEBUG */

/*--------------------------------------------------------------------*/

#ifdef __STDC__
int btadd (int type, void *record)
#else /* ! __STDC__ */
int btadd (type, record)
int type;
char *record;
#endif /* __STDC__ */
{
   int *root = roots [type];
   int parent = 0;
   int child = *(root + 1);
   int dir;
   int *newrec;
   int reclen = BT_PTR  + 1 + (sizeof (record) - 1) / sizeof (int);

   if (*(root + 1) > 0)
   {
      while (child > 0)
      {
         if ((dir = btcmpa (record, 
            (struct node *)(*(root + child + BT_PTR)))) == 0)
               return (0);
         parent = child;
         child = *(root + child + BT_UP + dir);
      }
   }
   if (*root + reclen > *(root + 2) && 
      (root = roots [type] = btinit (root)) == NULL)
         return (-1);
   newrec = root + (child = *root);
   *(newrec + BT_UP) = parent;
   *(newrec + BT_UP + BT_LSIB) = *(newrec + BT_UP + BT_RSIB) = 0;
   *(newrec + BT_BAL) = 0;
   memcpy ((char *)(newrec + BT_PTR), &record, sizeof (record));
   *root += reclen;
   if (parent)
   {
      *(root + parent + BT_UP + dir) = child;
      while (1)
      {
         dir = *(root + parent + BT_UP + BT_LSIB) == child ? BT_LSIB : BT_RSIB;
         if (*(root + parent + BT_BAL) == dir)
         {
            if (*(root + child + BT_BAL) == -dir)
               dub_rot (root, parent, child, dir);
            else 
               sing_rot (root, parent, child, dir);
            break;
         }
         if ((*(root + parent + BT_BAL) += dir) == 0)
            break;
         child = parent;
         parent = *(root + parent + BT_UP);
         if (parent == 0) break;
      }
   }
   else
      *(root + 1) = child;
   return (1);
}

/*--------------------------------------------------------------------*/

#ifdef __STDC__
struct node *btfind (int type, char *word)
#else /* ! __STDC__ */
struct node *btfind (type, word)
int type;
char *word;
#endif /* __STDC__ */
{
   int node;
   int dir;
   int *root = roots [type];
   
   if ((node = *(root + 1)) == 0)
      return (NULL);
   while (node)
   {
      if ((dir = btcmpf (word, (struct node *)(*(root + node + BT_PTR)))) == 0)
            return ((struct node *)(*(root + node + BT_PTR)));
      node = *(root + node + BT_UP + dir);
   }
   return (NULL);
}

/*====================================================================*/

#ifdef __STDC__
void btspan (int type, void (*btact)())
#else /* ! __STDC__ */
void btspan (type, btact)
int type;
void (*btact)();
#endif /* __STDC__ */
{
   int state = 0;
   int *root = roots [type];
   int node = *(root + 1);
   int next;
   
   if (node == 0)
      return;
      
   while (node)
   {
      switch (state)
      {
         case 0:
            if (next = *(root + node + BT_UP + BT_LSIB))
               node = next;
            else 
               state = (*(root + node + BT_UP + BT_RSIB)) ? 1 : 2;
            break;
            
         case 1:
            btact ((char *)(*(root + node + BT_PTR)));
            state = (next = *(root + node + BT_UP + BT_RSIB)) ? 0 : 3;
            if (state == 0)
               node = next;
            break;

         case 2:
            btact ((char *)(*(root + node + BT_PTR)));  
               /* And just fall through! */
            
         case 3:
            if (next = *(root + node + BT_UP))
               state = (*(root + next + BT_UP + BT_LSIB) == node) ? 1 : 3;
            node = next;
            break;
      }
   }
}

/*====================================================================*/

/* symbol.h (acdc) - copyleft Mike Arnautov 1990-2005.
 *
 * 03 Sep 06   MLA           Bug: longs should be ints.
 * 23 Dec 05   MLA           Bug: roots[] should be long, not int!
 * 15 Jan 05   MLA           Added the 'auto' flag.
 * 09 Jan 01   MLA           Allowed for active text types.
 * 13 Mar 94   MLA           Made conditional.
 * 15 Sep 90   MLA           Initial coding 
 *
 */

#ifndef SYMBOL_DONE

#  define SYMBOL_DONE

#  ifndef NULL
#     include <stdio.h>
#  endif

   extern int vocab_count;
   extern int type_counts[];
   extern int type_base[];
   struct proc_list
   {
      int procno;
      struct proc_list *next;
   } ;
      
   struct node 
   {
      char *name;
      int type;            /* Type of node */
      int refno;              /* Node id */
      struct proc_list *head;
      struct proc_list *tail;
      int state_count;     /* Number of states for objects, places and texts */
      int used_count;
      char auto_flag;
      union
      {
         struct
         {
            int text_addr[3];      /* Text address */
            int name_addr;         /* OBJECT/PLACE/VERB name or TEXT text */
            int text_type;         /* Types like plain, auto-increment, random... */
         } text;               /* Text pointers */
         struct
         {
            int voc_addr;         /* Vocabulary word address */
            int word_addr;        /* Voc. full word address */
            int word_types;       /* Type mask (noun, verb, adjective...) */
         } vocab;              /* Vocabulary words */
         struct
         {
            int min_args;          /* Minimal number of arguments */
            int max_args;          /* Maximal number of arguments */
         } directive;          /* Directives */
      } body;
   } ;

   extern int *roots[];

#ifdef __STDC__
   extern struct node *fndsymb(int, char *);
   extern struct node *addsymb(int, char *, int, int);
#else
   extern struct node *fndsymb();
   extern struct node *addsymb();
#endif

   extern int flag_field_size[];

#endif /* SYMBOL_DONE */

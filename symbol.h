/* symbol.h (acdc) - copyleft Mike Arnautov 1990-2003.
 *
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
      union
      {
         struct
         {
            long text_addr[3];      /* Text address */
            long name_addr;         /* OBJECT/PLACE/VERB name or TEXT text */
            int text_type;          /* Types like plain, auto-increment, random... */
         } text;               /* Text pointers */
         struct
         {
            long voc_addr;         /* Vocabulary word address */
            long word_addr;        /* Voc. full word address */
         } vocab;              /* Vocabulary words */
         struct
         {
            int min_args;          /* Minimal number of arguments */
            int max_args;          /* Maximal number of arguments */
         } directive;          /* Directives */
      } body;
   } ;

   extern int *roots[];

   extern struct node *fndsymb();
   extern struct node *addsymb();

   extern int flag_field_size[];

#endif /* SYMBOL_DONE */

/* Balanced tree handling on a linear stack.
 *
 * 07 Mar 03   Stuart Munro.     Fixed btfind() type for non-ANSI compilers.
 * 07 Jan 03   MLA               Adapted for use by acdc.
 * 24 Mar 01   MLA               Initial coding.
 */

#ifndef __BTREE__

#  define __BTREE__

#  ifdef __STDC__

      int *btinit (int *root);
      int btadd (int type, void *record);
      struct node *btfind (int type, char *word);
      void btspan (int type, void (*btact)());

#  else /* ! __STDC__ */

      int *btinit ();
      int btadd ();
      struct node *btfind ();
      void btspan ();

#  endif /* __STDC__ */

#endif /* __BTREE__ */


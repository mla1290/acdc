/* Balanced tree handling on a linear stack.
 *
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
#     ifdef BT_DEBUG
         void btshow (int *root, char *label);
#     endif /* BT_DEBUG */

#  else /* ! __STDC__ */

      int *btinit ();
      int btadd ();
      char *btfind ();
      void btspan ();
#     ifdef BT_DEBUG
         void btshow ();
#     endif /* BT_DEBUG */

#  endif /* __STDC__ */

#endif /* __BTREE__ */


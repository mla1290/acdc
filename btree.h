/* btree.c - balanced tree handling on a linear stack.
 *           Copyleft Mike Arnautov 2001-2006.
 *
 * Warning! This is 32bit code, for use in 32bit executables!
 *
 * Balanced tree handling on a linear stack.
 *
 * 03 Sep 06   MLA               Bug: longs should be ints.
 * 23 Dec 05   MLA               Bug: btinit() should be long, not int!
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


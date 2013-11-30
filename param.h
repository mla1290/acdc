/* param.h (acdc) - copyleft Mike Arnautov 1990-2013.
 *
 * 30 Dec 01   MLA          Initial coding.
 *
 */
 
#ifndef PARAM_DONE

#  define PARAM_DONE

   struct param_list
   {
      int in_use;
      int arg;
      char name [32];
      struct param_list *next;
   };
   
#  ifdef __STDC__
   extern void zapparam (void);
   extern void addparam (int, char *name);
   extern int iniparam (int);
   extern int  fndparam (char *name);
#  else
   extern void zapparam ();
   extern void addparam ();
   extern int iniparam ();
   extern int  fndparam ();
#  endif
   
#endif /* PARAM_DONE */

/* acdc.h (acdc) - copyleft Mike Arnautov 1990-2007.
 */

extern char author [40];
extern char datbuf [];
extern int quiet;
#ifdef __STDC__
#include <malloc.h>
#include <stdlib.h>
extern int getline (int key);
extern int gettxt (int descript, int *max_states, int frag, int *got_holder);
extern void gripe (char *error_token, char *error_message);
extern void deprecate (const char *directive, int from, int major);
extern void storchar (int ch);
extern void doswitch (char *text_ptr, int *max_states);
extern void write_ref (char *tag, char *token);
#ifdef EOF
   extern FILE *openout(char *name, char *mode);
   extern void openfrst (char *file_spec);
#endif /* EOF */
#else  /* ! __STDC__ */
extern int getline ();
extern void gripe ();
extern void deprecate();
extern int gettxt ();
extern void storchar ();
extern void doswitch ();
extern write_ref ();
extern void *calloc ();
extern void *malloc ();
extern void *realloc ();
#ifdef EOF
   extern FILE *openout ();
   extern void openfrst ();
#endif /* EOF */
#endif /* ! __STDC__ */

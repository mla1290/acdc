/* acdc.h (acdc) - copyleft Mike Arnautov 1990-2009.
 */

extern char author [40];
extern char datbuf [];
extern int quiet;
extern int stage;
extern int swap;
extern char *acdc_version;
#ifdef __STDC__
#include <malloc.h>
#include <stdlib.h>
extern int getline (int key);
extern int gettxt (int description, int *max_states, int *text_type);
extern void gripe (char *error_token, char *error_message);
extern void deprecate (const char *directive, int from, int major);
extern void storchar (int ch);
extern void doswitch (char *text_ptr, int *max_states, int cycle);
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

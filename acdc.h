/* acdc.h (acdc) - copyright Mike Arnautov 1990-2015.
 * Licensed under the Modified BSD Licence (see the supplied LICENCE file).
 */

extern char author [40];
extern char datbuf [];
extern int quiet;
extern int stage;
extern int swap;
extern char *acdc_version;
#ifdef __STDC__
#ifndef __MACH__
#include <malloc.h>
#endif /* __MACH__ */
#include <stdlib.h>
extern int nextline (int key);
extern int gettxt (int description, int *max_states, int *text_type);
extern void gripe (char *error_token, char *error_message);
extern void deprecate (const char *directive, int from, int major);
extern void storchar (int ch);
extern void doswitch (char *text_ptr, int *max_states, int cycle);
extern void write_ref (char *tag, char *token);
#ifdef EOF
   extern FILE *openout(char *name);
   extern void openfrst (char *file_spec);
#endif /* EOF */
#else  /* ! __STDC__ */
extern int nextline ();
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

/* acdc.h (acdc) - copyleft Mike Arnautov 1990-2004.
 */

extern char author [40];
extern char datbuf [];
extern int no_warn;
#ifdef __STDC__
extern int getline (int key);
extern int gettxt (int descript, int *max_states, int frag, int *got_holder);
extern void gripe (char *error_token, char *error_message);
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
extern int gettxt ();
extern void storchar ();
extern void doswitch ();
extern write_ref ();
#ifdef EOF
   extern FILE *openout ();
   extern void openfrst ();
#endif /* EOF */
#endif /* ! __STDC__ */

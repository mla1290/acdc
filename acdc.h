/* acdc.h (acdc) - copyleft Mike Arnautov 1990-2003.
 */

#ifdef COPYLEFT
   extern char datbuf [];
#endif /* COPYLEFT */
#ifdef __STDC__
extern int getline (int key);
extern void gripe (char *error_token, char *error_message);
extern int gettext (int description, int *max_states, int fragment);
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
extern int gettext ();
extern void storchar ();
extern void doswitch ();
extern write_ref ();
#ifdef EOF
   extern FILE *openout ();
   extern void openfrst ();
#endif /* EOF */
#endif /* ! __STDC__ */

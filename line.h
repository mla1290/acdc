/* line.h (acdc) - copyright Mike Arnautov 1990-2015.
 * Licensed under the Modified BSD Licence (see the supplied LICENCE file).
 *
 * 15 Mar 08   MLA           Version 12 changes.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

extern int line_status;
extern char line[];
extern char raw_line[];
extern char *line_ptr;
extern char *tp[];

#ifdef __STDC__
struct node *parse (int);
int chrtobin (char *);
int texttyp (void);
#else
struct node *parse ();
int chrtobin ();
int texttyp ();
#endif

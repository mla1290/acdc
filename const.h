/* const.h (acdc) - copyleft Mike Arnautov 1990-2013.
 *
 * 15 Mar 08   MLA           Version 12 changes.
 * 15 Jan 05   MLA           Added AUTOSYMBOL.
 * 19 Aug 04   MLA           Added FREE_ARG.
 * 03 Mar 03   MLA           Added REST.
 * 15 Sep 90   MLA           Initial coding.
 *
 */
 
/* Change_case keys */

#ifdef __STDC__
extern void recase (int, char *);
extern int getconst(char *);
#else
extern void recase ();
extern int getconst ();
#endif

#define CAPITALISE 1
#define UPPERCASE 2
#define LOWERCASE 4

/* Function return values */

#define OK 0
#define ERROR 1

/* Logicals */

#define FALSE 0
#define TRUE -1

/* End of list marker */

#define ENDOFLIST -1

/* Buffer sizes. */

#define MAXTOKEN 32
#define MAXLINE 132

/* Line status values. */

# define BOL 0
# define SOL 1
# define MOL 2
# define EOL 3

/* Get_line keys. */

#define IGNORE_BLANK 0
#define ACCEPT_BLANK 1

/* Maximal nesting of includes. */

#define MAXLEVEL 10

/* A large number of args. */

#define ANY_NUMBER 99

/* Rest of the line. */

#define REST         -1
#define FREE_ARG   1000

/* Size of a code chunk. */

#define CODE_CHUNK 2000

/* Predefined common flags. LAST_DAFAULT_FLAG specifies the last flag
 * taken by default flags and hence not available for object and place 
 * bit fields. Note that it should be equal VERBFLAG, since VARFLAG is 
 * *only* used by the FLAGS directive in definitions and should *not* be 
 * used in the A-code source.
 */

#define OBJFLAG 0
#define LOCFLAG 1
#define VERBFLAG 2
#define VARFLAG 3
#define LAST_DEFAULT_FLAG 2

/* major.h (acdc) - copyleft Mike Arnautov 1990-2004.
 *
 * 03 Mar 03   MLA           Added AUTHOR.
 * 10 Mar 02   MLA           Added XREF/NOXREF.
 * 21 Dec 01   MLA           Tidied up.
 * 14 Mar 01   MLA           Added STYLE directive.
 * 31 Dec 98   MLA           Added ARRAY type. 
 * 15 Sep 90   MLA           Initial coding.
 */

/* First the major data types -- the real ones */

#define OBJECT       0
#define PLACE        1
#define VERB         2
#define VARIABLE     3
#define TEXT         4
#define PROCEDURE    5
#define CONSTANT     6
#define MAXTYPES  CONSTANT   /* In case we ever need to define more of them */

/* Next lot are data psudo-types, i.e. special cases of the real ones */

#define FLAGS       10         /* A kind of constant */
#define STATE       11         /* Ditto */
#define FRAGMENT    12         /* A kind of text */
#define ARRAY       13         /* A kind of variable */
#define SYNONYM     14         /* An additional name for name or place */
#define NOUN        15         /* A kind of non-verb word */
#define ADJECTIVE   16         /* Ditto */
#define PREPOSITION 17         /* Ditto */
#
/* The rest are compilation directives */

#define ACTION      21
#define AT          22
#define INIT        23
#define REPEAT      24
#define NOISE       25
#define LIST        26
#define NOLIST      27
#define XREF        28
#define NOXREF      29
#define INCLUDE     30
#define GAMEID      31
#define DEFINE      32
#define STYLE       33
#define AUTHOR      34

/*====================================================================*/

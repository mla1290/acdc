/* initial.c (acdc) - copyleft Mike Arnautov 1990-2008.
 *
 * 02 May 08   MLA           Allow for global flags instead of var flags.
 * 15 Mar 08   MLA           Version 12 changes.
 * 07 Mar 08   MLA           Removed LIST, NOLIST, XREF and NOXREF.
 * 10 Nov 07   MLA           Added DUMP.
 * 19 May 07   MLA           Added "quiet".
 * 09 May 07   MLA           Code parts now start from 2!
 * 07 May 07   Stuart Munro  bug: need stdlib.h (for rand()).
 * 08 Sep 05   MLA           IFIS now takes multiple arguments.
 * 27 Aug 05   MLA           Retrofitted GAMED for style 10.
 * 20 Feb 05   MLA           Added separate VERSION and DATE.
 * 20 Aug 04   MLA           Added IFCGI.
 * 14 Aug 04   MLA           Added SAVE/RESTORE and VERBATIM.
 * 09 Feb 04   MLA           Added ADJECTIVE and PREPOSITION types.
 * 03 Feb 04   MLA           Added FEATURE.
 * 03 Mar 03   MLA           Added AUTHOR.
 * 20 Feb 03   MLA           Chage to code file naming conventions.
 * 27 Jul 02   MLA           Added CHECKPOINT.
 * 12 Jul 02   MLA           Added IFNE, IFLE, IFGE.
 *                           Allowed multiple locations with IFLOC.
 *                           Added DESCRIBE.
 * 01 Jan 02   MLA           Added LOCAL directive.
 * 18 Nov 01   MLA           Tag autop files with 2 digits.
 * 17 Oct 01   MLA           Improved compatibility with Platt's A-code.
 * 01 Jul 01   MLA           IFAT now takes multiple agruments.
 * 17 May 01   MLA           Added FAKECOM directive.
 * 16 Mar 01   MLA           Added ANYKEY. Allowed multiple args with IFKEY.
 * 14 Mar 01   MLA           Replaced old_style with style.
 * 05 Mar 01   MLA           Added FAKEARG directive.
 * 28 Feb 01   MLA           Added TIE directive.
 * 08 Aug 00   MLA           Added OTHERWISE directive.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 05 Apr 99   MLA           Added the DICT directive.
 * 27 Mar 99   MLA           Added the INTERSECT directive.
 * 01 Apr 94   MLA           Allowed range specification with ITOBJ/ITLOC.
 * 20 Mar 94   MLA           Suppressed source tracing of declarations.
 * 12 Dec 91   MLA           FRAGMENT major directive. Optional arg to INPUT.
 * 10 Mar 91   MLA           Changed STOPALL to FLUSH.
 * 23 Feb 91   MLA           Added predeclared common flag bits.
 * 30 Dec 90   MLA           Allocated vocab buffer.
 * 26 Dec 90   MLA           STATE and FLAG major directives.
 * 14 Dec 90   MLA           Allowed null keyword lists for (S)MOVE.
 * 12 Dec 90   MLA           New minor directive QUIP.
 * 17 Nov 90   MLA           Introduced NOVARARGS compilation symbol.
 *                           to cope with a nasty Ultrix compiler.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include <stdio.h>
#include <string.h>

#include "acdc.h"
#include "const.h"
#include "minor.h"
#include "major.h"
#include "line.h"
#include "game.h"
#include "symbol.h"
#include "text.h"
#include "output.h"

struct directive
{
   char *name;
   int type;
   int id;
   int min_args;
   int max_args;
} ;

struct directive *dp;
struct node *np;
struct directive keywords[] =
{
   {"object",      MAJOR, OBJ,         1,   ANY_NUMBER},
   {"location",    MAJOR, LOC,         1,   ANY_NUMBER},
   {"place",       MAJOR, LOC,         1,   ANY_NUMBER}, /* Synonym */
   {"variable",    MAJOR, VAR,         1,   ANY_NUMBER},
   {"var",         MAJOR, VAR,         1,   ANY_NUMBER},
   {"array",       MAJOR, ARRAY,       2,   2},
   {"verb",        MAJOR, VERB,        1,   ANY_NUMBER},
   {"noun",        MAJOR, NOUN,        1,   ANY_NUMBER},
   {"adjective",   MAJOR, ADJECTIVE,   1,   ANY_NUMBER},
   {"preposition", MAJOR, PREPOSITION, 1,   ANY_NUMBER},
   {"text",        MAJOR, TEXT,        0,   3},
   {"fragment",    MAJOR, FRAGMENT,    0,   2},
   {"procedure",   MAJOR, PROCEDURE,   1,   ANY_NUMBER},
   {"proc",        MAJOR, PROCEDURE,   1,   ANY_NUMBER},
   {"label",       MAJOR, PROCEDURE,   1,   ANY_NUMBER},  /* Compatibility */
   {"action",      MAJOR, ACTION,      1,   ANY_NUMBER},
   {"at",          MAJOR, AT,          1,   1},
   {"init",        MAJOR, INIT,        0,   0},
   {"initial",     MAJOR, INIT,        0,   0},
   {"initialise",  MAJOR, INIT,        0,   0},
   {"repeat",      MAJOR, REPEAT,      0,   0},
   {"constant",    MAJOR, CONSTANT,    1,   ANY_NUMBER},
   {"flags",       MAJOR, FLAGS,       0,   1},
   {"state",       MAJOR, STATE,       1,   ANY_NUMBER},
   {"noise",       MAJOR, NOISE,       1,   ANY_NUMBER},
   {"null",        MAJOR, NOISE,       1,   ANY_NUMBER},
   {"include",     MAJOR, INCLUDE,     1,   1},
   {"include?",    MAJOR, CONCLUDE,    1,   1},
   {"synonym",     MAJOR, SYNONYM,     1,   ANY_NUMBER},  /* Compatibility */
   {"synon",       MAJOR, SYNONYM,     1,   ANY_NUMBER},  /* Compatibility */
   {"define",      MAJOR, DEFINE,      1,   ANY_NUMBER},  /* Compatibility */
   {"name" ,       MAJOR, NAME  ,      1,   REST},
   {"title",       MAJOR, NAME,        1,   REST},
   {"dbname",      MAJOR, NAME,        1,   1},           /* Compatibility */
   {"version",     MAJOR, VERSION,     1,   REST},
   {"date",        MAJOR, DATE,        1,   REST},
   {"gameid",      MAJOR, GAMEID,      1,   REST},        /* Compatibility */
   {"author",      MAJOR, AUTHOR,      1,   REST},
   {"style",       MAJOR, STYLE,       1,   2},
   {"have",        MINOR, HAVE,        1,   ANY_NUMBER},
   {"near",        MINOR, NEAR,        1,   ANY_NUMBER},
   {"here",        MINOR, HERE,        1,   ANY_NUMBER},
   {"at",          MINOR, ATLOC,     1,   ANY_NUMBER},
#ifndef NOVARARGS
   {"anyof",       MINOR, ANYOF,       1,   ANY_NUMBER},
   {"keyword",     MINOR, KEYWORD,     1,   ANY_NUMBER},
#else
   {"anyof",       MINOR, ANYOF,       1,   16},
   {"keyword",     MINOR, KEYWORD,     1,   16},
#endif
   {"ifeq",        MINOR, IFEQ,        2,   2},
   {"ifne",        MINOR, IFNE,        2,   2},
   {"iflt",        MINOR, IFLT,        2,   2},
   {"ifle",        MINOR, IFLE,        2,   2},
   {"ifgt",        MINOR, IFGT,        2,   2},
   {"ifge",        MINOR, IFGE,        2,   2},
   {"ifat",        MINOR, IFAT,        1,   ANY_NUMBER},
   {"chance",      MINOR, CHANCE,      1,   1},
   {"ifhave",      MINOR, IFHAVE,      1,   2},
   {"ifflag",      MINOR, IFFLAG,      2,   2},
   {"bitst",       MINOR, IFFLAG,      2,   2},     /* For compatibility */
   {"bit",         MINOR, IFFLAG,      2,   2},     /* Ditto */
   {"ifkey",       MINOR, IFKEY,       1,   ANY_NUMBER},
   {"ifany",       MINOR, IFANY,       1,   ANY_NUMBER},
   {"query",       MINOR, QUERY,       1,   1},
   {"ifinrange",   MINOR, IFINRANGE,   3,   3},
   {"ifis",        MINOR, IFIS,        2,   ANY_NUMBER},
   {"ifloc",       MINOR, IFLOC,       2,   ANY_NUMBER},
   {"ifcgi",       MINOR, IFCGI,       0,   0},
   {"ifdoall",     MINOR, IFDOALL,     0,   0},
   {"and",         MINOR, AND,         0,   0},
   {"or",          MINOR, OR,          0,   0},
   {"xor",         MINOR, XOR,         0,   0},
   {"eor",         MINOR, XOR,         0,   0},     /* For compatibility */
   {"not",         MINOR, NOT,         0,   0},
   {"else",        MINOR, ELSE,        0,   0},
   {"otherwise",   MINOR, OTHERWISE,   0,   0},
   {"fin",         MINOR, FIN,         0,   0},
   {"eof",         MINOR, EOT,         0,   0},
   {"itobj",       MINOR, ITOBJ,       1,   ANY_NUMBER},
   {"itlist",      MINOR, ITOBJ,       1,   3},     /* For compatibility */
   {"itplace",     MINOR, ITLOC,     1,   3},
   {"iterate",     MINOR, ITERATE,     3,   3},
   {"next",        MINOR, NEXT,        0,   0},
   {"continue",    MINOR, NEXT,        0,   0},
   {"break",       MINOR, BREAK,       0,   0},
   {"last",        MINOR, BREAK,       0,   0},
   {"eoi",         MINOR, EOI,         0,   0},
   {"call",        MINOR, CALL,        1,   ANY_NUMBER},
   {"proceed",     MINOR, PROCEED,     0,   0},
   {"procede",     MINOR, PROCEED,     0,   0},
   {"quit",        MINOR, QUIT,        0,   0},
   {"stop",        MINOR, STOP,        0,   0},
   {"get",         MINOR, GET,         1,   1},
   {"drop",        MINOR, DROP,        1,   1},
   {"apport",      MINOR, APPORT,      2,   2},
   {"goto",        MINOR, GOTO,        1,   1},
#ifndef NOVARARGS
   {"move",        MINOR, MOVE,        1,   ANY_NUMBER},
   {"smove",       MINOR, SMOVE,       2,   ANY_NUMBER},
#else
   {"move",        MINOR, MOVE,        1,   16},
   {"smove",       MINOR, SMOVE,       2,   16},
#endif
   {"say",         MINOR, SAY,         1,   2},
   {"name",        MINOR, SAY,         2,   2},     /* For compatibility */
   {"describe",    MINOR, DESCRIBE,    1,   2},
   {"respond",     MINOR, RESPOND,     2,   ANY_NUMBER},
   {"quip",        MINOR, QUIP,        1,   2},
   {"value",       MINOR, VALUE,       2,   2},
   {"vocab",       MINOR, DICT,        4,   4},
   {"append",      MINOR, APPEND,      1,   2},
   {"set",         MINOR, SET,         2,   2},
   {"add",         MINOR, ADD,         2,   2},
   {"subtract",    MINOR, SUBTRACT,    2,   2},
   {"sub",         MINOR, SUBTRACT,    2,   2},
   {"multiply",    MINOR, MULTIPLY,    2,   2},
   {"mult",        MINOR, MULTIPLY,    2,   2},
   {"divide",      MINOR, DIVIDE,      2,   2},
   {"negate",      MINOR, NEGATE,      1,   1},
   {"intersect",   MINOR, INTERSECT,   2,   2},
   {"random",      MINOR, RANDOM,      2,   2},
   {"choose",      MINOR, CHOOSE,      3,   3},
   {"lda",         MINOR, LDA,         2,   2},
   {"eval",        MINOR, EVAL,        2,   2},
   {"deposit",     MINOR, DEPOSIT,     2,   2},
   {"locate",      MINOR, LOCATE,      2,   2},
   {"flag",        MINOR, FLAG,        2,   2},
   {"biset",       MINOR, FLAG,        2,   2},     /* For compatibility */
   {"bis",         MINOR, FLAG,        2,   2},     /* Ditto */
   {"unflag",      MINOR, UNFLAG,      2,   2},
   {"biclear",     MINOR, UNFLAG,      2,   2},     /* For compatibility */
   {"bic",         MINOR, UNFLAG,      2,   2},     /* Ditto */
   {"svar",        MINOR, SVAR,        2,   2},
   {"exec",        MINOR, EXEC,        1,   2},
   {"execute",     MINOR, EXEC,        2,   2},     /* Compatibility */
   {"executive",   MINOR, EXEC,        2,   2},     /* Compatibility */
   {"input",       MINOR, INPUT,       0,   1},
   {"default",     MINOR, DEFAULT,     1,   2},
   {"doall",       MINOR, DOALL,       1,   2},
   {"flush",       MINOR, FLUSH,       0,   0},
   {"ifnear",      MINOR, IFNEAR,      1,   2},
   {"ifhere",      MINOR, IFHERE,      1,   2},
   {"randomise",   MINOR, RANDOMISE,   2,   2},
   {"tie",         MINOR, TIE,         2,   ANY_NUMBER},
   {"fakearg",     MINOR, FAKEARG,     2,   2},
   {"fakecom",     MINOR, FAKECOM,     2,   2},
   {"save",        MINOR, SAVE,        FREE_ARG,   2},
   {"restore",     MINOR, RESTORE,     FREE_ARG,   2},
   {"delete",      MINOR, DELETE,      1,   1},
   {"savedlist",   MINOR, SAVEDLIST,   FREE_ARG + 1,   2},
   {"verbatim",    MINOR, VERBATIM,    1,   1},
   {"undo",        MINOR, UNDO,        0,   1},
   {"redo",        MINOR, REDO,        0,   1},
   {"local",       MINOR, LOCAL,       1,   ANY_NUMBER},
   {"checkpoint",  MINOR, CHECKPOINT,  0,   0},
   {"dump",        MINOR, DUMP,        0,   0},
   {"object",      SYMBOL,OBJFLAG,     0,   0},
   {"location",    SYMBOL,LOCFLAG,     0,   0},
   {"place",       SYMBOL,LOCFLAG,     0,   0},
   {"verb",        SYMBOL,VERBFLAG,    0,   0},
   {"variable",    SYMBOL,VARFLAG,     0,   0},
   {"var",         SYMBOL,VARFLAG,     0,   0},
   {"",            0,     ENDOFLIST,   0,   0}
} ;

#ifdef __STDC__
void initial(void)
#else
void initial()
#endif
{
   int index;
   char *tptr;
   int len;
   int type;
   int mask;
   struct node *np;
   
   index = 0;
   dp = &keywords[0];
   while (dp -> id != ENDOFLIST)
   {
      if (dp -> type == SYMBOL)
            np = addsymb(AUTOSYMBOL, dp -> name, FLAGS, dp -> id);
      else
      {
         np = addsymb(dp -> type, dp -> name, dp -> type, dp -> id);
         np -> min_args = dp -> min_args;
         np -> max_args = dp -> max_args;
      }
      dp = &keywords[ (++index) ];
   }

   line_status = EOL;

   if ((text_buf_ptr = (char *) calloc (text_buf_len, sizeof(char))) == NULL)
      (void) gripe ("text_buf", "Unable to allocate text buffer.");
   if ((cond_buf_ptr = (char *) calloc (cond_buf_len, sizeof(char))) == NULL)
      (void) gripe ("cond_buf", "unable to allocate condition buffer.");
   if ((voc_buf_ptr = (char *) calloc (voc_buf_len, sizeof(char))) == NULL)
      (void) gripe ("voc_buf", "unable to allocate vocabulary buffer.");
   voc_ptr = voc_buf_ptr;
   voc_top = voc_buf_ptr + VOC_INIT_LEN - 20;

   *gameid = '\0';
   *author = '\0';
   *date = '\0';
   *version = '\0';
   *title = '\0';
   style = -1;
   next_addr = 0;

   return;
}

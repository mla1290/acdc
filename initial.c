/* initial.c (acdc) - copyleft Mike Arnautov 1990-2003.
 *
 * 09 Mar 03   MLA           Replaced trace with debug.
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
 * 01 Apr 94   MLA           Allowed range specification with ITOBJ/ITPLACE.
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

#include <stdio.h>
#include <string.h>

#include "acdc.h"
#include "const.h"
#include "minor.h"
#include "major.h"
#include "line.h"
#include "source.h"
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
   {"object",    MAJOR,     OBJECT,    1,   ANY_NUMBER},
   {"place",     MAJOR,     PLACE,     1,   ANY_NUMBER},
   {"variable",  MAJOR,     VARIABLE,  1,   ANY_NUMBER},
   {"array",     MAJOR,     ARRAY,     2,   2},
   {"verb",      MAJOR,     VERB,      1,   ANY_NUMBER},
   {"text",      MAJOR,     TEXT,      0,   2},
   {"fragment",  MAJOR,     FRAGMENT,  0,   2},
   {"procedure", MAJOR,     PROCEDURE, 1,   ANY_NUMBER},
   {"proc",      MAJOR,     PROCEDURE, 1,   ANY_NUMBER},
   {"label",     MAJOR,     PROCEDURE, 1,   ANY_NUMBER},  /* Compatibility */
   {"action",    MAJOR,     ACTION,    1,   ANY_NUMBER},
   {"at",        MAJOR,     AT,        1,   1},
   {"init",      MAJOR,     INIT,      0,   0},
   {"initial",   MAJOR,     INIT,      0,   0},
   {"initialise",MAJOR,     INIT,      0,   0},
   {"repeat",    MAJOR,     REPEAT,    0,   0},
   {"constant",  MAJOR,     CONSTANT,  1,   ANY_NUMBER},
   {"flags",     MAJOR,     FLAGS,     1,   1},
   {"state",     MAJOR,     STATE,     1,   ANY_NUMBER},
   {"noise",     MAJOR,     NOISE,     1,   ANY_NUMBER},
   {"null",      MAJOR,     NOISE,     1,   ANY_NUMBER},
   {"list",      MAJOR,     LIST,      0,   0},
   {"nolist",    MAJOR,     NOLIST,    0,   0},
   {"xref",      MAJOR,     XREF,      0,   0},
   {"noxref",    MAJOR,     NOXREF,    0,   0},
   {"include",   MAJOR,     INCLUDE,   1,   1},
   {"synonym",   MAJOR,     SYNONYM,   1,   ANY_NUMBER},  /* Compatibility */
   {"synon",     MAJOR,     SYNONYM,   1,   ANY_NUMBER},  /* Compatibility */
   {"define",    MAJOR,     DEFINE,    1,   ANY_NUMBER},  /* Compatibility */
   {"gameid",    MAJOR,     VERSION,   1,   REST},
   {"title",     MAJOR,     VERSION,   1,   1},           /* Compatibility */
   {"dbname",    MAJOR,     VERSION,   1,   1},           /* Compatibility */
   {"author",    MAJOR,     AUTHOR,    1,   REST},
   {"style",     MAJOR,     STYLE,     1,   2},
   {"have",      MINOR,     HAVE,      1,   ANY_NUMBER},
   {"near",      MINOR,     NEAR,      1,   ANY_NUMBER},
   {"here",      MINOR,     HERE,      1,   ANY_NUMBER},
   {"at",        MINOR,     ATPLACE,   1,   ANY_NUMBER},
#ifndef NOVARARGS
   {"anyof",     MINOR,     ANYOF,     1,   ANY_NUMBER},
   {"keyword",   MINOR,     KEYWORD,   1,   ANY_NUMBER},
#else
   {"anyof",     MINOR,     ANYOF,     1,   16},
   {"keyword",   MINOR,     KEYWORD,   1,   16},
#endif
   {"ifeq",      MINOR,     IFEQ,      2,   2},
   {"ifne",      MINOR,     IFNE,      2,   2},
   {"iflt",      MINOR,     IFLT,      2,   2},
   {"ifle",      MINOR,     IFLE,      2,   2},
   {"ifgt",      MINOR,     IFGT,      2,   2},
   {"ifge",      MINOR,     IFGE,      2,   2},
   {"ifat",      MINOR,     IFAT,      1,   ANY_NUMBER},
   {"chance",    MINOR,     CHANCE,    1,   1},
   {"ifhave",    MINOR,     IFHAVE,    1,   2},
   {"ifflag",    MINOR,     IFFLAG,    2,   2},
   {"bitst",     MINOR,     IFFLAG,    2,   2},     /* For compatibility */
   {"bit",       MINOR,     IFFLAG,    2,   2},     /* Ditto */
   {"ifkey",     MINOR,     IFKEY,     1,   ANY_NUMBER},
   {"ifany",     MINOR,     IFANY,     1,   ANY_NUMBER},
   {"query",     MINOR,     QUERY,     1,   1},
   {"ifinrange", MINOR,     IFINRANGE, 3,   3},
   {"ifis",      MINOR,     IFIS,      2,   2},
   {"ifloc",     MINOR,     IFLOC,     2,   ANY_NUMBER},
   {"and",       MINOR,     AND,       0,   0},
   {"or",        MINOR,     OR,        0,   0},
   {"xor",       MINOR,     XOR,       0,   0},
   {"eor",       MINOR,     XOR,       0,   0},     /* For compatibility */
   {"not",       MINOR,     NOT,       0,   0},
   {"else",      MINOR,     ELSE,      0,   0},
   {"otherwise", MINOR,     OTHERWISE, 0,   0},
   {"fin",       MINOR,     FIN,       0,   0},
   {"eof",       MINOR,     EOT,       0,   0},
   {"itobj",     MINOR,     ITOBJ,     1,   ANY_NUMBER},
   {"itlist",    MINOR,     ITOBJ,     1,   3},     /* For compatibility */
   {"itplace",   MINOR,     ITPLACE,   1,   3},
   {"iterate",   MINOR,     ITERATE,   3,   3},
   {"next",      MINOR,     NEXT,      0,   0},
   {"continue",  MINOR,     NEXT,      0,   0},
   {"break",     MINOR,     BREAK,     0,   0},
   {"last",      MINOR,     BREAK,     0,   0},
   {"eoi",       MINOR,     EOI,       0,   0},
   {"call",      MINOR,     CALL,      1,   ANY_NUMBER},
   {"proceed",   MINOR,     PROCEED,   0,   0},
   {"procede",   MINOR,     PROCEED,   0,   0},
   {"quit",      MINOR,     QUIT,      0,   0},
   {"stop",      MINOR,     STOP,      0,   0},
   {"get",       MINOR,     GET,       1,   1},
   {"drop",      MINOR,     DROP,      1,   1},
   {"apport",    MINOR,     APPORT,    2,   2},
   {"goto",      MINOR,     GOTO,      1,   1},
#ifndef NOVARARGS
   {"move",      MINOR,     MOVE,      1,   ANY_NUMBER},
   {"smove",     MINOR,     SMOVE,     2,   ANY_NUMBER},
#else
   {"move",      MINOR,     MOVE,      1,   16},
   {"smove",     MINOR,     SMOVE,     2,   16},
#endif
   {"say",       MINOR,     SAY,       1,   2},
   {"name",      MINOR,     SAY,       2,   2},     /* For compatibility */
   {"describe",  MINOR,     DESCRIBE,  1,   2},
   {"respond",   MINOR,     RESPOND,   2,   ANY_NUMBER},
   {"quip",      MINOR,     QUIP,      1,   2},
   {"value",     MINOR,     VALUE,     2,   2},
   {"vocab",     MINOR,     DICT,      4,   4},
   {"set",       MINOR,     SET,       2,   2},
   {"add",       MINOR,     ADD,       2,   2},
   {"subtract",  MINOR,     SUBTRACT,  2,   2},
   {"sub",       MINOR,     SUBTRACT,  2,   2},
   {"multiply",  MINOR,     MULTIPLY,  2,   2},
   {"mult",      MINOR,     MULTIPLY,  2,   2},
   {"divide",    MINOR,     DIVIDE,    2,   2},
   {"negate",    MINOR,     NEGATE,    1,   1},
   {"intersect", MINOR,     INTERSECT, 2,   2},
   {"random",    MINOR,     RANDOM,    2,   2},
   {"choose",    MINOR,     CHOOSE,    3,   3},
   {"lda",       MINOR,     LDA,       2,   2},
   {"eval",      MINOR,     EVAL,      2,   2},
   {"deposit",   MINOR,     DEPOSIT,   2,   2},
   {"locate",    MINOR,     LOCATE,    2,   2},
   {"flag",      MINOR,     FLAG,      2,   2},
   {"biset",     MINOR,     FLAG,      2,   2},     /* For compatibility */
   {"bis",       MINOR,     FLAG,      2,   2},     /* Ditto */
   {"unflag",    MINOR,     UNFLAG,    2,   2},
   {"biclear",   MINOR,     UNFLAG,    2,   2},     /* For compatibility */
   {"bic",       MINOR,     UNFLAG,    2,   2},     /* Ditto */
   {"svar",      MINOR,     SVAR,      2,   2},
   {"exec",      MINOR,     EXEC,      1,   2},
   {"execute",   MINOR,     EXEC,      2,   2},     /* Compatibility */
   {"executive", MINOR,     EXEC,      2,   2},     /* Compatibility */
   {"input",     MINOR,     INPUT,     0,   1},
   {"default",   MINOR,     DEFAULT,   1,   2},
   {"doall",     MINOR,     DOALL,     1,   2},
   {"flush",     MINOR,     FLUSH,     0,   0},
   {"ifnear",    MINOR,     IFNEAR,    1,   2},
   {"ifhere",    MINOR,     IFHERE,    1,   2},
   {"randomise", MINOR,     RANDOMISE, 2,   2},
   {"tie",       MINOR,     TIE,       2,   ANY_NUMBER},
   {"fakearg",   MINOR,     FAKEARG,   2,   2},
   {"fakecom",   MINOR,     FAKECOM,   2,   2},
   {"local",     MINOR,     LOCAL,     1,   ANY_NUMBER},
   {"checkpoint",MINOR,     CHECKPOINT,0,   0},
   {"object",    SYMBOL,    OBJFLAG,   0,   0},
   {"place",     SYMBOL,    PLACEFLAG, 0,   0},
   {"verb",      SYMBOL,    VERBFLAG,  0,   0},
   {"variable",  SYMBOL,    VARFLAG,   0,   0},
   {"",          0,         ENDOFLIST, 0,   0}
} ;

#ifdef __STDC__
void initial(void)
#else
void initial()
#endif
{
   int index;
   char *tptr;
   char dbname [80];
   int len;
   int type;
   int mask;
   struct node *np;
   struct node *parse ();
   
   extern void *calloc();

   index = 0;
   dp = &keywords[0];
   while (dp -> id != ENDOFLIST)
   {
      if (dp -> type == SYMBOL)
      {
         if (style != 1)
            (void) addsymb(dp -> type, dp -> name, FLAGS, dp -> id);
      }
      else
      {
         np = addsymb(dp -> type, dp -> name, dp -> type, dp -> id);
         np -> body.directive.min_args = dp -> min_args;
         np -> body.directive.max_args = dp -> max_args;
      }
      dp = &keywords[ (++index) ];
   }

   listing = FALSE;
   line_status = EOL;

   (void) addsymb (SYMBOL, "INIT_PROC", INIT, 1);
   (void) addsymb (SYMBOL, "REPEAT_PROC", REPEAT, 2);

   if ((text_buf_ptr = (char *) calloc (text_buf_len, sizeof(char))) == NULL)
      (void) gripe ("text_buf", "Unable to allocate space.");
   if ((cond_buf_ptr = (char *) calloc (cond_buf_len, sizeof(char))) == NULL)
      (void) gripe ("cond_buf", "unable to allocate space.");
   if ((voc_buf_ptr = (char *) calloc (voc_buf_len, sizeof(char))) == NULL)
      (void) gripe ("voc_buf", "unable to allocate space.");
   voc_ptr = voc_buf_ptr;
   voc_top = voc_buf_ptr + VOC_INIT_LEN - 20;

/* Now see whether we have any game info lines (style, author, gameid). */
   
   *version = '\0';
   *author = '\0';
   style = -1;
   while (1)
   {
      if ((line_status = getline (IGNORE_BLANK)) == EOF)
         (void) gripe ("", "Unexpected end of file!");
      if ((np = parse (MAJOR)) == NULL)
         (void) gripe (tp [0], "Unknown major directive.");
      type = np -> refno;
      if (type == VERSION)
      {
         strncpy (version, tp [1], sizeof (version) - 1);
         *(version + sizeof (version) - 1) = '\0';
      }
      else if (type == AUTHOR)
      {
         strncpy (author, tp [1], sizeof (author) - 1);
         *(author + sizeof (author) - 1) = '\0';
      }
      else if (type == STYLE)
      {
         if (strcmp (tp[1], "old") == 0 || strcmp (tp[1], "original") == 0)
            style = 1;
         else
         {
            index = chrtobin (tp[2] ? tp[2] : tp[1]);
            if (index > 11)
               (void) gripe ("", 
                  "Style higher than current maximum of 11!");
            if (index < 10 && style != 1)
               (void) gripe ("", "Style values from 2 to 9 are meaningless.");
            style = index;
         }
      }
      else
      {
         strcpy (line, raw_line);
         line_status = BOL;
         break;
      }   
      line_status = EOL;
   }
   if (style == -1)
      style = *version ? 10 : 1;
 
   if ((code_file = openout("adv01.c", "w")) == NULL)
      (void) gripe ("adv01.c", "Unable to open code file.");
   (void) fprintf (code_file, "#include \"adv0.h\"\n");
   (void) fprintf (code_file, "#include \"adv3.h\"\n");

   next_addr = 0;
   if (debug)     /* Declarations over - if source output */
      debug = 2;  /* is required, turn it on!             */

#ifdef __50SERIES
#  define MODE "o"
#else
#  define MODE "wb"
#endif

   if (memory < 3)
   {
      strcpy (dbname, source_file);
      if (tptr = strrchr (dbname, '.'))
         *tptr = 0;
      strcat (dbname, ".dat");
      tptr =  dbname;
   }
   else
      tptr = "adv6.h";
   
   if ((text_file = openout (tptr, MODE)) == NULL)
      (void) gripe (tptr, "Unable to open data file.");

   tptr = *version ? version : source_file;
   len = strlen (tptr);
   if (len > 79) len = 79;
   (void) strncpy (version, tptr, len);
   *(version + len) = '\0';
   version [79] = '\0';
   tptr = version;
   if (memory == 3)
   {
      fprintf (text_file, "char text [TEXT_BYTES] = {\n");
      fputc ('0', text_file); 
   }
   else
      fputc ('\0', text_file);
   next_addr++;

   if (plain_text == 0)
   {
      key_mask = (rand() & 127) ^ 'x';
      if (key_mask == 0 || key_mask == 127) key_mask = 'y';
   }
   else
      key_mask = 0;
   mask = key_mask;

   while (*tptr && *tptr != '\n')
   {
      if (memory == 3)
         (void) fprintf (text_file, ",%d", *tptr ^ mask);
      else
         fputc (*tptr ^ mask, text_file);
      if (plain_text == 0)
         mask ^= *tptr;
      tptr++;
      next_addr++;
   }

   if (memory == 3)
      fprintf (text_file, ",%d", mask);
   else
      fputc (mask, text_file);
   next_addr++;

   np = addsymb (SYMBOL, ".version", TEXT, type_counts [TEXT]++);
   np -> body.text.name_addr = next_addr;
   tptr = version;
   while (*tptr)
      storchar(*tptr++);
   storchar ('\0');
   
   printf ("GameID: %s\n", version);
   printf ("Style:  ");
   if (style == 1)
      puts ("Dave Platt's original A-code");
   else
      printf ("A-code %d\n", style);
   if (*author)
      printf ("Author: %s\n", author);
   
   return;
}

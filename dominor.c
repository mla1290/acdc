/* dominor.c (acdc) - copyleft Mike Arnautov 1990-2010.
 *
 * 29 Jan 10   MLA           Added IFHTML.
 * 11 Jan 10   MLA           Renamed getline() to nextline() to avoid a
 *                           new gcc header clash.
 * 14 Jul 09   MLA           Fixed gcc --pedantic warnings.
 * 23 May 08   MLA           Added IFTYPED.
 * 01 May 08   MLA           Bug: Must check proc offset to be >= 0!
 * 15 Mar 08   MLA           Version 12 changes.
 * 10 Mar 08   MLA           Added redundant qualifier check.
 * 10 Nov 07   MLA           Added DUMP.
 * 07 May 07   Stuart Munro  bug: need to include string.h (for strcmp).
 * 06 May 07   MLA           Added deprecated warnings.
 * 15 Oct 05   MLA           BUG: fixed the NOT IFIS logic.
 * 08 Sep 05   MLA           IFIS now takes multiple args.
 * 27 Aug 05   MLA           bug: Allow SAY <place>.
 * 02 Jan 05   MLA           Added UNDO/REDO.
 * 22 Aug 04   MLA           Made CALL directive optional.
 * 20 Aug 04   MLA           Added IFCGI and IFDOALL.
 * 19 Aug 04   MLA           Added SAVE/RESTORE and VERBATIM.
 * 08 Aug 04   MLA           Added APPEND.
 * 28 Mar 03   MLA           Added NEXT and BREAK.
 * 19 Mar 03   MLA           Check QUIP argument.
 * 09 Mar 03   MLA           Replaced trace with debug.
 * 06 Mar 03   Stuart Munro  Fix non-ASCII dominor() args declaration;
 *                           declare addparam(), fndparam() and iniparam().
 * 01 Mar 03   MLA           Bug: Fake argtyp[2] if faking argval[2] in SAY.
 * 02 Jan 03   MLA           bug: Don't try for CALL args unless PROCEDURE.
 * 21 Dec 02   MLA           Bug: Make itobj...place cater for schizoids!
 * 30 Sep 02   MLA           bug: Fixed gripe argument in RESPOND code.
 * 27 Jul 02   MLA           Added CHECKPOINT.
 * 14 Jul 02   MLA           Fake second arg for EXEC. Also pass on address
 *                           of the second argument, not its refno.
 *                           Also, report non-declarative types in xref.
 * 13 Jul 02   MLA           Complete re-write of SET.
 *                           ITOBJ now takes state/flag as args.
 * 12 Jul 02   MLA           Added IFNE, IFGE, IFLE, DESCRIBE.
 *                           Also removed usage of +1024 in IFHAVE etc...
 * 11 Jul 02   MLA           BUG: fixed ITOBJ initalisation fo local pointer.
 * 22 Jun 02   MLA           RANDOMISE texts for real.
 * 10 Mar 02   MLA           bug: fixed xref for local variables and params.
 * 03 Mar 02   MLA           BUG: fixed passing of local var parameters.
 * 13 Jan 02   MLA           Allow constants as call parameters.
 *                           Pass "hidden" type parameters.
 *                           Added RESPOND.
 * 10 Jan 02   MLA           bug: sundry fixes of LOCAL code.
 * 01 Jan 02   MLA           Added local variable declarations.
 * 31 Dec 01   MLA           Allowed for argument passing; also added
 *                           cross-referencing code.
 * 17 Oct 01   MLA           Improved compatibility with Platt's A-code.
 * 01 Jul 01   MLA           bug: Make VALUE dereference text qualifiers.
 *                           IFAT now allows multiple arguments.
 * 23 May 01   MLA           BUG: FAKEARG/COM - set ARG2 if that's the match!
 * 17 Mar 01   MLA           Added FAKECOM.
 * 16 Mar 01   MLA           Added IFANY. Allowed multiple args with IFKEY.
 * 05 Mar 01   MLA           Added FAKEARG.
 * 28 Feb 01   MLA           Added TIE.
 * 08 Aug 00   MLA           Added code for OTHERWISE.
 * 24 Jul 99   MLA           Fixed compiler warnings.
 * 24 Mar 99   MLA           Allowed IFFLAG's second arg to be a variable.
 * 01 Apr 94   MLA           Allowed range specification with ITOBJ/ITLOC.
 * 12 Dec 91   MLA           Allowed optional argument to INPUT.
 * 10 Mar 91   MLA           Changed STOPALL to FLUSH - flushes command
 *                           line as well as aborting ALL processing.
 * 27 Dec 90   MLA           Separated FLAGS type from CONSTANT and
 *                           allowed flags or states with IFNEAR/HERE/HAVE.
 * 15 Dec 90   MLA           Compacted automatic code braces.
 * 14 Dec 90   MLA           Allowed no keyword matching in (S)MOVE.
 * 12 Dec 90   MLA           New minor directive QUIP.
 * 11 Dec 90   MLA           Allowed variable args to MOVE and SMOVE.
 * 17 Nov 90   MLA           Introduced SPRINTF compilation symbol.
 *                           to cope with the deviant Ultrix library.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "acdc.h"
#include "game.h"
#include "line.h"
#include "const.h"
#include "symbol.h"
#include "minor.h"
#include "major.h"
#include "output.h"
#include "text.h"
#include "source.h"

#ifdef SPRINTF
#define SPRINTF2(A,B) strlen(sprintf(A,B))
#define SPRINTF3(A,B,C) strlen(sprintf(A,B,C))
#define SPRINTF4(A,B,C,D) strlen(sprintf(A,B,C,D))
#define SPRINTF5(A,B,C,D,E) strlen(sprintf(A,B,C,D,E))
#else
#define SPRINTF2(A,B) sprintf(A,B)
#define SPRINTF3(A,B,C) sprintf(A,B,C)
#define SPRINTF4(A,B,C,D) sprintf(A,B,C,D)
#define SPRINTF5(A,B,C,D,E) sprintf(A,B,C,D,E)
#endif

#ifdef __STDC__
void addparam (int arg, char *name);
int fndparam (char *name);
int iniparam (int varsize);
void dominor (char *prochead, char *proccond)
#else
void addparam ();
int fndparam ();
int iniparam ();
void dominor (prochead, proccond)
char *prochead;
char *proccond;
#endif /* __STDC__ */
{
   int minor_type;
   int type;
   char ctype;
   int index;
   int end_index;
   int proc_index;
   int ifs_pending;
   int conjunction_pending;
   int not_pending;
   int multiple_pending;
   int brace_count;
   int brace_pending;
   int loop_count;
   int args_count;
   int got_code;
   int locals;
   char *chr;
   struct node *np;
   char *cond_ptr;
   char *cond_top;
   char *old_cond_ptr;
   int argtyp [ANY_NUMBER + 1];
   int argval [ANY_NUMBER + 1];
   struct node *ap [ANY_NUMBER + 1];

   got_code = 0;
   ifs_pending = 0;
   brace_count = 0;
   loop_count = 0;
   conjunction_pending = FALSE;
   not_pending = FALSE;
   multiple_pending = FALSE;
   brace_pending = FALSE;
   locals = 0;

   while (TRUE)
   {
      line_status = EOL;
      if (nextline (IGNORE_BLANK) == EOF)
         goto terminate;
      line_ptr = line;
      if (*line_ptr != ' ' && *line_ptr != '\t')
          break;

      if ((np = parse (MINOR)) == NULL)
      {
         if (style < 11)
            gripe (tp [0], "Not a known minor directive.");
         else
            gripe (tp [0], "Not a minor directive or callable symbol.");
      }
      minor_count++;
      ap[0] = np;

      minor_type = np -> refno;

      if (multiple_pending)
      {
         if (minor_type == multiple_pending)
            fprintf (code_file, "\n   ");
         else
         {
            fprintf (code_file, "-1)) return;\n");
            multiple_pending = FALSE;
         }
      }

      if (minor_type >= ELSE)
      {
         if (not_pending || conjunction_pending)
            gripe ("", "Another conditional expected.");
         if (ifs_pending)
         {
            fprintf (code_file, "   if ");
            if (minor_type == ELSE)
               fprintf (code_file, "(!");
            while(ifs_pending-- > 0)
               fputc ('(', code_file);
            cond_top = cond_ptr;
            cond_ptr = cond_buf_ptr - 1;
            while (++cond_ptr < cond_top)
               if (*cond_ptr)
                  fputc (*cond_ptr, code_file);
            ifs_pending = 0;
            fputc (')', code_file);
            if (minor_type == ELSE || minor_type == OTHERWISE)
               fputc (')', code_file);
            if (minor_type != OTHERWISE)
            {
               fprintf (code_file, " {\n");
               brace_count ++;
            }
            if (minor_type == ELSE || minor_type == OTHERWISE)
               continue;
         }
      }
      else
      {
         if (minor_type < NOT)
         {
            if (ifs_pending <= 0)
            {
               ifs_pending = 1;
               cond_ptr = cond_buf_ptr;
               cond_top = cond_buf_ptr + cond_buf_len - 128;
            }
            else
            {
               if (conjunction_pending == 0)
               {
                  fprintf (code_file, "   if ");
                  while (ifs_pending-- > 0)
                     fprintf (code_file, "(");
                  cond_top = cond_ptr;
                  cond_ptr = cond_buf_ptr - 1;
                  while (++cond_ptr < cond_top)
                     if (*cond_ptr)
                        fputc (*cond_ptr, code_file);
                  fprintf (code_file, ") {\n");
                  brace_count++;
                  ifs_pending = 1;
                  cond_ptr = cond_buf_ptr;
                  cond_top = cond_buf_ptr + cond_buf_len - 128;
               }
               else if (cond_ptr > cond_top)
               {
                  cond_buf_len += COND_INIT_LEN;
                  old_cond_ptr = cond_buf_ptr;
                  if ((cond_buf_ptr = (char *) realloc (cond_buf_ptr,
                     cond_buf_len)) == NULL)
                        gripe ("", "Unable to extend condition buffer.");
                  cond_ptr = cond_buf_ptr + (cond_ptr - old_cond_ptr);
                  cond_top = cond_buf_ptr + cond_buf_len - 128;
                  printf ("Condition buffer size increased to %d.\n",
                     cond_buf_len);
               }
            }
         }
         else
            if (conjunction_pending && minor_type != NOT)
               gripe ("", "Misplaced conjunction.");
      }

      if (minor_type != NOT)
         conjunction_pending = 0;

      if (brace_pending && minor_type != FIN && minor_type != EOI)
      {
         fputc ('\n', code_file);
         brace_pending = FALSE;
      }

      if (minor_type == LOCAL)
      {
         if (got_code)
            gripe ("", 
               "Local declarations must precede all local code!");
         for (index = 1; tp [index]; index++)
         {
            addparam (0, tp [index]);
            if (xref)
               write_ref ("LOCAL", tp [index]);
         }
         continue;
      }
            
      index = 0;
      while (tp [++index] != NULL)
      {
         chr = tp [index];
         argtyp [index] = -1;
         if (isdigit (*chr) || *chr == '+' || *chr == '-')
            ap [index] = NULL;        
         else
         {
            if (index == 1 && np -> min_args >= FREE_ARG)
            {
               argtyp [index] = FREE_ARG;
               argval [index] = 0;
               continue;
            }
            if ((argval [index] = fndparam (tp [index])) != -1)
            {
               argtyp [index] = LOCAL;
               ap [index] = NULL;
            }
            else if ((ap [index] = fndsymb (TESTSYMBOL, tp [index])) != NULL)
            {
               (ap [index] -> used_count)++;
               argtyp [index] = ap [index] -> type;
               if (argtyp [index] == PROCEDURE)
                  argval [index] = ap [index] -> proc_base;
               else
                  argval [index] = ap [index] -> refno;
            }
            if (xref)
            {
               char *tname;
               switch (argtyp [index])
               {
                  case OBJ:    tname = " obj "; break;
                  case LOC:     tname = "place"; break;
                  case VERB:      tname = " vrb "; break;
                  case VAR:  tname = " var "; break;
                  case TEXT:      tname = " txt "; break;
                  case PROCEDURE: tname = " prc "; break;
                  case CONSTANT:  tname = " cns "; break;
                  case STATE:     tname = "state"; break;
                  case FLAGS:     tname = " flg "; break;
                  case LOCAL:     tname = "local"; break;
                  default:        tname = " cns "; break;
               }
               write_ref (tname, tp [index]);
            }
         }
         if (ap [index] == NULL && argtyp [index] != LOCAL)
         {
            argtyp [index] = CONSTANT;
            argval [index] = getconst (tp [index]);
         }
      }

      if (got_code == 0)
      {
         locals = iniparam (flag_field_size[VARFLAG]/16 + 1);
         got_code = 1;
         if (debug && prochead && *prochead)
            fprintf (code_file, "   puts(\"*** %s ***\");\n", prochead);
         if (proccond && *proccond)
            fprintf (code_file, proccond);
      }
      
      index = 0;
      switch (minor_type)
      {
         case KEYWORD:
         case ANYOF:
            deprecate ((minor_type == KEYWORD) ? "KEYWORD" : "ANYOF", 11, 0);
            if (multiple_pending != minor_type)
               fprintf (code_file, "   if (!%s(",
                  (minor_type == KEYWORD) ? "keyword" : "anyof");
            multiple_pending = minor_type;
            while (tp [++index] != NULL)
            {
               type = argtyp [index];
               if (type != OBJ && type != LOC && type != VERB)
                  gripe (tp [index], "Bad multiple choice argument!");
               fprintf (code_file, "%d,", argval [index]);
            }
            break;

         case HAVE:
         case NEAR:
         case HERE:
            fprintf (code_file, "   if (");
            index = 1;
            while (tp [index])
            {
               if (argtyp [index] != OBJ && 
                   argtyp [index] != VAR &&
                   argtyp [index] != LOCAL)
                     gripe (tp [index], "not reducible to an object.");
               if (minor_type == HAVE)
               {
                  deprecate ("HAVE", 11, 0);
                  fprintf (code_file, "!have");
               }
               else if (minor_type == NEAR)
               {
                  deprecate ("NEAR", 11, 0);
                  fprintf (code_file, "!isnear");
               }
               else
               {
                  deprecate ("HERE", 11, 0);
                  fprintf (code_file, "!ishere");
               }
               if (argtyp [index] == VAR)
                  fprintf (code_file, "(value[%d],-1,-1)", 
                     argval [index]);
               else if (argtyp [index] == LOCAL)
                  fprintf (code_file, "(lval[%d],-1,-1)", 
                     argval [index]);
               else
                  fprintf (code_file, "(%d,-1,-1)", argval [index]);
               if (tp [++index])
                  fprintf (code_file, " || ");
            }
            fprintf (code_file, ") return;\n");
            break;

         case ATLOC:
            deprecate ("ATLOC", 11, 0);
            fprintf (code_file, "   if (");
            index = 1;
            while (tp [index])
            {
               if ((argtyp [index] > VAR && argtyp [index] != LOCAL)
                  || argtyp [index] == VERB)
               {
                  if (style <= 1) 
                  {
                     index++;
                     continue;
                  }
                  gripe (tp [index], "not reducible to a place or object.");
               }
               fprintf (code_file, "value[HERE]!=");
               if (argtyp [index] == VAR)
                  fprintf (code_file, "value[%d]", 
                     argval [index]);
               else if (argtyp [index] == LOCAL)
                  fprintf (code_file, "lval[%d]", 
                     argval [index]);
               else
                  fprintf (code_file, "%d", argval [index]);
               if (tp [++index])
                  fprintf (code_file, " && ");
            }
            fprintf (code_file, ") return;\n");
            break;

         case IFEQ:
         case IFNE:
         case IFLT:
         case IFLE:
         case IFGT:
         case IFGE:
            if (argtyp [1] <= TEXT)
               cond_ptr += SPRINTF3 (cond_ptr, "value[%d]", argval [1]);
            else if (argtyp [1] == LOCAL)
               cond_ptr += SPRINTF3 (cond_ptr, "lval[%d]", argval [1]);
            else
               cond_ptr += SPRINTF3 (cond_ptr, "%d", argval [1]);
            if (minor_type == IFEQ || minor_type == IFNE)
               cond_ptr += SPRINTF2 (cond_ptr, 
                  not_pending || minor_type == IFNE ? "!=" : "==");
            else if (minor_type == IFLT || minor_type == IFGE)
               cond_ptr += SPRINTF2 (cond_ptr, 
                  not_pending || minor_type == IFGE ? ">=" : "<");
            else
               cond_ptr += SPRINTF2 (cond_ptr, 
                  not_pending || minor_type == IFLE? "<=" : ">");
            if (argtyp [2] <= TEXT)
               cond_ptr += SPRINTF3 (cond_ptr, "value[%d]", argval [2]);
            else if (argtyp [2] == LOCAL)
               cond_ptr += SPRINTF3 (cond_ptr, "lval[%d]", argval [2]);
            else
               cond_ptr += SPRINTF3 (cond_ptr, "%d", argval [2]);
            break;

         case IFAT:
            cond_ptr += SPRINTF3 (cond_ptr, "%s(", not_pending ? "!" : "");
            while (tp [++index] != NULL)
            {
               if (argtyp [index] != OBJ && argtyp [index] != LOC &&
                   argtyp [index] != VAR && argtyp [index] != LOCAL)
                      gripe (tp [index], "not reducible to a location.");
               cond_ptr += SPRINTF3 
                  (cond_ptr, "%svalue[HERE]==", index > 1 ? "||" : "");
               if (argtyp [index] == VAR)
                  cond_ptr += SPRINTF3 (cond_ptr, "value[%d]", 
                     argval [index]);
               else if (argtyp [index] == LOCAL)
                  cond_ptr += SPRINTF3 (cond_ptr, "lval[%d])", 
                     argval [index]);
               else
                  cond_ptr += SPRINTF3 (cond_ptr, "%d", argval [index]);
            }
            cond_ptr += SPRINTF2 (cond_ptr, ")");
            break;

         case CHANCE:
            cond_ptr += SPRINTF3 (cond_ptr,
               "irand(100)%s", not_pending ? ">=" : "<");
            if (argtyp [1] <= TEXT)
               cond_ptr += SPRINTF3 (cond_ptr, "value[%d]", argval [1]);
            else if (argtyp [1] == LOCAL)
               cond_ptr += SPRINTF3 (cond_ptr, "lval[%d]", argval [1]);
            else
               cond_ptr += SPRINTF3 (cond_ptr, "%d", argval [1]);
            break;

         case IFFLAG:
            if (argtyp [1] > VERB && argtyp [1] != VAR &&
               argtyp [1] != LOCAL)
                  gripe (tp [1],"Wrong type for bit operation.");
            if ((argtyp [2] < CONSTANT && argtyp [2] != VAR &&
               argtyp [2] != LOCAL) || argtyp [2] == STATE)
                  gripe (tp [2],
                     "Not reducible to a flag.");
            if (not_pending) cond_ptr += SPRINTF2 (cond_ptr, "!(");
            if (argtyp [1] == VAR)
               cond_ptr += SPRINTF3 (cond_ptr, "bitest(evar(%d),", argval [1]);
            else if (argtyp [1] == LOCAL)
               cond_ptr += SPRINTF3 (cond_ptr, "lbitest(%d,", argval [1]);
            else
               cond_ptr += SPRINTF3 (cond_ptr, "bitest(%d,", argval [1]);
            if (argtyp [2] == VAR)
               cond_ptr += SPRINTF3 (cond_ptr, "value[%d]", argval [2]);
            else if (argtyp [2] == LOCAL)
               cond_ptr += SPRINTF3 (cond_ptr, "lval[%d]", argval [2]);
            else
               cond_ptr += SPRINTF3 (cond_ptr, "%d", argval [2]);
            cond_ptr += SPRINTF3 (cond_ptr, "%s)",
               argtyp [1] == LOCAL ? ",lval,lbts" : "");
            if (not_pending) *cond_ptr++ = ')';
            break;

         case IFKEY:
         case IFANY:
            if (argtyp [1] != OBJ && argtyp [1] != LOC && 
                argtyp [1] != VERB && argtyp [1] != VAR &&
                argtyp [1] != LOCAL)
                  gripe (tp [index], 
                     "Unsuitable IFKEY/IFANY argument type!");  
            if (tp [2] == NULL)
            {
               cond_ptr += SPRINTF3 (cond_ptr, "%sKEY(", 
                  (not_pending) ? "!" : "");
               if (argtyp [1] == VAR)
                  cond_ptr += SPRINTF3 (cond_ptr, "value[%d])", argval [1]);
               else if (argtyp [1] == LOCAL)
                  cond_ptr += SPRINTF3 (cond_ptr, "lval[%d])", argval [1]);
               else
                  cond_ptr += SPRINTF3 (cond_ptr, "%d)", argval [1]);
            }
            else
            {
               cond_ptr += SPRINTF4 (cond_ptr, "%s%s(", 
                  (not_pending) ? "!" : "",
                  (minor_type == IFKEY) ? "keyword" : "anyof");
               index = 0;
               while (tp [++index] != NULL)
               {
                  if (argtyp [1] == VAR)
                     cond_ptr += SPRINTF3 (cond_ptr, "value[%d],", 
                        argval [index]);
                  else if (argtyp [1] == LOCAL)
                     cond_ptr += SPRINTF3 (cond_ptr, "lval[%d],", 
                        argval [index]);
                  else
                     cond_ptr += SPRINTF3 (cond_ptr, "%d,", argval [index]);
               }
               cond_ptr += SPRINTF2 (cond_ptr, "-1)");
            }
            break;

         case QUERY:
            if (argtyp [1] != TEXT && argtyp [1] != VAR &&
               argtyp [1] != LOCAL)
                  gripe (tp [1], "Not reducible to text.");
            cond_ptr += SPRINTF3 (cond_ptr, "%squery(", 
               (not_pending) ? "!" : "");
            if (argtyp [1] == VAR)
               cond_ptr += SPRINTF3 (cond_ptr, "value[%d]", argval [1]);
            else if (argtyp [1] == LOCAL)
               cond_ptr += SPRINTF3 (cond_ptr, "lval[%d]", argval [1]);
            else
               cond_ptr += SPRINTF3 (cond_ptr, "%d", argval [1]);
            *cond_ptr++ = ')';
            break;

         case IFINRANGE:
            if (not_pending) *cond_ptr++ = '!';
            *cond_ptr++ = '(';
            if (argtyp [2] == VAR)
               cond_ptr += SPRINTF3 (cond_ptr, "value[%d]", argval [2]);
            else if (argtyp [2] == LOCAL)
               cond_ptr += SPRINTF3 (cond_ptr, "lval[%d]", argval [2]);
            else
               cond_ptr += SPRINTF3 (cond_ptr, "%d", argval [2]);
            if (argtyp [1] == VAR)
               cond_ptr += SPRINTF4 (cond_ptr, "<=value[%d] && value[%d]<=",
                  argval [1], argval [1]);
            else if (argtyp [1] == LOCAL)
               cond_ptr += SPRINTF4 (cond_ptr, "<=lval[%d] && lval[%d]<=",
                  argval [1], argval [1]);
            else
               cond_ptr += SPRINTF4 (cond_ptr,
                  "<=%d && %d<=", argval [1], argval [1]);
            if (argtyp [3] == VAR)
               cond_ptr += SPRINTF3 (cond_ptr, "value[%d]", argval [3]);
            else if (argtyp [3] == LOCAL)
               cond_ptr += SPRINTF3 (cond_ptr, "lval[%d]", argval [3]);
            else
               cond_ptr += SPRINTF3 (cond_ptr, "%d", argval [3]);
            *cond_ptr++ = ')';
            break;

         case IFIS:
            cond_ptr += SPRINTF3 (cond_ptr, "%s(", not_pending ? "!" : "");
            if (argtyp [1] != VAR && argtyp [1] != LOCAL)
               gripe (tp[1], "not a variable.");
            index = 1;
            while (tp [++index] != NULL)
            {
               if (argtyp [index] > TEXT)
                  gripe (tp [index], "Not a referrable entity.");
               cond_ptr += SPRINTF3 
                  (cond_ptr, "%s", index > 2 ? "||" : "");
               if (argtyp [1] == VAR)
                  cond_ptr += SPRINTF4 (cond_ptr, "value[%d]==%d", argval [1],
                     argval [index]);
               else if (argtyp [1] == LOCAL)
                  cond_ptr += SPRINTF4 (cond_ptr, "lval[%d]==%d", argval [1],
                     argval [index]);
            }
            cond_ptr += SPRINTF2 (cond_ptr, ")");
            break;

         case IFHTML:
         case IFCGI:
         case IFDOALL:
            cond_ptr += SPRINTF4 (cond_ptr, "%stest(\"%s\")",
               (not_pending) ? "!" : "", tp [0] + 2);
            break;
            
         case IFTYPED:
            cond_ptr += SPRINTF4 (cond_ptr, "%styped(\"%s\")",
               (not_pending) ? "!" : "", tp [1]);
            break;
            
         case IFLOC:
            if (argtyp [1] != OBJ && argtyp [1] != VAR &&
               argtyp [1] != LOCAL)
                  gripe (tp [1], "Not reducible to an object.");
            cond_ptr += SPRINTF3 (cond_ptr, "%s(", not_pending ? "!" : "");
            index = 1;
            while (tp [++index] != NULL)
            {
               if (argtyp [index] != OBJ && argtyp [index] != LOC &&
                   argtyp [index] != VAR && argtyp [index] != LOCAL)
                      gripe (tp [index], "not reducible to a location.");
               cond_ptr += SPRINTF3 (cond_ptr, "%slocation[", 
                  index > 2 ? "||" : "");
               if (argtyp [1] == VAR)
                  cond_ptr += SPRINTF3 (cond_ptr, "value[%d]]==", argval [1]);
               else if (argtyp [1] == LOCAL)
                  cond_ptr += SPRINTF3 (cond_ptr, "lval[%d]]==", argval [1]);
               else
                  cond_ptr += SPRINTF3 (cond_ptr, "%d]==", argval [1]);
               if (argtyp [index] == VAR)
                  cond_ptr += SPRINTF3 (cond_ptr, "value[%d]", 
                     argval [index]);
               else if (argtyp [index] == LOCAL)
                  cond_ptr += SPRINTF3 (cond_ptr, "lval[%d]", 
                     argval [index]);
               else
                  cond_ptr += SPRINTF3 (cond_ptr, "%d", argval [index]);
            }
            cond_ptr += SPRINTF2 (cond_ptr, ")");
            break;

         case AND:
            *cond_ptr++ = ')';
            ifs_pending++;
            cond_ptr += SPRINTF2 (cond_ptr, " &&\n   ");
            conjunction_pending = TRUE;
            break;

         case OR:
         case XOR:
            *cond_ptr++ = ')';
            ifs_pending++;
            cond_ptr += SPRINTF3 (cond_ptr, " %s\n   ",
               (minor_type == OR) ? "||" : "^");
            conjunction_pending = TRUE;
            break;

         case NOT:
            not_pending = TRUE;
            break;

         case ELSE:
            fprintf (code_file, "   } else {\n");
            break;
         
         case OTHERWISE:
            fprintf (code_file, "   } else ");
            brace_count--;
            break;

         case EOI:
            deprecate ("EOI", 11, 0);
         case EOT:
            if (minor_type == EOT) deprecate ("EOT", 11, 0);
         case FIN:
            if (minor_type != EOT && --brace_count < 0)
            {
               if (style <= 1) break;
               gripe (tp [0], "Too many code closures.");
            }
            if (!brace_pending)
               fprintf (code_file, "   ");
            if (minor_type == EOT)
            {
               loop_count = 0;
               brace_pending = FALSE;
               while (brace_count--)
                  fputc ('}', code_file);
               brace_count = 0;
               fputc ('\n', code_file);
            }
            else
            {
               fputc ('}', code_file);
               brace_pending = TRUE;
               if (minor_type == EOI)
                  loop_count--;
            }
            break;

         case ITOBJ:
            if (argtyp [1] != VAR && argtyp [1] != LOCAL)
               gripe (tp [1], "Not a variable.");
            brace_count++;
            loop_count++;
            if (argtyp [1] == VAR)
               fprintf (code_file, "   *bitword(%d)= -1; value[%d]=",
                  argval [1], argval [1]);
            else if (argtyp [1] == LOCAL)
               fprintf (code_file, "   lbts[%d*VARSIZE]=-1; lval[%d]=",
                  argval [1], argval [1]);
            fprintf (code_file, "FOBJ-1; while (++");
            if (argtyp [1] == VAR)
               fprintf (code_file, "value[%d]", argval [1]);
            else if (argtyp [1] == LOCAL)
               fprintf (code_file, "lval[%d]", argval [1]);
            fprintf (code_file, "<=LOBJ) {\n");
            index = 1;
            while (tp [++index] != NULL)
            {
               if (argtyp [index] != LOC && argtyp [index] != FLAGS &&
                   argtyp [index] != VAR && argtyp [index] != LOCAL)
                     gripe (tp [index], "Not reducible to a place or a flag!");
               fprintf (code_file, "   if (");
               if (argtyp [index] == FLAGS)
               {
                  if (argtyp [1] == VAR)
                     fprintf (code_file, "!bitest(value[%d],%d)",
                        argval [1], argval [index]);
                  else if (argtyp [1] == LOCAL)
                     fprintf (code_file, "!lbitest(%d,%d,lval,lbts)",
                        argval [1], argval [index]);
                  else
                     fprintf (code_file, "!bitest(%d,%d)",
                        argval [1], argval [index]);
               }
               else
               {
                  fprintf (code_file, "!isat(");
                  if (argtyp [1] == VAR)
                     fprintf (code_file, "value[%d]", 
                        argval [1]);
                  else if (argtyp [1] == LOCAL)
                     fprintf (code_file, "lval[%d]",
                        argval [1]);
                  else
                     fprintf (code_file, "%d", argval [1]);
                  fprintf (code_file, ",-1,-1,");
                  if (argtyp [index] == VAR)
                     fprintf (code_file, "value[%d])", 
                        argval [index]);
                  else if (argtyp [index] == LOCAL)
                     fprintf (code_file, "lval[%d])", 
                        argval [index]);
                  else
                     fprintf (code_file, "%d)", argval [index]);
              }
               fprintf (code_file, ") continue;\n");       
            }
            break;

         case ITLOC:
            if (argtyp [1] != VAR && argtyp [1] != LOCAL)
               gripe (tp [1], "Not a variable.");
            if (tp [2] && !tp [3])
               gripe (tp [0], 
                  "Wrong parameter count - should be 1 or 3.");
            if (tp [2] && argtyp [2] != LOC)
               gripe (tp [2], "Not a place!");
            if (tp[2] && tp [3] && argtyp [3] != LOC)
               gripe (tp [3], "Not a place!");
            brace_count++;
            loop_count++;
            if (argtyp [1] == VAR)
               fprintf (code_file, "   *bitword(%d)= -1; value[%d]=",
                  argval [1], argval [1]);
            else if (argtyp [1] == LOCAL)
               fprintf (code_file, "   lbts[%d*VARSIZE]=-1; lval[%d]=",
                  argval [1], argval [1]);
            if (tp [2])
               fprintf (code_file, "%d", argval [2] - 1);
            else
               fprintf (code_file, "FLOC-1");
            if (argtyp [1] == VAR)
               fprintf (code_file, "; while (++value[%d]<=", argval [1]);
            else if (argtyp [1] == LOCAL)
               fprintf (code_file, "; while (++lval[%d]<=", argval [1]);
            if (tp [2] && tp [3])
               fprintf (code_file, "%d) {\n", argval [3]);
            else
               fprintf (code_file, "LLOC) {\n");
            break;

         case ITERATE:
            if (argtyp [1] == VAR)
               fprintf (code_file, "   value[%d]=", argval [1]);
            else if (argtyp [1] == LOCAL)
               fprintf (code_file, "   lval[%d]=", argval [1]);
            else
               gripe (tp [1], "Not a variable.");
            if (argtyp [2] == VAR)
               fprintf (code_file, "value[%d]", argval [2]);
            else if (argtyp [2] == LOCAL)
               fprintf (code_file, "lval[%d]", argval [2]);
            else
               fprintf (code_file, "%d", argval [2]);
            if (argtyp [1] == VAR)
               fprintf (code_file, "-1; while(++value[%d]<=",
                  argval [1]);
            else
               fprintf (code_file, "-1; while(++lval[%d]<=",
                  argval [1]);
            if (argtyp [3] == VAR)
               fprintf (code_file, "value[%d]", argval [3]);
            else if (argtyp [3] == LOCAL)
               fprintf (code_file, "lval[%d]", argval [3]);
            else
               fprintf (code_file, "%d", argval [3]);
            fprintf (code_file, ") {\n");
            brace_count++;
            loop_count++;
            break;

         case NEXT:
         case BREAK:
            if (loop_count <= 0)
               gripe (tp [0], "Directive not inside a loop!");
            if (minor_type == NEXT)
               fprintf (code_file, "continue;\n");
            else
               fprintf (code_file, "break;\n");
            break;
            
         case PROCEED:
            fprintf (code_file, "   return;\n");
            break;

         case QUIT:
            fprintf (code_file, "   longjmp(loop_back,1);\n");
            break;

         case STOP:
            fprintf (code_file, "   finita();\n");
            break;

         case GET:
         case DROP:
         case APPORT:
            if (argtyp [1] != OBJ && argtyp [1] != VAR &&
               argtyp [1] != LOCAL)
                  gripe (tp [1], "Not reducible to an object.");
            if (minor_type == APPORT && argtyp [2] != LOC &&
               argtyp [2] != VAR && argtyp [2] != LOCAL)
                  gripe (tp [2], "Not reducible to a place.");
            fprintf (code_file, "   apport(");
            if (argtyp [1] == VAR)
               fprintf (code_file, "value[%d]", argval [1]);
            else if (argtyp [1] == LOCAL)
               fprintf (code_file, "lval[%d]", argval [1]);
            else
               fprintf (code_file, "%d", argval [1]);
            fputc (',', code_file);
            if (minor_type != APPORT)
               fprintf (code_file, "%s",
                  (minor_type == GET) ? "INHAND" : "HERE");
            else if (argtyp [2] == VAR)
               fprintf (code_file, "value[%d]", argval [2]);
            else if (argtyp [2] == LOCAL)
               fprintf (code_file, "lval[%d]", argval [2]);
            else
               fprintf (code_file, "%d", argval [2]);
            fprintf (code_file, ");\n");
            break;

         case GOTO:
            if (argtyp [1] == VAR)
               fprintf (code_file, "   move(value[%d],-1);\n",
                  argval [1]);
            else if (argtyp [1] == LOCAL)
               fprintf (code_file, "   move(lval[%d],-1);\n",
                  argval [1]);
            else if (argtyp [1] == LOC)
               fprintf (code_file, "   move(%d,-1);\n", argval [1]);
            else
               gripe (tp [1], "Not reducible to a place.");
            break;

/* Note that it is safe to negate the second argument to signal the lack of */
/* keyword arguments, since no text can have the value of 0, 1 or 2 - these */
/* being reserved by the translator, the first possible refno for anything  */
/* is 3! */

         case MOVE:
         case SMOVE:
            end_index = 1;
            while (tp [++end_index] != NULL);
            end_index -= (minor_type == MOVE) ? 1 : 2;
            if (argtyp [end_index] == LOC)
               fprintf (code_file, "   move(%d", argval [end_index]);
            else if (argtyp [end_index] == VAR)
               fprintf (code_file,
                  "   move(value[%d]", argval [end_index]);
            else if (argtyp [end_index] == LOCAL)
               fprintf (code_file,
                  "   move(lval[%d]", argval [end_index]);
            else
               gripe (tp [end_index], "Not reducible to a place.");
            if (minor_type == SMOVE)
            {
               index = end_index + 1;
               if (argtyp [index] == TEXT)
                  fprintf (code_file, ",%s%d",
                     (index == 2) ? "-" : "", argval [index]);
               else if (argtyp [index] == VAR)
                  fprintf (code_file, ",%svalue[%d]",
                     (index == 2) ? "-" : "", argval [index]);
               else if (argtyp [index] == LOCAL)
                  fprintf (code_file, ",%slval[%d]",
                     (index == 2) ? "-" : "", argval [index]);
               else
                  gripe (tp [index], "Not reducible to text.");
            }
            else
               fprintf (code_file, ",%d", (end_index == 1) ? -2: 0);
            end_index--;
            index = 0;
            while (++index <= end_index)
               fprintf (code_file, ",%s%d",
                  (index == end_index) ? "-" : "", argval [index]);
            fprintf (code_file, ");\n");
            break;

         case DICT:
            if (argtyp [1] > VERB &&
               (argtyp [1] != CONSTANT || argval [1] != 0))
                  gripe (tp [1], "Not verb, place or object.");
            if (argtyp [2] > LOC &&
               (argtyp [2] != CONSTANT || argval [2] != 0))
                  gripe (tp [2], "Not a place, object or zero.");
            if (argtyp [3] != FLAGS &&
               (argtyp [3] != CONSTANT || argval [3] != 0))
                  gripe (tp [3], "Not a flag name or zero.");
            if (tp [4] != NULL)
            {
               if (argtyp [4] != TEXT && 
                  (argtyp [4] != CONSTANT || argval [4] != 0))
                     gripe (tp [4], "Not a text or zero.");
               index = argval [4];
            }
            else
               index = 0;
            fprintf (code_file, "   voc (%d, %d, %d, %d);\n",
               argval [1], argval [2], argval [3], index);
            break;
            
         case RESPOND:
            if (argtyp [1] != OBJ && argtyp [1] != LOC && 
               argtyp [1] != VERB)
                  gripe (tp [1], 
                     "Not an object, place or verb!");
            fprintf (code_file, "   if (anyof(%d,", argval[1]);
            index = 2;
            while (tp [index] && argtyp [index] < TEXT && 
               argtyp [index] != VAR)
            {
               fprintf (code_file, "%d,", argval [index]);
               index++;
            }
            if (tp [index] == NULL)
               gripe ("", "Missing text argument!");
            fprintf (code_file, "-1))");
            ap [1]     = ap [index];     ap [2]     = ap [index + 1];
            tp [1]     = tp [index];     tp [2]     = tp [index + 1];
            argtyp [1] = argtyp [index]; argtyp [2] = argtyp [index + 1];
            argval [1] = argval [index]; argval [2] = argval [index + 1];
            minor_type = QUIP;
         case APPEND:
            if (minor_type == APPEND) /* I.e. we didn't fall through to here */
               fprintf (code_file, "glue_text();\n");
         case QUIP:
         case SAY:
         case VALUE:
         case DESCRIBE:
            if (minor_type == VALUE)
            {
               deprecate ("VALUE", 11, 0);
               type = 1;
            }
            else if (minor_type == DESCRIBE)
            {
               if (style >= 10)
                  type = 32;
               else
                  gripe (tp [0], "Only valid for STYLE >= 11!");
            }
            else if (minor_type == QUIP)
               type = 64;
            else
               type = 0;

            if (argtyp [1] != TEXT && argtyp [1] != VAR &&
                argtyp [1] != LOCAL && argtyp [1] != OBJ &&
                argtyp [1] != LOC)
                   gripe (tp[1], "Argument not reducible to a text!");
            if (argtyp [1] == TEXT && style >= 11)
            {
               if ((ap[1] -> text_type & QUALIFIER_ALLOWED) == 0 && 
                    ap[1] -> state_count < 2 && tp[2])
                  gripe (tp[1], "Redundant word qualifier.");
               else if (
                  (ap[1] -> text_type & QUALIFIER_MANDATORY) == QUALIFIER_MANDATORY 
                     && tp[2] == NULL)
                  gripe (tp[1], "Missing required word qualifier.");
	    }
            if (minor_type == DESCRIBE && 
               (argtyp [1] == VERB || argtyp [1] == TEXT ||
                argtyp [1] == LOC))
                  gripe (tp [1], "Not reducible to a detailed description!");
            if (argtyp [1] == VAR)
               type += 2;
               
            if (tp [2] != NULL)
            {
                type += 8;
                if (argtyp [2] <= LOC || argtyp [2] == VAR ||
                   (argtyp [2] == TEXT && minor_type == VALUE))
                   type +=4;
            }
            else
            {
               argtyp [2] = CONSTANT;
               argval [2] = 0;
            }

            fprintf (code_file, "   say(%d,", type);

/* Indirections through global variables will be sorted out by
 * the say() routine at run time.
 */
            if (argtyp [1] == LOCAL)
               fprintf (code_file, "lval[%d],", argval [1]);
            else
               fprintf (code_file, "%d,", argval [1]);
            if (argtyp [2] == LOCAL)
              fprintf (code_file, "lval[%d]);\n", argval [2]);
            else
              fprintf (code_file, "%d);\n", argval [2]);
            break;

         case SET:
            if      (argtyp [1] == VAR) ctype = 'V';
            else if (argtyp [1] == LOCAL)    ctype = 'L';
            else if (argtyp [1] == TEXT)     ctype = 'T';
            else if (argtyp [1] == OBJ ||
                     argtyp [1] == LOC)    ctype = 'E'; /* All else */
            else gripe (tp [1], "Not a value holder.");

            fprintf (code_file, "   set('%c',%d,", ctype, argval [1]);
            if      (argtyp [2] == CONSTANT || 
                     argtyp [2] == SYNONYM ||
                     argtyp [2] == STATE)    ctype = 'c';
            else if (argtyp [2] == VAR) ctype = 'v';
            else if (argtyp [2] == LOCAL)    ctype = 'l';
            else if (argtyp [2] == OBJ ||
                     argtyp [2] == LOC ||
                     argtyp [2] == TEXT)     ctype = 'e'; /* All else */
            else gripe (tp [2], "Not reducible to a value!");

            fprintf (code_file, "'%c',%d,", ctype, argval [2]);
            fprintf (code_file, "%s);\n",
               (argtyp [1] == LOCAL || argtyp [2] == LOCAL) ? 
                  "lval, lbts" : "NULL,NULL");
            break;

         case ADD:
         case SUBTRACT:
         case MULTIPLY:
         case DIVIDE:
         case INTERSECT:
            if (argtyp [1] > TEXT && argtyp [1] != LOCAL)
               gripe (tp [1], "Not a value holder.");
            if (argtyp [1] != LOCAL)
               fprintf (code_file, "   value[%d] ", argval [1]);
            else
               fprintf (code_file, "   lval[%d] ", argval [1]);
            if (minor_type == ADD) fputc ('+', code_file);
            if (minor_type == SUBTRACT) fputc ('-', code_file);
            if (minor_type == DIVIDE) fputc ('/', code_file);
            if (minor_type == MULTIPLY) fputc ('*', code_file);
            if (minor_type == INTERSECT) fputc ('&', code_file);
            if (argtyp [2] <= TEXT)
               fprintf (code_file, "= value[%d];\n",argval [2]);
            else if (argtyp [2] == LOCAL)
               fprintf (code_file, "= lval[%d];\n",argval [2]);
            else
               fprintf (code_file, "= %d;\n", argval [2]);
            break;

         case NEGATE:
            if (argtyp [1] > TEXT && argtyp [1] != LOCAL)
               gripe (tp [1], "Has no value to be negated.");
            if (argtyp [1] == LOCAL)
               fprintf (code_file, "   lval[%d] *= -1;\n",
                  argval [1]);
            else if (argtyp [1] < TEXT)
               fprintf (code_file, "   value[%d] *= -1;\n",
                  argval [1]);
            else if (argtyp [1] == TEXT)
               gripe (tp [1], "Texts cannot be negative.");
            else
               gripe (tp [1], "Bad negation.");
            break;

         case RANDOM:
            if (argtyp [1] > VAR && argtyp [1] != LOCAL)
               gripe (tp [1], "Not a randomisable holder.");
            if (argtyp [1] != LOCAL)
               fprintf (code_file, "   value[%d] = irand(", argval [1]);
            else
               fprintf (code_file, "   lval[%d] = irand(", argval [1]);
            if (argtyp [2] <= VAR)
               fprintf (code_file, "value[%d]", argval [2]);
            else if (argtyp [2] == LOCAL)
               fprintf (code_file, "lval[%d]", argval [2]);
            else
               fprintf (code_file, "%d", argval [2]);
            fprintf (code_file, ");\n");
            break;

         case CHOOSE:
            if (argtyp [1] > VAR && argtyp [1] != LOCAL)
               gripe (tp [1], "Not a value holder.");
            if (argtyp [1] != LOCAL)
               fprintf (code_file, "   value[%d] = irand(", argval [1]);
            else
               fprintf (code_file, "   lval[%d] = irand(", argval [1]);
            if (argtyp [3] == VAR)
               fprintf (code_file, "value[%d]-", argval [3]);
            else if (argtyp [3] == LOCAL)
               fprintf (code_file, "lval[%d]-", argval [3]);
            else
               fprintf (code_file, "%d-", argval [3]);
            if (argtyp [2] == VAR)
               fprintf (code_file, "value[%d]+1)+value[%d];\n",
                  argval [2], argval [2]);
            else if (argtyp [2] == LOCAL)
               fprintf (code_file, "lval[%d]+1)+lval[%d];\n",
                  argval [2], argval [2]);
            else
               fprintf (code_file, "%d+1)+%d;\n", argval [2], argval [2]);
            break;

         case DEPOSIT:       /* DEPOSIT variable {objptr|placeptr} */
            if (argtyp [1] != VAR && argtyp [1] != LOCAL)
               gripe (tp [1], "Not a variable.");
            if (argtyp [1] == VAR)
               fprintf (code_file, "value[value[%d]]=", argval [1]);
            else
               fprintf (code_file, "value[lval[%d]]=", argval [1]);
            if (argtyp [2] <= TEXT)
               fprintf (code_file, "value[%d];\n", argval[2]);
            else if (argtyp [2] == LOCAL)
               fprintf (code_file, 
                  "lbts[%d*VARSIZE]==-1?value[lval[%d]]:lval[%d];\n", 
                     argval[2], argval[2], argval[2]);
            else
               fprintf (code_file, "%d;\n", argval[2]);
            break;
            
         case LDA:           /* LDA variable refno */
         case EVAL:          /* EVAL variable refno */
         case LOCATE:        /* LOCATE variable {objptr} */
            if (argtyp [1] == VAR)
               fprintf (code_file, "   value[%d]=", argval[1]);
            else if (argtyp [1] == LOCAL)
               fprintf (code_file, "   lval[%d]=", argval[1]);
            else
               gripe (tp [1], "Not a variable.");

            if (minor_type == LDA)
            {
               if (argtyp [2] == LOCAL)
                  gripe (tp [2], "Cannot idirect to a local variable!");
               if (argtyp [2] > TEXT && argtyp [2] != CONSTANT)
                  gripe (tp [2], "Not an indirectable entity.");
               fprintf (code_file, "%d;", argval [2]);
               type = -1;
            }
            else if (minor_type == EVAL)
            {
               if (argtyp [2] != VAR && argtyp [2] != LOCAL)
                  gripe (tp [2], "Not a variable.");
               if (argtyp [2] == VAR)
                  fprintf (code_file, "value[value[%d]];",
                     argval [2]);
               else
                  fprintf (code_file, "value[lval[%d]];",
                     argval [2]);
               type = 0;
            }
            else if (minor_type == LOCATE)
            {
               if (argtyp [2] != VAR && argtyp [2] != LOCAL &&
                  argtyp [2] != OBJ)
                     gripe (tp [2], "Not reducible to an object.");
               if (argtyp [2] == VAR)
                  fprintf (code_file, "location[value[%d]];",
                     argval [2]);
               else if (argtyp [2] == LOCAL)
                  fprintf (code_file, "location[lval[%d]];",
                     argval [2]);
               else
                  fprintf (code_file, "location[%d];",
                     argval [2]);
               type = -1;
            }
            if (argtyp [1] == VAR)
               fprintf (code_file, "*bitword(%d)=%d;\n",
                  argval [1], type);
            else
               fprintf (code_file, "lbts[%d*VARSIZE]=%d;\n",
                  argval [1], type);
            break;

         case FLAG:      /* FLAG/UNFLAG {variable|place|obj} {flag} */
         case UNFLAG:
            if (argtyp [1] > VAR && argtyp [1] != LOCAL)
               gripe (tp [1], "Not a bit mask holder.");
            if ((argtyp [2] < CONSTANT && argtyp [2] != VAR) || 
               argtyp [2] == STATE)
                  gripe (tp [2],"Bit mask not a flag or constant.");
            if (argtyp [1] == LOCAL)
               fprintf (code_file, "   lbitmod(%d,'%c',", locals,
                  (minor_type == FLAG) ? 's' : 'c');
            else
               fprintf (code_file, "   bitmod('%c',", 
                  (minor_type == FLAG) ? 's' : 'c');
            if (argtyp [1] == VAR)
               fprintf (code_file, "evar(%d)", argval [1]);
            else
               fprintf (code_file, "%d", argval [1]);
            if (argtyp [2] == VAR)            
               fprintf (code_file, ",value[%d]", argval [2]);
            else if (argtyp [2] == LOCAL)            
               fprintf (code_file, ",lval[%d]", argval [2]);
            else
               fprintf (code_file, ",%d", argval [2]);
            fprintf (code_file, "%s);\n", 
               argtyp [1] == LOCAL ? ",lval,lbts" : "");
            break;

         case SVAR:  /* EXEC/SVAR {constant | variable} variable */
         case EXEC:
            if (argtyp [1] != VAR && argtyp [1] != CONSTANT &&
               argtyp [1] != LOCAL)
               gripe (tp [1], "Not a constant or variable.");
            if (tp [2] == NULL)
            {
               argval [2] = 0;         /* value[0] not actually in use! */
               argtyp [2] = VAR;
            }
            else if (argtyp [2] != VAR && argtyp [2] != LOCAL)
               gripe (tp [2], "Not a variable.");
            fprintf (code_file, "   %s(",
               (minor_type == SVAR) ? "svar" : "special");
            if (argtyp [1] == VAR)
               fprintf (code_file, "value[%d]", argval [1]);
            else if (argtyp [1] == LOCAL)
               fprintf (code_file, "lval[%d]", argval [1]);
            else
               fprintf (code_file, "%d", argval [1]);
            fprintf (code_file, ",&%s[%d]);\n",
               argtyp [2] == VAR ? "value" : "lval", argval [2]);
            break;

         case INPUT:
            if (tp [1] != NULL)
            {
               if (argtyp [1] != TEXT && argtyp [1] != VAR &&
                  argtyp [1] != LOCAL)
                  gripe (tp [1], "Not a text or variable.");
               fprintf (code_file, "   input(");
               if (argtyp [1] == VAR)
                  fprintf (code_file, "value[%d]);\n", argval [1]);
               else if (argtyp [1] == LOCAL)
                  fprintf (code_file, "lval[%d]);\n", argval [1]);
               else
                  fprintf (code_file, "%d);\n", argval [1]);
            }
            else
               fprintf (code_file, "   input(0);\n");
            break;

         case DEFAULT:      /* DEFAULT/DOALL {place | placeptr} [flag] */
         case DOALL:        /* DEFAULT/DOALL flag */
            if (argtyp [1] != LOC && argtyp [1] != VAR &&
               argtyp [1] != LOCAL && argtyp [1] != FLAGS &&
                  (style != 1 || argtyp [1] != SYNONYM))
                  gripe (tp [1], "Not a place, bit flag or variable.");
            if (tp [2] != NULL)
            {
               if (argtyp [1] == FLAGS)
                  gripe (tp [1], 
                     "Only second of two arguments may be a flag!");
               if (argtyp [2] != FLAGS)
                  gripe (tp [2], "Not a bit flag.");
            }
            fprintf (code_file, "   default_to(%d,",
               (minor_type == DEFAULT) ? 0 : 1);
            if (argtyp [1] == LOC)
               fprintf (code_file, "%d,", argval [1]);
            else if (argtyp [1] == VAR)
               fprintf (code_file, "value[%d],", argval [1]);
            else if (argtyp [1] == LOCAL)
               fprintf (code_file, "lval[%d],", argval [1]);
            else
               fprintf (code_file, "value[HERE],");
            index = (tp [2] == NULL) ? 1 : 2;
            fprintf (code_file, "%d);\n",
               (argtyp [index] == FLAGS) || argtyp [index] == SYNONYM ?
                  argval [index] : -1);
            break;

         case FLUSH:
            fprintf (code_file, "   flush_command();\n");
            break;

         case IFHAVE:   /* IFHAVE/IFHERE/IFNEAR {obj|objptr} [{flag|state}] */
         case IFNEAR:
         case IFHERE:
            if (argtyp [1] != OBJ && argtyp [1] != VAR &&
               argtyp [1] != LOCAL)
                  gripe (tp [1], "Not reducible to an object.");
            if (not_pending) *cond_ptr++ = '!';
            if (minor_type == IFHAVE)
               cond_ptr += SPRINTF2 (cond_ptr, "have");
            else if (minor_type == IFNEAR)
               cond_ptr += SPRINTF2 (cond_ptr, "isnear");
            else
               cond_ptr += SPRINTF2 (cond_ptr, "ishere");
            if (argtyp [1] == VAR)
               cond_ptr += SPRINTF3 (cond_ptr, "(value[%d],",
                  argval [1]);
            else if (argtyp [1] == LOCAL)
               cond_ptr += SPRINTF3 (cond_ptr, "(lval[%d],",
                  argval [1]);
            else
               cond_ptr += SPRINTF3 (cond_ptr, "(%d,", argval [1]);
            if (tp [2] == NULL)
               cond_ptr += SPRINTF2 (cond_ptr, "-1,-1)");
            else if (argtyp [2] == LOCAL)
               cond_ptr += SPRINTF3 (cond_ptr, "0,lval[%d]",
                  argval [2]);
            else if (argtyp [2] < CONSTANT)
               cond_ptr += SPRINTF3 (cond_ptr, "0,value[%d])", 
                  argval [2]);
            else
               cond_ptr += SPRINTF4 (cond_ptr, "%d,%d)", 
                  (argtyp [2] == FLAGS) ? 1 : 0, argval [2]);
            break;

         case RANDOMISE:
            if (argtyp [1] != OBJ && argtyp [1] != LOC && 
                argtyp [1] != TEXT)
                  gripe (tp [1], "Not a place, object or text.");
            if (argtyp [2] != CONSTANT)
               gripe (tp [2], "Not a constant.");
            if (argval [2] + 1 >= ap [1] -> state_count)
               gripe (tp [2], "Randomisation offset too high.");
            if (argval [2] < 0) 
               gripe (tp [2], "Negative offset not legal.");
            fprintf (code_file, "   value[%d] = %crand(%d - %d) + %d;\n",
               argval [1], argtyp [1] == TEXT ? 'j' : 'i',
                  ap [1] -> state_count, argval [2], argval [2]);
            break;

         case TIE:
            if (argtyp [1] == VERB || argtyp [1] > TEXT)
               gripe (tp[1],  "Not a global value holder!");
            index = 1;
            while (tp [++index] != NULL)
            {
               if (argtyp [index] != TEXT)
                  gripe (tp [index], "Not a text.");
               fprintf (code_file, "   tie (%d, %d);\n",
                  argval[index], argval [1]);
            }
            break;
            
         case FAKEARG:
         case FAKECOM:
            for (index = 1; index <= 2; index++)
               if (argtyp [index] > TEXT && argtyp [index] != LOCAL)
                  gripe (tp [index], 
                     "Not reducible to a place, object or verb");
            if (argtyp [1] == VAR)
               fprintf (code_file, "if (value[ARG1] == value[%d])", 
                  argval [1]);
            else if (argtyp [1] == LOCAL)
               fprintf (code_file, "if (value[ARG1] == lval[%d])", 
                  argval [1]);
            else
               fprintf (code_file, "if (value[ARG1] == %d)", argval [1]);
            if (minor_type == FAKECOM)
            {
               fprintf (code_file, " {value[ARG1]=");
               if (argtyp [2] == VAR)
                  fprintf (code_file, "value[%d]", argval [2]);
               else if (argtyp [2] == LOCAL)
                  fprintf (code_file, "lval[%d]", argval [2]);
               else
                  fprintf (code_file, "%d", argval [2]);
               fprintf (code_file, "; (void)fake(1,");
               if (argtyp [2] == VAR)
                  fprintf (code_file, "value[%d]);}\n", argval [2]);
               else if (argtyp [2] == LOCAL)
                  fprintf (code_file, "lval[%d]);}\n", argval [2]);
               else
                  fprintf (code_file, "%d);}\n", argval [2]);
            }
            else
            {
               fprintf (code_file, " value[ARG1]=");
               if (argtyp [2] == VAR)
                  fprintf (code_file, "value[%d];\n", argval [2]);
               else if (argtyp [2] == LOCAL)
                  fprintf (code_file, "lval[%d];\n", argval [2]);
               else
                  fprintf (code_file, "%d;\n", argval [2]);
	    }
            if (argtyp [1] == VAR)
               fprintf (code_file, "if (value[ARG2] == value[%d])", 
                  argval [1]);
            else if (argtyp [1] == LOCAL)
               fprintf (code_file, "if (value[ARG2] == lval[%d])", 
                  argval [1]);
            else
               fprintf (code_file, "if (value[ARG2] == %d)", argval [1]);
            if (minor_type == FAKECOM)
            {
               fprintf (code_file, " {value[ARG2]=");
               if (argtyp [2] == VAR)
                  fprintf (code_file, "value[%d]", argval [2]);
               else if (argtyp [2] == LOCAL)
                  fprintf (code_file, "lval[%d]", argval [2]);
               else
                  fprintf (code_file, "%d", argval [2]);
               fprintf (code_file, "; (void)fake(2,");
               if (argtyp [2] == VAR)
                  fprintf (code_file, "value[%d]);}\n", argval [2]);
               else if (argtyp [2] == LOCAL)
                  fprintf (code_file, "lval[%d]);}\n", argval [2]);
               else
                  fprintf (code_file, "%d);}\n", argval [2]);
            }
            else
            {
               fprintf (code_file, " value[ARG2]=");
               if (argtyp [2] == VAR)
                  fprintf (code_file, "value[%d];\n", argval [2]);
               else if (argtyp [2] == LOCAL)
                  fprintf (code_file, "lval[%d];\n", argval [2]);
               else
                  fprintf (code_file, "%d;\n", argval [2]);
	    }
            break;
            
         case CHECKPOINT:
            fprintf (code_file, 
               "   puts (\"=== Checkpoint: %s, line %d ===\");",
                  pathname[level], line_count[level]);
            break;
         
         case SAVE:
         case RESTORE:
            if (tp[1] == NULL)
            {
               gripe (tp[0], "Missing operation type indicator");
            }
            if (tp[2] == NULL)
            {
               tp[3] = NULL;
               argval[2] = 0;
            }
            else if (argtyp[2] != VAR && argtyp[2] != LOCAL)
            {
               gripe (tp [2], "Not a variable.");
            }
            if (strcmp (tp[1], "file") == 0)
            {
               type = (minor_type == SAVE) ? 1 : 2;
            }
            else if (strcmp (tp[1], "memory") == 0)
            {
               type = (minor_type == SAVE) ? 0 : 1;
               fprintf (code_file, "   %s[%d] = memstore (%d);\n",
                  (argtyp[2] == LOCAL) ? "lval" : "value", argval[2],
                     (minor_type == SAVE) ? 0 : 1);
               break;
            }
            else if (strcmp (tp[1], "command") == 0)
            {
               type = (minor_type == SAVE) ? 23 : 24;
            }
            else if (strcmp (tp[1], "value") == 0)
            {
               type = (minor_type == SAVE) ? 6 : 7;
            }
            else
            {
               gripe (tp[1], "Unknown save/restore operation type");
            }
            fprintf (code_file, "   special(%d, &%s[%d]);\n",
               type, argtyp [2] == LOCAL ? "lval" : "value", argval [2]);
            break;
            
           case VERBATIM:
              fprintf (code_file, "   verbatim(%d);\n", argval [1]);
              break;
              
           case UNDO:
           case REDO:
              fprintf (code_file, "   %s();\n", 
                 minor_type == UNDO ? "undo" : "redo");
              break;
              
/*         case DELETE:
 *            if (tp[1])
 *            {
 *               if (strcmp (tp[1], "value")
 *               {
 *               }
 *               else
 *               {
 *               }
 *            }
 *            else
 *            {
 *            }
 *            break;
 */
             
/*         case SAVEDLIST:
 *            if (tp[1] == NULL)
 *            {
 *            }
 *            if (strcmp (tp[1], "count")
 *            {
 *            }
 *            else if (strcmp (tp[1], "show")
 *            {
 *            }
 *            else
 *            {
 *            }
 *            break;
 */
             
         case DUMP:
            fprintf (code_file, " show_data();");
            break;

         case CALL:
            index = 1;        /* Point at proc name and fall through. */
         default:
            if (index == 0)
            {
               argtyp[0] = ap[0] -> type;
               argval[0] = ap[0] -> refno;
               write_ref (" prc ", tp [index]);
            }
            proc_index = index;
            if (argtyp [index] != PROCEDURE && argtyp [index] > VERB &&
               argtyp [index] != VAR && argtyp [index] != LOCAL)
                  gripe (tp [index], 
                     "Not a minor directive or a callable symbol.");
            if (argtyp [index] == PROCEDURE)
               fprintf (code_file, "   p%d(", argval [index]);
            else if (argtyp [index] == VAR)
               fprintf (code_file, 
                  "   if (value[%d]<LPROC && value[%d]>= 0) (*procs[value[%d]])(", 
                     argval [index], argval [index], argval[index]);
            else if (argtyp [index] == LOCAL)
               fprintf (code_file, 
                  "   if (lval[%d]<LPROC && lval[%d] >= 0) (*procs[lval[%d]])(",
                     argval [index], argval [index], argval[index]);
            else
               fprintf (code_file, "   (*procs[%d])(", argval [index]);
            if (argtyp [index] == PROCEDURE)
            {
               args_count = (ap [index] -> arg_count);
               for (++index; tp [index]; index++)
               { 
                  if (args_count == 0)
                     gripe (tp [proc_index], "too many arguments!");
                  if (argtyp [index] > TEXT && argtyp [index] != CONSTANT &&
                      argtyp [index] != LOCAL)
                        gripe (tp [index], "illegal argument type!");
                  fprintf (code_file, "%s", index == 2 ? "" : ",");
                  if (argtyp [index] != VAR)
                     fprintf (code_file, "%d,", 
                        argtyp [index] == CONSTANT ? 0 : -1);
                  else
                     fprintf (code_file, "*bitword(%d),", 
                        argval [index]);
                  if (argtyp [index] == VAR)
                     fprintf (code_file, "value[%d]", argval [index]);
                  else if (argtyp [index] == LOCAL)
                     fprintf (code_file, "lval[%d]", argval [index]);
                  else
                     fprintf (code_file, "%d", argval [index]);
                  args_count--;
               }
               if (args_count)
                  gripe (tp [proc_index], "too few arguments!");
	    }
            if (argtyp [proc_index] == VAR)
               fprintf (code_file, "); else pcall(value[%d]);\n", argval [proc_index]);
            else if (argtyp [proc_index] == LOCAL)
               fprintf (code_file, "); else pcall(lval[%d]);\n", argval [proc_index]);
	    else
               fprintf (code_file, ");\n");
            break;
      }
      if (minor_type < NOT) not_pending = FALSE;
   }

terminate:
   if (brace_count > 0)
   {
      if (style <= 1)
         while (brace_count--)
            fputc('}', code_file);
      else
         gripe ("", "Not enough code closures.");
   }
   if (brace_pending)
      fputc ('\n', code_file);
   return;
}

/*********************************************************************/

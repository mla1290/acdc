/* dominor.c (acdc) - copyleft Mike Arnautov 1990-2003.
 *
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
 * 01 Apr 94   MLA           Allowed range specification with ITOBJ/ITPLACE.
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

#include <stdio.h>
#include <ctype.h>

#include "acdc.h"
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
void dominor (char *prochead, char *proccond)
#else
void dominor (prochead, proccond)
char *prochead
char *proccond;
#endif
{
   int minor_type;
   int type;
   char ctype;
   int index;
   int end_index;
   int ifs_pending;
   int conjunction_pending;
   int not_pending;
   int multiple_pending;
   int brace_count;
   int brace_pending;
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

   struct node *parse ();
   int getconst ();
   extern void *realloc ();

   got_code = 0;
   ifs_pending = 0;
   brace_count = 0;
   conjunction_pending = FALSE;
   not_pending = FALSE;
   multiple_pending = FALSE;
   brace_pending = FALSE;
   locals = 0;

   while (TRUE)
   {
      line_status = EOL;
      if (getline (IGNORE_BLANK) == EOF)
         goto terminate;
      line_ptr = line;
      if (*line_ptr != ' ' && *line_ptr != '\t')
          break;

      if ((np = parse (MINOR)) == NULL)
         (void) gripe (tp [0], "Unknown minor directive.");
      minor_count++;

      minor_type = np -> refno;

      if (multiple_pending)
      {
         if (minor_type == multiple_pending)
            (void) fprintf (code_file, "\n   ");
         else
         {
            (void) fprintf (code_file, "-1)) return;\n");
            multiple_pending = FALSE;
         }
      }

      if (minor_type >= ELSE)
      {
         if (not_pending || conjunction_pending)
            (void) gripe ("", "Another conditional expected.");
         if (ifs_pending)
         {
            (void) fprintf (code_file, "   if ");
            if (minor_type == ELSE)
               (void) fprintf (code_file, "(!");
            while(ifs_pending-- > 0)
               (void) fputc ('(', code_file);
            cond_top = cond_ptr;
            cond_ptr = cond_buf_ptr - 1;
            while (++cond_ptr < cond_top)
               if (*cond_ptr)
                  (void) fputc (*cond_ptr, code_file);
            ifs_pending = 0;
            fputc (')', code_file);
            if (minor_type == ELSE || minor_type == OTHERWISE)
               fputc (')', code_file);
            if (minor_type != OTHERWISE)
            {
               (void) fprintf (code_file, " {\n");
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
                  (void) fprintf (code_file, "   if ");
                  while (ifs_pending-- > 0)
                     (void) fprintf (code_file, "(");
                  cond_top = cond_ptr;
                  cond_ptr = cond_buf_ptr - 1;
                  while (++cond_ptr < cond_top)
                     if (*cond_ptr)
                        (void) fputc (*cond_ptr, code_file);
                  (void) fprintf (code_file, ") {\n");
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
                        (void) gripe ("", "Unable to extend condition buffer.");
                  cond_ptr = cond_buf_ptr + (cond_ptr - old_cond_ptr);
                  cond_top = cond_buf_ptr + cond_buf_len - 128;
                  (void) printf ("Condition buffer size increased to %d.\n",
                     cond_buf_len);
               }
            }
         }
         else
            if (conjunction_pending && minor_type != NOT)
               (void) gripe ("", "Misplaced conjunction.");
      }

      if (minor_type != NOT)
         conjunction_pending = 0;

      if (brace_pending && minor_type != FIN && minor_type != EOI)
      {
         (void) fputc ('\n', code_file);
         brace_pending = FALSE;
      }

      if (minor_type == LOCAL)
      {
         if (got_code)
            (void) gripe ("", 
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
            if ((argval [index] = fndparam (tp [index])) != -1)
            {
               argtyp [index] = LOCAL;
               ap [index] = NULL;
            }
            else if (ap [index] = fndsymb (SYMBOL_OR_CONSTANT, tp [index]))
            {
               argval [index] = ap [index] -> refno;
               argtyp [index] = ap [index] -> type;
            }
            if (xref)
            {
               char *tname;
               switch (argtyp [index])
               {
                  case OBJECT:    tname = " obj "; break;
                  case PLACE:     tname = "place"; break;
                  case VERB:      tname = " vrb "; break;
                  case VARIABLE:  tname = " var "; break;
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
         if (trace & TRACE_ECHO && prochead && *prochead)
            fprintf (code_file, "   puts(\"*** %s ***\");\n", prochead);
         if (proccond && *proccond)
            fprintf (code_file, proccond);
      }
      
      switch (minor_type)
      {
         case KEYWORD:
         case ANYOF:
            if (multiple_pending != minor_type)
               (void) fprintf (code_file, "   if (!%s(",
                  (minor_type == KEYWORD) ? "keyword" : "anyof");
            multiple_pending = minor_type;
            index = 0;
            while (tp [++index] != NULL)
            {
               type = argtyp [index];
               if (type != OBJECT && type != PLACE && type != VERB)
                  (void) gripe (tp [index], "Bad multiple choice argument!");
               (void) fprintf (code_file, "%d,", argval [index]);
            }
            break;

         case HAVE:
         case NEAR:
         case HERE:
            (void) fprintf (code_file, "   if (");
            index = 1;
            while (tp [index])
            {
               if (argtyp [index] != OBJECT && 
                   argtyp [index] != VARIABLE &&
                   argtyp [index] != LOCAL)
                     (void) gripe (tp [index], "not reducible to an object.");
               if (minor_type == HAVE)
                  (void) fprintf (code_file, "!have");
               else if (minor_type == NEAR)
                  (void) fprintf (code_file, "!isnear");
               else
                  (void) fprintf (code_file, "!ishere");
               if (argtyp [index] == VARIABLE)
                  (void) fprintf (code_file, "(value[%d],-1,-1)", 
                     argval [index]);
               else if (argtyp [index] == LOCAL)
                  (void) fprintf (code_file, "(lval[%d],-1,-1)", 
                     argval [index]);
               else
                  (void) fprintf (code_file, "(%d,-1,-1)", argval [index]);
               if (tp [++index])
                  (void) fprintf (code_file, " || ");
            }
            (void) fprintf (code_file, ") return;\n");
            break;

         case ATPLACE:
            (void) fprintf (code_file, "   if (");
            index = 1;
            while (tp [index])
            {
               if ((argtyp [index] > VARIABLE && argtyp [index] != LOCAL)
                  || argtyp [index] == VERB)
               {
                  if (style == 1) 
                  {
                     index++;
                     continue;
                  }
                  (void) gripe (tp [index], "not reducible to a place or object.");
               }
               (void) fprintf (code_file, "value[HERE]!=");
               if (argtyp [index] == VARIABLE)
                  (void) fprintf (code_file, "value[%d]", 
                     argval [index]);
               else if (argtyp [index] == LOCAL)
                  (void) fprintf (code_file, "lval[%d]", 
                     argval [index]);
               else
                  (void) fprintf (code_file, "%d", argval [index]);
               if (tp [++index])
                  (void) fprintf (code_file, " && ");
            }
            (void) fprintf (code_file, ") return;\n");
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
            index = 0;
            while (tp [++index] != NULL)
            {
               if (argtyp [index] != OBJECT && argtyp [index] != PLACE &&
                   argtyp [index] != VARIABLE && argtyp [index] != LOCAL)
                      (void) gripe (tp [index], "not reducible to a location.");
               cond_ptr += SPRINTF3 
                  (cond_ptr, "%svalue[HERE]==", index > 1 ? "||" : "");
               if (argtyp [index] == VARIABLE)
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
            if (argtyp [1] <= VARIABLE)
               cond_ptr += SPRINTF3 (cond_ptr, "value[%d]", argval [1]);
            else if (argtyp [1] == LOCAL)
               cond_ptr += SPRINTF3 (cond_ptr, "lval[%d]", argval [1]);
            else
               cond_ptr += SPRINTF3 (cond_ptr, "%d", argval [1]);
            break;

         case IFFLAG:
            if (argtyp [1] > VERB && argtyp [1] != VARIABLE &&
               argtyp [1] != LOCAL)
                  (void) gripe (tp [1],"Wrong type for bit operation.");
            if ((argtyp [2] < CONSTANT && argtyp [2] != VARIABLE) &&
               argtyp [2] != LOCAL || argtyp [2] == STATE)
                  (void) gripe (tp [2],
                     "Not reducible to a flag.");
            if (not_pending) cond_ptr += SPRINTF2 (cond_ptr, "!(");
            if (argtyp [1] == VARIABLE)
               cond_ptr += SPRINTF3 (cond_ptr, "bitest(evar(%d),", argval [1]);
            else if (argtyp [1] == LOCAL)
               cond_ptr += SPRINTF3 (cond_ptr, "lbitest(%d,", argval [1]);
            else
               cond_ptr += SPRINTF3 (cond_ptr, "bitest(%d,", argval [1]);
            if (argtyp [2] == VARIABLE)
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
            if (tp [2] == NULL)
            {
               if (argtyp [1] != OBJECT && argtyp [1] != PLACE && 
                  argtyp [1] != VERB)
                    (void) gripe (tp [index], 
                       "Unsuitable IFKEY/IFANY argument type!");  
               cond_ptr += SPRINTF4 (cond_ptr, "%sKEY(%d)", 
                  (not_pending) ? "!" : "", argval [1]);
            }
            else
            {
               cond_ptr += SPRINTF4 (cond_ptr, "%s%s(", 
                  (not_pending) ? "!" : "",
                  (minor_type == IFKEY) ? "keyword" : "anyof");
               index = 0;
               while (tp [++index] != NULL)
                  cond_ptr += SPRINTF3 (cond_ptr, "%d,", argval [index]);
               cond_ptr += SPRINTF2 (cond_ptr, "-1)");
            }
            break;

         case QUERY:
            if (argtyp [1] != TEXT && argtyp [1] != VARIABLE &&
               argtyp [1] != LOCAL)
                  (void) gripe (tp [1], "Not reducible to text.");
            cond_ptr += SPRINTF3 (cond_ptr, "%squery(", 
               (not_pending) ? "!" : "");
            if (argtyp [1] == VARIABLE)
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
            if (argtyp [2] == VARIABLE)
               cond_ptr += SPRINTF3 (cond_ptr, "value[%d]", argval [2]);
            else if (argtyp [2] == LOCAL)
               cond_ptr += SPRINTF3 (cond_ptr, "lval[%d]", argval [2]);
            else
               cond_ptr += SPRINTF3 (cond_ptr, "%d", argval [2]);
            if (argtyp [1] == VARIABLE)
               cond_ptr += SPRINTF4 (cond_ptr, "<=value[%d] && value[%d]<=",
                  argval [1], argval [1]);
            else if (argtyp [1] == LOCAL)
               cond_ptr += SPRINTF4 (cond_ptr, "<=lval[%d] && lval[%d]<=",
                  argval [1], argval [1]);
            else
               cond_ptr += SPRINTF4 (cond_ptr,
                  "<=%d && %d<=", argval [1], argval [1]);
            if (argtyp [3] == VARIABLE)
               cond_ptr += SPRINTF3 (cond_ptr, "value[%d]", argval [3]);
            else if (argtyp [3] == LOCAL)
               cond_ptr += SPRINTF3 (cond_ptr, "lval[%d]", argval [3]);
            else
               cond_ptr += SPRINTF3 (cond_ptr, "%d", argval [3]);
            *cond_ptr++ = ')';
            break;

         case IFIS:
            if (argtyp [2] > TEXT)
               (void) gripe (tp [2], "Not a referrable entity.");
            if (argtyp [1] == VARIABLE)
               cond_ptr += SPRINTF5 (cond_ptr, "value[%d]%s%d", argval [1],
                  (not_pending) ? "!=" : "==", argval [2]);
            else if (argtyp [1] == LOCAL)
               cond_ptr += SPRINTF5 (cond_ptr, "lval[%d]%s%d", argval [1],
                  (not_pending) ? "!=" : "==", argval [2]);
            else
               (void) gripe (tp [1], "Not a variable.");
            break;

         case IFLOC:
            if (argtyp [1] != OBJECT && argtyp [1] != VARIABLE &&
               argtyp [1] != LOCAL)
                  (void) gripe (tp [1], "Not reducible to an object.");
            cond_ptr += SPRINTF3 (cond_ptr, "%s(", not_pending ? "!" : "");
            index = 1;
            while (tp [++index] != NULL)
            {
               if (argtyp [index] != OBJECT && argtyp [index] != PLACE &&
                   argtyp [index] != VARIABLE && argtyp [index] != LOCAL)
                      (void) gripe (tp [index], "not reducible to a location.");
               cond_ptr += SPRINTF3 (cond_ptr, "%slocation[", 
                  index > 2 ? "||" : "");
               if (argtyp [1] == VARIABLE)
                  cond_ptr += SPRINTF3 (cond_ptr, "value[%d]]==", argval [1]);
               else if (argtyp [1] == LOCAL)
                  cond_ptr += SPRINTF3 (cond_ptr, "lval[%d]]==", argval [1]);
               else
                  cond_ptr += SPRINTF3 (cond_ptr, "%d]==", argval [1]);
               if (argtyp [index] == VARIABLE)
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
            (void) fprintf (code_file, "   } else {\n");
            break;
         
         case OTHERWISE:
            (void) fprintf (code_file, "   } else ");
            brace_count--;
            break;

         case FIN:
         case EOI:
         case EOT:
            if (minor_type != EOT && --brace_count < 0)
            {
               if (style == 1) break;
               (void) gripe (tp [0], "Too many code closures.");
            }
            if (!brace_pending)
               (void) fprintf (code_file, "   ");
            if (minor_type == EOT)
            {
               brace_pending = FALSE;
               while (brace_count--)
                  (void) fputc ('}', code_file);
               brace_count = 0;
               (void) fputc ('\n', code_file);
            }
            else
            {
               (void) fputc ('}', code_file);
               brace_pending = TRUE;
            }
            break;

         case ITOBJ:
            if (argtyp [1] != VARIABLE && argtyp [1] != LOCAL)
               (void) gripe (tp [1], "Not a variable.");
            brace_count++;
            if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, "   *bitword(%d)= -1; value[%d]=",
                  argval [1], argval [1]);
            else if (argtyp [1] == LOCAL)
               (void) fprintf (code_file, "   lbts[%d*VARSIZE]=-1; lval[%d]=",
                  argval [1], argval [1]);
            (void) fprintf (code_file, "FOBJECT-1; while (++");
            if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, "value[%d]", argval [1]);
            else if (argtyp [1] == LOCAL)
               (void) fprintf (code_file, "lval[%d]", argval [1]);
            (void) fprintf (code_file, "<=LOBJECT) {\n");
            index = 1;
            while (tp [++index] != NULL)
            {
               if (argtyp [index] != PLACE && argtyp [index] != FLAGS &&
                   argtyp [index] != VARIABLE && argtyp [index] != LOCAL)
                     gripe (tp [index], "Not reducible to a place or a flag!");
               (void) fprintf (code_file, "   if (");
               if (argtyp [index] == FLAGS)
               {
                  if (argtyp [1] == VARIABLE)
                     (void) fprintf (code_file, "!bitest(value[%d],%d)",
                        argval [1], argval [index]);
                  else if (argtyp [1] == LOCAL)
                     (void) fprintf (code_file, "!lbitest(%d,%d,lval,lbts)",
                        argval [1], argval [index]);
                  else
                     (void) fprintf (code_file, "!bitest(%d,%d)",
                        argval [1], argval [index]);
               }
               else
               {
                  fprintf (code_file, "!isat(");
                  if (argtyp [1] == VARIABLE)
                     (void) fprintf (code_file, "value[%d]", 
                        argval [1]);
                  else if (argtyp [1] == LOCAL)
                     (void) fprintf (code_file, "lval[%d]",
                        argval [1]);
                  else
                     (void) fprintf (code_file, "%d", argval [1]);
                  fprintf (code_file, ",-1,-1,");
                  if (argtyp [index] == VARIABLE)
                     (void) fprintf (code_file, "value[%d])", 
                        argval [index]);
                  else if (argtyp [index] == LOCAL)
                     (void) fprintf (code_file, "lval[%d])", 
                        argval [index]);
                  else
                     (void) fprintf (code_file, "%d)", argval [index]);
/*
 *                 fprintf (code_file, "location[");
 *                 if (argtyp [1] == VARIABLE)
 *                    (void) fprintf (code_file, "value[%d]]!=", 
 *                       argval [1]);
 *                 else if (argtyp [1] == LOCAL)
 *                    (void) fprintf (code_file, "lval[%d]]!=",
 *                       argval [1]);
 *                 else
 *                    (void) fprintf (code_file, "%d]!=", argval [1]);
 *                 if (argtyp [index] == VARIABLE)
 *                    (void) fprintf (code_file, "evar(%d)", 
 *                       argval [index]);
 *                 else if (argtyp [index] == LOCAL)
 *                    (void) fprintf (code_file, "levar(%d,lval,lbts)", 
 *                       argval [index]);
 *                 else
 *                    (void) fprintf (code_file, "%d", argval [index]);
 */
              }
               (void) fprintf (code_file, ") continue;\n");       
            }
            break;

         case ITPLACE:
            if (argtyp [1] != VARIABLE && argtyp [1] != LOCAL)
               (void) gripe (tp [1], "Not a variable.");
            if (tp [2] && !tp [3])
               (void) gripe (tp [0], 
                  "Wrong parameter count - should be 1 or 3.");
            if (tp [2] && argtyp [2] != PLACE)
               (void) gripe (tp [2], "Not a place!");
            if (tp[2] && tp [3] && argtyp [3] != PLACE)
               (void) gripe (tp [3], "Not a place!");
            brace_count++;
            if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, "   *bitword(%d)= -1; value[%d]=",
                  argval [1], argval [1]);
            else if (argtyp [1] == LOCAL)
               (void) fprintf (code_file, "   lbts[%d*VARSIZE]=-1; lval[%d]=",
                  argval [1], argval [1]);
            if (tp [2])
               (void) fprintf (code_file, "%d", argval [2] - 1);
            else
               (void) fprintf (code_file, "FPLACE-1");
            if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, "; while (++value[%d]<=", argval [1]);
            else if (argtyp [1] == LOCAL)
               (void) fprintf (code_file, "; while (++lval[%d]<=", argval [1]);
            if (tp [2] && tp [3])
               (void) fprintf (code_file, "%d) {\n", argval [3]);
            else
               (void) fprintf (code_file, "LPLACE) {\n");
            break;

         case ITERATE:
            if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, "   value[%d]=", argval [1]);
            else if (argtyp [1] == LOCAL)
               (void) fprintf (code_file, "   lval[%d]=", argval [1]);
            else
               (void) gripe (tp [1], "Not a variable.");
            if (argtyp [2] == VARIABLE)
               (void) fprintf (code_file, "value[%d]", argval [2]);
            else if (argtyp [2] == LOCAL)
               (void) fprintf (code_file, "lval[%d]", argval [2]);
            else
               (void) fprintf (code_file, "%d", argval [2]);
            if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, "-1; while(++value[%d]<=",
                  argval [1]);
            else
               (void) fprintf (code_file, "-1; while(++lval[%d]<=",
                  argval [1]);
            if (argtyp [3] == VARIABLE)
               (void) fprintf (code_file, "value[%d]", argval [3]);
            else if (argtyp [3] == LOCAL)
               (void) fprintf (code_file, "lval[%d]", argval [3]);
            else
               (void) fprintf (code_file, "%d", argval [3]);
            (void) fprintf (code_file, ") {\n");
            brace_count++;
            break;

         case CALL:
            if (argtyp [1] != PROCEDURE && argtyp [1] > VERB &&
               argtyp [1] != VARIABLE && argtyp [1] != LOCAL)
                  (void) gripe (tp [1], "Not a CALLable symbol.");
            if (argtyp [1] == PROCEDURE)
               (void) fprintf (code_file, "   p%d(", argval [1]);
            else if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, 
                  "   (*procs[value[%d]])(", argval [1]);
            else if (argtyp [1] == LOCAL)
               (void) fprintf (code_file, 
                  "   (*procs[lval[%d]])(", argval [1]);
            else
               (void) fprintf (code_file, "   (*procs[%d])(", argval [1]);
            if (argtyp [1] == PROCEDURE)
            {
               args_count = -(ap [1] -> state_count);
               for (index = 2; tp [index]; index++)
               { 
                  if (args_count == 0)
                     (void) gripe (tp [1], "too many arguments!");
                  if (argtyp [index] > TEXT && argtyp [index] != CONSTANT &&
                      argtyp [index] != LOCAL)
                        (void) gripe (tp [index], "illegal argument type!");
                  (void) fprintf (code_file, "%s", index == 2 ? "" : ",");
                  if (argtyp [index] != VARIABLE)
                     (void) fprintf (code_file, "%d,", 
                        argtyp [index] == CONSTANT ? 0 : -1);
                  else
                     (void) fprintf (code_file, "*bitword(%d),", 
                        argval [index]);
                  if (argtyp [index] == VARIABLE)
                     (void) fprintf (code_file, "value[%d]", argval [index]);
                  else if (argtyp [index] == LOCAL)
                     (void) fprintf (code_file, "lval[%d]", argval [index]);
                  else
                     (void) fprintf (code_file, "%d", argval [index]);
                  args_count--;
               }
               if (args_count)
                  (void) gripe (tp [1], "too few arguments!");
	    }
            (void) fprintf (code_file, ");\n");
            break;

         case PROCEED:
            (void) fprintf (code_file, "   return;\n");
            break;

         case QUIT:
            (void) fprintf (code_file, "   longjmp(loop_back,1);\n");
            break;

         case STOP:
            (void) fprintf (code_file, "   finita();\n");
            break;

         case GET:
         case DROP:
         case APPORT:
            if (argtyp [1] != OBJECT && argtyp [1] != VARIABLE &&
               argtyp [1] != LOCAL)
                  (void) gripe (tp [1], "Not reducible to an object.");
            if (minor_type == APPORT && argtyp [2] != PLACE &&
               argtyp [2] != VARIABLE && argtyp [2] != LOCAL)
                  (void) gripe (tp [2], "Not reducible to a place.");
            (void) fprintf (code_file, "   apport(");
            if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, "value[%d]", argval [1]);
            else if (argtyp [1] == LOCAL)
               (void) fprintf (code_file, "lval[%d]", argval [1]);
            else
               (void) fprintf (code_file, "%d", argval [1]);
            (void) fputc (',', code_file);
            if (minor_type != APPORT)
               (void) fprintf (code_file, "%s",
                  (minor_type == GET) ? "INHAND" : "HERE");
            else if (argtyp [2] == VARIABLE)
               (void) fprintf (code_file, "value[%d]", argval [2]);
            else if (argtyp [2] == LOCAL)
               (void) fprintf (code_file, "lval[%d]", argval [2]);
            else
               (void) fprintf (code_file, "%d", argval [2]);
            (void) fprintf (code_file, ");\n");
            break;

         case GOTO:
            if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, "   move(value[%d],-1);\n",
                  argval [1]);
            else if (argtyp [1] == LOCAL)
               (void) fprintf (code_file, "   move(lval[%d],-1);\n",
                  argval [1]);
            else if (argtyp [1] == PLACE)
               (void) fprintf (code_file, "   move(%d,-1);\n", argval [1]);
            else
               (void) gripe (tp [1], "Not reducible to a place.");
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
            if (argtyp [end_index] == PLACE)
               (void) fprintf (code_file, "   move(%d", argval [end_index]);
            else if (argtyp [end_index] == VARIABLE)
               (void) fprintf (code_file,
                  "   move(value[%d]", argval [end_index]);
            else if (argtyp [end_index] == LOCAL)
               (void) fprintf (code_file,
                  "   move(lval[%d]", argval [end_index]);
            else
               (void) gripe (tp [end_index], "Not reducible to a place.");
            if (minor_type == SMOVE)
            {
               index = end_index + 1;
               if (argtyp [index] == TEXT)
                  (void) fprintf (code_file, ",%s%d",
                     (index == 2) ? "-" : "", argval [index]);
               else if (argtyp [index] == VARIABLE)
                  (void) fprintf (code_file, ",%svalue[%d]",
                     (index == 2) ? "-" : "", argval [index]);
               else if (argtyp [index] == LOCAL)
                  (void) fprintf (code_file, ",%slval[%d]",
                     (index == 2) ? "-" : "", argval [index]);
               else
                  (void) gripe (tp [index], "Not reducible to text.");
            }
            else
               (void) fprintf (code_file, ",%d", (end_index == 1) ? -2: 0);
            end_index--;
            index = 0;
            while (++index <= end_index)
               (void) fprintf (code_file, ",%s%d",
                  (index == end_index) ? "-" : "", argval [index]);
            (void) fprintf (code_file, ");\n");
            break;

         case DICT:
            if (argtyp [1] > VERB &&
               (argtyp [1] != CONSTANT || argval [1] != 0))
                  (void) gripe (tp [1], "Not verb, place or object.");
            if (argtyp [2] > PLACE &&
               (argtyp [2] != CONSTANT || argval [2] != 0))
                  (void) gripe (tp [2], "Not a place, object or zero.");
            if (argtyp [3] != FLAGS &&
               (argtyp [3] != CONSTANT || argval [3] != 0))
                  (void) gripe (tp [3], "Not a flag name or zero.");
            if (tp [4] != NULL)
            {
               if (argtyp [4] != TEXT && 
                  (argtyp [4] != CONSTANT || argval [4] != 0))
                     (void) gripe (tp [4], "Not a text or zero.");
               index = argval [4];
            }
            else
               index = 0;
            (void) fprintf (code_file, "   voc (%d, %d, %d, %d);\n",
               argval [1], argval [2], argval [3], index);
            break;
            
         case RESPOND:
            if (argtyp [1] != OBJECT && argtyp [1] != PLACE && 
               argtyp [1] != VERB)
                  (void) gripe (tp [1], 
                     "Not an object, place or verb!");
            (void) fprintf (code_file, "   if (anyof(%d,", argval[1]);
            index = 2;
            while (tp [index] && argtyp [index] < TEXT && 
               argtyp [index] != VARIABLE)
            {
               (void) fprintf (code_file, "%d,", argval [index]);
               index++;
            }
            if (tp [index] == NULL)
               (void) gripe ("", "Missing text argument!");
            (void) fprintf (code_file, "-1))");
            tp [1]     = tp [index];     tp [2]     = tp [index + 1];
            argtyp [1] = argtyp [index]; argtyp [2] = argtyp [index + 1];
            argval [1] = argval [index]; argval [2] = argval [index + 1];
            minor_type = QUIP;
 
         case SAY:
         case QUIP:
         case VALUE:
         case DESCRIBE:
            if (minor_type == VALUE)
               type = 1;
            else if (minor_type == DESCRIBE)
            {
               if (style >= 11)
                  type = 32;
               else
                  gripe (tp [0], "Only valid for STYLE >= 11!");
            }
            else if (minor_type == QUIP)
               type = 64;
            else
               type = 0;

            if (argtyp [1] > TEXT && argtyp [1] != LOCAL)
               gripe (tp [1], "Not reducible to a text!");
            if (minor_type == DESCRIBE && 
               (argtyp [1] == VERB || argtyp [1] == TEXT ||
                argtyp [1] == PLACE))
                  gripe (tp [1], "Not reducible to a detailed description!");
            if (argtyp [1] == VARIABLE)
               type += 2;
               
            if (tp [2] != NULL)
            {
                type += 8;
                if (argtyp [2] <= PLACE || argtyp [2] == VARIABLE ||
                   (argtyp [2] == TEXT && minor_type == VALUE))
                   type +=4;
            }
            else
               argval [2] = 0;

            (void) fprintf (code_file, "   say(%d,", type);

/* Indirections through global variables will be sorted out by
 * the say() routine at run time.
 */
            if (argtyp [1] == LOCAL)
               (void) fprintf (code_file, "lval[%d],", argval [1]);
            else
               (void) fprintf (code_file, "%d,", argval [1]);
            if (argtyp [2] == LOCAL)
              (void) fprintf (code_file, "lval[%d]);\n", argval [2]);
            else
              (void) fprintf (code_file, "%d);\n", argval [2]);
            break;

         case SET:
            if      (argtyp [1] == VARIABLE) ctype = 'V';
            else if (argtyp [1] == LOCAL)    ctype = 'L';
            else if (argtyp [1] == TEXT)     ctype = 'T';
            else if (argtyp [1] == OBJECT ||
                     argtyp [1] == PLACE)    ctype = 'E'; /* All else */
            else (void) gripe (tp [1], "Not a value holder.");

            (void) fprintf (code_file, "   set('%c',%d,", ctype, argval [1]);
            if      (argtyp [2] == CONSTANT ||
                     argtyp [2] == STATE)    ctype = 'c';
            else if (argtyp [2] == VARIABLE) ctype = 'v';
            else if (argtyp [2] == LOCAL)    ctype = 'l';
            else if (argtyp [2] == OBJECT ||
                     argtyp [2] == PLACE ||
                     argtyp [2] == TEXT)     ctype = 'e'; /* All else */
            else (void) gripe (tp [2], "Not reducible to a value!");

            (void) fprintf (code_file, "'%c',%d,", ctype, argval [2]);
            (void) fprintf (code_file, "%s);\n",
               (argtyp [1] == LOCAL || argtyp [2] == LOCAL) ? 
                  "lval, lbts" : "NULL,NULL");
            break;

         case ADD:
         case SUBTRACT:
         case MULTIPLY:
         case DIVIDE:
         case INTERSECT:
            if (argtyp [1] > TEXT && argtyp [1] != LOCAL)
               (void) gripe (tp [1], "Not a value holder.");
            if (argtyp [1] != LOCAL)
               (void) fprintf (code_file, "   value[%d] ", argval [1]);
            else
               (void) fprintf (code_file, "   lval[%d] ", argval [1]);
            if (minor_type == ADD) (void) fputc ('+', code_file);
            if (minor_type == SUBTRACT) (void) fputc ('-', code_file);
            if (minor_type == DIVIDE) (void) fputc ('/', code_file);
            if (minor_type == MULTIPLY) (void) fputc ('*', code_file);
            if (minor_type == INTERSECT) (void) fputc ('&', code_file);
            if (argtyp [2] <= TEXT)
               (void) fprintf (code_file, "= value[%d];\n",argval [2]);
            else if (argtyp [2] == LOCAL)
               (void) fprintf (code_file, "= lval[%d];\n",argval [2]);
            else
               (void) fprintf (code_file, "= %d;\n", argval [2]);
            break;

         case NEGATE:
            if (argtyp [1] > TEXT && argtyp [1] != LOCAL)
               (void) gripe (tp [1], "Has no value to be negated.");
            if (argtyp [1] == LOCAL)
               (void) fprintf (code_file, "   lval[%d] *= -1;\n",
                  argval [1]);
            else if (argtyp [1] < TEXT)
               (void) fprintf (code_file, "   value[%d] *= -1;\n",
                  argval [1]);
            else if (argtyp [1] == TEXT)
               (void) gripe (tp [1], "Texts cannot be negative.");
            else
               (void) gripe (tp [1], "Bad negation.");
            break;

         case RANDOM:
            if (argtyp [1] > VARIABLE && argtyp [1] != LOCAL)
               (void) gripe (tp [1], "Not a randomisable holder.");
            if (argtyp [1] != LOCAL)
               (void) fprintf (code_file, "   value[%d] = irand(", argval [1]);
            else
               (void) fprintf (code_file, "   lval[%d] = irand(", argval [1]);
            if (argtyp [2] <= VARIABLE)
               (void) fprintf (code_file, "value[%d]", argval [2]);
            else if (argtyp [2] == LOCAL)
               (void) fprintf (code_file, "lval[%d]", argval [2]);
            else
               (void) fprintf (code_file, "%d", argval [2]);
            (void) fprintf (code_file, ");\n");
            break;

         case CHOOSE:
            if (argtyp [1] > VARIABLE && argtyp [1] != LOCAL)
               (void) gripe (tp [1], "Not a value holder.");
            if (argtyp [1] != LOCAL)
               (void) fprintf (code_file, "   value[%d] = irand(", argval [1]);
            else
               (void) fprintf (code_file, "   lval[%d] = irand(", argval [1]);
            if (argtyp [3] == VARIABLE)
               (void) fprintf (code_file, "value[%d]-", argval [3]);
            else if (argtyp [3] == LOCAL)
               (void) fprintf (code_file, "lval[%d]-", argval [3]);
            else
               (void) fprintf (code_file, "%d-", argval [3]);
            if (argtyp [2] == VARIABLE)
               (void) fprintf (code_file, "value[%d]+1)+value[%d];\n",
                  argval [2], argval [2]);
            else if (argtyp [2] == LOCAL)
               (void) fprintf (code_file, "lval[%d]+1)+lval[%d];\n",
                  argval [2], argval [2]);
            else
               (void) fprintf (code_file, "%d+1)+%d;\n", argval [2], argval [2]);
            break;

         case DEPOSIT:       /* DEPOSIT variable {objptr|placeptr} */
            if (argtyp [1] != VARIABLE && argtyp [1] != LOCAL)
               (void) gripe (tp [1], "Not a variable.");
            if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, "value[value[%d]]=", argval [1]);
            else
               (void) fprintf (code_file, "value[lval[%d]]=", argval [1]);
            if (argtyp [2] <= TEXT)
               (void) fprintf (code_file, "value[%d];\n", argval[2]);
            else if (argtyp [2] == LOCAL)
               (void) fprintf (code_file, 
                  "lbts[%d*VARSIZE]==-1?value[lval[%d]]:lval[%d];\n", 
                     argval[2], argval[2], argval[2]);
            else
               (void) fprintf (code_file, "%d;\n", argval[2]);
            break;
            
         case LDA:           /* LDA variable refno */
         case EVAL:          /* EVAL variable refno */
         case LOCATE:        /* LOCATE variable {objptr} */
            if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, "   value[%d]=", argval[1]);
            else if (argtyp [1] == LOCAL)
               (void) fprintf (code_file, "   lval[%d]=", argval[1]);
            else
               (void) gripe (tp [1], "Not a variable.");

            if (minor_type == LDA)
            {
               if (argtyp [2] == LOCAL)
                  (void) gripe (tp [2], "Cannot idirect to a local variable!");
               if (argtyp [2] > TEXT && argtyp [2] != CONSTANT)
                  (void) gripe (tp [2], "Not an indirectable entity.");
               (void) fprintf (code_file, "%d;", argval [2]);
               type = -1;
            }
            else if (minor_type == EVAL)
            {
               if (argtyp [2] != VARIABLE && argtyp [2] != LOCAL)
                  (void) gripe (tp [2], "Not a variable.");
               if (argtyp [2] == VARIABLE)
                  (void) fprintf (code_file, "value[value[%d]];",
                     argval [2]);
               else
                  (void) fprintf (code_file, "value[lval[%d]];",
                     argval [2]);
               type = 0;
            }
            else if (minor_type == LOCATE)
            {
               if (argtyp [2] != VARIABLE && argtyp [2] != LOCAL &&
                  argtyp [2] != OBJECT)
                     (void) gripe (tp [2], "Not reducible to an object.");
               if (argtyp [2] == VARIABLE)
                  (void) fprintf (code_file, "location[value[%d]];",
                     argval [2]);
               else if (argtyp [2] == LOCAL)
                  (void) fprintf (code_file, "location[lval[%d]];",
                     argval [2]);
               else
                  (void) fprintf (code_file, "location[%d];",
                     argval [2]);
               type = -1;
            }
            if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, "*bitword(%d)=%d;\n",
                  argval [1], type);
            else
               (void) fprintf (code_file, "lbts[%d*VARSIZE]=%d;\n",
                  argval [1], type);
            break;

         case FLAG:      /* FLAG/UNFLAG {variable|place|obj} {flag} */
         case UNFLAG:
            if (argtyp [1] > VARIABLE && argtyp [1] != LOCAL)
               (void) gripe (tp [1], "Not a bit mask holder.");
            if ((argtyp [2] < CONSTANT && argtyp [2] != VARIABLE) || 
               argtyp [2] == STATE)
                  (void) gripe (tp [2],"Bit mask not a flag or constant.");
            if (argtyp [1] == LOCAL)
               (void) fprintf (code_file, "   lbitmod(%d,'%c',", locals,
                  (minor_type == FLAG) ? 's' : 'c');
            else
               (void) fprintf (code_file, "   bitmod('%c',", 
                  (minor_type == FLAG) ? 's' : 'c');
            if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, "evar(%d)", argval [1]);
            else
               (void) fprintf (code_file, "%d", argval [1]);
            if (argtyp [2] == VARIABLE)            
               (void) fprintf (code_file, ",value[%d]", argval [2]);
            else if (argtyp [2] == LOCAL)            
               (void) fprintf (code_file, ",lval[%d]", argval [2]);
            else
               (void) fprintf (code_file, ",%d", argval [2]);
            fprintf (code_file, "%s);\n", 
               argtyp [1] == LOCAL ? ",lval,lbts" : "");
            break;

         case SVAR:  /* EXEC/SVAR {constant | variable} variable */
         case EXEC:
            if (argtyp [1] != VARIABLE && argtyp [1] != CONSTANT &&
               argtyp [1] != LOCAL)
               (void) gripe (tp [1], "Not a constant or variable.");
            if (tp [2] == NULL)
            {
               argval [2] = 0;         /* value[0] not actually in use! */
               argtyp [2] = VARIABLE;
            }
            else if (argtyp [2] != VARIABLE && argtyp [2] != LOCAL)
               (void) gripe (tp [2], "Not a variable.");
            (void) fprintf (code_file, "   %s(",
               (minor_type == SVAR) ? "svar" : "special");
            if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, "value[%d]", argval [1]);
            else if (argtyp [1] == LOCAL)
               (void) fprintf (code_file, "lval[%d]", argval [1]);
            else
               (void) fprintf (code_file, "%d", argval [1]);
            (void) fprintf (code_file, ",&%s[%d]);\n",
               argtyp [2] == VARIABLE ? "value" : "lval", argval [2]);
            break;

         case INPUT:
            if (tp [1] != NULL)
            {
               if (argtyp [1] != TEXT && argtyp [1] != VARIABLE &&
                  argtyp [1] != LOCAL)
                  (void) gripe (tp [1], "Not a text or variable.");
               (void) fprintf (code_file, "   input(");
               if (argtyp [1] == VARIABLE)
                  (void) fprintf (code_file, "value[%d]);\n", argval [1]);
               else if (argtyp [1] == LOCAL)
                  (void) fprintf (code_file, "lval[%d]);\n", argval [1]);
               else
                  (void) fprintf (code_file, "%d);\n", argval [1]);
            }
            else
               (void) fprintf (code_file, "   input(0);\n");
            break;

         case DEFAULT:      /* DEFAULT/DOALL {place | placeptr} [flag] */
         case DOALL:        /* DEFAULT/DOALL flag */
            if (argtyp [1] != PLACE && argtyp [1] != VARIABLE &&
               argtyp [1] != LOCAL && argtyp [1] != FLAGS &&
                  (style != 1 || argtyp [1] != SYNONYM))
                  (void) gripe (tp [1], "Not a place, bit flag or variable.");
            if (tp [2] != NULL)
            {
               if (argtyp [1] == FLAGS)
                  (void) gripe (tp [1], 
                     "Only second of two arguments may be a flag!");
               if (argtyp [2] != FLAGS)
                  (void) gripe (tp [2], "Not a bit flag.");
            }
            (void) fprintf (code_file, "   default_to(%d,",
               (minor_type == DEFAULT) ? 0 : 1);
            if (argtyp [1] == PLACE)
               (void) fprintf (code_file, "%d,", argval [1]);
            else if (argtyp [1] == VARIABLE)
               (void) fprintf (code_file, "value[%d],", argval [1]);
            else if (argtyp [1] == LOCAL)
               (void) fprintf (code_file, "lval[%d],", argval [1]);
            else
               (void) fprintf (code_file, "value[HERE],");
            index = (tp [2] == NULL) ? 1 : 2;
            (void) fprintf (code_file, "%d);\n",
               (argtyp [index] == FLAGS) || argtyp [index] == SYNONYM ?
                  argval [index] : -1);
            break;

         case FLUSH:
            (void) fprintf (code_file, "   flush_command();\n");
            break;

         case IFHAVE:   /* IFHAVE/IFHERE/IFNEAR {obj|objptr} [{flag|state}] */
         case IFNEAR:
         case IFHERE:
            if (argtyp [1] != OBJECT && argtyp [1] != VARIABLE &&
               argtyp [1] != LOCAL)
                  (void) gripe (tp [1], "Not reducible to an object.");
            if (not_pending) *cond_ptr++ = '!';
            if (minor_type == IFHAVE)
               cond_ptr += SPRINTF2 (cond_ptr, "have");
            else if (minor_type == IFNEAR)
               cond_ptr += SPRINTF2 (cond_ptr, "isnear");
            else
               cond_ptr += SPRINTF2 (cond_ptr, "ishere");
            if (argtyp [1] == VARIABLE)
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
            if (argtyp [1] != OBJECT && argtyp [1] != PLACE && 
                argtyp [1] != TEXT)
                  (void) gripe (tp [1], "Not a place, object or text.");
            if (argtyp [2] != CONSTANT)
               (void) gripe (tp [2], "Not a constant.");
            if (argval [2] + 1 >= ap [1] -> state_count)
               (void) gripe (tp [2], "Randomisation offset too high.");
            if (argval [2] < 0) 
               (void) gripe (tp [2], "Negative offset not legal.");
            (void) fprintf (code_file, "   value[%d] = %crand(%d - %d) + %d;\n",
               argval [1], argtyp [1] == TEXT ? 'j' : 'i',
                  ap [1] -> state_count, argval [2], argval [2]);
            break;

         case TIE:
            if (argtyp [1] == VERB || argtyp [1] > TEXT)
               (void) gripe (tp[1],  "Not a global value holder!");
            index = 1;
            while (tp [++index] != NULL)
            {
               if (argtyp [index] != TEXT)
                  (void) gripe (tp [index], "Not a text.");
               (void) fprintf (code_file, "   tie (%d, %d);\n",
                  argval[index], argval [1]);
            }
            break;
            
         case FAKEARG:
         case FAKECOM:
            if (argtyp [1] != PLACE && argtyp [1] != OBJECT &&
               argtyp [1] != VERB)
                  (void) gripe (tp [1], "Not a place, object or verb.");
            if (argtyp [2] > TEXT && argtyp [2] != LOCAL)
                  (void) gripe (tp [2], 
                     "Not reducible to a place, object or verb");
            (void) fprintf (code_file, "if (value[ARG1] == %d)", argval [1]);
            if (minor_type == FAKECOM)
            {
               (void) fprintf (code_file, " {value[ARG1]=");
               if (argtyp [2] == VARIABLE)
                  (void) fprintf (code_file, "value[%d]", argval [2]);
               else if (argtyp [2] == LOCAL)
                  (void) fprintf (code_file, "lval[%d]", argval [2]);
               else
                  (void) fprintf (code_file, "%d", argval [2]);
               (void) fprintf (code_file, "; (void)fake(1,");
               if (argtyp [2] == VARIABLE)
                  (void) fprintf (code_file, "value[%d]);}\n", argval [2]);
               else if (argtyp [2] == LOCAL)
                  (void) fprintf (code_file, "lval[%d]);}\n", argval [2]);
               else
                  (void) fprintf (code_file, "%d);}\n", argval [2]);
            }
            else
            {
               (void) fprintf (code_file, " value[ARG1]=");
               if (argtyp [2] == VARIABLE)
                  (void) fprintf (code_file, "value[%d];\n", argval [2]);
               else if (argtyp [2] == LOCAL)
                  (void) fprintf (code_file, "lval[%d];\n", argval [2]);
               else
                  (void) fprintf (code_file, "%d;\n", argval [2]);
	    }
            (void) fprintf (code_file, "if (value[ARG2] == %d)", argval [1]);
            if (minor_type == FAKECOM)
            {
               (void) fprintf (code_file, " {value[ARG2]=");
               if (argtyp [2] == VARIABLE)
                  (void) fprintf (code_file, "value[%d]", argval [2]);
               else if (argtyp [2] == LOCAL)
                  (void) fprintf (code_file, "lval[%d]", argval [2]);
               else
                  (void) fprintf (code_file, "%d", argval [2]);
               (void) fprintf (code_file, "; (void)fake(2,");
               if (argtyp [2] == VARIABLE)
                  (void) fprintf (code_file, "value[%d]);}\n", argval [2]);
               else if (argtyp [2] == LOCAL)
                  (void) fprintf (code_file, "lval[%d]);}\n", argval [2]);
               else
                  (void) fprintf (code_file, "%d);}\n", argval [2]);
            }
            else
            {
               (void) fprintf (code_file, " value[ARG2]=");
               if (argtyp [2] == VARIABLE)
                  (void) fprintf (code_file, "value[%d];\n", argval [2]);
               else if (argtyp [2] == LOCAL)
                  (void) fprintf (code_file, "lval[%d];\n", argval [2]);
               else
                  (void) fprintf (code_file, "%d;\n", argval [2]);
	    }
            break;
            
         case CHECKPOINT:
            (void) fprintf (code_file, 
               "puts (\"=== Checkpoint: %s, line %d ===\");",
                  pathname[level], line_count[level]);
            break;
         
         default:
            (void) gripe (tp [0], "Unimplemented minor directive.");
      }
      if (minor_type < NOT) not_pending = FALSE;
   }

terminate:
   if (brace_count > 0)
   {
      if (style == 1)
         while (brace_count--)
            (void) fputc('}', code_file);
      else
         (void) gripe ("", "Not enough code closures.");
   }
   if (brace_pending)
      (void) fputc ('\n', code_file);
   return;
}

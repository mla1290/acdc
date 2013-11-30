/* domajor.c (acdc) - copyleft Mike Arnautov 1990-2013.
 *
 * 11 Jan 10   MLA           Renamed getline() to nextline() to avoid a
 *                           new gcc header clash.
 * 14 Jul 09   MLA           Fixed gcc --pedantic warnings.
 * 22 Oct 08   MLA           Bug: fixed handling of the DATE directive.
 * 15 Mar 08   MLA           Version 12 changes.
 * 06 May 07   MLA           Added deprecated warnings.
 * 14 Feb 04   MLA           Converted longs to ints.
 * 24 Mar 03   MLA           Added 4th arg to gettxt().
 * 09 Mar 03   MLA           Replaced trace with debug.
 * 23 Feb 03   MLA           Preserve mask value.
 * 21 Feb 03   MLA           bug: Display full version string.
 * 20 Feb 03   MLA           Chage to code file naming conventions.
 * 10 Mar 02   MLA           bug: Write xref for parameter declarations.
 * 13 Jan 02   MLA           Pass "hidden" type parameters.
 * 12 Jan 02   MLA           bug: construct own parameter names.
 * 10 Jan 02   MLA           dominor() now takes an argument.
 * 01 Jan 02   MLA           bug: only zap local vars at code chunk start.
 * 30 Dec 01   MLA           Added call parameters code.
 * 18 Nov 01   MLA           Tag autop files with 2 digits.
 * 17 Nov 01   MLA           Bug: Corrected title construction.
 * 04 Nov 01   MLA           Construct a better default name.
 * 14 Mar 01   MLA           Replaced old_style with style.
 * 28 Feb 01   MLA           Allowed assigned text type.
 * 10 Jan 01   MLA           Allowed for text type specification.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 31 Dec 98   MLA           Allowed array declarations.
 * 20 Mar 94   MLA           Tracing now done dynamically.
 * 18 Mar 94   MLA           Second arg to gettxt should be NULL, not 0!
 * 14 Mar 94   MLA           Corrected FLAGS type handling.
 * 12 Dec 91   MLA           Added the FRAGMENT directive.
 * 23 Feb 91   MLA           Allowed variable bit fields.
 * 30 Dec 90   MLA           Use voc_buf for intermediate vocabulary storage.
 * 29 Dec 90   MLA           Trapped for output file errors.
 * 27 Dec 90   MLA           Separated FLAGS and STATE from CONSTANT.
 * 26 Dec 90   MLA           STATE and FLAGS directives + new CONSTANT syntax.
 * 15 Sep 90   MLA           Initial coding.
 */

#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "acdc.h"
#include "const.h"
#include "line.h"
#include "symbol.h"
#include "major.h"
#include "source.h"
#include "text.h"
#include "output.h"
#include "param.h"
#include "game.h"

#ifdef __STDC__
int skip (int check_text)
#else
int skip (check_text)
int check_text;
#endif
{
   int qualifiers = 0;
   
   line_status = EOL;

   while (1)
   {
      if (nextline (IGNORE_BLANK) == EOF || (*line != ' ' && *line != '\t'))
         break;
      if (check_text == 1)
      {
         char *cptr = line;
         char lc = '\0';
         while (*cptr)
         {
            if (lc != '\\')
            {
               if (*cptr == '$' || *cptr == '#')
                  qualifiers |= QUALIFIER_MANDATORY;
               if (*cptr == '[' || *cptr == '{')
                  qualifiers |= QUALIFIER_ALLOWED;
               lc = *cptr;
            }
            else
               lc = '\0';
            cptr++;
         }
      }
      if (check_text == 2 && style >= 12)
      {
         char autoname [32];
         struct node *np;
         int got_end;
         char *cptr = line + 1;           /* We *know* first one is a blank! */

         while (*cptr == ' ' || *cptr == '\t')
            cptr++;
         while (* cptr && *cptr != ' ' && *cptr != '\t')
            cptr++;
         while (*cptr == ' ' || *cptr == '\t')
            cptr++;
         if (*cptr != '"')
         {
            line_status = EOL;
            continue;
         }
         sprintf (autoname, ".auto_text_%d_", ++inline_count);
         np = addsymb (SYMBOL, autoname, TEXT, type_counts[TEXT]++);
         got_end = 0;
         while (1)
         {
            char lc = '\0';
            cptr++;
            while (*cptr)
            {
               if (lc != '\\')
               {
                  lc = *cptr;
                  if (*cptr == '$' || *cptr == '#')
                     np -> text_type |= QUALIFIER_MANDATORY;
                  else if (*cptr == '[' || *cptr == '{')
                     np -> text_type |= QUALIFIER_ALLOWED;
                  if (*cptr == '"')
                  {
                     got_end = 1;
                     break;
                  }
               }
               else
                  lc = '\0';
               cptr++;
            }
            if (got_end)
            {
               line_status = EOL;
               break;
            }
            if (nextline (IGNORE_BLANK) == EOF || 
                (*line != ' ' && *line != '\t'))
            {
               line_status = BOL;
               break;
            }
            cptr = line;
         }
      }
      line_status = EOL;
   }
   return (qualifiers);
}

#ifdef __STDC__
void domajor (void)
#else
void domajor ()
#endif
{
   int index;
   struct node *np;
   int refno;
   int value;
   int flag_type;
   int major_type;
   int type;
   int next_arg;
   char *chr;
   char *tag;
   char dummy_text [10];
   char proc_name [10];
   char prochead [MAXLINE];
   char proccond [MAXLINE];
   char proctemp [MAXLINE];

   static int next_dummy;
   static int last_stage = -1;
   
#ifdef __STDC__
   extern void dominor (char *, char *);
   extern void opnsrc (char *, int);
   extern void organise (void);
   extern struct node *getnames (int, struct node *);
#else
   extern void dominor ();
   extern void opnsrc ();
   extern void organise ();
   extern struct node *getnames ();
#endif

   line_ptr = line;
   if (isspace (*line_ptr))
      gripe ("","Major directives must start in column 1!");

   if ((np = parse (MAJOR)) == NULL)
      gripe (tp [0], "Unknown major directive.");

   index = 0;
   major_type = np -> refno;

   if (stage != last_stage)
   {
      last_stage = stage;
      next_dummy = 0;
      if (stage == 1)
         organise ();
   }
   
   if (stage == 1)
   {
      switch (major_type)
      {
         case STYLE:
         case NAME:
         case GVERSION:
         case DATE:
         case AUTHOR:
         case GAMEID:
         case NOUN:
         case ADJECTIVE:
         case PREPOSITION:
         case VERB:
         case NOISE:
         case VAR:
         case ARRAY:
         case SYNONYM:
         case DEFINE:
            line_status = EOL;
            return;

         case INCLUDE:
         case CONCLUDE:
            break;
            
         case STATE:
         case CONSTANT:
         case FLAGS:
            skip (0);
            return;              /* Preserving line status as BOL! */
            
         case PROCEDURE:
         case ACTION:
         case AT:
         case INIT:
         case REPEAT:
            zapparam ();
            if (minor_count >= CODE_CHUNK)
            {
               clsfile (code_file, "Automatic code");
               sprintf (proc_name, "adv%02d.c", ++code_part);
               if ((code_file = openout (proc_name, "w")) == NULL)
                  gripe (proc_name, "Unable to open new code chunk.");
               fprintf (code_file, "#include \"adv0.h\"\n");
               fprintf (code_file, "#include \"adv3.h\"\n");
               minor_count = 0;
            }
            break;
   
         default:
            break;
      }
   }

   switch (major_type)
   {
      case INCLUDE:
      case CONCLUDE:
         recase (LOWERCASE, tp [1]);
         if (level == MAXLEVEL - 1)
            gripe ("", "Includes nested too deeply.");
         opnsrc (tp [1], major_type == INCLUDE ? 1 : 0);
         break;

      case NOISE:
         while (tp [++index] != NULL)
            storword (tp [index], NOISE, next_vocaddr);
         break;

      case ARRAY:
         np = addsymb (SYMBOL, tp [++index], VAR, type_counts[VAR]);
         if (xref)
            write_ref ("ARRAY", tp [index]);
         if (tp [++index] == NULL)
            gripe ("", "Missing array size specification.");
         if ((value = atoi (tp [index])) <= 0)
            gripe (tp [index], "Invalid array size specification.");
         if (tp [++index] != NULL)
            gripe ("", "Invalid array declaration.");
         np -> state_count = value;
         type_counts[VAR] += value;
         break;
         
      case VAR:
         while (tp [++index] != NULL)
         {
            np = addsymb (SYMBOL, tp [index], VAR, type_counts[VAR]++);
            if (xref)
               write_ref (" VAR ", tp [index]);
         }
         break;

      case SYNONYM:
         deprecate ("SYNONYM", 10, 1);
         if ((np = fndsymb (TESTSYMBOL, tp [1])) != NULL)
         {
            getnames (np -> type, np);
            break;
         }           /* Otherwise assume a constant and fall through! */
            
      case STATE:
      case CONSTANT:
      case FLAGS:
         index = 1;
         value = -1;
         flag_type = -1;
         if (major_type == FLAGS)
         {
            if (tp [1] == NULL)
               flag_type = LAST_DEFAULT_FLAG;
            else 
            {
               if ((np = fndsymb (SYMBOL, tp [1])) == NULL)
                  gripe (tp [1], "Unknown symbol used as a flag type.");
               flag_type = np -> refno;
               if (flag_type != OBJFLAG && flag_type != LOCFLAG && 
                  flag_type != VARFLAG)
                     gripe (tp [1], "Illegal flag type.");
               if (flag_type != VARFLAG)
                  value = LAST_DEFAULT_FLAG;
            }
            line_status = EOL;
            if ((line_status = nextline (IGNORE_BLANK)) == EOF)
               return;
            if (*line_ptr != ' ' && *line_ptr != '\t')
               gripe ("", "FLAGS directive has no flag declarations!");
            if (flag_type != VARFLAG && 
               flag_field_size [flag_type] > LAST_DEFAULT_FLAG)
                  gripe (tp [1], 
                     "Only one set of this flag type allowed!");
            parse (NONE);
            index = 0;
         }

         while (TRUE)
         {
            chr = tp [index];
            if (*chr == '+')
            {
               if (major_type != FLAGS && flag_type != OBJFLAG && 
                  flag_type != LOCFLAG)
                     gripe ("", 
                        "Only object and place flags may have vocab entries.");

/* Adjective flags not in use yet, so for the moment ignore the vocabulary
 * entry request.
 */
               chr++;
            }
            if (isdigit (*chr) || *chr == '-')
            {
               if (major_type == FLAGS)
                  gripe (tp [index], 
                     "FLAGS may not be set to explicit values!");
               if (tp [index + 1] == NULL)
                  gripe ("", "Missing symbolic name.");
               value = chrtobin (tp [index]);
            }
            else
            {
               value++;
               index--;
            }
            if (major_type == FLAGS)
               flag_field_size [flag_type] = value;
            else if (major_type == STATE)
               if (value < 0)
                  gripe ("", "Negative state values not legal!");

            while (tp [++index] != NULL)
            {
               np = addsymb (SYMBOL, tp [index], major_type, value);
               if (xref)
               {
                  if (major_type == FLAGS)
                     tag = " FLG ";
                  else if (major_type == STATE)
                     tag = "STATE";
                  else
                     tag = "CONST";
                  write_ref (tag, tp [index]);
               }
            }
            line_status = EOL;
            if ((line_status = nextline (IGNORE_BLANK)) == EOF)
               return;
            if (*line_ptr != ' ' && *line_ptr != '\t')
               break;
            parse (NONE);
            index = 0;
         }
         return;          /* Preserve line_status as BOL ! */
	 
      case STYLE:
         if (style > 0)
            gripe ("", "Repeated STYLE directive.");
         if (strcmp (tp[1], "old") == 0 || strcmp (tp[1], "original") == 0)
               style = 1;
         else
         {
            index = chrtobin (tp[2] ? tp[2] : tp[1]);
            if (index > 12)
               gripe ("",
                  "Style higher than current maximum of 12!");
            if (index < 10 && style != 1)
               gripe ("", "Style values from 2 to 9 are meaningless.");
            style = index;
         }
         break;
         
      case NAME:
         if (*title)
            gripe ("", "Repeated NAME directive.");
         strncpy (title, tp [1], sizeof (title) - 1);
         *(title + sizeof (title) - 1) = '\0';
         break;

      case GVERSION:
         if (*version)
            gripe ("", "Repeated VERSION directive.");
         strncpy (version, tp [1], sizeof (version) - 1);
         *(version + sizeof (version) - 1) = '\0';
         break;

      case DATE:
        if (*date)
           gripe ("", "Repeated DATE directive.");
        strncpy (date, tp [1], sizeof (date) - 1);
/*         if (tp [2])
 *         {
 *            strncat (date, " ", sizeof (date) - strlen (date) - 1);
 *            strncat (date, tp [2], sizeof (date) - strlen (date) - 1);
 *         }
 *         if (tp [3])
 *         {
 *            strncat (date, " ", sizeof (date) - strlen (date) - 1);
 *            strncat (date, tp [3], sizeof (date) - strlen (date) - 1);
 *         }
 */
         *(date + sizeof (date) - 1) = '\0';
         break;

      case AUTHOR:
         if (*author)
            gripe ("", "Repeated AUTHOR directive.");
         strncpy (author, tp [1], sizeof (author) - 1);
         *(author + sizeof (author) - 1) = '\0';
         break;

      case GAMEID:
         if (*gameid)
            gripe ("", "Repeated GAMEID directive.");
         strncpy (gameid, tp [1], 80);
         *(gameid + 79) = '\0';
          break;

      case NOUN:
      case ADJECTIVE:
      case PREPOSITION:
      case VERB:
         np = getnames (major_type, NULL);
         if (np -> type == ACTION)
            np -> type = major_type;
         else if (np -> type != VERB)
            gripe (tp [1], "Already declared as other than a command word.");
         np -> refno = type_counts[VERB]++;
         break;

      case OBJ:
      case LOC:
         line_status = EOL;
         if (stage == 0)
         {
            np = getnames (major_type, NULL);
            if (np -> type == AT || np -> type == ACTION || np -> type == WORD)
               np -> type = major_type;
            else if (np -> type != major_type)
               gripe (tp [1], "Alreday declared as other than a place or an object.");
            np -> refno = type_counts[major_type]++;
            skip (0);
	 }
         else
         {
            int description = 1;
            int max_states = 0;
            int states = 0;

            char ch = *tp[1];
            if (ch == '-' || ch == '+' || ch == '!')
               tp[1]++;
            np = fndsymb (SYMBOL, tp[1]);
            while (description > 0)
            {
               np -> text_addr [description - 1] = next_addr;
               description = gettxt (description, &states, NULL);
               if (max_states < states) max_states = states;
            }
            np -> state_count = max_states;
         }
         return;          /* Preserve line_status as BOL ! */

      case TEXT:
      case FRAGMENT:
         line_status = EOL;
         type = texttyp();

         if (type & FRAGMENT_TEXT)
            major_type = FRAGMENT;
         else if (major_type == FRAGMENT)
            type |= FRAGMENT_TEXT;
         
         if (tp[1] == NULL) /* Might have changed! */
         {
            sprintf (dummy_text, ".t%d", next_dummy++);
            tp [1] = dummy_text;
         }
      
         if (stage == 0)
         {
            np = addsymb (SYMBOL, tp [1], TEXT, type_counts[TEXT]++);
            if (xref && tp [1] != dummy_text)
               write_ref ((char *)(major_type == TEXT ? " TXT " : " FRG "), 
                  tp [1]);
            np -> text_type = type | skip (1);
         }
         else
         {
            np = fndsymb (SYMBOL, tp[2] ? tp[2] : tp[1]);
            np -> name_addr = next_addr; /* Sort of "name" */
            gettxt (0, &(np -> state_count), &(np -> text_type));
         }
         return;          /* preserve the BOL line_status! */

      case INIT:
      case REPEAT:
         tp [2] = NULL;
         tp [1] = major_type == INIT ? "INIT_PROC" : "REPEAT_PROC";

      case AT:
      case ACTION:
      case PROCEDURE:
         if (stage == 0)
         {
            if ((np = fndsymb (TESTSYMBOL, tp [1])) == NULL)
               np = addsymb (SYMBOL, tp [1], major_type, -1);
            else if (major_type == AT && np -> type != AT && 
               np -> type != OBJ && np -> type != LOC)
                  gripe (tp[1], "Already defined as other than a place or an object.");
            else if (major_type == ACTION && np -> type != ACTION &&
               np -> type != VERB && np -> type != OBJ && np -> type != LOC && 
                  np -> type != NOUN && np -> type != PREP && np -> type != ADJ)
                     gripe (tp[1], "Already defined as other than a command word.");

            if (major_type == PROCEDURE)
            {
               int args = 0;
               index = 2;
               while (tp [index++]) args++;
               if (np -> arg_count < 0)
                  np -> arg_count = args;
               else if (np -> arg_count != args)
                  gripe (tp[1], "Inconsistent procedure argument count!");
	    }

            (np -> proc_count)++;
            if (xref)
               write_ref (" PRC ", tp [1]);
            skip (2);
            return;
         }

         np = fndsymb (SYMBOL, tp[1]);
         if (major_type != PROCEDURE)
            (np -> used_count)++;
         if (debug)
            fprintf (code_file, "/* %s */\n", tp [1]);
         if (np -> proc_count > 1)
            (np -> proc_done)++;
         refno = np -> proc_base + (np -> proc_done);
         fprintf (code_file, "#ifdef __STDC__\nvoid p%d(", refno);
         next_arg = 0;
         if (tp [2] == NULL || major_type != PROCEDURE)
            fprintf (code_file, "void");
         else
         {
            for (index = 2; tp [index]; index++, next_arg++)
            {
               addparam (1, tp [index]);
               fprintf (code_file, "%sint typ%d,int par%d",
                  index == 2 ? "" : ",", index - 2, index - 2);
               if (xref)
                  write_ref (" ARG ", tp [index]);
            }
         }
         fprintf (code_file, ")\n#else\nvoid p%d(", refno);
         if (tp [2])
         {
            for (index = 2; tp [index]; index++)
               fprintf (code_file, "%styp%d,par%d",
                  index == 2 ? "" : ",", index - 2, index - 2);
            fprintf (code_file, ")\n");
            for (index = 2; tp [index]; index++)
               fprintf (code_file, "int typ%d;int par%d;\n",
                  index - 2, index - 2);
         }
         else
            fprintf (code_file, ")\n");
         fprintf (code_file, "#endif\n{\n");
         if (debug)
         {
            strcpy (prochead, tp [0]);
            for (index = 1; tp [index]; index++)
            {
               strcat (prochead, " ");
               strcat (prochead, tp [index]);
            }
         }
         else
            *prochead = '\0';
         
         *proccond = '\0';
         index = 1;
         if (major_type == ACTION)
         {
            while (tp [++index] != NULL)
            {
               np = fndsymb (SYMBOL, tp [index]);
               if (np -> type > VERB)
                  gripe (tp [index], "Not a verb, place or object.");
               sprintf (proctemp, "if (!KEY(%d)) return;\n", np -> refno);
               strcat (proccond, proctemp);
            }
	 }
         dominor (prochead, proccond);
         fprintf (code_file, "}\n");
         return;

      case DEFINE:
         deprecate ("DEFINE", 10, 1);
         index = 0;
         while (tp [++index] != NULL)
         {
            np = fndsymb (SYMBOL, tp [index]);
            if (np -> type != VAR)
            {
               if (np -> type != LOC)
                  gripe (tp [index], "Only places can be defined!");
               if (fndsymb (VOCAB, tp [index]) == NULL)
               {
                  struct node *vp = storword (tp [index], LOC, next_vocaddr);
                  vp -> symbol = np;
               }
               if (xref)
                  write_ref ("(LOC)", tp [index]);
            }
         }
         break;

      default:
         gripe (tp [0], "Unimplemented major directive.");
   }
   line_status = EOL;
   return;
}

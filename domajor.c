/* domajor.c (acdc) - copyleft Mike Arnautov 1990-2003.
 *
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
 * 18 Mar 94   MLA           Second arg to gettext should be NULL, not 0!
 * 14 Mar 94   MLA           Corrected FLAGS type handling.
 * 12 Dec 91   MLA           Added the FRAGMENT directive.
 * 23 Feb 91   MLA           Allowed variable bit fields.
 * 30 Dec 90   MLA           Use voc_buf for intermediate vocabulary storage.
 * 29 Dec 90   MLA           Trapped for output file errors.
 * 27 Dec 90   MLA           Separated FLAGS and STATE from CONSTANT.
 * 26 Dec 90   MLA           STATE and FLAGS directives + new CONSTANT syntax.
 * 15 Sep 90   MLA           Initial coding.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "acdc.h"
#include "const.h"
#include "line.h"
#include "symbol.h"
#include "major.h"
#include "source.h"
#include "text.h"
#include "output.h"
#include "param.h"

#ifdef __STDC__
void domajor (void)
#else
void domajor ()
#endif
{
   int index;
   struct node *np;
   struct node *parse ();
   int value;
   int flag_type;
   int major_type;
   int type;
   int proc;
   int next_arg;
   char *chr;
   char *tag;
   char dummy_text [10];
   char proc_name [10];
   struct proc_list *temp_list;
   char prochead [MAXLINE];
   char proccond [MAXLINE];
   char proctemp [MAXLINE];

   static int got_code = FALSE;

#ifdef __STDC__
   extern int chrtobin (char *);
   extern void dominor (char *, char *);
   extern void opnsrc (char *);
   extern void organise (void);
   extern void storword (char *, int, int, long);
   extern void getdesc (struct node *);
   extern struct node *getnames (int, struct node *);
   extern void *malloc (size_t);
#else
   extern int chrtobin ();
   extern void dominor ();
   extern void opnsrc ();
   extern void organise ();
   extern void storword ();
   extern void getdesc ();
   extern struct node *getnames ();
   extern void *malloc ();
#endif

   line_ptr = line;
   if (isspace (*line_ptr))
      (void) gripe ("","Major directives must start in column 1!");

   if ((np = parse (MAJOR)) == NULL)
      (void) gripe (tp [0], "Unknown major directive.");

   index = 0;
   major_type = np -> refno;

   switch (major_type)
   {
      case INCLUDE:
      case LIST:
      case NOLIST:
      case XREF:
      case NOXREF:
         break;

      case PROCEDURE:
      case ACTION:
      case AT:
      case INIT:
      case REPEAT:
         if (got_code == FALSE)
         {
            got_code = TRUE;
            organise ();
         }
         zapparam ();
         if (minor_count >= CODE_CHUNK)
         {
            (void) clsfile (code_file, "Automatic code");
            (void) sprintf (proc_name, "adv%02d.c", ++code_part);
            if ((code_file = openout (proc_name, "w")) == NULL)
               (void) gripe (proc_name, "Unable to open new code chunk.");
            (void) fprintf (code_file, "#include \"adv0.h\"\n");
            (void) fprintf (code_file, "#include \"adv3.h\"\n");
            minor_count = 0;
         }
         break;

      default:
         if (got_code)
            (void) gripe (tp [0],
               "All declarations must precede any code sections!");
   }

   switch (major_type)
   {
      case XREF:
         if (xref == FALSE)
            xref = TRUE;
         break;

      case NOXREF:
         if (xref == TRUE)
            xref = FALSE;
         break;
         
      case LIST:
         listing = TRUE;
         break;

      case NOLIST:
         listing = FALSE;
         break;

      case INCLUDE:
         recase (LOWERCASE, tp [1]);
         if (level == MAXLEVEL - 1)
            (void) gripe ("", "Includes nested too deeply.");
         opnsrc (tp [1]);
         break;

      case NOISE:
         while (tp [++index] != NULL)
            storword (tp [index], NOISE, -1, next_vocaddr);
         break;

      case ARRAY:
         (void) addsymb (SYMBOL, tp [++index], VARIABLE, 
            type_counts [VARIABLE]);
         if (xref)
            write_ref ("ARRAY", tp [index]);
         if (tp [++index] == NULL)
            (void) gripe ("", "Missing array size specification.");
         if ((value = atoi (tp [index])) <= 0)
            (void) gripe (tp [index], "Invalid array size specification.");
         if (tp [++index] != NULL)
            (void) gripe ("", "Invalid array declaration.");
         type_counts [VARIABLE] += value;
         break;
         
      case VARIABLE:
         while (tp [++index] != NULL)
         {
            (void) addsymb (SYMBOL, tp [index], VARIABLE,
               type_counts [VARIABLE]++);
            if (xref)
               write_ref (" VAR ", tp [index]);
         }
         break;

      case SYNONYM:
         if ((np = fndsymb (SYMBOL_OR_CONSTANT, tp [1])) != NULL)
         {
            (void) getnames (np -> type, np);
            break;
         }           /* Otherwise assume a constant and fall through! */

      case STATE:
 index = 1;
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
                  (void) gripe (tp [1], "Unknown symbol used as a flag type.");
               flag_type = np -> refno;
               if (flag_type != OBJFLAG && flag_type != PLACEFLAG && 
                  flag_type != VARFLAG)
                     (void) gripe (tp [1], "Illegal flag type.");
               if (flag_type != VARFLAG)
                  value = LAST_DEFAULT_FLAG;
            }
            line_status = EOL;
            if ((line_status = getline (IGNORE_BLANK)) == EOF)
               return;
            if (*line_ptr != ' ' && *line_ptr != '\t')
               (void) gripe ("", "FLAGS directive has no flag declarations!");
            if (flag_type != VARFLAG && 
               flag_field_size [flag_type] > LAST_DEFAULT_FLAG)
                  (void) gripe (tp [1], 
                     "Only one set of this flag type allowed!");
            (void) parse (NONE);
            index = 0;
         }

         while (TRUE)
         {
            chr = tp [index];
            if (*chr == '+')
            {
               if (major_type != FLAGS && flag_type != OBJFLAG && 
                  flag_type != PLACEFLAG)
                     (void) gripe ("", 
                        "Only object and place flags may have vocab entries.");

/* Adjective flags not in use yet, so for the moment ignore the vocabulary
 * entry request.
 */
               chr++;
            }
            if (isdigit (*chr) || *chr == '-')
            {
               if (major_type == FLAGS)
                  (void) gripe (tp [index], 
                     "FLAGS may not be set to explicit values!");
               if (tp [index + 1] == NULL)
                  (void) gripe ("", "Missing symbolic name.");
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
                  (void) gripe ("", "Negative state values not legal!");

            while (tp [++index] != NULL)
            {
               (void) addsymb (SYMBOL, tp [index], major_type, value);
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
            if ((line_status = getline (IGNORE_BLANK)) == EOF)
               return;
            if (*line_ptr != ' ' && *line_ptr != '\t')
               break;
            (void) parse (NONE);
            index = 0;
         }
         return;          /* Preserve line_status as BOL ! */

      case STYLE:
      case VERSION:
      case AUTHOR:
         (void) gripe (tp[0], "Descriptive keyword too late.");
         break;

      case VERB:
         (void) getnames (major_type, NULL);
         break;

      case OBJECT:
      case PLACE:
         np = getnames (major_type, NULL);
         line_status = EOL;
         getdesc (np);
         return;          /* Preserve line_status as BOL ! */

      case TEXT:
      case FRAGMENT:
         type = 0;
         if (tp [1])
         {
            if (strcmp (tp [1], "random") == 0) type = 1;
            if (strcmp (tp [1], "increment") == 0) type = 2;
            if (strcmp (tp [1], "cycle") == 0) type = 3;
            if (strcmp (tp [1], "cyclic") == 0) type = 3;
            if (strcmp (tp [1], "assigned") == 0) type = 4;
            if (type)
            {
               tp [1] = tp [2];
               if (tp [2]) tp [2] = NULL;
            }
            else if (tp [2])
               (void) gripe (tp [1], "Illegal typifier.");
         }
            
         if (tp [1] == NULL)
         {
            (void) sprintf (dummy_text, ".t%d", type_counts [TEXT]);
            tp [1] = dummy_text;
         }
         np = addsymb (SYMBOL, tp [1], TEXT, type_counts [TEXT]++);
         np -> body.text.name_addr = next_addr; /* Sort of "name" */
         if (xref && tp [1] != dummy_text)
            write_ref (major_type == TEXT ? " TXT " : " FRG ", tp [1]);
         line_status = EOL;
         (void) gettext (0, &(np -> state_count), major_type == FRAGMENT);
         np -> body.text.text_type = type;
         return;          /* preserve the BOL line_status! */

      case PROCEDURE:
         np = addsymb (SYMBOL, tp [1], PROCEDURE, next_procno);
         if (xref)
            write_ref (" PRC ", tp [1]);
         if (debug)
            (void) fprintf (code_file, "/* %s */\n", tp [1]);
         (void) fprintf (code_file, "#ifdef __STDC__\nvoid p%d(", next_procno);
         next_arg = 0;
         if (tp [2] == NULL)
            (void) fprintf (code_file, "void");
         else
         {
            for (index = 2; tp [index]; index++, next_arg++)
            {
               addparam (1, tp [index]);
               (void) fprintf (code_file, "%sint typ%d,int par%d",
                  index == 2 ? "" : ",", index - 2, index - 2);
               if (xref)
                  write_ref (" ARG ", tp [index]);
            }
         }
         np -> state_count = -next_arg;
         (void) fprintf (code_file, ")\n#else\nvoid p%d(", next_procno);
         if (tp [2])
         {
            for (index = 2; tp [index]; index++)
               (void) fprintf (code_file, "%styp%d,par%d",
                  index == 2 ? "" : ",", index - 2, index - 2);
            (void) fprintf (code_file, ")\n");
            for (index = 2; tp [index]; index++)
               (void) fprintf (code_file, "int typ%d;int par%d;\n",
                  index - 2, index - 2);
         }
         else
            (void) fprintf (code_file, ")\n");
         (void) fprintf (code_file, "#endif\n{\n");
         next_procno++;
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
         dominor (prochead, NULL);
         (void) fprintf (code_file, "}\n");
         return;

      case INIT:
      case REPEAT:
         tp [2] = NULL;
         tp [1] = "REPEAT_PROC";
         if (major_type == INIT) tp [1] = "INIT_PROC";
         major_type = INIT;       /* Simplifies the test below */

      case ACTION:
      case AT:
         np = fndsymb (SYMBOL, tp [1]);
         if ((temp_list =
            (struct proc_list *) malloc (sizeof (struct proc_list))) == NULL)
               (void) gripe (tp [1], "Unable to allocate proc list element.");
         if (np -> head == NULL && major_type != INIT) /* && != REPEAT */
            proc = np -> refno;
         else
            proc = next_procno++;
         temp_list -> procno = proc;
         temp_list -> next = NULL;
         if (np -> head == NULL)
            np -> head = temp_list;
         else
            (np -> tail) -> next = temp_list;
         np -> tail = temp_list;

         if (debug)
            (void) fprintf (code_file, "/* %s %s */\n", tp [1],
               (tp [2] == NULL) ? "" : tp [2]);
         (void) fprintf (code_file,
            "#ifdef __STDC__\nvoid p%d(void)\n#else\nvoid p%d()\n#endif\n{\n",
               proc, proc);
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
         index = 1;
         *proccond = '\0';
         while (tp [++index] != NULL)
         {
            np = fndsymb (SYMBOL, tp [index]);
            if (np -> type > VERB)
               (void) gripe (tp [index], "Not a verb, place or object.");
            (void) sprintf (proctemp, 
               "if (!KEY(%d)) return;\n", np -> refno);
            strcat (proccond, proctemp);
         }

         dominor (prochead, proccond);
         (void) fprintf (code_file, "}\n");
         return;

      case DEFINE:
         index = 0;
         while (tp [++index] != NULL)
         {
            np = fndsymb (SYMBOL, tp [index]);
            if (np -> type != VARIABLE)
            {
               if (np -> type != PLACE)
                  (void) gripe (tp [index], "Only places can be defined!");
               if (fndsymb (VOCAB, tp [index]) == NULL)
                  storword (tp [index], PLACE, np -> refno, next_vocaddr);
               if (xref)
                  write_ref ("(PLACE)", tp [index]);
            }
         }
         break;
      default:
         (void) gripe (tp [0], "Unimplemented major directive.");
   }

   line_status = EOL;
   return;
}


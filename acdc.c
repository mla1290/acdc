/* acdc.c (acdc) - copyleft Mike Arnautov 1990-2005.
 */
#define VERSION "11.61, MLA 11 Jan 13"
/*
 * 11 Jan 05   MLA           Added pre-declarations for UNDO handling.
 * 01 Jan 05   MLA           Added UNDO/REDO.
 * 12 Sep 04   MLA           CALL is optional.
 * 20 Aug 04   MLA           Added IFCGI.
 * 19 Aug 04   MLA           Added FREE_ARG notation.
 * 14 Aug 04   MLA           Added SAVE/RESTORE.
 * 08 Aug 04   MLA           Added APPEND.
 * 14 Feb 04   MLA           Converted longs to ints.
 * 09 Feb 04   MLA           Added ADJECTIVE and PREPOSITION types.
 * 03 Feb 04   MLA           Added NOUN type.
 * 09 Mar 03   MLA           Replaced trace with debug.
 * 03 Mar 03   MLA           Added author.
 * 23 Feb 03   MLA           Initialise random number generator.
 * 02 Feb 03   MLA           Count autop chunks form 1 (kernel will be 0).
 * 07 Jan 03   MLA           Use btree instead of tsearch.
 * 01 Oct 02   MLA           Added dynamic copyleft notice.
 * 11 Jul 02   MLA           BUG: Fixed ITOBJ initalisation.
 * 23 Jun 02   MLA           Use "real" randomisation for texts.
 * 10 Mar 02   MLA           bug: Fixed source file display.
 * 03 Mar 02   MLA           Improved call argument passing.
 * 31 Dec 01   MLA           Allowed call argument passing; also added
 *                           cross-referencing code.
 * 26 Dec 01   MLA           Added text_lines.
 * 22 Dec 01   MLA           Allowed tying texts to texts.
 * 06 Nov 01   MLA           Added check for PLAIN and DEBUG.
 * 04 Nov 01   MLA           Made source_path a global buffer.
 * 06 Oct 01   MLA           Added switch and text counts for final report.
 * 14 Mar 01   MLA           Replaced old_style with style.
 * 12 Dec 99   MLA           Version 11.03 (HTML tag handling)
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 20 Mar 94   MLA           Parse comline to allow dynamic tracing.
 * 06 Mar 94   MLA           Made INCLUDEs relative to umbrella source.
 * 23 Feb 91   MLA           Allowed variable bit fields.
 * 30 Dec 90   MLA           Declared voc_buf necessaries.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "acdc.h"
char author [40];
char datbuf [16];
int memory;
time_t now;

#include "const.h"
#include "line.h"

int listing;
int line_status;
char line [MAXLINE + 1];
char raw_line [MAXLINE + 1];
char *line_ptr;
char *tp [ANY_NUMBER + 1];

#include "source.h"
int level = -1;
int file_count;
int total_lines = 0;
int text_lines = 0;
int line_count [MAXLEVEL];
char pathname [MAXLEVEL] [MAXLINE + 1];
FILE *infile [MAXLEVEL];
int style = 10;                  /* Default to A-code 10.* style */

#include "major.h"           /* Pick up the MAXTYPES value! */
#include "symbol.h"
#include "btree.h"
int vocab_count;
int type_counts [MAXTYPES];
int type_base [MAXTYPES];
int flag_field_size [VARFLAG + 1];
int *used_counts;
int *roots [] = {NULL, NULL, NULL, NULL};

#include "text.h"
int next_addr;
char *text_buf_ptr;
int text_buf_len = TEXT_INIT_LEN;
int switch_count = 0;
int text_count = 0;
int next_vocaddr;
char *voc_buf_ptr;
char *voc_ptr;
char *voc_top;
int voc_buf_len = VOC_INIT_LEN;
FILE *text_file;
char version [80];
char *version_ptr = &version [1];
int plain_text = 0;

#include "output.h"
FILE *defs_file;
FILE *code_file;
FILE *xref_file = NULL;

int code_part = 1;
int minor_count = 0;
int debug = 0;
int xref = 0;
int next_procno;
char *cond_buf_ptr;
int cond_buf_len = COND_INIT_LEN;

#ifdef __STDC__
int main (
   int argc, 
   char **argv)
#else
int main (argc, argv)
   int argc;
   char **argv;
#endif
{
   int offset;
   int file = 0;
   int readin = 0;
   int builtin = 0;
   int len;
   int i;
   char *arg;
   char source_path [MAXLINE + 1];

   extern void opebfrst ();
   extern void domajor ();
   extern void initial ();
   extern void finalise ();
   
   (void) printf (
      "[A-code to C translator, version %s]\n", VERSION);
   srand ((unsigned int)(now = time (NULL)));
   (void) strftime (datbuf, sizeof (datbuf), "%d %b %Y", localtime (&now));

/* Initialise the search stacks.
 */
   for (i = 0; i < sizeof (roots) / sizeof (roots [0]); i++)
      roots [i] = btinit (NULL);
      
/* Obtain the name of the program to process. This may be present on
 * the command line or prompted for.
 */
   *source_path = '\0';
   if (argc > 1)
   {
      while (--argc)
      {
         argv++;
         len = strlen(*argv);
         arg = *argv;
         if (len == 1 && **argv == '-')  arg = "-rhubarb";
         else if (strncmp (arg, "-debug", len) == 0)
            debug = 1;
         else if (strncmp (arg, "-plain", len) == 0)
            plain_text = 1;
         else if (strncmp (arg, "-xref", len) == 0)
            xref = 2;         /* "Super TRUE" -- noxref doesn't override */
         else if (strncmp (arg, "-file", len) == 0)
            file = 1;
         else if (strncmp (arg, "-memory", len) == 0)
            readin = 1;
         else if (strncmp (arg, "-preload", len) == 0)
            builtin = 1;
         else if (*arg == '-')
         {
            (void) puts ("\n   Usage: acdc [options] [(path)name]") ;
            (void) puts ("\n   Where allowed options are:");
            (void) puts ("      -plain (abbreviable to -p)");
            (void) puts ("         Do not encrypt text.");
            (void) puts ("      -file (abbreviable to -f)");
            (void) puts ("         Read all text directly from the text file.");
            (void) puts ("      -memory (abbreviable to -m)");
            (void) puts ("         Read all text into memory on startup.");
            (void) puts ("      -preload (abbreviable to -pr)");
            (void) puts ("         Do not create a separate text file.");
            (void) puts ("      -xref (abbreviable to -x)");
            (void) puts ("         Generate the .xrf cross-reference listing.");
            (void) puts ("      -debug (abbreviable to -d)");
            (void) puts ("         Add A-code source as comments in C source and");
            (void) puts ("         announce entry to distinct A-code chunks.\n");
            return (ERROR);
         }
         else         
            (void) strcpy (source_path, arg);
      }
      if (file)
      {
         if (readin || builtin)
         {
            (void) puts ("Keyword -file no tcompatible with -memory or -preload.");
            exit (ERROR);
         }
         memory = 0;
      }
      else if (builtin)
         memory = 3;
      else if (readin)
         memory = 2;
      else
         memory = 1;
   }
   
   if (getenv ("PLAIN"))
      plain_text = 1;
   if (getenv ("DEBUG"))
      debug = 1;   
   
   if (*source_path == '\0')
   {
      (void) printf ("Adventure name: ");
      (void) fgets (source_path, 99, stdin);
      if ((offset = strlen (source_path)) <= 1)
      {
         (void) printf ("No input file specified.\n");
         return (ERROR);
      }
      source_path [offset - 1] = '\0';
   }

/* Open the source file. Note that openfirst maps the name to lower
 * case and appends .acd, if not present.
 */
   openfrst (source_path);
   file_count = 1;

/* Initialise search trees and other things */

   (void) initial ();

   while (line_status != EOF)
   {
      if ((line_status = getline (IGNORE_BLANK)) == EOF)
         break;
      (void) domajor ();     /* line_status changes here! */
   }

   (void) finalise ();

   free(text_buf_ptr);
   free(voc_buf_ptr);

   if (xref_file) fclose (xref_file);
   
   (void) printf (
      "Finished translating \"%s\":\n", source_file);
   (void) printf (
      "   ... Program files: %d\n", file_count);
   (void) printf (
      "   ... Program lines: %ld - code %ld, text %ld\n",
         total_lines, total_lines - text_lines, text_lines);
   (void) printf (
      "   ... Vocabulary size: %d vocabulary words\n", vocab_count);
   (void) printf (
      "   ... Separate texts: %ld, embedding %d text switches\n", 
         text_count, switch_count);
   (void) printf (
      "   ... Overall data file size: %ld bytes\n", next_addr);
   return (OK);
}

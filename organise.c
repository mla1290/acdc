/* organise.c (acdc) - copyleft Mike Arnautov 1990-2010.
 *
 * 30 Mar 10   MLA           CGINAME starts with an _ on DOS, Windows, VMS.
 * 22 Aug 09   MLA           Spare 3 obj/loc bits restricted to style >= 20. 
 * 31 Jul 09   MLA           Write adv6.h as signed char array, not just char.
 * 22 Jul 09   MLA           Added SWAP definition to adv1.h.
 *                           Also changed check for the quiet mode.
 * 14 Jul 09   MLA           Fixed gcc --pedantic warnings.
 * 13 Jul 09   MLA           Added PERSISTENT_DATA.
 * 31 Mar 09   MLA           Added definition of CGINAME.
 * 30 Apr 08   MLA           Added PROMPT.
 * 22 Mar 08   MLA           BADWORD etc now go from -2 down.
 * 15 Mar 08   MLA           Version 12 changes.
 * 04 May 07   MLA           Change CONTEXT to ADVCONTEXT (bloody MS!!).
 * 03 May 07   MLA           Cosmetic fix to first elements of text arrays
 *                           in adv5.h.
 * 15 Oct 06   MLA           Added PROMPTED.
 * 23 Dec 05   MLA           bug: Need string.h.
 * 13 Jan 04   MLA           Added UNDO/REDO handling.
 * 06 Mar 03   Stuart Munro  Remove unused variable.
 * 04 Mar 03   MLA           VERSION repaced with GAMEID and DBNAME.
 * 21 Feb 03   MLA           Added conditional VERSION definition.
 * 20 Feb 03   MLA           Chage to code file naming conventions.
 * 09 Feb 03   MLA           Added SAY, F/LDIR, F/LMAGIC.
 * 04 Feb 03   MLA           Added EXCEPT and TYPO.
 * 02 Feb 03   MLA           Renamed autod0 to autod5 (advkern.h will be 0).
 * 07 Jan 03   MLA           Use btree instead of tsearch.
 * 02 Jan 03   MLA           bug: text_count already defined in acdc.c.
 * 01 Jan 03   MLA           Added refno dumpnig for xref.
 * 21 Dec 01   MLA           Removed obsolete MAX* definitions.
 * 06 Nov 01   MLA           If style == 1, define QUICKIE etc...
 *                           Also added automatic definition of PLAIN.
 * 04 Nov 01   MLA           Define old-style display flags.
 * 02 Nov 01   MLA           Added BADSYNTAX.
 * 30 Oct 01   MLA           De-wired value of OBJ (objflag!) etc in
 *                           style 1.
 * 17 Oct 01   MLA           Improved compatibility with Platt's A-code.
 * 04 Aug 01   MLA           Added definition of IT.
 * 12 May 01   MLA           Conditionally define F/LSPECIAL.
 * 14 Mar 01   MLA           Replaced old_style with style.
 *                           Added AMBIGTYPO abd SCENEWORD.
 * 10 Jan 01   MLA           Allowed for dynamic text types.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 14 Mar 94   MLA           MOVED, JUGGLED and SCHIZOID now truly optional.
 *                           PLS.CLARIFY defined automatically if absent.
 * 27 Jun 93   MLA           Make sure ARG3 is defined too.
 * 23 Feb 91   MLA           Allowed variable bit fields.
 * 30 Dec 90   MLA           Dump vocabulary into text file.
 * 29 Dec 90   MLA           Trapped for output file errors.
 * 22 Nov 90   MLA           Own TSEARCH - some systems don't have it.
 * 15 Sep 90   MLA           Initial coding.
 *
 */
 
#if defined(__cplusplus) && !defined(__STDC__)
#  define __STDC__
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "acdc.h"
#include "btree.h"
#include "const.h"
#include "symbol.h"
#include "major.h"
#include "text.h"
#include "output.h"
#include "source.h"
#include "game.h"

/*====================================================================*/

#ifdef __STDC__
static void process_voc_proc (struct node *np)
#else
static void process_voc_proc (np)
struct node *np;
#endif
{
   if (np -> proc_count && np -> symbol)
      (np -> symbol) -> proc_count += np -> proc_count;
}

/*======================================================================*/

#ifdef __STDC__
void define_constant(char *name, char *upname, char *type)
#else
void define_constant(name, upname, type)
char *name;
char *upname;
char *type;
#endif
{
   struct node *np;
   char temp[60];

   if ((np = fndsymb(TESTSYMBOL, name)) != NULL)
   {
      if (np -> type != FLAGS && (style != 1 || np -> type != SYNONYM))
      {
         sprintf
            (temp, "Declared as other than a %s.", type);
         gripe (upname, temp);
      }
      else fprintf (defs_file, "#define %s %d\n", upname, np -> refno);
   }
   return;
}

/*======================================================================*/

#ifdef __STDC__
void declare_constant(char *name, char *upname, int value)
#else
void declare_constant(name, upname, value)
char *name;
char *upname;
int value;
#endif
{
   struct node *np;

   if ((np = fndsymb(TESTSYMBOL, name)) != NULL)
   {
      if (np -> type != CONSTANT && (style != 1 || np -> type != SYNONYM))
         gripe (upname, "Declared as other than a constant.");
      else
         value = np -> refno;
   }
   else
      np = addsymb (AUTOSYMBOL, name, CONSTANT, value);
   
   fprintf (defs_file, "#define %s %d\n", upname, value);

   return;
}

/*======================================================================*/

#ifdef __STDC__
void declare_flag(char *name, char *upname, int value)
#else
void declare_flag(name, upname, value)
char *name;
char *upname;
int   value;
#endif
{
   struct node *np;

   if ((np = fndsymb(TESTSYMBOL, name)) != NULL)
   {
      if (np -> type != FLAGS && (style != 1 || np -> type != SYNONYM))
         gripe (upname, "Declared as other than a flag.");
      else
         value = np -> refno;
   }
   else
      np = addsymb (AUTOSYMBOL, name, FLAGS, value);
   
   fprintf (defs_file, "#define %s %d\n", upname, value);
   return;
}

/*======================================================================*/

#ifdef __STDC__
void process_procnos (struct node *np)
#else
void process_procnos (np)
struct node *np;
#endif
{
   if (np -> proc_count)
   {
         np -> proc_base = next_procno++;
         if (np -> proc_count > 1)
            next_procno += np -> proc_count;
   }      
}

/*======================================================================*/

#ifdef __STDC__
void process_refnos (struct node *np)
#else
void process_refnos (np)
struct node *np;
#endif
{
   int type = np -> type;
   switch (type)
   {
      case OBJ:
      case LOC:
      case VERB:
      case VAR:
      case TEXT:
         np -> refno += type_base [type];
         break;
      
      case AT:
      case ACTION:
         gripe (np -> name, "Symbol used, but not defined.");
   }
}

/*======================================================================*/

#ifdef __STDC__
void organise(void)
#else
void organise()
#endif
{
   int index;
   int next_constant;
   int mask;
   int len;
   char *tptr;
   char *cptr;
   struct node *np;
   char dbname [80];
   struct node *undo_stat = NULL;
   struct node *undo = NULL;

   stage = -1;         /* Tell gripe we are not reading source file at present */
/* Deal with the game header info */

   btspan (VOCAB, process_voc_proc);

   if (style == -1)
      style = *title ? 10 : 1;
   if (style != 10 && *gameid)
      gripe ("", "The GAMEID directive only valid in A-code style 10!");

   if ((code_file = openout("adv02.c", "w")) == NULL)
      gripe ("adv01.c", "Unable to open code file.");
   fprintf (code_file, "#include \"adv0.h\"\n");
   fprintf (code_file, "#include \"adv3.h\"\n");

   next_addr = 0;
   if (debug)     /* Declarations over - if source output */
      debug = 2;  /* is required, turn it on! */

   if (*title)
      strcpy (dbname, title);
   else
      strcpy (dbname, source_file);
   if ((tptr = strpbrk (dbname, " \t")) != NULL)
      *tptr = 0;
   if ((tptr = strrchr (dbname, '.')) != NULL)
      *tptr = 0;
   if (*title == '\0')
   {
      strcpy (title, dbname);
      if (*title >= 'a' && *title <= 'z')
         *title += 'A' - 'a';
   }
   cptr = dbname;
   while (*cptr)
   {
      *cptr = tolower (*cptr);
      cptr++;
   }
   strcat (dbname, ".dat");

/* Assemble the game ID */

   if (style >= 11)
   {
      strcpy (gameid, title);
      if (*version)
      {
         strcat (gameid, " version ");
         strcat (gameid, version);
      }
      if (*date)
      {
         strcat (gameid, ", ");
         strcat (gameid, date);
      }
   }
      
   if (memory < 3)
      tptr =  dbname;
   else
      tptr = "adv6.h";
   
   if ((text_file = openout (tptr, "wb")) == NULL)
      gripe (tptr, "Unable to open data file.");

   tptr = *gameid ? gameid : source_file;
   len = strlen (tptr);
   if (len > 79) len = 79;
   strncpy (gameid, tptr, len);
   *(gameid + len) = '\0';
   gameid [79] = '\0';
   tptr = gameid;
   if (memory == 3)
   {
      fprintf (text_file, "unsigned char text [TEXT_BYTES] = {\n");
      fputc ('0', text_file); 
   }
   else
      fputc ('\0', text_file);
   next_addr++;

/* Write off the game ID. Can't use the standard encryption, because that
 * requires the knowledge of the game ID! I.e. there would be no way of 
 * decrypting it!
 */
 
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
         fprintf (text_file, ",%d", *tptr ^ mask);
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

/* Write off the game ID again, this time using the standard encryption.
 * This is necessary in order to give the game access to the string as
 * a standard text.
 */
 
   np = addsymb (AUTOSYMBOL, "game.id", TEXT, type_counts[TEXT]++);
   np -> name_addr = next_addr;
   tptr = gameid;
   while (*tptr)
      storchar(*tptr++);
   storchar ('\0');
   
   if ((quiet & 1) == 0)
   {
      printf ("GameID: %s\n", gameid);
      printf ("Style:  ");
      if (style <= 1)
         puts ("Dave Platt's original A-code");
      else
         printf ("A-code %d\n", style);
      if (*author)
         printf ("Author: %s\n", author);
   }   
   else
      printf ("Translating: %s ... ", gameid);

/* Check for mandatory symbols and add them if missing */

   if ((np = fndsymb(TESTSYMBOL, "inhand")) == NULL)
      addsymb (AUTOSYMBOL, "inhand", LOC, type_counts[LOC]++);
   else if (np -> type != LOC)
      gripe ("INHAND", "Declared as a non-place!");
   if ((np = fndsymb(TESTSYMBOL, "here")) == NULL)
      addsymb (AUTOSYMBOL, "here", VAR, type_counts[VAR]++);
   else if (np -> type != VAR)
      gripe ("HERE", "Declared as a non-variable!");
   if ((np = fndsymb(TESTSYMBOL, "there")) == NULL)
      addsymb (AUTOSYMBOL, "there", VAR, type_counts[VAR]++);
   else if (np -> type != VAR)
      gripe ("THERE", "Declared as a non-variable!");
   if ((np = fndsymb(TESTSYMBOL, "status")) == NULL)
      addsymb (AUTOSYMBOL, "status", VAR, type_counts[VAR]++);
   else if (np -> type != VAR)
      gripe ("STATUS", "Declared as a non-variable!");
   if ((np = fndsymb(TESTSYMBOL, "arg1")) == NULL)
      addsymb (AUTOSYMBOL, "arg1", VAR, type_counts[VAR]++);
   else if (np -> type != VAR)
      gripe ("ARG1", "Declared as a non-variable!");
   if ((np = fndsymb(TESTSYMBOL, "arg2")) == NULL)
      addsymb (AUTOSYMBOL, "arg2", VAR, type_counts[VAR]++);
   else if (np -> type != VAR)
      gripe ("ARG2", "Declared as a non-variable!");
   if ((np = fndsymb(TESTSYMBOL, "arg3")) == NULL)
      addsymb (AUTOSYMBOL, "arg3", VAR, type_counts[VAR]++);
   else if (np -> type != VAR)
      gripe ("ARG3", "Declared as a non-variable!");

   if ((np = fndsymb(TESTSYMBOL, "undo")) != NULL &&
      np -> type == VERB)
   {
      undo = np;
      if ((np = fndsymb(TESTSYMBOL, "undo.status")) == NULL)
      {
         np = addsymb (AUTOSYMBOL, "undo.status", VAR, type_counts[VAR]++);
      }
      else if (np -> type != VAR)
         gripe ("UNDO.STATUS", "Declared as a non-variable!");
      undo_stat = np;
   }

/*  Calculate individual type bases, and adjust refnos to their non-relative values.
 */
   next_procno = 1;
   btspan (SYMBOL, process_procnos);
   type_base[0] = 3;
   for (index = 0; index <= TEXT; index++)
      type_base[index + 1] = type_base[index] + type_counts[index];
   btspan (SYMBOL, process_refnos);

/*  Now create the include file which will define various symbolic
 *  constants required by the kernel routines.
 */
   if ((defs_file = openout("adv1.h","w")) == NULL)
      gripe ("","Unable to open adv1.h (defs.h).");
   fprintf (defs_file, "#ifndef ADV1_H\n#define ADV1_H\n");
   fprintf (defs_file, "#define GAME_NAME \"%s\"\n", title);
   fprintf (defs_file, "#define GAME_VERSION \"%s\"\n", 
      *version ? version : "99.99");
   fprintf (defs_file, "#define GAME_ID \"%s\"\n", gameid);
   fprintf (defs_file, "#define STYLE %d\n", style);
   fprintf (defs_file, "#ifndef ADV01\n");
   if (*date)
      fprintf (defs_file, "#define GAME_DATE \"%s\"\n", date);
   fprintf (defs_file, "#define ACDC_VERSION \"%s\"\n", acdc_version);
   if (swap)
      fprintf (defs_file, "#define SWAP %d\n", swap);
   strcpy (dbname, gameid);
   cptr = dbname;
   while (isalnum (*cptr))
   {
      *cptr = tolower (*cptr);
      cptr++;
   }
   *cptr = '\0';
   if (style >= 11)
      fprintf (defs_file, "#define PERSISTENT_DATA \".%s.adp\"\n", dbname);
   fprintf (defs_file, "#if defined(MSDOS) || defined(_WIN32) || defined(VMS)\n");
   fprintf (defs_file, "#  define CGINAME \"_%s\"\n#else\n", dbname);
   fprintf (defs_file, "#  define CGINAME \".%s\"\n#endif\n", dbname);
   strcat (dbname, ".dat");
   fprintf (defs_file, 
      "#define DBNAME \"%s\"\n", dbname);
   {
      int offset = style >= 20 ? 3 : 0;
      fprintf (defs_file, "#define OBJSIZE %d\n", 
         (flag_field_size[OBJFLAG] - offset)/16 + 1);
      fprintf (defs_file, "#define LOCSIZE %d\n", 
         (flag_field_size[LOCFLAG] - offset)/16 + 1);
      fprintf (defs_file, "#define VARSIZE %d\n", 
         (flag_field_size[VARFLAG] - offset)/16 + 1);
   }
   fprintf (defs_file, "#define FOBJ %d\n", type_base[OBJ]);
   fprintf
      (defs_file, "#define LOBJ %d\n", type_base[OBJ + 1] - 1);
   fprintf (defs_file, "#define FLOC %d\n", type_base[LOC]);
   fprintf
      (defs_file, "#define LLOC %d\n", type_base[LOC + 1] - 1);
   fprintf (defs_file, "#define FVERB %d\n", type_base[VERB]);
   fprintf
      (defs_file, "#define LVERB %d\n", type_base[VERB + 1] - 1);
   fprintf
      (defs_file, "#define FVAR %d\n", type_base[VAR]);
   fprintf
      (defs_file, "#define LVAR %d\n", type_base[VAR + 1]);
   fprintf (defs_file, "#define FTEXT %d\n", type_base[TEXT]);
   fprintf (defs_file, "#define LTEXT %d\n", type_base[TEXT + 1]);
   fprintf (defs_file, "#define NOISE %d\n", NOISE);
   fprintf (defs_file, "#define KNOT %d\n", key_mask);
   fprintf (defs_file, "#define VOCAB_SIZE %d\n", vocab_count);

   fprintf (defs_file,
      "#define INHAND %d\n", fndsymb(SYMBOL, "inhand") -> refno);
   fprintf (defs_file,
      "#define HERE %d\n", fndsymb(SYMBOL, "here") -> refno);
   fprintf (defs_file,
      "#define THERE %d\n", fndsymb(SYMBOL, "there") -> refno);
   fprintf (defs_file,
      "#define STATUS %d\n", fndsymb(SYMBOL, "status") -> refno);
   fprintf (defs_file,
      "#define ARG1 %d\n", fndsymb(SYMBOL, "arg1") -> refno);
   fprintf (defs_file,
      "#define ARG2 %d\n", fndsymb(SYMBOL, "arg2") -> refno);
   fprintf (defs_file,
      "#define ARG3 %d\n", fndsymb(SYMBOL, "arg3") -> refno);

   if (undo_stat)
   {
      fprintf (defs_file, "#define UNDO %d\n", undo -> refno);
      if ((np = fndsymb(TESTSYMBOL, "redo")) != NULL)
      {
         if (np -> type == VERB)
            fprintf (defs_file, "#define REDO %d\n", np -> refno);
         if (undo == NULL)
            gripe ("REDO", "Verb declared without UNDO!");
         else if (np -> type != VERB)
            gripe ("UNDO", "Declared as a non-verb!");
      }
      fprintf (defs_file, "#define UNDO_STAT %d\n", undo_stat -> refno);
      declare_flag ("undo.off",  "UNDO_OFF",   3);
      declare_flag ("undo.info", "UNDO_INFO" , 4);
      declare_flag ("undo.none", "UNDO_NONE" , 5);
      declare_flag ("undo.trim", "UNDO_TRIM" , 6);
      declare_flag ("undo.inv",  "UNDO_INV"  , 7);
      declare_flag ("undo.bad",  "UNDO_BAD"  , 8);
   }
   
   define_constant ("schizoid",   "SCHIZOID",   "object flag");
   define_constant ("juggled",    "JUGGLED",    "variable flag");
   define_constant ("moved",      "MOVED",      "variable flag");

   if ((np = fndsymb(TESTSYMBOL, "pls.clarify")) != NULL)
   {
      if (np -> type != FLAGS && (style != 1 || np -> type != SYNONYM))
         gripe ("PLS.CLARIFY", "Declared as other than a bit constant.");
      else fprintf (defs_file, "#define PLSCLARIFY %d\n", np -> refno);
   }
   
   if ((np = fndsymb(TESTSYMBOL, "again")) != NULL)
   {
      if (np -> type != VERB)
         gripe ("AGAIN", "Declared as other than a verb.");
      else
         fprintf (defs_file, "#define AGAIN %d\n", np -> refno);
   }

   if ((np = fndsymb(TESTSYMBOL, "all")) != NULL ||
       (np = fndsymb(TESTSYMBOL, "everything")) != NULL)
      if (np -> type == OBJ)
         fprintf (defs_file, "#define ALL %d\n", np -> refno);

   if ((np = fndsymb(TESTSYMBOL, "except")) != NULL ||
       (np = fndsymb(TESTSYMBOL, "but")) != NULL)
   {
      if (np -> type == VERB || np -> type == OBJ)
      {
         fprintf (defs_file, "#define EXCEPT %d\n", np -> refno);
         np -> auto_flag = 1;
      }
   }
   if ((np = fndsymb(TESTSYMBOL, "typo")) != NULL &&
      np -> type == TEXT)
         fprintf (defs_file, "#define TYPO %d\n", np -> refno);

   if ((np = fndsymb(TESTSYMBOL, "define")) != NULL &&
      np -> type == VERB)
         fprintf (defs_file, "#define DEFINE %d\n", np -> refno);

   if ((np = fndsymb(TESTSYMBOL, "undefine")) != NULL &&
      np -> type == VERB)
         fprintf (defs_file, "#define UNDEFINE %d\n", np -> refno);

   if ((np = fndsymb(TESTSYMBOL, "it")) != NULL)
      if (np -> type == OBJ)
         fprintf (defs_file, "#define IT %d\n", np -> refno);
         
   if ((np = fndsymb(TESTSYMBOL, "context")) != NULL)
      if (np -> type == VAR)
         fprintf (defs_file, "#define ADVCONTEXT %d\n", np -> refno);

   if ((np = fndsymb(TESTSYMBOL, "prompt")) != NULL)
      if (np -> type == VAR)
         fprintf (defs_file, "#define PROMPT %d\n", np -> refno);

   if ((np = fndsymb(TESTSYMBOL, "dwarven")) != NULL)
      if (np -> type == VAR)
         fprintf (defs_file, "#define DWARVEN %d\n", np -> refno);

#ifdef OBSOLETE
   if ((np = fndsymb(TESTSYMBOL, "fulldisplay")) != NULL)
   {
      if (np -> type != FLAGS && (style != 1 || np -> type != SYNONYM))
         gripe ("FULLDISPLAY", "Declared as other than a bit constant.");
      else
         fprintf (defs_file, "#define FULL %d\n", np -> refno);
   }
#endif /* OBSOLETE */

   if ((np = fndsymb(TESTSYMBOL, "detaildisplay")) != NULL)
   {
      if (np -> type != FLAGS && (style != 1 || np -> type != SYNONYM))
         gripe ("DETAILDISPLAY", "Declared as other than a bit constant.");
      else if (style == 10)
         fprintf (defs_file, "#define DETAIL %d\n", np -> refno);
   }

   if ((np = fndsymb(TESTSYMBOL, "prompted")) != NULL)
   {
      if (np -> type != FLAGS && (style != 1 || np -> type != SYNONYM))
         gripe ("PROMPTED", "Declared as other than a bit constant.");
      else
         fprintf (defs_file, "#define PROMPTED %d\n", np -> refno);
   }

   if ((np = fndsymb(TESTSYMBOL, "first.special")) != NULL)
   {
      if (np -> type != VERB)
         gripe ("first.special", "Declared as other than a verb.");
      else
         fprintf (defs_file, "#define FSPECIAL %d\n", np -> refno);
   }

   if ((np = fndsymb(TESTSYMBOL, "last.special")) != NULL)
   {
      if (np -> type != VERB)
         gripe ("last.special", "Declared as other than a verb.");
      else
         fprintf (defs_file, "#define LSPECIAL %d\n", np -> refno);
   }
   
   if ((np = fndsymb(TESTSYMBOL, "first.magic")) != NULL)
   {
      if (np -> type != VERB)
         gripe ("first.magic", "Declared as other than a verb.");
      else
         fprintf (defs_file, "#define FACT %d\n", np -> refno);
   }

   if ((np = fndsymb(TESTSYMBOL, "last.magic")) != NULL)
   {
      if (np -> type != VERB)
         gripe ("last.magic", "Declared as other than a verb.");
      else
         fprintf (defs_file, "#define LACT %d\n", np -> refno);
   }
   
   if ((np = fndsymb(TESTSYMBOL, "first.direction")) != NULL)
   {
      if (np -> type != VERB)
         gripe ("first.direction", "Declared as other than a verb.");
      else
         fprintf (defs_file, "#define FDIR %d\n", np -> refno);
   }

   if ((np = fndsymb(TESTSYMBOL, "last.direction")) != NULL)
   {
      if (np -> type != VERB)
         gripe ("last.direction", "Declared as other than a verb.");
      else
         fprintf (defs_file, "#define LDIR %d\n", np -> refno);
   }
   
   if ((np = fndsymb(TESTSYMBOL, "say")) != NULL)
   {
      if (np -> type != VERB)
         gripe ("say", "Declared as other than a verb.");
      else
         fprintf (defs_file, "#define SAY %d\n", np -> refno);
   }
   
   next_constant = -2;
   declare_constant ("badword", "BADWORD", next_constant--);
   if (style >= 10)
      declare_constant ("ambigword", "AMBIGWORD", next_constant--);
   if (style >= 11)
   {
      declare_constant ("ambigtypo", "AMBIGTYPO", next_constant--);
      declare_constant ("sceneword", "SCENEWORD", next_constant--);
      declare_constant ("badsyntax", "BADSYNTAX", next_constant--);
   }
   if (style <= 1)
   {
      if ((np = fndsymb(TESTSYMBOL, "objflag")) != NULL)
         fprintf (defs_file, "#define OBJ %d\n", np -> refno);
      else
         fprintf (defs_file, "#define OBJ %d\n", OBJFLAG);
      if ((np = fndsymb(TESTSYMBOL, "placeflag")) != NULL)
         fprintf (defs_file, "#define LOC %d\n", np -> refno);
      else
         fprintf (defs_file, "#define LOC %d\n", LOCFLAG);
      if ((np = fndsymb(TESTSYMBOL, "verbflag")) != NULL)
         fprintf (defs_file, "#define VERB %d\n", np -> refno);
      else
         fprintf (defs_file, "#define VERB %d\n", VERBFLAG);
   }
   else
   {
      fprintf (defs_file, "#define OBJ %d\n", OBJFLAG);
      fprintf (defs_file, "#define LOC %d\n", LOCFLAG);
      fprintf (defs_file, "#define VERB %d\n", VERBFLAG);
   }

   define_constant ("fulldisplay",   "FULL",     "flag or synonym");
   define_constant ("fulldisp",      "FULL",     "flag or synonym");
   define_constant ("beenhere",      "BEENHERE", "flag or synonym");
   define_constant ("been.here",     "BEENHERE", "flag or synonym");
   define_constant ("fastmode",      "TERSE",    "flag or synonym");
   define_constant ("tersedisplay",  "TERSE",    "flag or synonym");
   define_constant ("quickie",       "BRIEF",    "flag or synonym");
   define_constant ("briefdisplay",  "BRIEF",    "flag or synonym");

   if (plain_text)
      fprintf (defs_file, "#define PLAIN\n");

   fprintf (defs_file, "#define DBSTATUS %d\n", 3 - memory);
   fprintf (defs_file, "#define LPROC %d\n", type_base [VERB + 1]);
   fprintf (defs_file,
      "#define KEY(X) (value[%d]==X || value[%d]==X)\n",
      fndsymb(SYMBOL, "arg1") -> refno, fndsymb(SYMBOL, "arg2") -> refno);

   {
      int procno = 0;
      if ((np = fndsymb (SYMBOL, "INIT_PROC")) != NULL)
         procno = np -> proc_base;
      fprintf (defs_file, "#define INIT_PROC p%d\n", procno);
      if ((np = fndsymb (SYMBOL, "REPEAT_PROC")) == NULL)
         gripe ("", "No REPEAT sections found in the source code.");
      procno = np -> proc_base;
      fprintf (defs_file, "#define REPEAT_PROC p%d\n", procno);
   }
   fprintf (defs_file, "#endif\n");
   stage = 1;        /* Restore stage value */
   return;
}

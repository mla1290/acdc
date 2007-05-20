/* organise.c (acdc) - copyleft Mike Arnautov 1990-2007.
 *
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
 * 30 Oct 01   MLA           De-wired value of OBJECT (objflag!) etc in
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

int node_count;
int desc_count;
int *text_info;
int *text_base;
int *brief_base;
int *int_base;
int *detail_base;
int base_voc_addr;
int key_mask;

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

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, name)) != NULL)
   {
      if (np -> type != FLAGS && (style != 1 || np -> type != SYNONYM))
      {
         (void) sprintf
            (temp, "Declared as other than a %s.", type);
         (void) gripe (upname, temp);
      }
      else (void) fprintf (defs_file, "#define %s %d\n", upname, np -> refno);
   }
   return;
}

#ifdef __STDC__
void processsymb(struct node *np)
#else
void processsymb(np)
struct node *np;
#endif
{
   int refno;
   int *array_ptr;

   if (np -> type < PROCEDURE)
   {
      if (np -> type == TEXT) 
      {
         *(text_info + 2 * (np -> refno)) =
            (np -> body.text.text_type & 1023);   /* Strip off the 1024 flag */
         *(text_info + 2 * (np -> refno) + 1) =
            np -> state_count;
      }
      refno = np -> refno += type_base[ np -> type ];
      if (xref_file)
         fprintf (xref_file, "+ %d %s\n", refno, np -> name);
      *(text_base + refno) = np -> body.text.name_addr;
      if (np -> type != TEXT)
         *(text_base + refno) += base_voc_addr;
      if (np -> type <= PLACE)
      {
         array_ptr = np -> body.text.text_addr;
         *(brief_base + refno) = *array_ptr++;
         if ((*(int_base + refno) = *array_ptr++) == -1L)
            *(int_base + refno) = *(brief_base + refno);
         if ((*(detail_base + refno) = *array_ptr) == -1L)
            *(detail_base + refno) = *(int_base + refno);
      }
   }
   return;
}

#ifdef __STDC__
void process_voc_refno(struct node *np)
#else
void process_voc_refno(np)
struct node *np;
#endif
{
   int refno;
   int feature;
   
   if (np -> type == NOUN)
   {
      np -> type = VERB;
      feature = 1 << 15;
   }
   else
      feature = 1;
   if (np -> type <= VERB)
      refno = np -> refno += type_base [np -> type];
   else
      refno = 0;
   (void) fprintf (defs_file, "%4d, ", feature * refno);
   if (++node_count == 11)
   {
      node_count = 0;
      (void) fputc ('\n', defs_file);
   }
   return;
}

#ifdef __STDC__
void process_voc_type(struct node *np)
#else
void process_voc_type(np)
struct node *np;
#endif
{
   (void) fprintf (defs_file, "%4d, ", np -> type);
   if (++node_count == 11)
   {
      node_count = 0;
      (void) fputc ('\n', defs_file);
   }
   return;
}

#ifdef __STDC__
void process_voc_addr(struct node *np)
#else
void process_voc_addr(np)
struct node *np;
#endif
{
   (void) fprintf (defs_file, "%8ldL, ", 
      base_voc_addr + np -> body.vocab.voc_addr);
   if (++node_count == 7)
   {
      node_count = 0;
      (void) fputc ('\n', defs_file);
   }
   return;
}

#ifdef __STDC__
void process_voc_word(struct node *np)
#else
void process_voc_word(np)
struct node *np;
#endif
{
   (void) fprintf (defs_file, "%8ldL, ", 
      base_voc_addr + np -> body.vocab.word_addr);
   if (++node_count == 7)
   {
      node_count = 0;
      (void) fputc ('\n', defs_file);
   }
   return;
}

#ifdef __STDC__
void dump_array (int *addr, int count, char *pattern, int group)
#else
void dump_array (addr, count, pattern, group)
int *addr;
int count;
char *pattern;
int group;
#endif
{
    int *base;
    int tokens;

    tokens = 0;
    base = addr;
    while(count-- > 0)
    {
       (void) fprintf (defs_file, pattern, *addr++);
       tokens++;
       if (tokens == group)
       {
          tokens=0;
          (void) fputc ('\n', defs_file);
       }
    }
    free(base);
    return;
}

#ifdef __STDC__
void declare_constant(char *name, char *upname)
#else
void declare_constant(name, upname)
char *name;
char *upname;
#endif
{
   struct node *np;
   int value;

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, name)) != NULL)
   {
      if (np -> type != CONSTANT && (style != 1 || np -> type != SYNONYM))
         (void) gripe (upname, "Declared as other than a constant.");
      else
         value = np -> refno;
   }
   else
   {
      value = next_procno++;
      (type_base [PROCEDURE])++;
      (void) addsymb (AUTOSYMBOL, name, CONSTANT, value);
   }
   (void) fprintf (defs_file, "#define %s %d\n", upname, value);

   return;
}

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

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, name)) != NULL)
   {
      if (np -> type != FLAGS && (style != 1 || np -> type != SYNONYM))
         (void) gripe (upname, "Declared as other than a flag.");
      else
         value = np -> refno;
   }
   else
      (void) addsymb (AUTOSYMBOL, name, FLAGS, value);
   (void) fprintf (defs_file, "#define %s %d\n", upname, value);
   return;
}

#ifdef __STDC__
void organise(void)
#else
void organise()
#endif
{
   int index;
   char *cptr;
   struct node *np;
   char dbname [80];
   struct node *undo_stat = NULL;
   struct node *undo = NULL;

#ifdef __STDC__   
   void processsymb(struct node *);
   void process_voc_refno(struct node *);
   void process_voc_type(struct node *);
   void process_voc_addr(struct node *);
   void process_voc_word(struct node *);
#else
   void processsymb();
   void process_voc_refno();
   void process_voc_type();
   void process_voc_addr();
   void process_voc_word();
#endif

   base_voc_addr = next_addr;
   voc_top = voc_ptr;
   voc_ptr = voc_buf_ptr;
   while (voc_ptr < voc_top)
      storchar(*voc_ptr++);
   if (memory == 3)
      fprintf (text_file, "};\n");
   (void) clsfile (text_file, "Text");

/* Check for mandatory symbols and add them if missing */

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "inhand")) == NULL)
      (void) addsymb (AUTOSYMBOL, "inhand", PLACE, type_counts[PLACE]++);
   else if (np -> type != PLACE)
      (void) gripe ("INHAND", "Declared as a non-place!");
   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "here")) == NULL)
      (void) addsymb (AUTOSYMBOL, "here", VARIABLE, type_counts[VARIABLE]++);
   else if (np -> type != VARIABLE)
      (void) gripe ("HERE", "Declared as a non-variable!");
   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "there")) == NULL)
      (void) addsymb (AUTOSYMBOL, "there", VARIABLE, type_counts[VARIABLE]++);
   else if (np -> type != VARIABLE)
      (void) gripe ("THERE", "Declared as a non-variable!");
   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "status")) == NULL)
      (void) addsymb (AUTOSYMBOL, "status", VARIABLE, type_counts[VARIABLE]++);
   else if (np -> type != VARIABLE)
      (void) gripe ("STATUS", "Declared as a non-variable!");
   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "arg1")) == NULL)
      (void) addsymb (AUTOSYMBOL, "arg1", VARIABLE, type_counts[VARIABLE]++);
   else if (np -> type != VARIABLE)
      (void) gripe ("ARG1", "Declared as a non-variable!");
   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "arg2")) == NULL)
      (void) addsymb (AUTOSYMBOL, "arg2", VARIABLE, type_counts[VARIABLE]++);
   else if (np -> type != VARIABLE)
      (void) gripe ("ARG2", "Declared as a non-variable!");
   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "arg3")) == NULL)
      (void) addsymb (AUTOSYMBOL, "arg3", VARIABLE, type_counts[VARIABLE]++);
   else if (np -> type != VARIABLE)
      (void) gripe ("ARG3", "Declared as a non-variable!");
/*
 *   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "pls.clarify")) == NULL)
 *      (void) addsymb (AUTOSYMBOL, "pls.clarify", FLAGS, 
 *         ++flag_field_size [VARFLAG]);
 *   else if (np -> type != FLAGS)
 *      (void) gripe ("PLS.CLARIFY", "Declared as a non-flag!");
 */

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "undo")) != NULL &&
      np -> type == VERB)
   {
      undo = np;
      if ((np = fndsymb(SYMBOL_OR_CONSTANT, "undo.status")) == NULL)
      {
         np = addsymb (AUTOSYMBOL, "undo.status", VARIABLE, 
            type_counts[VARIABLE]++);
      }
      else if (np -> type != VARIABLE)
         (void) gripe ("UNDO.STATUS", "Declared as a non-variable!");
      undo_stat = np;
   }

/*  Calculate the individual type bases, making sure that the refno numbering
 *  starts from 3. This is done to have two fixed refnos for INIT_PROC and
 *  REPEAT_PROC (1 and two respectively), even though true procs come at the
 *  very end of the sequence (because at this point we still don't know how
 *  many there are going to be).
 */
   type_base[0] = 3;            /* Skip INIT_PROC and REPEAT_PROC */
   for (index = 0; index <= TEXT; index++)
      type_base[index + 1] = type_base[index] + type_counts[index];
   next_procno = type_base[PROCEDURE];  /* Other procs will start from here */

/*  Allocate the space for message addresses and types  */

   text_count = type_base[TEXT + 1];
   if ((text_base = (int *) calloc (text_count, sizeof(int))) == NULL)
      (void) gripe ("", "Unable to allocate text address memory.");
   if ((text_info = (int *) calloc (2 * (type_base[TEXT + 1] - 
      type_base[TEXT]), sizeof(int))) == NULL)
        (void) gripe ("", "Unable to allocate text type memory.");

/*  Allocate space for description addresses - brief, int and detailed.  */

   desc_count = type_base[PLACE + 1];
   if ((brief_base = (int *) calloc (desc_count, sizeof(int))) == NULL)
      (void) gripe ("", "Unable to allocate brief description address memory.");
   if ((int_base = (int *) calloc (desc_count, sizeof(int))) == NULL)
      (void) gripe ("", "Unable to allocate int description address memory.");
   if ((detail_base = (int *) calloc (desc_count, sizeof(int))) == NULL)
      (void) gripe ("", "Unable to allocate detailed description address memory.");

/*  Open include file which will initialise respective arrays.  Then
 *  walk through all symbols (a) calculating their correct refnos (by
 *  adding the appropriate type base to their stored relative type refnos),
 *  (b) filling in the four address arrays - these then get written out
 *  into the insert file.
 */
   btspan(SYMBOL, processsymb);

   if ((defs_file = openout("adv5.h","w")) == NULL)
      (void) gripe ("","Unable to open adv5.h (words.h).");

/* A cosmetic fix to avoid worrying people. The relevant array elements
 * don't get actually referebced by the game. They shouldn't have been
 * there in the first place, but they do no harm and it is much easier
 * to leave them there.
 */
   *text_base = 0;
   *text_info = 0;
   *(text_info + 1) = 0;
   *brief_base = 0;
   *int_base = 0;
   *detail_base = 0;
   
   (void) fprintf (defs_file, "   int textadr[] = {\n");
   dump_array(text_base, text_count,  " %8ldL,", 7);
   (void) fprintf (defs_file, "        0L};\n char text_info[] = {\n");
   dump_array(text_info, 2 * (text_count - type_base[TEXT]),  " %4ld,", 12);
   (void) fprintf (defs_file, "    0};\n int brief_desc[] = {\n");
   dump_array (brief_base, desc_count,  " %8ldL,", 7);
   (void) fprintf (defs_file, "        0L};\n int long_desc[] = {\n");
   dump_array (int_base, desc_count,  " %8ldL,", 7);
   (void) fprintf (defs_file, "        0L};\n int detail_desc[] = {\n");
   dump_array (detail_base, desc_count,  " %8ldL,", 7);
   (void) fprintf (defs_file, "        0L};\n");

   (void) clsfile (defs_file, "adv5.h");      /* Done with this file */

/*  Now create the include file which will define various symbolic
 *  constants required by the kernel routines.
 */
   if ((defs_file = openout("adv1.h","w")) == NULL)
      (void) gripe ("","Unable to open adv1.h (defs.h).");
   (void) fprintf (defs_file, "#define GAMEID \"%s\"\n", gameid);
   (void) fprintf (defs_file, "#ifndef ADV01\n");
   strcpy (dbname, gameid);
   cptr = dbname;
   while (isalnum (*cptr))
   {
      *cptr = tolower (*cptr);
      cptr++;
   }
   *cptr = '\0';
   strcat (dbname, ".dat");
   (void) fprintf (defs_file, 
      "#ifdef USEDB\n#  define DBNAME \"%s\"\n#endif\n", dbname);
   (void) fprintf (defs_file, "#define TEXT_BYTES %ld\n", next_addr);
   (void) fprintf (defs_file, "#define OBJSIZE %d\n", 
      flag_field_size[OBJFLAG]/16 + 1);
   (void) fprintf (defs_file, "#define PLACESIZE %d\n", 
      flag_field_size[PLACEFLAG]/16 + 1);
   (void) fprintf (defs_file, "#define VARSIZE %d\n", 
      flag_field_size[VARFLAG]/16 + 1);
   (void) fprintf (defs_file, "#define FOBJECT %d\n", type_base[OBJECT]);
   (void) fprintf
      (defs_file, "#define LOBJECT %d\n", type_base[OBJECT + 1] - 1);
   (void) fprintf (defs_file, "#define FPLACE %d\n", type_base[PLACE]);
   (void) fprintf
      (defs_file, "#define LPLACE %d\n", type_base[PLACE + 1] - 1);
   (void) fprintf (defs_file, "#define FVERB %d\n", type_base[VERB]);
   (void) fprintf
      (defs_file, "#define LVERB %d\n", type_base[VERB + 1] - 1);
   (void) fprintf
      (defs_file, "#define FVARIABLE %d\n", type_base[VARIABLE]);
   (void) fprintf
      (defs_file, "#define LVARIABLE %d\n", type_base[VARIABLE + 1]);
   (void) fprintf (defs_file, "#define FTEXT %d\n", type_base[TEXT]);
   (void) fprintf (defs_file, "#define LTEXT %d\n", type_base[TEXT + 1]);
   (void) fprintf (defs_file, "#define NOISE %d\n", NOISE);
   (void) fprintf (defs_file, "#define KNOT %d\n", key_mask);
   (void) fprintf (defs_file, "#define VOCAB_SIZE %d\n", vocab_count);
   (void) fprintf (defs_file, "#define STYLE %d\n", style);

   (void) fprintf (defs_file,
      "#define INHAND %d\n", fndsymb(SYMBOL, "inhand") -> refno);
   (void) fprintf (defs_file,
      "#define HERE %d\n", fndsymb(SYMBOL, "here") -> refno);
   (void) fprintf (defs_file,
      "#define THERE %d\n", fndsymb(SYMBOL, "there") -> refno);
   (void) fprintf (defs_file,
      "#define STATUS %d\n", fndsymb(SYMBOL, "status") -> refno);
   (void) fprintf (defs_file,
      "#define ARG1 %d\n", fndsymb(SYMBOL, "arg1") -> refno);
   (void) fprintf (defs_file,
      "#define ARG2 %d\n", fndsymb(SYMBOL, "arg2") -> refno);
   (void) fprintf (defs_file,
      "#define ARG3 %d\n", fndsymb(SYMBOL, "arg3") -> refno);

   if (undo_stat)
   {
      (void) fprintf (defs_file, "#define UNDO %d\n", undo -> refno);
      if ((np = fndsymb(SYMBOL_OR_CONSTANT, "redo")) != NULL)
      {
         if (np -> type == VERB)
            (void) fprintf (defs_file, "#define REDO %d\n", np -> refno);
         if (undo == NULL)
            (void) gripe ("REDO", "Verb declared without UNDO!");
         else if (np -> type != VERB)
            (void) gripe ("UNDO", "Declared as a non-verb!");
      }
      (void) fprintf (defs_file, "#define UNDO_STAT %d\n", undo_stat -> refno);
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

/* PLS.CLARIFY is special, 'cause we map it into the PLSCLARIFY macro */
/*
 *  if ((np = fndsymb(SYMBOL_OR_CONSTANT, "pls.clarify")) == NULL)
 *     (void) gripe ("PLS.CLARIFY",
 *        "Missing mandatory definition of the status bit.");
 *  else
 */
   if (np = fndsymb(SYMBOL_OR_CONSTANT, "pls.clarify"))
   {
      if (np -> type != FLAGS && (style != 1 || np -> type != SYNONYM))
         (void) gripe ("PLS.CLARIFY", "Declared as other than a bit constant.");
      else (void) fprintf (defs_file, "#define PLSCLARIFY %d\n", np -> refno);
   }
   
   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "again")) != NULL)
   {
      if (np -> type != VERB)
         (void) gripe ("AGAIN", "Declared as other than a verb.");
      else
         (void) fprintf (defs_file, "#define AGAIN %d\n", np -> refno);
   }

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "all")) != NULL ||
       (np = fndsymb(SYMBOL_OR_CONSTANT, "everything")) != NULL)
      if (np -> type == OBJECT)
         (void) fprintf (defs_file, "#define ALL %d\n", np -> refno);

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "except")) != NULL ||
       (np = fndsymb(SYMBOL_OR_CONSTANT, "but")) != NULL)
      if (np -> type == VERB || np -> type == OBJECT)
         (void) fprintf (defs_file, "#define EXCEPT %d\n", np -> refno);

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "typo")) != NULL &&
      np -> type == TEXT)
         (void) fprintf (defs_file, "#define TYPO %d\n", np -> refno);

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "define")) != NULL &&
      np -> type == VERB)
         (void) fprintf (defs_file, "#define DEFINE %d\n", np -> refno);

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "undefine")) != NULL &&
      np -> type == VERB)
         (void) fprintf (defs_file, "#define UNDEFINE %d\n", np -> refno);

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "it")) != NULL)
      if (np -> type == OBJECT)
         (void) fprintf (defs_file, "#define IT %d\n", np -> refno);
         
   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "context")) != NULL)
      if (np -> type == VARIABLE)
         (void) fprintf (defs_file, "#define ADVCONTEXT %d\n", np -> refno);

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "dwarven")) != NULL)
      if (np -> type == VARIABLE)
         (void) fprintf (defs_file, "#define DWARVEN %d\n", np -> refno);

#ifdef OBSOLETE
   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "fulldisplay")) != NULL)
   {
      if (np -> type != FLAGS && (style != 1 || np -> type != SYNONYM))
         (void) gripe ("FULLDISPLAY", "Declared as other than a bit constant.");
      else
         (void) fprintf (defs_file, "#define FULL %d\n", np -> refno);
   }
#endif /* OBSOLETE */

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "detaildisplay")) != NULL)
   {
      if (np -> type != FLAGS && (style != 1 || np -> type != SYNONYM))
         (void) gripe ("DETAILDISPLAY", "Declared as other than a bit constant.");
      else
         (void) fprintf (defs_file, "#define DETAIL %d\n", np -> refno);
   }

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "prompted")) != NULL)
   {
      if (np -> type != FLAGS && (style != 1 || np -> type != SYNONYM))
         (void) gripe ("PROMPTED", "Declared as other than a bit constant.");
      else
         (void) fprintf (defs_file, "#define PROMPTED %d\n", np -> refno);
   }

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "first.special")) != NULL)
   {
      if (np -> type != VERB)
         (void) gripe ("first.special", "Declared as other than a verb.");
      else
         (void) fprintf (defs_file, "#define FSPECIAL %d\n", np -> refno);
   }

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "last.special")) != NULL)
   {
      if (np -> type != VERB)
         (void) gripe ("last.special", "Declared as other than a verb.");
      else
         (void) fprintf (defs_file, "#define LSPECIAL %d\n", np -> refno);
   }
   
   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "first.magic")) != NULL)
   {
      if (np -> type != VERB)
         (void) gripe ("first.magic", "Declared as other than a verb.");
      else
         (void) fprintf (defs_file, "#define FACT %d\n", np -> refno);
   }

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "last.magic")) != NULL)
   {
      if (np -> type != VERB)
         (void) gripe ("last.magic", "Declared as other than a verb.");
      else
         (void) fprintf (defs_file, "#define LACT %d\n", np -> refno);
   }
   
   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "first.direction")) != NULL)
   {
      if (np -> type != VERB)
         (void) gripe ("first.direction", "Declared as other than a verb.");
      else
         (void) fprintf (defs_file, "#define FDIR %d\n", np -> refno);
   }

   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "last.direction")) != NULL)
   {
      if (np -> type != VERB)
         (void) gripe ("last.direction", "Declared as other than a verb.");
      else
         (void) fprintf (defs_file, "#define LDIR %d\n", np -> refno);
   }
   
   if ((np = fndsymb(SYMBOL_OR_CONSTANT, "say")) != NULL)
   {
      if (np -> type != VERB)
         (void) gripe ("say", "Declared as other than a verb.");
      else
         (void) fprintf (defs_file, "#define SAY %d\n", np -> refno);
   }
   
   (void) declare_constant ("badword", "BADWORD");
   if (style >= 10)
      (void) declare_constant ("ambigword", "AMBIGWORD");
   if (style >= 11)
   {
      (void) declare_constant ("ambigtypo", "AMBIGTYPO");
      (void) declare_constant ("sceneword", "SCENEWORD");
   }
   if (style >= 11)
      (void) declare_constant ("badsyntax", "BADSYNTAX");
   if (style == 1)
   {
      if ((np = fndsymb(SYMBOL_OR_CONSTANT, "objflag")) != NULL)
         (void) fprintf (defs_file, "#define OBJECT %d\n", np -> refno);
      else
         (void) fprintf (defs_file, "#define OBJECT %d\n", OBJFLAG);
      if ((np = fndsymb(SYMBOL_OR_CONSTANT, "placeflag")) != NULL)
         (void) fprintf (defs_file, "#define PLACE %d\n", np -> refno);
      else
         (void) fprintf (defs_file, "#define PLACE %d\n", PLACEFLAG);
      if ((np = fndsymb(SYMBOL_OR_CONSTANT, "verbflag")) != NULL)
         (void) fprintf (defs_file, "#define VERB %d\n", np -> refno);
      else
         (void) fprintf (defs_file, "#define VERB %d\n", VERBFLAG);
   }
   else
   {
      (void) fprintf (defs_file, "#define OBJECT %d\n", OBJFLAG);
      (void) fprintf (defs_file, "#define PLACE %d\n", PLACEFLAG);
      (void) fprintf (defs_file, "#define VERB %d\n", VERBFLAG);
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
      (void) fprintf (defs_file, "#define PLAIN\n");

   (void) fprintf (defs_file, "#define DBSTATUS %d\n", 3 - memory);

   (void) fprintf (defs_file,
      "#define KEY(X) (value[%d]==X || value[%d]==X)\n",
      fndsymb(SYMBOL, "arg1") -> refno, fndsymb(SYMBOL, "arg2") -> refno);
   (void) fprintf (defs_file, "#endif\n");
   (void) clsfile (defs_file, "adv1.h");     /* Ahhh.... done at last! */

   if ((defs_file = openout("adv2.h","w")) == NULL)
      (void) gripe ("adv2.h","Unable to open (adv2.h).");

/*  Now for the vocabulary include file */

   (void) fprintf (defs_file, "   short voc_refno[] = {\n");
   node_count = 0;
   btspan(VOCAB, process_voc_refno);
   (void) fprintf (defs_file, "0};\n   short voc_type[] = {\n");
   node_count = 0;
   btspan(VOCAB, process_voc_type);
   (void) fprintf (defs_file, "0};\n   int voc_addr[] = {\n");
   node_count = 0;
   btspan(VOCAB, process_voc_addr);
   (void) fprintf (defs_file, "0};\n   int voc_word[] = {\n");
   node_count = 0;
   btspan(VOCAB, process_voc_word);
   (void) fprintf (defs_file, "0};\n");

   (void) clsfile (defs_file, "adv2.h");      /* Vocabulary done */

   return;
}

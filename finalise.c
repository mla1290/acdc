/* finalise.c (acdc) - copyleft Mike Arnautov 1990-2002.
 *
 * 02 Jan 03   MLA           bug: removed a redundant arg to fprintf.
 * 13 Jan 02   MLA           Pass "hidden" type parameters.
 * 30 Dec 01   MLA           Added proc arguments code.
 * 18 Nov 01   MLA           Autop* files now tagged with 2 digits.
 * 17 May 01   MLA           Defined fake() in autod3.h.
 * 24 Jul 99   MLA           Fixed complier warnings.
 * 20 Mar 94   MLA           Declare umbrella functions as ints.
 * 29 Dec 90   MLA           Trapped for output file errors.
 * 22 Nov 90   MLA           Own tsearch () - some systems don't have it.
 * 15 Sep 90   MLA           Initial coding.
 *
 */
 
#include <stdio.h>
#include <string.h>

#include "acdc.h"
#include "text.h"
#include "search.h"
#include "symbol.h"
#include "output.h"
#include "major.h"
#include "const.h"

int *proc_array;
int *proc_args;
int *proc_arr_ptr;

#ifdef __STDC__
void finalise (void)
#else
void finalise ()
#endif
{
   char proc_name [20];
   int proc_count;
   int count;
   int index;

   void process_proc ();
   extern void *calloc ();

   (void) clsfile (code_file, "Automatic code");
   (void) sprintf (proc_name, "autop%02d.c", ++code_part);
   if ((code_file = openout (proc_name, "w")) == NULL)
      (void) gripe (proc_name, "Unable to open final code chunk.");
   (void) fprintf (code_file, "#include \"autod3.h\"\n");

   proc_count = type_base [VERB + 1];
   if ((proc_array = (int *) calloc (proc_count, sizeof (int))) == NULL)
      (void) gripe ("", "Unable to allocate procedure array space.");

   if ((proc_args = (int *) calloc (next_procno, sizeof (int))) == NULL)
      (void) gripe ("", "Unable to allocate argument array space.");

   twalk (root [SYMBOL], process_proc);
   (void) fprintf (code_file, 
      "#ifdef __STDC__\nvoid p0(void)\n#else\nvoid p0()\n#endif\n{return;}\n");

   (void) clsfile (code_file, "Final automatic code");
   if ((code_file = openout ("autod3.h", "w")) == NULL)
      (void) gripe ("finalise", "Unable to open autod3.h.");

   proc_arr_ptr = proc_array;
   (void) fprintf (code_file, 
      "#ifdef __STDC__\nextern void fake(int x, int y);\n");
   (void) fprintf (code_file, 
      "extern void p0(void);\nextern void p1(void);\nextern void p2(void);\n");
   for (index = 0; index < next_procno; index++)
   {
      (void) fprintf (code_file, "extern void p%d(", index);
      if (index >= type_base [PROCEDURE] && 
         (count = *(proc_args + index - type_base [PROCEDURE])) < 0)
         while (count++)
            (void) fprintf (code_file, "int,int%s", count ? "," : "");
      else
         (void) fprintf (code_file, "void");
      (void) fprintf (code_file, ");\n");
   }
   (void) fprintf (code_file, "#else\nextern void fake();\n");
   (void) fprintf (code_file, 
      "extern void p0();\nextern void p1();\nextern void p2();\n");
   for (index = 0; index < next_procno; index++)
   {
      (void) fprintf (code_file, "extern void p%d();\n", index);
   }
   (void) fprintf (code_file, "#endif\n");

   (void) clsfile (code_file, "autod3.h");
   if ((code_file = openout ("autod4.h", "w")) == NULL)
      (void) gripe ("finalise", "Unable to open autod4.h.");

   proc_arr_ptr = proc_array;
   (void) fprintf (code_file, "void (*procs[])() = {\n");
   count = 0;
   for (index = 0; index < proc_count; index++)
   {
      (void) fprintf (code_file, "p%d,", *proc_arr_ptr++);
      if (count++ == 10)
      {
         count = 0;
         (void) fputc ('\n', code_file);
      }
   }
   (void) fprintf (code_file, "p0 };\n");
   (void) clsfile (code_file, "autod4.h");

   return;
}

#ifdef __STDC__
void process_proc (struct node **npp, VISIT order, int level)
#else
void process_proc (npp, order, level)
struct node **npp;
VISIT order;
int level;
#endif
{
   struct proc_list *head;
   int procno;
   int refno;
   int type;

   if (order != postorder && order != leaf)
      return;
   type = (*npp) -> type;

   if ((*npp) -> used_count == 0 && *((*npp) -> name) != '.' &&
      strcmp ((*npp) -> name, "place") && 
      strcmp ((*npp) -> name, "verb") && 
      strcmp ((*npp) -> name, "variable") && 
      strcmp ((*npp) -> name, "object") && 
      strncmp ((*npp) -> name, "spare", 5) &&
      strcmp ((*npp) -> name, "arg3"))
         (void) printf ("%-20s symbol defined but not used.\n", (*npp) -> name);

   procno = refno = (*npp) -> refno;

   if (procno >= type_base [PROCEDURE] && procno < next_procno)
      *(proc_args + procno - type_base [PROCEDURE]) = (*npp) -> state_count;
   
   procno = refno;
   if (type > VERB && type != INIT && type != REPEAT)
      return;

   if ((*npp) -> head == NULL)
      return;

   head = (*npp) -> head;

   if (head -> next != NULL || type > VERB)
   {
      if (type <= VERB)
         procno = next_procno++;
      else
         procno = (type == INIT) ? 1 : 2;
      (void) fprintf (code_file, 
         "#ifdef __STDC__\nvoid p%d(void)\n#else\nvoid p%d()\n#endif\n{\n", 
            procno, procno);
      while (TRUE)
      {
         (void) fprintf (code_file, "   p%d();\n", head -> procno);
         if ((head = head -> next) == NULL)
            break;
      }
      (void) fprintf (code_file, "   return;\n}\n");
   }
   *(proc_array + refno) = procno;
   return;
}

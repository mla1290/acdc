/* source.h (acdc) - copyleft Mike Arnautov 1990-2002.
 *
 * 17 Nov 01   MLA           Added the source_* buffers.
 * 14 Mar 01   MLA           Replaced old_style with style and moved to text.h.
 * 06 Mar 94   MLA           Made INCLUDEs relative to umbrella source.
 * 15 Sep 90   MLA           Initial coding.
 *
 */
 
#ifndef FILE
#include <stdio.h>
#endif

#ifndef MAXLINE
#include "const.h"
#endif

#if defined(unix) || defined(__CYGWIN__)
#  define SEP '/'
#else
#  if defined(MSDOS) || defined (_WIN32_)
#     define SEP '\\'
#  else
#     ifdef __50SERIES
#        define SEP '>'
#     else
#        define SEP '?'
#     endif
#  endif
#endif

extern int file_count;
extern int level;
extern int line_count [];
extern long total_lines;
extern char pathname [MAXLEVEL] [MAXLINE + 1];
extern FILE *infile [];
extern char source_stem [];
extern char source_file [];
extern char xref_path [];

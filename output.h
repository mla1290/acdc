/* output.h (acdc) - copyleft Mike Arnautov 1990-2002.
 *
 * 31 Dec 01   MLA           Added xref.
 * 11 Dec 99   MLA           Upped initial condition buffer to 1K.
 * 20 Mar 94   MLA           Tracing now done dynamically.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#ifndef FILE
#include <stdio.h>
#endif

extern FILE *defs_file;
extern FILE *code_file;
extern FILE *xref_file;

extern int code_part;
extern int minor_count;
extern int trace;
extern int next_procno;
extern int xref;

#define COND_INIT_LEN 1024
extern char *cond_buf_ptr;
extern int cond_buf_len;

#define TRACE_HEADERS  1
#define TRACE_ECHO     2
#define TRACE_SOURCE   4
#define PRINT_SOURCE   8

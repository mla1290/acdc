/* text.h (acdc) - copyleft Mike Arnautov 1990-2006.
 *
 * 15 Oct 06   MLA           Eliminated redundand NEST_VAR.
 * 03 Mar 03   MLA           Adedd memory.
 * 26 Dec 01   MLA           Added text_lines.
 * 25 Nov 01   MLA           Added IGNORE_EOL.
 * 06 Oct 01   MLA           Added text_count and switch_count.
 * 14 Mar 01   MLA           Moved style definition to here frmo source.h.
 * 28 Feb 01   MLA           Added in-text Dwarvish. Added text types.
 * 06 Jan 01   MLA           Added CENTER_START.
 * 11 Dec 99   MLA           Added block and tag delimiters, and hard-space.
 * 31 Dec 98   MLA           Doubled VOC_INIT_LEN.
 * 30 Dec 90   MLA           Added voc_buf necesseries.
 * 15 Sep 90   MLA           Initial coding.
 *
 */

#ifndef FILE
#include <stdio.h>
#endif

#ifdef __STDC__
extern void storword (char *, int, int, int);
extern void clsfile (FILE *outfile, char *text);
#else
extern void storword ();
extern void clsfile ();
#endif

#define TEXT_INIT_LEN  8192
#define LOGICAL_ESCAPE '\\'
#define SWITCH_START   '['
#define SWITCH_BREAK   '/'
#define SWITCH_END     ']'
#define NEST_START     '{'
#define NEST_END       '}'
#define DWARVISH_TAG   '%'
#define MAX_BREAKS     100
#define SW_START     '\377'
#define SW_BREAK     '\376'
#define HOLDER       '\375'
#define SILENCE      '\374'
#define IGNORE_EOL   '\373'
#define NEST_TEXT    '\372'
                   /* \371 is spare */
#define TAG_START    '\370'
#define TAG_END      '\367'
#define NBSP         '\366'
#define BLOCK_START  '\365'
#define BLOCK_END    '\364'
#define CENTRE_START '\363'
#define DWARVISH     '\362'
#define VHOLDER      '\361'

#define RANDOM_TEXT       1
#define INCREMENTAL_TEXT  2
#define CYCLIC_TEXT       3
#define ASSIGNED_TEXT     4
#define TIED_TEXT         5

extern int next_addr;
extern char *text_buf_ptr;
extern int text_buf_len;
extern FILE *text_file;
extern char gameid [250];
extern char *gameid_ptr;
extern int plain_text;
extern int memory;
extern int style;
extern int text_count;
extern int switch_count;
extern int text_lines;
extern int key_mask;

#define VOC_INIT_LEN 8192
extern int next_vocaddr;
extern char *voc_buf_ptr;
extern int voc_buf_len;
extern char *voc_ptr;
extern char *voc_top;

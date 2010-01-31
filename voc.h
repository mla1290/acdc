/* voc.h (acdc) - copyleft Mike Arnautov 1993-2010.
 *
 * 02 Feb 03   MLA            Initial coding.
 */

/* Vocabulary types -- note these are not necessarily mutually exclusive! */

#define V_NOUN         000001
#define V_VERB         000002
#define V_ADJECTIVE    000004
#define V_ADVERB       000010
#define V_PREPOSITION  000020
#define V_NULL         000040
#define V_EXACT        000100
#define V_ALL          000200
#define V_EXCEPT       000400
#define V_REPEAT       001000
#define V_LOC          002000
#define V_OBJ          004000
#define V_SHARED       010000
#define V_UNIQUE       020000

/*====================================================================*/

/*
 * "search.h", Peter Valkenburg, november '89.
 */

#ifndef _SEARCH_H
#define _SEARCH_H

extern void twalk ();

/*
 * Type for twalk node visiting.  Normally part of "/usr/include/search.h".
 */
typedef enum {
   preorder, postorder, endorder, leaf
} VISIT;

#endif /* _SEARCH_H */

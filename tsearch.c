/* tsearch.c (acdc) - copyleft Peter Valkenburg, November '89. */

#include "search.h"

typedef struct node_s {
        char *key;                      /* ptr to datum */
        struct node_s *parent;          /* ptr to parent ancestor */
        struct node_s *sibls[2];        /* ptrs to L/R siblings */
        int balance;                    /* balance value (-1, 0 or +1) */
} NODE;

#define NIL_NODE        ((NODE *) 0)    /* ptr for empty (sub)trees */

typedef int DIR;                        /* type for indexing siblings */
#define L       ((DIR) 0)
#define R       ((DIR) 1)

#define LEFT            sibls[L]        /* left sibling pointer NODE field */
#define RIGHT           sibls[R]        /* right sibling pointer NODE field */

#define direction(p, c) ((DIR) ((p)->RIGHT == (c)))
#define cmp_dir(v)      ((DIR) ((v) > 0))
#define siblingp(n, d)  ((n)->sibls + (d))
#define parentp(r, n)   ((n)->parent == NIL_NODE ? (r) : \
                             siblingp((n)->parent, direction((n)->parent, (n))))
#define dir_bal(d)      ((d) == L ? -1 : 1)

static NODE *balance();
static NODE *single_rotation();
static NODE *double_rotation();
extern void *malloc();
extern void free();

#ifdef __STDC__
NODE *tsearch(char *key, register NODE **rootp, int (*compar)())
#else
NODE *tsearch(key, rootp, compar)
char *key;
register NODE **rootp;
int (*compar)();
#endif
{
        register NODE *parent, *child;
        NODE *nnode;
        register DIR d;
        register int cmp;

        if (rootp == 0)
                return NIL_NODE;

        /* find place where key should go */
        parent = NIL_NODE;
        child = *rootp;
        while (child != NIL_NODE) {
                if ((cmp = compar(key, child->key)) == 0)
                        return child;
                parent = child;
                child = *siblingp(child, cmp_dir(cmp));
        }

        if ((nnode = (NODE *) malloc(sizeof(NODE))) == NIL_NODE)
                return NIL_NODE;
        nnode->key = key;
        nnode->balance = 0;
        nnode->parent = parent;
        nnode->LEFT = nnode->RIGHT = NIL_NODE;
        if (parent == NIL_NODE) {
                *rootp = nnode;
                return nnode;
        }
        *siblingp(parent, cmp_dir(cmp)) = nnode;
        child = nnode;

        do {
                d = direction(parent, child);
                if (parent->balance == dir_bal(d)) {
                        (void) balance(rootp, parent, d);
                        return nnode;
                }
                else if ((parent->balance += dir_bal(d)) == 0)
                        return nnode;
                child = parent;
                parent = parent->parent;
        } while (parent != NIL_NODE);

        return nnode;
}

#ifdef __STDC__
static NODE *balance(NODE **rootp, NODE *sb, DIR d)
#else
static NODE *balance(rootp, sb, d)
NODE **rootp;
NODE *sb;
DIR d;
#endif
{
        NODE *sb_next = *siblingp(sb, d);

        if (sb_next->balance == -dir_bal(d))
                return double_rotation(rootp, sb, sb_next, d);
        else
                return single_rotation(rootp, sb, sb_next, d);
}

#ifdef __STDC__
static NODE *single_rotation(NODE **rootp, register NODE *sb, 
       register NODE *sb_next, register DIR d)
#else
static NODE *single_rotation(rootp, sb, sb_next, d)
NODE **rootp;
register NODE *sb, *sb_next;
register DIR d;
#endif
{
        *siblingp(sb, d)       = *siblingp(sb_next, !d);
        *siblingp(sb_next, !d) = sb;

        sb->balance     -= sb_next->balance;
        sb_next->balance = -sb->balance;

        *parentp(rootp, sb) = sb_next;
        sb_next->parent = sb->parent;
        sb->parent = sb_next;
        if (*siblingp(sb, d) != NIL_NODE)
                (*siblingp(sb, d))->parent = sb;

        return sb_next;
}

#ifdef __STDC__
static NODE *double_rotation(NODE **rootp, register NODE *sb, 
       register NODE *sb_next, DIR d)
#else
static NODE *double_rotation(rootp, sb, sb_next, d)
NODE **rootp;
register NODE *sb, *sb_next;
register DIR d;
#endif
{
        register NODE *sb_next2 = *siblingp(sb_next, !d);

        *siblingp(sb_next, !d)  = *siblingp(sb_next2, d);
        *siblingp(sb, d)        = *siblingp(sb_next2, !d);
        *siblingp(sb_next2, d)  = sb_next;
        *siblingp(sb_next2, !d) = sb;

        if (sb_next2->balance == sb_next->balance)
                sb_next->balance = -sb_next->balance;
        else
                sb_next->balance = 0;
        if (sb_next2->balance == sb->balance)
                sb->balance = -sb->balance;
        else
                sb->balance = 0;
        sb_next2->balance = 0;

        *parentp(rootp, sb) = sb_next2;
        sb_next2->parent = sb->parent;
        sb->parent = sb_next->parent = sb_next2;
        if (*siblingp(sb_next, !d) != NIL_NODE)
                (*siblingp(sb_next, !d))->parent = sb_next;
        if (*siblingp(sb, d) != NIL_NODE)
                (*siblingp(sb, d))->parent = sb;

        return sb_next2;
}

#ifdef __STDC__
NODE *tfind(char *key, NODE **rootp, int (*compar)())
#else
NODE *tfind(key, rootp, compar)
char *key;
NODE **rootp;
int (*compar)();
#endif
{
        register NODE *node;
        register int cmp;

        if (rootp == 0)
                return NIL_NODE;

        node = *rootp;
        while (node != NIL_NODE) {
                if ((cmp = compar(key, node->key)) == 0)
                        return node;
                node = *siblingp(node, cmp_dir(cmp));
        }

        return NIL_NODE;
}

#ifdef __STDC__
void twalk(register NODE *node, register void (*action)())
#else
void twalk(node, action)
register NODE *node;
register void (*action)();
#endif
{
        register VISIT visit;
        register int level;

        if (node == 0 || action == 0)
                return;

        visit = preorder;
        level = 0;
        while (node != NIL_NODE) {
                if (visit == preorder &&
                    node->LEFT == NIL_NODE && node->RIGHT == NIL_NODE)
                        visit = leaf;

                action(node, visit, level);

                switch (visit) {
                case preorder:
                        if (node->LEFT != NIL_NODE) {
                                node = node->LEFT;
                                level++;
                        }
                        else
                                visit = postorder;
                        break;
                case postorder:
                        if (node->RIGHT != NIL_NODE) {
                                node = node->RIGHT;
                                visit = preorder;
                                level++;
                        }
                        else
                                visit = endorder;
                        break;
                case endorder:
                case leaf:
                        if (node->parent != NIL_NODE)
                        {
                                if (direction(node->parent, node) == L)
                                        visit = postorder;
                                else
                                        visit = endorder;
                        }
                        node = node->parent;
                        level--;
                        break;
                }
        }
}

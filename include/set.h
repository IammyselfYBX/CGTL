#ifndef T
#error "Template type T undefined for <set.h>"
#endif
#include "cgtl.h"

#define SET JOIN(set, T)
#define RBT JOIN(SET, node)
#define SETIT JOIN(SET, it)

typedef struct RBT
{
    struct RBT* l;
    struct RBT* r;
    struct RBT* p;
    T key;
    int color;
} RBT;

typedef struct SET {
    RBT* root;
    size_t size;
    int (*compare)(T*, T*);
    void (*free)(T*);
    T (*copy)(T*);
} SET;

typedef struct SETIT {
    RBT* end;
    RBT* node;
    T* ref;
    RBT* next;
    int done;
    void (*step)(struct SETIT*);
} SETIT;

static inline RBT*
JOIN(SET, begin)(SET* self)
{
    return self->root;
}

static inline RBT*
JOIN(SET, end)(SET* self)
{
    (void) self;
    return NULL;
}

static inline RBT*
JOIN(RBT, min)(RBT* self)
{
    while(self->l)
        self = self->l;
    return self;
}

static inline RBT*
JOIN(RBT, max)(RBT* self)
{
    while(self->r)
        self = self->r;
    return self;
}

static inline RBT*
JOIN(RBT, next)(RBT* self)
{
    if(self->r)
    {
        self = self->r;
        while(self->l)
            self = self->l;
    }
    else
    {
        RBT* parent = self->p;
        while(parent && self == parent->r)
        {
            self = parent;
            parent = parent->p;
        }
        self = parent;
    }
    return self;
}

static inline void
JOIN(SETIT, step)(SETIT* self)
{
    if(self->next == self->end)
        self->done = 1;
    else
    {
        self->node = self->next;
        self->ref = &self->node->key;
        self->next = JOIN(RBT, next)(self->node);
    }
}

static inline SETIT
JOIN(SETIT, range)(SET* container, RBT* begin, RBT* end)
{
    (void) container;
    static SETIT zero;
    SETIT self = zero;
    if(begin)
    {
        self.step = JOIN(SETIT, step);
        self.node = JOIN(RBT, min)(begin);
        self.ref = &self.node->key;
        self.next = JOIN(RBT, next)(self.node);
        self.end = end;
    }
    else
        self.done = 1;
    return self;
}

static inline int
JOIN(SET, empty)(SET* self)
{
    return self->size == 0;
}

static inline SETIT
JOIN(SETIT, each)(SET* a)
{
    return JOIN(SET, empty)(a)
         ? JOIN(SETIT, range)(a, NULL, NULL)
         : JOIN(SETIT, range)(a, JOIN(SET, begin)(a), JOIN(SET, end)(a));
}

static inline int
JOIN(SET, equal)(SET* self, SET* other, int _equal(T*, T*))
{
    if(self->size != other->size)
        return 0;
    SETIT a = JOIN(SETIT, each)(self);
    SETIT b = JOIN(SETIT, each)(other);
    while(!a.done && !b.done)
    {
        if(!_equal(a.ref, b.ref))
            return 0;
        a.step(&a);
        b.step(&b);
    }
    return 1;
}

static inline void
JOIN(SET, swap)(SET* self, SET* other)
{
    SET temp = *self;
    *self = *other;
    *other = temp;
}

static inline SET
JOIN(SET, init)(int _compare(T*, T*))
{
    static SET zero;
    SET self = zero;
    self.compare = _compare;
    self.free = NULL;
    self.copy = NULL;
    return self;
}

static inline void
JOIN(SET, free_node)(SET* self, RBT* node)
{
    if(self->free)
        self->free(&node->key);
    free(node);
}

static inline int
JOIN(RBT, color)(RBT* self)
{
    return self ? self->color : 1;
}

static inline int
JOIN(RBT, is_blk)(RBT* self)
{
    return JOIN(RBT, color)(self) == 1;
}

static inline int
JOIN(RBT, is_red)(RBT* self)
{
    return JOIN(RBT, color)(self) == 0;
}

static inline RBT*
JOIN(RBT, grandfather)(RBT* self)
{
    return self->p->p;
}

static inline RBT*
JOIN(RBT, sibling)(RBT* self)
{
    if(self == self->p->l)
        return self->p->r;
    else
        return self->p->l;
}

static inline RBT*
JOIN(RBT, uncle)(RBT* self)
{
    return JOIN(RBT, sibling)(self->p);
}

static inline RBT*
JOIN(RBT, init)(T key, int color)
{
    RBT* self = (RBT*) malloc(sizeof(RBT));
    self->key = key;
    self->color = color;
    self->l = self->r = self->p = NULL;
    return self;
}

static inline RBT*
JOIN(SET, lower_bound)(SET* self, T key)
{
    RBT* node = self->root;
    RBT* result = NULL;
    while(node)
    {
        int diff = self->compare(&key, &node->key);
        if(diff <= 0)
        {
            result = node;
            node = node->l;
        }
        else
            node = node->r;
    }
    return result;
}

static inline RBT*
JOIN(SET, upper_bound)(SET* self, T key)
{
    RBT* node = self->root;
    RBT* result = NULL;
    while(node)
    {
        int diff = self->compare(&key, &node->key);
        if(diff >= 0){
            result = node;
            node = node->r;
        }else{
            node = node->l;
        }
    }
    return result;
}

static inline RBT*
JOIN(SET, find)(SET* self, T key)
{
    RBT* node = self->root;
    while(node){
        int diff = self->compare(&key, &node->key);
        if(diff == 0){
            return node;
        }else if(diff < 0){
            node = node->l;
        }else{
            node = node->r;
        }
    }
    return NULL;
}

static inline int
JOIN(SET, count)(SET* self, T key)
{
    return JOIN(SET, find)(self, key) ? 1 : 0;
}

static inline void
JOIN(RBT, replace)(SET* self, RBT* a, RBT* b)
{
    if(a->p)
    {
        if(a == a->p->l)
            a->p->l = b;
        else
            a->p->r = b;
    }
    else
        self->root = b;
    if(b)
        b->p = a->p;
}

#ifdef USE_INTERNAL_VERIFY
    #include <assert.h>

    static inline void
    JOIN(RBT, verify_property_1)(RBT* self)
    {
        assert(JOIN(RBT, is_red)(self) || JOIN(RBT, is_blk)(self));
        if(self)
        {
            JOIN(RBT, verify_property_1)(self->l);
            JOIN(RBT, verify_property_1)(self->r);
        }
    }

    static inline void
    JOIN(RBT, verify_property_2)(RBT* self)
    {
        assert(JOIN(RBT, is_blk)(self));
    }

    static inline void
    JOIN(RBT, verify_property_3)(RBT* self)
    {
        if(self == NULL)
        {

            return;
        }
        else
        {

            JOIN(RBT, verify_property_3)(self->l);
            JOIN(RBT, verify_property_3)(self->r);
        }
    }

    static inline void
    JOIN(RBT, verify_property_4)(RBT* self)
    {
        if(JOIN(RBT, is_red)(self))
        {
            assert(JOIN(RBT, is_blk)(self->l));
            assert(JOIN(RBT, is_blk)(self->r));
            assert(JOIN(RBT, is_blk)(self->p));
        }
        if(self)
        {
            JOIN(RBT, verify_property_4)(self->l);
            JOIN(RBT, verify_property_4)(self->r);
        }
    }

    static inline void
    JOIN(RBT, count_blk)(RBT* self, int nodes, int* in_path)
    {
        if(JOIN(RBT, is_blk)(self))
            nodes += 1;
        if(self)
        {
            JOIN(RBT, count_blk)(self->l, nodes, in_path);
            JOIN(RBT, count_blk)(self->r, nodes, in_path);
        }
        else
        {
            if(*in_path == -1)
                *in_path = nodes;
            else
                assert(nodes == *in_path);
        }
    }

    static inline void
    JOIN(RBT, verify_property_5)(RBT* self)
    {
        int in_path = -1;
        JOIN(RBT, count_blk)(self, 0, &in_path);
    }

    static inline void
    JOIN(SET, verify)(SET* self)
    {
        JOIN(RBT, verify_property_1)(self->root);
        JOIN(RBT, verify_property_2)(self->root);
        JOIN(RBT, verify_property_3)(self->root);
        JOIN(RBT, verify_property_4)(self->root);
        JOIN(RBT, verify_property_5)(self->root);
    }
#endif

static inline void
JOIN(SET, rotate_l)(SET* self, RBT* node)
{
    RBT* r = node->r;
    JOIN(RBT, replace)(self, node, r);
    node->r = r->l;
    if(r->l)
        r->l->p = node;
    r->l = node;
    node->p = r;
}

static inline void
JOIN(SET, rotate_r)(SET* self, RBT* node)
{
    RBT* l = node->l;
    JOIN(RBT, replace)(self, node, l);
    node->l = l->r;
    if(l->r)
        l->r->p = node;
    l->r = node;
    node->p = l;
}

static inline void
JOIN(SET, insert_1)(SET*, RBT*),
JOIN(SET, insert_2)(SET*, RBT*),
JOIN(SET, insert_3)(SET*, RBT*),
JOIN(SET, insert_4)(SET*, RBT*),
JOIN(SET, insert_5)(SET*, RBT*);

static inline RBT*
JOIN(SET, insert)(SET* self, T key)
{
    RBT* insert = JOIN(RBT, init)(key, 0);
    if(self->root)
    {
        RBT* node = self->root;
        while(1)
        {
            int diff = self->compare(&key, &node->key);
            if(diff == 0)
            {
#ifdef ALLOW_DUPLICATES

                if(node->l)
                    node = node->l;
                else
                {
                    node->l = insert;
                    break;
                }
#else

                JOIN(SET, free_node)(self, insert);
                return node;
#endif
            }
            else
            if(diff < 0)
            {
                if(node->l)
                    node = node->l;
                else
                {
                    node->l = insert;
                    break;
                }
            }
            else
            {
                if(node->r)
                    node = node->r;
                else
                {
                    node->r = insert;
                    break;
                }
            }
        }
        insert->p = node;
    }
    else
        self->root = insert;
    JOIN(SET, insert_1)(self, insert);
    self->size += 1;
#ifdef USE_INTERNAL_VERIFY
    JOIN(SET, verify)(self);
#endif
    return insert;
}

static inline void
JOIN(SET, insert_1)(SET* self, RBT* node)
{
    if(node->p)
        JOIN(SET, insert_2)(self, node);
    else
        node->color = 1;
}

static inline void
JOIN(SET, insert_2)(SET* self, RBT* node)
{
    if(JOIN(RBT, is_blk)(node->p))
        return;
    else
       JOIN(SET, insert_3)(self, node);
}

static inline void
JOIN(SET, insert_3)(SET* self, RBT* node)
{
    if(JOIN(RBT, is_red)(JOIN(RBT, uncle)(node)))
    {
        node->p->color = 1;
        JOIN(RBT, uncle)(node)->color = 1;
        JOIN(RBT, grandfather)(node)->color = 0;
        JOIN(SET, insert_1)(self, JOIN(RBT, grandfather)(node));
    }
    else
        JOIN(SET, insert_4)(self, node);
}

static inline void
JOIN(SET, insert_4)(SET* self, RBT* node)
{
    if(node == node->p->r && node->p == JOIN(RBT, grandfather)(node)->l)
    {
        JOIN(SET, rotate_l)(self, node->p);
        node = node->l;
    }
    else
    if(node == node->p->l && node->p == JOIN(RBT, grandfather)(node)->r)
    {
        JOIN(SET, rotate_r)(self, node->p);
        node = node->r;
    }
    JOIN(SET, insert_5)(self, node);
}

static inline void
JOIN(SET, insert_5)(SET* self, RBT* node)
{
    node->p->color = 1;
    JOIN(RBT, grandfather)(node)->color = 0;
    if(node == node->p->l && node->p == JOIN(RBT, grandfather)(node)->l)
        JOIN(SET, rotate_r)(self, JOIN(RBT, grandfather)(node));
    else
        JOIN(SET, rotate_l)(self, JOIN(RBT, grandfather)(node));
}

static inline void
JOIN(SET, erase_1)(SET*, RBT*),
JOIN(SET, erase_2)(SET*, RBT*),
JOIN(SET, erase_3)(SET*, RBT*),
JOIN(SET, erase_4)(SET*, RBT*),
JOIN(SET, erase_5)(SET*, RBT*),
JOIN(SET, erase_6)(SET*, RBT*);

static inline void
JOIN(SET, erase_node)(SET* self, RBT* node)
{
    if(node->l && node->r)
    {
        RBT* pred = JOIN(RBT, max)(node->l);
        SWAP(T, &node->key, &pred->key);
        node = pred;
    }
    RBT* child = node->r ? node->r : node->l;
    if(JOIN(RBT, is_blk)(node))
    {
        node->color = JOIN(RBT, color)(child);
        JOIN(SET, erase_1)(self, node);
    }
    JOIN(RBT, replace)(self, node, child);
    if(node->p == NULL && child)
        child->color = 1;
    JOIN(SET, free_node)(self, node);
    self->size -= 1;
#ifdef USE_INTERNAL_VERIFY
    JOIN(SET, verify)(self);
#endif
}

static inline void
JOIN(SET, erase)(SET* self, T key)
{
    RBT* node = JOIN(SET, find)(self, key);
    if(node)
        JOIN(SET, erase_node)(self, node);
}

static inline void
JOIN(SET, erase_1)(SET* self, RBT* node)
{
    if(node->p)
        JOIN(SET, erase_2)(self, node);
}

static inline void
JOIN(SET, erase_2)(SET* self, RBT* node)
{
    if(JOIN(RBT, is_red)(JOIN(RBT, sibling)(node)))
    {
        node->p->color = 0;
        JOIN(RBT, sibling)(node)->color = 1;
        if(node == node->p->l)
            JOIN(SET, rotate_l)(self, node->p);
        else
            JOIN(SET, rotate_r)(self, node->p);
    }
    JOIN(SET, erase_3)(self, node);
}

static inline void
JOIN(SET, erase_3)(SET* self, RBT* node)
{
    if(JOIN(RBT, is_blk)(node->p)
    && JOIN(RBT, is_blk)(JOIN(RBT, sibling)(node))
    && JOIN(RBT, is_blk)(JOIN(RBT, sibling)(node)->l)
    && JOIN(RBT, is_blk)(JOIN(RBT, sibling)(node)->r))
    {
        JOIN(RBT, sibling)(node)->color = 0;
        JOIN(SET, erase_1)(self, node->p);
    }
    else
        JOIN(SET, erase_4)(self, node);
}

static inline void
JOIN(SET, erase_4)(SET* self, RBT* node)
{
    if(JOIN(RBT, is_red)(node->p)
    && JOIN(RBT, is_blk)(JOIN(RBT, sibling)(node))
    && JOIN(RBT, is_blk)(JOIN(RBT, sibling)(node)->l)
    && JOIN(RBT, is_blk)(JOIN(RBT, sibling)(node)->r))
    {
        JOIN(RBT, sibling)(node)->color = 0;
        node->p->color = 1;
    }
    else
        JOIN(SET, erase_5)(self, node);
}

static inline void
JOIN(SET, erase_5)(SET* self, RBT* node)
{
    if(node == node->p->l
    && JOIN(RBT, is_blk)(JOIN(RBT, sibling)(node))
    && JOIN(RBT, is_red)(JOIN(RBT, sibling)(node)->l)
    && JOIN(RBT, is_blk)(JOIN(RBT, sibling)(node)->r))
    {
        JOIN(RBT, sibling)(node)->color = 0;
        JOIN(RBT, sibling)(node)->l->color = 1;
        JOIN(SET, rotate_r)(self, JOIN(RBT, sibling)(node));
    }
    else
    if(node == node->p->r
    && JOIN(RBT, is_blk)(JOIN(RBT, sibling)(node))
    && JOIN(RBT, is_red)(JOIN(RBT, sibling)(node)->r)
    && JOIN(RBT, is_blk)(JOIN(RBT, sibling)(node)->l))
    {
        JOIN(RBT, sibling)(node)->color = 0;
        JOIN(RBT, sibling)(node)->r->color = 1;
        JOIN(SET, rotate_l)(self, JOIN(RBT, sibling)(node));
    }
    JOIN(SET, erase_6)(self, node);
}

static inline void
JOIN(SET, erase_6)(SET* self, RBT* node)
{
    JOIN(RBT, sibling)(node)->color = JOIN(RBT, color)(node->p);
    node->p->color = 1;
    if(node == node->p->l)
    {
        JOIN(RBT, sibling)(node)->r->color = 1;
        JOIN(SET, rotate_l)(self, node->p);
    }
    else
    {
        JOIN(RBT, sibling)(node)->l->color = 1;
        JOIN(SET, rotate_r)(self, node->p);
    }
}

static inline void
JOIN(SET, clear)(SET* self)
{
    while(!JOIN(SET, empty)(self))
        JOIN(SET, erase)(self, self->root->key);
}

static inline void
JOIN(SET, free)(SET* self)
{
    JOIN(SET, clear)(self);
    *self = JOIN(SET, init)(self->compare);
}

static inline SET
JOIN(SET, shallow_copy)(SET* self)
{
    SETIT it = JOIN(SETIT, each)(self);
    SET copy = JOIN(SET, init)(self->compare);
    copy.copy = self->copy;
    copy.free = NULL;
    while(!it.done)
    {
        JOIN(SET, insert)(&copy, it.node->key);
        it.step(&it);
    }
    return copy;
}

static inline SET
JOIN(SET, deep_copy)(SET* self)
{
    SETIT it = JOIN(SETIT, each)(self);
    SET copy = JOIN(SET, init)(self->compare);
    copy.copy = self->copy;
    copy.free = self->free;
    while(!it.done)
    {
        if(copy.copy)
            JOIN(SET, insert)(&copy, copy.copy(&it.node->key));
        else
            JOIN(SET, insert)(&copy, it.node->key);
        it.step(&it);
    }
    return copy;
}

static inline size_t
JOIN(SET, remove_if)(SET* self, int _match(T*))
{
    size_t erases = 0;
    foreach(SET, self, it)
        if(_match(&it.node->key))
        {
            JOIN(SET, erase_node)(self, it.node);
            erases += 1;
        }
    return erases;
}

static inline SET
JOIN(SET, intersection)(SET* a, SET* b)
{
    SET self = JOIN(SET, init)(a->compare);
    foreach(SET, a, i)
        if(JOIN(SET, find)(b, *i.ref)){
            if(self.copy){
                JOIN(SET, insert)(&self, self.copy(i.ref));
            }else{
                JOIN(SET, insert)(&self, *i.ref);
            }
        }
    return self;
}

static inline SET
JOIN(SET, union)(SET* a, SET* b)
{
    SET self = JOIN(SET, init)(a->compare);
    self.copy = a->copy;
    foreach(SET, a, i)
        if(self.copy)
            JOIN(SET, insert)(&self, self.copy(i.ref));
        else
            JOIN(SET, insert)(&self, *i.ref);
    foreach(SET, b, i)
        if(self.copy)
            JOIN(SET, insert)(&self, self.copy(i.ref));
        else
            JOIN(SET, insert)(&self, *i.ref);
    return self;
}

static inline SET
JOIN(SET, difference)(SET* a, SET* b)
{
    SET self = JOIN(SET, deep_copy)(a);
    foreach(SET, b, i)
        JOIN(SET, erase)(&self, *i.ref);
    return self;
}

static inline SET
JOIN(SET, symmetric_difference)(SET* a, SET* b)
{
    SET self = JOIN(SET, union)(a, b);
    SET intersection = JOIN(SET, intersection)(a, b);
    foreach(SET, &intersection, i)
        JOIN(SET, erase)(&self, *i.ref);
    JOIN(SET, free)(&intersection);
    return self;
}

#ifdef ALLOW_DUPLICATES

static inline void
JOIN(SET, equal_range)(SET* self, T key, RBT** begin, RBT** end)
{
    *begin = NULL;
    *end = NULL;

    RBT* node = self->root;
    RBT* first = NULL;
    while(node)
    {
        int diff = self->compare(&key, &node->key);
        if(diff < 0)
            node = node->l;
        else if(diff > 0)
            node = node->r;
        else
        {

            first = node;
            node = node->l;
        }
    }

    if(!first)
        return;

    *begin = first;

    node = first;
    while(node && self->compare(&key, &node->key) == 0)
    {
        *end = JOIN(RBT, next)(node);
        node = JOIN(RBT, next)(node);
    }
}

static inline size_t
JOIN(SET, erase_all)(SET* self, T key)
{
    size_t count = 0;

    RBT* begin = NULL;
    RBT* end = NULL;
    JOIN(SET, equal_range)(self, key, &begin, &end);

    RBT* node = begin;
    while(node != end)
    {
        RBT* next = JOIN(RBT, next)(node);
        JOIN(SET, erase_node)(self, node);
        count++;
        node = next;
    }
    return count;
}

static inline size_t
JOIN(SET, count_all)(SET* self, T key)
{
    size_t count = 0;

    RBT* begin = NULL;
    RBT* end = NULL;
    JOIN(SET, equal_range)(self, key, &begin, &end);

    RBT* node = begin;
    while(node != end)
    {
        count++;
        node = JOIN(RBT, next)(node);
    }
    return count;
}
#endif

#ifdef HOLD
#undef HOLD
#else
    #undef T
#endif
#undef SET
#undef RBT
#undef SETIT

#ifdef USE_INTERNAL_VERIFY
#undef USE_INTERNAL_VERIFY
#endif

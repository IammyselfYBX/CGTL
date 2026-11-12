#ifndef CGTL_ITERATOR_H
#define CGTL_ITERATOR_H

#include <stddef.h>
#include "cgtl.h"

typedef enum cgtl_iter_category
{
    CGTL_ITER_INPUT = 0,
    CGTL_ITER_FORWARD = 1,
    CGTL_ITER_BIDIRECTIONAL = 2,
    CGTL_ITER_RANDOM_ACCESS = 3
} cgtl_iter_category;

typedef struct cgtl_iter cgtl_iter;
typedef struct cgtl_iter_ops cgtl_iter_ops;

struct cgtl_iter_ops
{
    void (*step)(cgtl_iter*);
    int (*equal)(const cgtl_iter*, const cgtl_iter*);
    ptrdiff_t (*distance)(const cgtl_iter*, const cgtl_iter*);
    cgtl_iter (*at)(const cgtl_iter*, ptrdiff_t);
};

struct cgtl_iter
{
    void* ref;
    void* cur;
    void* end_ref;
    void* container;
    size_t index;
    size_t index_end;
    size_t repeat;
    int done;
    cgtl_iter_category category;
    const cgtl_iter_ops* ops;
};

typedef struct cgtl_range
{
    cgtl_iter first;
    cgtl_iter last;
} cgtl_range;

typedef struct cgtl_out_iter cgtl_out_iter;

typedef struct cgtl_out_iter_ops
{
    void (*put)(cgtl_out_iter*, const void*);
} cgtl_out_iter_ops;

struct cgtl_out_iter
{
    void* target;
    const cgtl_out_iter_ops* ops;
};

#define CGTL_ITER_REF(T, iter) ((T*)((iter).ref))

static inline int
cgtl_iter_is_end(const cgtl_iter* it)
{
    return it == NULL || it->done;
}

static inline void
cgtl_iter_step(cgtl_iter* it)
{
    if(it && !it->done && it->ops && it->ops->step)
        it->ops->step(it);
}

static inline int
cgtl_iter_equal(const cgtl_iter* a, const cgtl_iter* b)
{
    if(!a || !b)
        return a == b;

    if(a->done || b->done)
        return a->done && b->done;

    if(a->ops && a->ops->equal)
        return a->ops->equal(a, b);

    return a->cur == b->cur
        && a->container == b->container
        && a->index == b->index;
}

static inline cgtl_iter
cgtl_iter_at(const cgtl_iter* it, ptrdiff_t n)
{
    if(!it)
    {
        cgtl_iter zero = {0};
        zero.done = 1;
        return zero;
    }

    if(n < 0)
        n = 0;

    if(it->ops && it->ops->at)
        return it->ops->at(it, n);

    cgtl_iter out = *it;
    while(n-- > 0 && !out.done)
        cgtl_iter_step(&out);
    return out;
}

static inline ptrdiff_t
cgtl_iter_distance(const cgtl_iter* first, const cgtl_iter* last)
{
    if(!first || !last || cgtl_iter_equal(first, last))
        return 0;

    if(first->ops && first->ops->distance)
        return first->ops->distance(first, last);

    ptrdiff_t n = 0;
    cgtl_iter it = *first;
    while(!it.done && !cgtl_iter_equal(&it, last))
    {
        cgtl_iter_step(&it);
        n++;
    }
    return n;
}

static inline int
cgtl_range_empty(const cgtl_range* range)
{
    return range == NULL || cgtl_iter_equal(&range->first, &range->last);
}

static inline ptrdiff_t
cgtl_range_distance(const cgtl_range* range)
{
    if(!range)
        return 0;
    return cgtl_iter_distance(&range->first, &range->last);
}

static inline void
cgtl_out_put(cgtl_out_iter* out, const void* value)
{
    if(out && out->ops && out->ops->put)
        out->ops->put(out, value);
}

#endif

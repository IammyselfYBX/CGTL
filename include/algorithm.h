#ifndef CGTL_ALGORITHM_T
#define CGTL_ALGORITHM_T

#ifndef T
#error "Template type T undefined for <algorithm.h>"
#endif

#include "iterator.h"

#ifndef CGTL_ALGO_PREFIX
#define CGTL_ALGO_PREFIX JOIN(algo, T)
#define CGTL_ALGO_PREFIX_LOCAL 1
#endif

#define ALG CGTL_ALGO_PREFIX

static inline void
JOIN(ALG, for_each)(cgtl_range range, void (*fn)(T*))
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        fn((T*)it.ref);
}

static inline int
JOIN(ALG, all_of)(cgtl_range range, int (*pred)(T*))
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        if(!pred((T*)it.ref))
            return 0;
    return 1;
}

static inline int
JOIN(ALG, any_of)(cgtl_range range, int (*pred)(T*))
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        if(pred((T*)it.ref))
            return 1;
    return 0;
}

static inline int
JOIN(ALG, none_of)(cgtl_range range, int (*pred)(T*))
{
    return !JOIN(ALG, any_of)(range, pred);
}

static inline cgtl_iter
JOIN(ALG, find_if)(cgtl_range range, int (*pred)(T*))
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        if(pred((T*)it.ref))
            return it;
    return range.last;
}

static inline cgtl_iter
JOIN(ALG, find_if_not)(cgtl_range range, int (*pred)(T*))
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        if(!pred((T*)it.ref))
            return it;
    return range.last;
}

static inline cgtl_iter
JOIN(ALG, find)(cgtl_range range, T value, int (*equal)(T*, T*))
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        if(equal((T*)it.ref, &value))
            return it;
    return range.last;
}

static inline size_t
JOIN(ALG, count_if)(cgtl_range range, int (*pred)(T*))
{
    size_t count = 0;
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        if(pred((T*)it.ref))
            count++;
    return count;
}

static inline size_t
JOIN(ALG, count)(cgtl_range range, T value, int (*equal)(T*, T*))
{
    size_t count = 0;
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        if(equal((T*)it.ref, &value))
            count++;
    return count;
}

static inline int
JOIN(ALG, equal)(cgtl_range lhs, cgtl_range rhs, int (*equal)(T*, T*))
{
    cgtl_iter a = lhs.first;
    cgtl_iter b = rhs.first;

    while(!cgtl_iter_equal(&a, &lhs.last)
       && !cgtl_iter_equal(&b, &rhs.last))
    {
        if(!equal((T*)a.ref, (T*)b.ref))
            return 0;
        cgtl_iter_step(&a);
        cgtl_iter_step(&b);
    }

    return cgtl_iter_equal(&a, &lhs.last)
        && cgtl_iter_equal(&b, &rhs.last);
}

static inline T*
JOIN(ALG, copy)(cgtl_range range, T* out)
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        *out++ = *(T*)it.ref;
    return out;
}

static inline T*
JOIN(ALG, copy_if)(cgtl_range range, T* out, int (*pred)(T*))
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        if(pred((T*)it.ref))
            *out++ = *(T*)it.ref;
    return out;
}

static inline T*
JOIN(ALG, transform)(cgtl_range range, T* out, T (*op)(T*))
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        *out++ = op((T*)it.ref);
    return out;
}

static inline cgtl_out_iter*
JOIN(ALG, copy_to)(cgtl_range range, cgtl_out_iter* out)
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        cgtl_out_put(out, it.ref);
    return out;
}

static inline cgtl_out_iter*
JOIN(ALG, transform_to)(cgtl_range range, cgtl_out_iter* out, T (*op)(T*))
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
    {
        T value = op((T*)it.ref);
        cgtl_out_put(out, &value);
    }
    return out;
}

static inline void
JOIN(ALG, fill)(cgtl_range range, T value)
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        *(T*)it.ref = value;
}

static inline void
JOIN(ALG, replace)(cgtl_range range, T old_value, T new_value,
                   int (*equal)(T*, T*))
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        if(equal((T*)it.ref, &old_value))
            *(T*)it.ref = new_value;
}

static inline void
JOIN(ALG, replace_if)(cgtl_range range, int (*pred)(T*), T new_value)
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        if(pred((T*)it.ref))
            *(T*)it.ref = new_value;
}

static inline cgtl_iter
JOIN(ALG, remove_if)(cgtl_range range, int (*pred)(T*))
{
    cgtl_iter out = range.first;

    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
    {
        if(!pred((T*)it.ref))
        {
            if(!cgtl_iter_equal(&out, &it))
                *(T*)out.ref = *(T*)it.ref;
            cgtl_iter_step(&out);
        }
    }
    return out;
}

static inline cgtl_iter
JOIN(ALG, remove)(cgtl_range range, T value, int (*equal)(T*, T*))
{
    cgtl_iter out = range.first;

    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
    {
        if(!equal((T*)it.ref, &value))
        {
            if(!cgtl_iter_equal(&out, &it))
                *(T*)out.ref = *(T*)it.ref;
            cgtl_iter_step(&out);
        }
    }
    return out;
}

static inline cgtl_iter
JOIN(ALG, unique)(cgtl_range range, int (*equal)(T*, T*))
{
    if(cgtl_iter_equal(&range.first, &range.last))
        return range.first;

    cgtl_iter out = range.first;
    cgtl_iter it = range.first;
    cgtl_iter_step(&it);

    while(!cgtl_iter_equal(&it, &range.last))
    {
        if(!equal((T*)it.ref, (T*)out.ref))
        {
            cgtl_iter_step(&out);
            if(!cgtl_iter_equal(&out, &it))
                *(T*)out.ref = *(T*)it.ref;
        }
        cgtl_iter_step(&it);
    }

    if(!cgtl_iter_equal(&out, &range.first))
        cgtl_iter_step(&out);
    return out;
}

static inline cgtl_iter
JOIN(ALG, min_element)(cgtl_range range, int (*cmp)(T*, T*))
{
    cgtl_iter best = range.first;
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        if(cmp((T*)it.ref, (T*)best.ref) < 0)
            best = it;
    return best;
}

static inline cgtl_iter
JOIN(ALG, max_element)(cgtl_range range, int (*cmp)(T*, T*))
{
    cgtl_iter best = range.first;
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        if(cmp((T*)it.ref, (T*)best.ref) > 0)
            best = it;
    return best;
}

static inline int
JOIN(ALG, lexicographical_compare)(cgtl_range lhs, cgtl_range rhs,
                                  int (*cmp)(T*, T*))
{
    cgtl_iter a = lhs.first;
    cgtl_iter b = rhs.first;

    while(!cgtl_iter_equal(&a, &lhs.last)
       && !cgtl_iter_equal(&b, &rhs.last))
    {
        if(cmp((T*)a.ref, (T*)b.ref) < 0)
            return 1;
        if(cmp((T*)a.ref, (T*)b.ref) > 0)
            return 0;
        cgtl_iter_step(&a);
        cgtl_iter_step(&b);
    }
    return cgtl_iter_equal(&a, &lhs.last)
        && !cgtl_iter_equal(&b, &rhs.last);
}

static inline void
JOIN(ALG, replace_at)(cgtl_range range, ptrdiff_t index, T value)
{
    cgtl_iter it = cgtl_iter_at(&range.first, index);
    if(!cgtl_iter_equal(&it, &range.last))
        *(T*)it.ref = value;
}

static inline void
JOIN(ALG, reverse)(cgtl_range range)
{
    if(range.first.category < CGTL_ITER_RANDOM_ACCESS)
        return;

    ptrdiff_t n = cgtl_iter_distance(&range.first, &range.last);
    for(ptrdiff_t i = 0, j = n - 1; i < j; i++, j--)
    {
        cgtl_iter a = cgtl_iter_at(&range.first, i);
        cgtl_iter b = cgtl_iter_at(&range.first, j);
        T tmp = *(T*)a.ref;
        *(T*)a.ref = *(T*)b.ref;
        *(T*)b.ref = tmp;
    }
}

static inline void
JOIN(ALG, legacy_sort_range)(cgtl_range range,
                             ptrdiff_t a, ptrdiff_t b,
                             int (*cmp)(T*, T*), int inclusive)
{
    if(a >= b)
        return;

    ptrdiff_t mid = a + (b - a) / 2;
    cgtl_iter pa = cgtl_iter_at(&range.first, a);
    cgtl_iter pm = cgtl_iter_at(&range.first, mid);
    T tmp = *(T*)pa.ref;
    *(T*)pa.ref = *(T*)pm.ref;
    *(T*)pm.ref = tmp;

    ptrdiff_t z = a;
    for(ptrdiff_t i = a + 1; i <= b; i++)
    {
        cgtl_iter pi = cgtl_iter_at(&range.first, i);
        cgtl_iter pa2 = cgtl_iter_at(&range.first, a);
        int order = cmp((T*)pi.ref, (T*)pa2.ref);
        if(inclusive ? (order <= 0) : (order < 0))
        {
            z++;
            cgtl_iter pz = cgtl_iter_at(&range.first, z);
            pi = cgtl_iter_at(&range.first, i);
            tmp = *(T*)pz.ref;
            *(T*)pz.ref = *(T*)pi.ref;
            *(T*)pi.ref = tmp;
        }
    }

    pa = cgtl_iter_at(&range.first, a);
    cgtl_iter pz = cgtl_iter_at(&range.first, z);
    tmp = *(T*)pa.ref;
    *(T*)pa.ref = *(T*)pz.ref;
    *(T*)pz.ref = tmp;

    JOIN(ALG, legacy_sort_range)(range, a, z - 1, cmp, inclusive);
    JOIN(ALG, legacy_sort_range)(range, z + 1, b, cmp, inclusive);
}

static inline void
JOIN(ALG, legacy_sort)(cgtl_range range, int (*cmp)(T*, T*), int inclusive)
{
    ptrdiff_t n = cgtl_iter_distance(&range.first, &range.last);
    if(n > 1)
        JOIN(ALG, legacy_sort_range)(range, 0, n - 1, cmp, inclusive);
}

static inline void
JOIN(ALG, sort)(cgtl_range range, int (*cmp)(T*, T*))
{
    if(range.first.category < CGTL_ITER_RANDOM_ACCESS)
        return;

    ptrdiff_t n = cgtl_iter_distance(&range.first, &range.last);
    if(n < 2)
        return;

    for(ptrdiff_t gap = n / 2; gap > 0; gap /= 2)
    {
        for(ptrdiff_t i = gap; i < n; i++)
        {
            cgtl_iter cur = cgtl_iter_at(&range.first, i);
            T key = *(T*)cur.ref;
            ptrdiff_t j = i;

            while(j >= gap)
            {
                cgtl_iter prev = cgtl_iter_at(&range.first, j - gap);
                if(cmp((T*)prev.ref, &key) <= 0)
                    break;
                cgtl_iter dest = cgtl_iter_at(&range.first, j);
                *(T*)dest.ref = *(T*)prev.ref;
                j -= gap;
            }

            cgtl_iter dest = cgtl_iter_at(&range.first, j);
            *(T*)dest.ref = key;
        }
    }
}

static inline int
JOIN(ALG, is_sorted)(cgtl_range range, int (*cmp)(T*, T*))
{
    if(cgtl_iter_equal(&range.first, &range.last))
        return 1;

    cgtl_iter prev = range.first;
    cgtl_iter it = range.first;
    cgtl_iter_step(&it);

    while(!cgtl_iter_equal(&it, &range.last))
    {
        if(cmp((T*)prev.ref, (T*)it.ref) > 0)
            return 0;
        prev = it;
        cgtl_iter_step(&it);
    }
    return 1;
}

static inline cgtl_out_iter*
JOIN(ALG, set_union)(cgtl_range lhs, cgtl_range rhs,
                     cgtl_out_iter* out, int (*cmp)(T*, T*))
{
    cgtl_iter a = lhs.first;
    cgtl_iter b = rhs.first;

    while(!cgtl_iter_equal(&a, &lhs.last)
       && !cgtl_iter_equal(&b, &rhs.last))
    {
        int order = cmp((T*)a.ref, (T*)b.ref);
        if(order < 0)
        {
            cgtl_out_put(out, a.ref);
            cgtl_iter_step(&a);
        }
        else if(order > 0)
        {
            cgtl_out_put(out, b.ref);
            cgtl_iter_step(&b);
        }
        else
        {
            cgtl_out_put(out, a.ref);
            cgtl_iter_step(&a);
            cgtl_iter_step(&b);
        }
    }
    while(!cgtl_iter_equal(&a, &lhs.last))
    {
        cgtl_out_put(out, a.ref);
        cgtl_iter_step(&a);
    }
    while(!cgtl_iter_equal(&b, &rhs.last))
    {
        cgtl_out_put(out, b.ref);
        cgtl_iter_step(&b);
    }
    return out;
}

static inline cgtl_out_iter*
JOIN(ALG, set_intersection)(cgtl_range lhs, cgtl_range rhs,
                            cgtl_out_iter* out, int (*cmp)(T*, T*))
{
    cgtl_iter a = lhs.first;
    cgtl_iter b = rhs.first;

    while(!cgtl_iter_equal(&a, &lhs.last)
       && !cgtl_iter_equal(&b, &rhs.last))
    {
        int order = cmp((T*)a.ref, (T*)b.ref);
        if(order < 0)
            cgtl_iter_step(&a);
        else if(order > 0)
            cgtl_iter_step(&b);
        else
        {
            cgtl_out_put(out, a.ref);
            cgtl_iter_step(&a);
            cgtl_iter_step(&b);
        }
    }
    return out;
}

static inline cgtl_out_iter*
JOIN(ALG, set_difference)(cgtl_range lhs, cgtl_range rhs,
                          cgtl_out_iter* out, int (*cmp)(T*, T*))
{
    cgtl_iter a = lhs.first;
    cgtl_iter b = rhs.first;

    while(!cgtl_iter_equal(&a, &lhs.last)
       && !cgtl_iter_equal(&b, &rhs.last))
    {
        int order = cmp((T*)a.ref, (T*)b.ref);
        if(order < 0)
        {
            cgtl_out_put(out, a.ref);
            cgtl_iter_step(&a);
        }
        else if(order > 0)
            cgtl_iter_step(&b);
        else
        {
            cgtl_iter_step(&a);
            cgtl_iter_step(&b);
        }
    }
    while(!cgtl_iter_equal(&a, &lhs.last))
    {
        cgtl_out_put(out, a.ref);
        cgtl_iter_step(&a);
    }
    return out;
}

static inline cgtl_out_iter*
JOIN(ALG, set_symmetric_difference)(cgtl_range lhs, cgtl_range rhs,
                                    cgtl_out_iter* out, int (*cmp)(T*, T*))
{
    cgtl_iter a = lhs.first;
    cgtl_iter b = rhs.first;

    while(!cgtl_iter_equal(&a, &lhs.last)
       && !cgtl_iter_equal(&b, &rhs.last))
    {
        int order = cmp((T*)a.ref, (T*)b.ref);
        if(order < 0)
        {
            cgtl_out_put(out, a.ref);
            cgtl_iter_step(&a);
        }
        else if(order > 0)
        {
            cgtl_out_put(out, b.ref);
            cgtl_iter_step(&b);
        }
        else
        {
            cgtl_iter_step(&a);
            cgtl_iter_step(&b);
        }
    }
    while(!cgtl_iter_equal(&a, &lhs.last))
    {
        cgtl_out_put(out, a.ref);
        cgtl_iter_step(&a);
    }
    while(!cgtl_iter_equal(&b, &rhs.last))
    {
        cgtl_out_put(out, b.ref);
        cgtl_iter_step(&b);
    }
    return out;
}

static inline cgtl_iter
JOIN(ALG, lower_bound)(cgtl_range range, T value, int (*cmp)(T*, T*))
{
    if(cgtl_range_empty(&range)
    || range.first.category < CGTL_ITER_RANDOM_ACCESS)
        return range.last;

    ptrdiff_t first = 0;
    ptrdiff_t count = cgtl_iter_distance(&range.first, &range.last);

    while(count > 0)
    {
        ptrdiff_t step = count / 2;
        cgtl_iter mid = cgtl_iter_at(&range.first, first + step);

        if(cmp((T*)mid.ref, &value) < 0)
        {
            first += step + 1;
            count -= step + 1;
        }
        else
            count = step;
    }
    return cgtl_iter_at(&range.first, first);
}

static inline cgtl_iter
JOIN(ALG, upper_bound)(cgtl_range range, T value, int (*cmp)(T*, T*))
{
    if(cgtl_range_empty(&range)
    || range.first.category < CGTL_ITER_RANDOM_ACCESS)
        return range.last;

    ptrdiff_t first = 0;
    ptrdiff_t count = cgtl_iter_distance(&range.first, &range.last);

    while(count > 0)
    {
        ptrdiff_t step = count / 2;
        cgtl_iter mid = cgtl_iter_at(&range.first, first + step);

        if(cmp(&value, (T*)mid.ref) < 0)
            count = step;
        else
        {
            first += step + 1;
            count -= step + 1;
        }
    }
    return cgtl_iter_at(&range.first, first);
}

static inline int
JOIN(ALG, binary_search)(cgtl_range range, T value, int (*cmp)(T*, T*))
{
    cgtl_iter it = JOIN(ALG, lower_bound)(range, value, cmp);
    return !cgtl_iter_equal(&it, &range.last)
        && cmp((T*)it.ref, &value) == 0;
}

static inline T*
JOIN(ALG, data)(cgtl_range range)
{
    return range.first.ref;
}

#ifdef U
#define ALG2 JOIN(ALG, JOIN(to, U))

static inline U*
JOIN(ALG2, copy)(cgtl_range range, U* out, U (*convert)(T*))
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
        *out++ = convert((T*)it.ref);
    return out;
}

static inline U*
JOIN(ALG2, transform)(cgtl_range range, U* out, U (*op)(T*))
{
    return JOIN(ALG2, copy)(range, out, op);
}

static inline cgtl_out_iter*
JOIN(ALG2, copy_to)(cgtl_range range, cgtl_out_iter* out, U (*convert)(T*))
{
    for(cgtl_iter it = range.first;
        !cgtl_iter_equal(&it, &range.last);
        cgtl_iter_step(&it))
    {
        U value = convert((T*)it.ref);
        cgtl_out_put(out, &value);
    }
    return out;
}

static inline cgtl_out_iter*
JOIN(ALG2, transform_to)(cgtl_range range, cgtl_out_iter* out, U (*op)(T*))
{
    return JOIN(ALG2, copy_to)(range, out, op);
}

#undef ALG2
#endif

#undef ALG

#ifdef CGTL_ALGO_PREFIX_LOCAL

#undef CGTL_ALGO_PREFIX
#undef CGTL_ALGO_PREFIX_LOCAL

#ifdef HOLD
#undef HOLD
#else
#undef T
#endif
#else

#endif

#ifdef U
#undef U
#endif

#undef CGTL_ALGORITHM_T
#endif

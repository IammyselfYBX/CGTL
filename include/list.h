#ifndef T
#error "Template type T undefined for <list.h>"
#endif

#include "cgtl.h"

#define LST JOIN(lst, T)
#define LSTNODE JOIN(LST, node)
#define LSTIT JOIN(LST, it)

typedef struct LSTNODE
{
    struct LSTNODE* prev;
    struct LSTNODE* next;
    T value;
} LSTNODE;

typedef struct LST
{
    LSTNODE* head;
    LSTNODE* tail;
    size_t size;
    void (*free)(T*);
    T (*copy)(T*);
} LST;

typedef struct LSTIT
{
    T* ref;
    LSTNODE* begin;
    LSTNODE* node;
    LSTNODE* next;
    LSTNODE* end;
    int done;
    void (*step)(struct LSTIT*);
} LSTIT;

static inline T*
JOIN(LST, front)(LST* self)
{
    return &self->head->value;
}

static inline T*
JOIN(LST, back)(LST* self)
{
    return &self->tail->value;
}

static inline LSTNODE*
JOIN(LST, begin)(LST* self)
{
    return self->head;
}

static inline LSTNODE*
JOIN(LST, end)(LST* self)
{
    (void) self;
    return NULL;
}

static inline void
JOIN(LSTIT, step)(LSTIT* self)
{
    if(self->next == self->end)
        self->done = 1;
    else
    {
        self->node = self->next;
        self->ref = &self->node->value;
        self->next = self->node->next;
    }
}

static inline LSTIT
JOIN(LSTIT, range)(LST* container, LSTNODE* begin, LSTNODE* end)
{
    (void) container;
    static LSTIT zero;
    LSTIT self = zero;
    if(begin)
    {
        self.step = JOIN(LSTIT, step);
        self.begin = begin;
        self.end = end;
        self.next = begin->next;
        self.node = begin;
        self.ref = &begin->value;
    }
    else
        self.done = 1;
    return self;
}

static inline int
JOIN(LST, empty)(LST* self)
{
    return self->size == 0;
}

static inline LSTIT
JOIN(LSTIT, each)(LST* a)
{
    return JOIN(LST, empty)(a)
         ? JOIN(LSTIT, range)(a, NULL, NULL)
         : JOIN(LSTIT, range)(a, JOIN(LST, begin)(a), JOIN(LST, end)(a));
}

static inline T
JOIN(LST, implicit_copy)(T* self)
{
    return *self;
}

static inline int
JOIN(LST, equal)(LST* self, LST* other, int _equal(T*, T*))
{
    if(self->size != other->size)
        return 0;
    LSTIT a = JOIN(LSTIT, each)(self);
    LSTIT b = JOIN(LSTIT, each)(other);
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
JOIN(LST, swap)(LST* self, LST* other)
{
    LST temp = *self;
    *self = *other;
    *other = temp;
}

static inline LST
JOIN(LST, init)(void)
{
    static LST zero;
    LST self = zero;

    self.free = NULL;
    self.copy = NULL;
    return self;
}

static inline LSTNODE*
JOIN(LSTNODE, init)(T value)
{
    LSTNODE* self = (LSTNODE*) malloc(sizeof(LSTNODE));
    self->prev = self->next = NULL;
    self->value = value;
    return self;
}

static inline void
JOIN(LST, disconnect)(LST* self, LSTNODE* node)
{
    if(node == self->tail) self->tail = self->tail->prev;
    if(node == self->head) self->head = self->head->next;
    if(node->prev) node->prev->next = node->next;
    if(node->next) node->next->prev = node->prev;
    node->prev = node->next = NULL;
    self->size -= 1;
}

static inline void
JOIN(LST, connect)(LST* self, LSTNODE* position, LSTNODE* node, int before)
{
    if(JOIN(LST, empty)(self))
        self->head = self->tail = node;
    else
    if(before)
    {
        node->next = position;
        node->prev = position->prev;
        if(position->prev)
            position->prev->next = node;
        position->prev = node;
        if(position == self->head)
            self->head = node;
    }
    else
    {
        node->prev = position;
        node->next = position->next;
        if(position->next)
            position->next->prev = node;
        position->next = node;
        if(position == self->tail)
            self->tail = node;
    }
    self->size += 1;
}

static inline void
JOIN(LST, push_back)(LST* self, T value)
{
    LSTNODE* node = JOIN(LSTNODE, init)(value);
    JOIN(LST, connect)(self, self->tail, node, 0);
}

static inline void
JOIN(LST, push_front)(LST* self, T value)
{
    LSTNODE* node = JOIN(LSTNODE, init)(value);
    JOIN(LST, connect)(self, self->head, node, 1);
}

static inline void
JOIN(LST, transfer)(LST* self, LST* other, LSTNODE* position, LSTNODE* node, int before)
{
    JOIN(LST, disconnect)(other, node);
    JOIN(LST, connect)(self, position, node, before);
}

static inline void
JOIN(LST, erase)(LST* self, LSTNODE* node)
{
    JOIN(LST, disconnect)(self, node);
    if(self->free)
        self->free(&node->value);
    free(node);
}

static inline void
JOIN(LST, pop_back)(LST* self)
{
    JOIN(LST, erase)(self, self->tail);
}

static inline void
JOIN(LST, pop_front)(LST* self)
{
    JOIN(LST, erase)(self, self->head);
}

static inline void
JOIN(LST, insert)(LST* self, LSTNODE* position, T value)
{
    LSTNODE* node = JOIN(LSTNODE, init)(value);
    JOIN(LST, connect)(self, position, node, 1);
}

static inline void
JOIN(LST, clear)(LST* self)
{
    while(!JOIN(LST, empty)(self))
        JOIN(LST, pop_back)(self);
}

static inline void
JOIN(LST, free)(LST* self)
{
    JOIN(LST, clear)(self);
    *self = JOIN(LST, init)();
}

static inline void
JOIN(LST, resize)(LST* self, size_t size, T value)
{
    if(size != self->size) {
        for(size_t i = 0; size != self->size; i++) {
            if(size < self->size) {
                JOIN(LST, pop_back)(self);
            } else {

                if(self->copy) {

                    JOIN(LST, push_back)(self, self->copy(&value));
                } else {

                    JOIN(LST, push_back)(self, value);
                }
            }
        }
    }

}

static inline LST
JOIN(LST, shallow_copy)(LST* self)
{
    LST other = JOIN(LST, init)();

    other.copy = self->copy;
    other.free = NULL;

    for(LSTNODE* node = self->head; node; node = node->next) {
        JOIN(LST, push_back)(&other, node->value);
    }
    return other;
}

static inline LST
JOIN(LST, deep_copy)(LST* self)
{
    LST other = JOIN(LST, init)();

    other.copy = self->copy;
    other.free = self->free;

    for(LSTNODE* node = self->head; node; node = node->next) {
        if(other.copy) {

            JOIN(LST, push_back)(&other, other.copy(&node->value));
        } else {

            JOIN(LST, push_back)(&other, node->value);
        }
    }
    return other;
}

static inline void
JOIN(LST, assign)(LST* self, size_t size, T value)
{

    if(self->copy) {

        JOIN(LST, resize)(self, size, self->copy(&value));
        size_t i = 0;
        foreach(LST, self, it)
        {
            if(self->free)
                self->free(it.ref);
            *it.ref = self->copy(&value);
            i += 1;
        }
    } else {

        JOIN(LST, clear)(self);
        for(size_t i = 0; i < size; i++) {
            JOIN(LST, push_back)(self, value);
        }
    }

}

static inline void
JOIN(LST, reverse)(LST* self)
{
    foreach(LST, self, it)
    {
        LSTNODE* next = it.node->next;
        LSTNODE* prev = it.node->prev;
        it.node->prev = next;
        it.node->next = prev;
    }
    LSTNODE* tail = self->tail;
    LSTNODE* head = self->head;
    self->tail = head;
    self->head = tail;
}

static inline size_t
JOIN(LST, remove_if)(LST* self, int _equal(T*))
{
    size_t erases = 0;
    foreach(LST, self, it)
        if(_equal(it.ref))
        {
            JOIN(LST, erase)(self, it.node);
            erases += 1;
        }
    return erases;
}

static inline void
JOIN(LST, splice)(LST* self, LSTNODE* position, LST* other)
{
    if(self->size == 0 && position == NULL)
        JOIN(LST, swap)(self, other);
    else
        foreach(LST, other, it)
            JOIN(LST, transfer)(self, other, position, it.node, 1);
}

static inline void
JOIN(LST, merge)(LST* self, LST* other, int _compare(T*, T*))
{
    if(JOIN(LST, empty)(self))
        JOIN(LST, swap)(self, other);
    else
    {
        for(LSTNODE* node = self->head; node; node = node->next)
            while(!JOIN(LST, empty)(other) && _compare(&node->value, &other->head->value))
                JOIN(LST, transfer)(self, other, node, other->head, 1);

        while(!JOIN(LST, empty)(other))
            JOIN(LST, transfer)(self, other, self->tail, other->head, 0);
    }
}

static inline void
JOIN(LST, sort)(LST* self, int _compare(T*, T*))
{
    if(self->size > 1)
    {
        LST carry = JOIN(LST, init)();
        LST temp[64];
        for(size_t i = 0; i < len(temp); i++)
            temp[i] = JOIN(LST, init)();
        LST* fill = temp;
        LST* counter = NULL;
        do
        {
            JOIN(LST, transfer)(&carry, self, carry.head, self->head, 1);
            for(counter = temp; counter != fill && !JOIN(LST, empty)(counter); counter++)
            {
                JOIN(LST, merge)(counter, &carry, _compare);
                JOIN(LST, swap)(&carry, counter);
            }
            JOIN(LST, swap)(&carry, counter);
            if(counter == fill)
                fill++;
        }
        while(!JOIN(LST, empty)(self));
        for(counter = temp + 1; counter != fill; counter++)
            JOIN(LST, merge)(counter, counter - 1, _compare);
        JOIN(LST, swap)(self, fill - 1);
    }
}

static inline void
JOIN(LST, unique)(LST* self, int _equal(T*, T*))
{
    foreach(LST, self, it)
        if(it.next && _equal(it.ref, &it.next->value))
            JOIN(LST, erase)(self, it.node);
}

static inline void
JOIN(LST, shallow_free)(LST* self)
{

    while(!JOIN(LST, empty)(self)) {
        LSTNODE* node = self->head;
        JOIN(LST, disconnect)(self, node);
        free(node);
    }
    *self = JOIN(LST, init)();
}

static inline LSTNODE*
JOIN(LST, find)(LST* self, T key, int _equal(T*, T*))
{
    foreach(LST, self, it)
        if(_equal(it.ref, &key))
            return it.node;
    return NULL;
}

#undef T
#undef LST
#undef LSTNODE
#undef LSTIT

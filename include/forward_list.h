#ifndef T
#error "Template type T undefined for <forward_list.h>"
#endif
#include "cgtl.h"

#define FLST JOIN(flst, T)
#define FLSTNODE JOIN(FLST, node)
#define FLSTIT JOIN(FLST, it)

typedef struct FLSTNODE
{
    struct FLSTNODE* next;
    T value;
} FLSTNODE;

typedef struct FLST
{
    FLSTNODE* head;
    size_t size;
    void (*free)(T*);
    T (*copy)(T*);
} FLST;

typedef struct FLSTIT
{
    T* ref;
    FLSTNODE* begin;
    FLSTNODE* node;
    FLSTNODE* next;
    FLSTNODE* end;
    int done;
    void (*step)(struct FLSTIT*);
} FLSTIT;

static inline T*
JOIN(FLST, front)(FLST* self)
{
    return &self->head->value;
}

static inline FLSTNODE*
JOIN(FLST, begin)(FLST* self)
{
    return self->head;
}

static inline FLSTNODE*
JOIN(FLST, end)(FLST* self)
{
    (void) self;
    return NULL;
}

static inline void
JOIN(FLSTIT, step)(FLSTIT* self)
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

static inline FLSTIT
JOIN(FLSTIT, range)(FLST* container, FLSTNODE* begin, FLSTNODE* end)
{
    (void) container;
    static FLSTIT zero;
    FLSTIT self = zero;
    if(begin)
    {
        self.step = JOIN(FLSTIT, step);
        self.begin = begin;
        self.end = end;
        self.node = begin;
        self.next = self.node->next;
        self.ref = &self.node->value;
    }
    else
        self.done = 1;
    return self;
}

static inline int
JOIN(FLST, empty)(FLST* self)
{
    return self->size == 0;
}

static inline FLSTIT
JOIN(FLSTIT, each)(FLST* a)
{
    return JOIN(FLST, empty)(a)
         ? JOIN(FLSTIT, range)(a, NULL, NULL)
         : JOIN(FLSTIT, range)(a, JOIN(FLST, begin)(a), JOIN(FLST, end)(a));
}

static inline FLST
JOIN(FLST, init)(void)
{
    static FLST zero;
    FLST self = zero;

    self.copy = NULL;
    self.free = NULL;
    return self;
}

static inline FLSTNODE*
JOIN(FLSTNODE, init)(T value)
{
    FLSTNODE* node = (FLSTNODE*) malloc(sizeof(FLSTNODE));
    node->value = value;
    node->next = NULL;
    return node;
}

static inline void
JOIN(FLST, push_front)(FLST* self, T value)
{
    FLSTNODE* node = JOIN(FLSTNODE, init)(value);
    node->next = self->head;
    self->head = node;
    self->size += 1;
}

static inline void
JOIN(FLST, pop_front)(FLST* self)
{
    if(!JOIN(FLST, empty)(self))
    {
        FLSTNODE* old_head = self->head;
        if(self->free)
            self->free(&old_head->value);
        self->head = self->head->next;
        free(old_head);
        self->size -= 1;
    }
}

static inline FLSTNODE*
JOIN(FLST, insert_after)(FLST* self, FLSTNODE* pos, T value)
{
    if(pos == NULL) {

        JOIN(FLST, push_front)(self, value);
        return self->head;
    }

    FLSTNODE* node = JOIN(FLSTNODE, init)(value);
    node->next = pos->next;
    pos->next = node;
    self->size += 1;
    return node;
}

static inline FLSTNODE*
JOIN(FLST, erase_after)(FLST* self, FLSTNODE* pos)
{
    if(pos == NULL) {

        if(!JOIN(FLST, empty)(self)) {
            JOIN(FLST, pop_front)(self);
            return self->head;
        }
        return NULL;
    }

    if(pos->next != NULL) {
        FLSTNODE* to_delete = pos->next;
        if(self->free)
            self->free(&to_delete->value);
        pos->next = to_delete->next;
        free(to_delete);
        self->size -= 1;
        return pos->next;
    }
    return NULL;
}

static inline void
JOIN(FLST, clear)(FLST* self)
{
    while(!JOIN(FLST, empty)(self))
        JOIN(FLST, pop_front)(self);
}

static inline void
JOIN(FLST, shallow_free)(FLST* self)
{

    FLSTNODE* current = self->head;
    while(current != NULL) {
        FLSTNODE* next = current->next;
        free(current);
        current = next;
    }
    *self = JOIN(FLST, init)();
}

static inline void
JOIN(FLST, free)(FLST* self)
{
    JOIN(FLST, clear)(self);
    *self = JOIN(FLST, init)();
}

static inline size_t
JOIN(FLST, remove)(FLST* self, T value, int _equal(T*, T*))
{
    size_t removed = 0;

    while(!JOIN(FLST, empty)(self) && _equal(&self->head->value, &value)) {
        JOIN(FLST, pop_front)(self);
        removed++;
    }

    if(!JOIN(FLST, empty)(self)) {
        FLSTNODE* current = self->head;
        while(current->next != NULL) {
            if(_equal(&current->next->value, &value)) {
                FLSTNODE* to_delete = current->next;
                if(self->free)
                    self->free(&to_delete->value);
                current->next = to_delete->next;
                free(to_delete);
                self->size -= 1;
                removed++;
            } else {
                current = current->next;
            }
        }
    }
    return removed;
}

static inline size_t
JOIN(FLST, remove_if)(FLST* self, int _match(T*))
{
    size_t removed = 0;

    while(!JOIN(FLST, empty)(self) && _match(&self->head->value)) {
        JOIN(FLST, pop_front)(self);
        removed++;
    }

    if(!JOIN(FLST, empty)(self)) {
        FLSTNODE* current = self->head;
        while(current->next != NULL) {
            if(_match(&current->next->value)) {
                FLSTNODE* to_delete = current->next;
                if(self->free)
                    self->free(&to_delete->value);
                current->next = to_delete->next;
                free(to_delete);
                self->size -= 1;
                removed++;
            } else {
                current = current->next;
            }
        }
    }
    return removed;
}

static inline FLSTNODE*
JOIN(FLST, find)(FLST* self, T value, int _equal(T*, T*))
{
    foreach(FLST, self, it)
        if(_equal(it.ref, &value))
            return it.node;
    return NULL;
}

static inline void
JOIN(FLST, reverse)(FLST* self)
{
    if(self->size <= 1)
        return;

    FLSTNODE* prev = NULL;
    FLSTNODE* current = self->head;
    FLSTNODE* next;

    while(current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    self->head = prev;
}

static inline void
JOIN(FLST, swap)(FLST* self, FLST* other)
{
    FLST temp = *self;
    *self = *other;
    *other = temp;
}

static inline FLST
JOIN(FLST, shallow_copy)(FLST* self)
{
    FLST other = JOIN(FLST, init)();

    other.copy = self->copy;
    other.free = NULL;

    FLST temp = JOIN(FLST, init)();
    foreach(FLST, self, it)
        JOIN(FLST, push_front)(&temp, *it.ref);

    foreach(FLST, &temp, it)
        JOIN(FLST, push_front)(&other, *it.ref);

    JOIN(FLST, free)(&temp);
    return other;
}

static inline FLST
JOIN(FLST, deep_copy)(FLST* self)
{
    FLST other = JOIN(FLST, init)();

    other.copy = self->copy;
    other.free = self->free;

    FLST temp = JOIN(FLST, init)();
    foreach(FLST, self, it) {
        if(self->copy) {
            JOIN(FLST, push_front)(&temp, self->copy(it.ref));
        } else {
            JOIN(FLST, push_front)(&temp, *it.ref);
        }
    }

    foreach(FLST, &temp, it) {
        JOIN(FLST, push_front)(&other, *it.ref);
    }

    JOIN(FLST, free)(&temp);
    return other;
}

static inline size_t
JOIN(FLST, unique)(FLST* self, int _equal(T*, T*))
{
    size_t removed = 0;
    if(self->size <= 1)
        return removed;

    FLSTNODE* current = self->head;
    while(current && current->next) {
        if(_equal(&current->value, &current->next->value)) {
            FLSTNODE* to_delete = current->next;
            if(self->free)
                self->free(&to_delete->value);
            current->next = to_delete->next;
            free(to_delete);
            self->size -= 1;
            removed++;
        } else {
            current = current->next;
        }
    }
    return removed;
}

static inline void
JOIN(FLST, sort)(FLST* self, int _compare(T*, T*))
{
    if(self->size <= 1)
        return;

    for(FLSTNODE* i = self->head; i != NULL; i = i->next) {
        FLSTNODE* min = i;
        for(FLSTNODE* j = i->next; j != NULL; j = j->next) {
            if(_compare(&j->value, &min->value) < 0) {
                min = j;
            }
        }
        if(min != i) {

            T temp = i->value;
            i->value = min->value;
            min->value = temp;
        }
    }
}

static inline void
JOIN(FLST, splice_after)(FLST* self, FLSTNODE* pos, FLST* other)
{
    if(JOIN(FLST, empty)(other))
        return;

    if(pos == NULL) {

        FLSTNODE* other_tail = other->head;
        while(other_tail->next != NULL)
            other_tail = other_tail->next;
        other_tail->next = self->head;
        self->head = other->head;
    } else {

        FLSTNODE* other_tail = other->head;
        while(other_tail->next != NULL)
            other_tail = other_tail->next;
        other_tail->next = pos->next;
        pos->next = other->head;
    }

    self->size += other->size;
    other->head = NULL;
    other->size = 0;
}

#undef T
#undef FLST
#undef FLSTNODE
#undef FLSTIT

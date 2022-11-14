//
// Created by Utilisateur on 14/11/2022.
//

#include "stdlib.h"

#include "linked_lists.h"


int ll_count(struct ll * list)
{
    int ct = 0;
    while (list != NULL) {
        ct++;
        list = list->next;
    }

    return ct;
}

struct ll * ll_add(struct ll * at, void* data)
{
    struct ll* old = at ? at->next : NULL;
    struct ll* new = malloc(sizeof new);
    new->data = data;

    new->next = old;
    new->prev = at;

    old && (old->prev = new);

    at && (at->next = new);

    return new;
}

void ll_remove(struct ll * at) {
    struct ll* prev = at ? at->prev : NULL;
    struct ll * next = at ? at->next : NULL;
    free(at); // free() is null-safe

    prev && (prev->next = next);
    next && (next->prev = prev);
}

struct ll * ll_find(struct ll * at, _Bool(*fun)(void* cur))
{
    while (at) {
        if(fun(at->data)) {
            return at;
        }
        at = at->next;
    }

    return NULL;
}

struct ll * ll_at(struct ll* at, int where)
{
    struct ll* cur = at;
    while (where--) cur=cur->next;

    return cur;
}
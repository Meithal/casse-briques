//
// Created by Utilisateur on 14/11/2022.
//

#ifndef CASSES_BRIQUES_LINKED_LISTS_H
#define CASSES_BRIQUES_LINKED_LISTS_H

struct ll {
    void * data;
    struct ll * prev;
    struct ll * next;
};

int ll_count(struct ll * list);
struct ll* ll_add(struct ll * at, void* data);
void ll_remove(struct ll * at);
struct ll * ll_find(struct ll * at, _Bool(*fun)(void* cur));
struct ll * ll_at(struct ll* at, int where);

#endif //CASSES_BRIQUES_LINKED_LISTS_H

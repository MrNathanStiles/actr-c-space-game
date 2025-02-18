#ifndef ACTRLIST_H
#define ACTRLIST_H
#include "actralloc.h"

struct ActrList {
    void * item;
    struct ActrList * next;
};

struct ActrList * actr_list(struct ActrList * list, void * item) {
    struct ActrList * result = actr_malloc(sizeof(struct ActrList));
    result->item = item;
    result->next = list;
    return result;
}


#endif

#ifndef ACTRVECTOR_H
#define ACTRVECTOR_H
#include "actralloc.h"

#define ACTRVECTOR_INCREMENT 8

struct ActrVector
{
    int increment;
    int count;
    int allocated;
    void **head;
};

struct ActrVector *actr_vector_init(int initialSize, int incrementSize)
{
    struct ActrVector *result = actr_malloc(sizeof(struct ActrVector));

    result->increment = incrementSize;
    result->count = 0;
    result->allocated = 0;

    if (initialSize > 0)
    {
        result->head = actr_malloc(sizeof(void *) * initialSize);
        result->allocated = initialSize;
    }

    return result;
}
int actr_vector_find(struct ActrVector *list, void *item)
{
    for (int i = 0; i < list->count; i++)
    {
        if (list->head[i] == item)
        {
            return i;
        }
    }
    return -1;
}
int actr_vector_add(struct ActrVector *list, void *item)
{
    if (list->allocated == 0)
    {
        if (list->increment == 0)
        {
            return 0;
        }
        list->head = actr_malloc(sizeof(void *) * list->increment);
        list->allocated = list->increment;
    }
    else if (list->count == list->allocated)
    {
        if (list->increment == 0)
        {
            return 0;
        }
        list->allocated += list->increment;
        void **newHead = actr_malloc(sizeof(void *) * list->allocated);

        for (int i = 0; i < list->count; i++)
        {
            newHead[i] = list->head[i];
        }
        actr_free(list->head);
        list->head = newHead;
    }
    // *(list->head + list->count++) = item;
    list->head[list->count] = item;
    list->count++;
    return 1;
}
void actr_vector_remove(struct ActrVector *list, int index)
{
    if (list->count > 0)
    {
        list->head[index] = list->head[list->count - 1];
    }
    list->count--;
}
void actr_vector_free(struct ActrVector *list)
{
    actr_free(list->head);
    actr_free(list);
}
struct ActrVector *actr_vector_slice(struct ActrVector *source, int start, int end)
{
    if (end == 0) {
        end = source->count;
    }
    struct ActrVector *result = actr_vector_init(8, 8);
    for (int i = start; i < end; i++) {
        actr_vector_add(result, source->head[i]);
    }
    return result;
}

struct ActrVector *actr_vector_concat(struct ActrVector *first, struct ActrVector *second)
{
    struct ActrVector *result = actr_vector_init(8, 8);
    for (int i = 0; i < first->count; i++) {
        actr_vector_add(result, first->head[i]);
    }
    for (int i = 0; i < second->count; i++) {
        actr_vector_add(result, second->head[i]);
    }
    return result;
}

#endif

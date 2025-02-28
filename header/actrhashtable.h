#ifndef ACTRHASHTABLE_H
#define ACTRHASHTABLE_H
#include "actralloc.h"

#define ACTRHASHTABLE_H_ALLOC_SIZE 32

struct ActrHashTableEntry
{
    int id;
    void *item;
    struct ActrHashTableEntry *next;
};

struct ActrHashTable
{
    int allocated;
    int count;
    struct ActrHashTableEntry **head;
};

struct ActrHashTable *actr_hash_table_init()
{
    return (struct ActrHashTable *)actr_malloc(sizeof(struct ActrHashTable));
}
int _actr_hash_table_index(struct ActrHashTable *table, int id)
{
    unsigned int index = (unsigned int)id;
    return index % table->allocated;
}
void _actr_hash_table_expand(struct ActrHashTable *table)
{
    struct ActrHashTableEntry **newHead;
    struct ActrHashTableEntry *entry;
    struct ActrHashTableEntry *next;
    table->allocated += ACTRHASHTABLE_H_ALLOC_SIZE;
    newHead = (struct ActrHashTableEntry **)actr_malloc(sizeof(void *) * table->allocated);
    if (table->head)
    {
        for (int i = 0; i < table->count; i++)
        {
            entry = table->head[i];
            while (entry)
            {
                next = entry->next;
                int index = _actr_hash_table_index(table, entry->id);
                entry->next = newHead[index];
                newHead[index] = entry;
                entry = next;
            }
        }
        actr_free(table->head);
    }
    table->head = newHead;
}

void actr_hash_table_insert(struct ActrHashTable *table, int id, void *item)
{
    if (table->allocated == table->count)
    {
        _actr_hash_table_expand(table);
    }
    struct ActrHashTableEntry * entry = (struct ActrHashTableEntry *)actr_malloc(sizeof(struct ActrHashTableEntry));
    entry->id = id;
    entry->item = item;
    int index = _actr_hash_table_index(table, id);
    entry->next = table->head[index];
    table->head[index] = entry;
}

void * actr_hash_table_find(struct ActrHashTable *table, int id)
{
    struct ActrHashTableEntry * entry = table->head[_actr_hash_table_index(table, id)];

    while (entry) {
        if (entry->id == id) {
            return entry->item;
        }
        entry = entry->next;
    }
    return 0;
}

void actr_hash_table_delete(struct ActrHashTable *table, int id) {
    int index = _actr_hash_table_index(table, id);
    struct ActrHashTableEntry * entry = table->head[index];
    struct ActrHashTableEntry * previous;
    while (entry) {
        if (entry->id == id) {
            if (previous) {
                previous->next = entry->next;
            } else {
                table->head[index] = entry->next;
            }
            actr_free(entry);
        }
        previous = entry;
        entry = entry->next;
    }
}

#endif

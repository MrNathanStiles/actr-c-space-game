#ifndef ACTRMERGESORT_H
#define ACTRMERGESORT_H
#include "actrvector.h"

// this is probably leaking memory, prefer functions in actrmergesortmutate.h

struct ActrVector * _actr_merge_sort_merge(struct ActrVector *left, struct ActrVector *right, struct ActrVector *updates)
{
    struct ActrVector *result = actr_vector_init(8, 8);
    int i = 0;
    int j = 0;

    while (i < left->count && j < right->count)
    {

        if ((int)left->head[i] < (int)right->head[j])
        {
            actr_vector_add(result, left->head[i++]);
        }
        else
        {
            actr_vector_add(result, right->head[j++]);
        }
    }

    struct ActrVector *tempLeft = actr_vector_slice(left, i, 0);
    actr_vector_free(left);

    struct ActrVector *tempRight = actr_vector_slice(right, j, 0);    
    actr_vector_free(right);

    struct ActrVector *tempResult = actr_vector_concat(result, tempLeft);
    actr_free(result);
    actr_free(tempLeft);

    result = actr_vector_concat(tempResult, tempRight);
    actr_free(tempResult);
    actr_free(tempRight);

    if (updates) {
        actr_vector_add(updates, actr_vector_slice(result, 0, 0));
    }

    return result;
}
struct ActrVector *actr_merge_sort(struct ActrVector *arr, struct ActrVector *updates)
{
    int len = arr->count;
    int middle, left, right;
    struct ActrVector *result;
    if (len < 2)
    {
        result = actr_vector_init(8, 8);
        for (int i = 0; i < arr->count; i++)
        {
            actr_vector_add(result, arr->head[i]);
        }
        return result;
    }

    middle = (len / 2);


    return _actr_merge_sort_merge(
        actr_merge_sort(actr_vector_slice(arr, 0, middle), updates), 
        actr_merge_sort(actr_vector_slice(arr, middle, 0), updates),
        updates
    );
}
#endif

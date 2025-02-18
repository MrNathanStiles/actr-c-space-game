#ifndef ACTRMERGESORTMUTATE_H
#define ACTRMERGESORTMUTATE_H
#include "actrvector.h"

int _actr_merge_sort_mutate_comparator(void * a, void * b)
{
    return (int)a > (int)b;
}

void _actr_merge_sort_mutate_merge(struct ActrVector *arr, int start, int mid, int end, int (*comparator)(void*, void*))
{
    int leftIndex = start;
    int rightIndex = mid + 1;
    
    struct ActrVector *tempArray = actr_vector_init(8, 8);

    while (leftIndex <= mid && rightIndex <= end)
    {
        if (comparator(arr->head[leftIndex], arr->head[rightIndex]))
        {
            actr_vector_add(tempArray, arr->head[leftIndex++]);
        }
        else
        {
            actr_vector_add(tempArray, arr->head[rightIndex++]);
        }
    }

    while (leftIndex <= mid)
    {
        actr_vector_add(tempArray, arr->head[leftIndex++]);
    }

    while (rightIndex <= end)
    {
        actr_vector_add(tempArray, arr->head[rightIndex++]);
    }

    for (int i = 0; i < tempArray->count; i++)
    {
        
        arr->head[start + i] = tempArray->head[i];
    }
    actr_vector_free(tempArray);
}
void actr_merge_sort_mutate(struct ActrVector * arr, int start, int end, int (*comparator)(void*, void*), struct ActrVector * updates)
{
    if (start < end)
    {
        if (comparator == 0) {
            comparator = _actr_merge_sort_mutate_comparator;
        }
        int mid = (start + end) / 2;
        if (updates) {
            actr_vector_add(updates, actr_vector_slice(arr, 0, 0));
        }
        actr_merge_sort_mutate(arr, start, mid, comparator, updates);
        actr_merge_sort_mutate(arr, mid + 1, end, comparator, updates);
        _actr_merge_sort_mutate_merge(arr, start, mid, end, comparator);
    }
}

/* Example usage:
const array = [ 5, 2, 8, 1, 9, 4 ];
mergeSortMutable(array);
console.log(array); // Output: [1, 2, 4, 5, 8, 9]
*/
#endif

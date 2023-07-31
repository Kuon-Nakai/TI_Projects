#ifndef __ARRUTILS_H__
#define __ARRUTILS_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief A struct to hold an expandable array.
 * 
 * @note To directly access the array, use [ptr]->arr. Note that the arr pointer is subject to change if the array is expanded and reallocation is needed.
 * 
 */
typedef struct ExpandableArr_s {
    void *arr;
    uint16_t size;
    uint16_t capacity;
    uint16_t elemSize;
} ExpandableArray;

ExpandableArray *Expandable_Array(uint16_t elemSize, uint16_t capacity, uint16_t initialSize);
void *ExpArr_GetElem(ExpandableArray *ea, uint16_t index);
void ExpArr_SetElem(ExpandableArray *ea, uint16_t index, void *elem);
int ExpArr_Expand(ExpandableArray *ea, uint16_t increment);

#endif

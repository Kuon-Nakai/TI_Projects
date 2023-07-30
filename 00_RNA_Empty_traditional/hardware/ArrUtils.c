#include "ArrUtils.h"

/**
 * @brief Creates an expandable array managed by this module.
 * 
 * @param elemSize          Size of each element in bytes.
 * @param capacity          Maximum number of elements the array can hold.
 * @param initialSize       Initial size of the array.
 * @return ExpandableArray* A pointer as a handle to the array.
 */
ExpandableArray *Expandable_Array(uint16_t elemSize, uint16_t capacity, uint16_t initialSize) {
    ExpandableArray *arr = malloc(sizeof(ExpandableArray));
    arr->arr = malloc(elemSize * initialSize);
    arr->size = initialSize;
    arr->capacity = capacity;
    arr->elemSize = elemSize;
    return arr;
}

/**
 * @brief Gets the element at the specified index.
 * 
 * @param ea        An ExpandableArray pointer.
 * @param index     The index of the element to get.
 * @return void*    A pointer to the element.
 */
inline void *ExpArr_GetElem(ExpandableArray *ea, uint16_t index) {
    return (void *)((uint8_t *)ea->arr + index * ea->elemSize);
}

/**
 * @brief Sets the element at the specified index.
 * 
 * @param ea    An ExpandableArray pointer.
 * @param index The index of the element to set.
 * @param elem  A pointer to the value.
 */
inline void ExpArr_SetElem(ExpandableArray *ea, uint16_t index, void *elem) {
    memcpy((uint8_t *)ea->arr + index * ea->elemSize, elem, ea->elemSize);
}

/**
 * @brief Expands the array by the specified increment.
 * 
 * @param ea        An ExpandableArray pointer.
 * @param increment The number of elements to expand the array by.
 * @return int      0 if expansion succeeded, 1 if expansion partially succeeded and expanded to capacity, 2 if expansion failed as it's already at the capacity, -1 if out of memory.
 */
int ExpArr_Expand(ExpandableArray *ea, uint16_t increment) {
    if(ea->size == ea->capacity) return 2; // Expansion failed: Capacity reached.
    uint16_t newSize =  ea->size + increment;
    if (newSize <= ea->capacity) {
        ExpandableArray *p = realloc(ea->arr, ea->elemSize * newSize);
        if (p == NULL) return -1; // Out of memory.
        ea->arr = p;
        ea->capacity = newSize;
        return 0; // Expansion succeeded.
    }
    ExpandableArray *p = realloc(ea->arr, ea->elemSize * ea->capacity);
    if(p == NULL) return -1; // Out of memory.
    ea->arr = p;
    ea->size = ea->capacity;
    return 1; // Expansion partially succeeded: expanded to capacity.
}

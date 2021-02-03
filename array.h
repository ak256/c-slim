/* array.h - simple automatically-resizing array implementation
 * author: Andrew Klinge
*/

#ifndef _ARRAY_H_
#define _ARRAY_H_

typedef struct Array {
    void **items;
    int size;
    int count;
} Array;

int array_init(Array *arr, int size);
void array_deinit(Array *arr);
void array_destroy_all(Array *arr);
void array_add(Array *arr, void *item);

#endif

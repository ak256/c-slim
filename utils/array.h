/* array.h 
 * simple automatically-resizing array implementation
 * author: Andrew Klinge
*/

#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <stdbool.h>

typedef struct Array {
	void **items;
	int size;
	int count;
} Array;

bool array_init(Array *arr, int size);

void array_deinit(Array *arr);
void array_destroy_all(Array *arr);
void array_add(Array *arr, void *item);

#endif

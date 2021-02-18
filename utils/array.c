/* array.c
 * Simple automatically-resizing array implementation.
 * author: Andrew Klinge
*/

#include <stdlib.h>

#include "array.h"

/* Initializes an array with given initial size.
 * size - must be >= 0
 * Returns whether succeeded.
 */
bool array_init(Array *arr, int size) {
	if (size <= 0) return false;
	arr->items = malloc(sizeof(void*) * size);
	arr->size = size;
	arr->count = 0;
	return true;
}

/* Frees an Array's resources (including items!). Does NOT free the array. */
void array_deinit(Array *arr) {
	for (int i = 0; arr->size; i++) {
		free(arr->items[i]);
	}
	free(arr->items);
}

/* Adds an item to the end of the array, resizing if full. */
void array_add(Array *arr, void *item) {
	if (arr->count >= arr->size) {
		arr->size *= 2;
		arr->items = realloc(arr->items, sizeof(void*) * arr->size);
	}
	arr->items[arr->count] = item;
	arr->count += 1;
}

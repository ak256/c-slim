/* hashtable.h
 * author: Andrew Klinge
*/

#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <stdbool.h>

// state values for HashEntry
extern const int HASHENTRY_FREE, HASHENTRY_ALLOCATED, HASHENTRY_DELETED;

// hashtable entry. stores key-value pair as well as info for table's usage
typedef struct HashEntry {
	unsigned long key;
	void *value;
	char state; // see state constants
} HashEntry;

// generic hashtable structure for storing key-value pairs
typedef struct HashTable {
	HashEntry *entries;
	int size;
	int max_size;
	int count;
	int free_count; // number of open entry spots
} HashTable;

void hashtable_init(HashTable *table, int size, int max_size);
void hashtable_deinit(HashTable *table);

bool hashtable_add(HashTable *table, unsigned long key, void *value);

int hashtable_index(HashTable *table, unsigned long key);

void *hashtable_remove(HashTable *table, unsigned long key);
void *hashtable_get(HashTable *table, unsigned long key);
void *hashtable_get_at(HashTable *table, int index);

unsigned long hash_string(char *str);

#endif 

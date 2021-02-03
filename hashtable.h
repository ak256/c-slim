/* hashtable.h
 * author: Andrew Klinge
*/

#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

// state values for HashEntry
extern const int HASHENTRY_FREE, HASHENTRY_ALLOCATED, HASHENTRY_DELETED;

// hashtable entry. stores key-value pair as well as info for table's usage
typedef struct HashEntry {
    unsigned long key; // the identifier for this entry
    void *value; // the value stored under the key (generic value pointer)
    char state; // see state constants
} HashEntry;

// generic hashtable structure for storing key-value pairs
typedef struct HashTable {
    HashEntry *entries; // array of entries
    int size; // allocated size of entries array
    int max_size; // maximum allowed size of entries
    int count; // current number of entries
    int free_count; // number of open entries
} HashTable;

void hashtable_init(HashTable *table, int size, int max_size);
void hashtable_deinit(HashTable *table);

int hashtable_add(HashTable *table, unsigned long key, void *value);
int hashtable_index(HashTable *table, unsigned long key);

void *hashtable_remove(HashTable *table, unsigned long key);
void *hashtable_get(HashTable *table, unsigned long key);
void *hashtable_get_at(HashTable *table, int index);

unsigned long hash(char *str);

#endif 

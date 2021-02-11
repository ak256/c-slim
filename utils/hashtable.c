/* hashtable.c - An implementation of a hashtable for generic use.
 *               Uses unsigned long as key type and void* as value type.
 *               Supports insertion, lookup, and deletion with ~constant
 *               time complexity. Uses Linear Probing.
 *
 * Referred to https://www.codingalpha.com/hash-table-c-program/
 * by Tushar Soni, August 31, 2016 for supporting deletion.
*/

#include <stdlib.h>

#include "hashtable.h"

const int HASHENTRY_FREE = 0, HASHENTRY_ACTIVE = 1, HASHENTRY_DELETED = 2;

static void resize(HashTable *table);

/* Initializes the hashtable with default values.
 *
 * table - the hashtable to initialize.
 * size - the initial size for the table. if this is greater than max_size,
 *      then the initial size is instead max_size.
 * max_size - the maximum size of the table (will not resize past this).
 *      set to 0 if table should not have a size limit.
 */
void hashtable_init(HashTable *table, int size, int max_size) {
    if (size > max_size && max_size != 0) size = max_size; // bound size
    table->entries = calloc(size, sizeof(HashEntry));
    table->max_size = max_size;
    table->size = size;
    table->count = 0;
    table->free_count = size;
}

/* Deinitializes the hashtable (frees any resources allocated during init)
 *
 * table - the hashtable to deinitialize.
 */
void hashtable_deinit(HashTable *table) {
    free(table->entries);
}

/* Adds the key-value pair to the given hashtable.
 * Best time: O(1). Average time: O(log n). Worst time: O(n) (rare, only
 * when table is full and must be resized. depends on init table size).
 * Returns: 1 if failed to add (table reached capacity), 0 if succeeded.
 *
 * table - the hashtable to add to
 * key - identifier for the value to be paired
 * value - the value associated with the key
 */
int hashtable_add(HashTable *table, unsigned long key, void *value) {
    // ensure hashtable load factor is not excessive
    if ((double) table->count / table->size >= 0.5) {
        if (table->size == table->max_size) return 1; // failed, table maxed 
        // unfortunately, must re-hash every item into a double-size table.
        resize(table);
    }

    // find open index in table (or existing, same-key entry)
    int index = key % table->size; // index in hashtable
    HashEntry *entries = table->entries;
    while (entries[index].state == HASHENTRY_ACTIVE && entries[index].key != key) {
        // hash collision! use linear probing
        index = (index + 1) % table->size;
    }
    
    // add entry to table
    if (entries[index].state != HASHENTRY_ACTIVE) {
        // don't count again if simply updating existing entry
        table->count++;
        table->free_count--;
    }
    HashEntry entry = { 
        .key = key, 
        .value = value, 
        .state = HASHENTRY_ACTIVE
    };
    entries[index] = entry;
    return 0; // success
}

/* Removes the entry of the key in the given table. Does nothing
 * if the key is not present in the table.
 * NOTE: does NOT free the value if it is allocated!
 * Returns: the removed value (NULL if none removed)
 *
 * table - the hashtable to access
 * key - the key of the entry to remove
 */
void *hashtable_remove(HashTable *table, unsigned long key) {
    int index = hashtable_index(table, key);
    if (index == -1) return NULL; // not in table
	
    // remove
    table->entries[index].state = HASHENTRY_DELETED;
    table->count--;
    return table->entries[index].value;
}

/* Fetches the value stored under the given key in the hashtable.
 * Returns: pointer to the value paired with the key, NULL if key 
 *      is not present in the table.
 *
 * table - the hashtable to look in.
 * key - the hash key to the value to get.
 */
void *hashtable_get(HashTable *table, unsigned long key) {
    int index = hashtable_index(table, key);
    if (index == -1) return NULL; // not in table
    return table->entries[index].value;
}

/* Fetches the value stored at the given index in the hashtable.
 * Returns: pointer to the value at the index, NULL if entry not 
 *      is not present at the index in the table.
 * NOTE: index should be obtained from hashtable_index() and must 
 *       be checked to make sure is valid!
 *
 * table - the hashtable to look in.
 * index - the index in table to access
 */
void *hashtable_get_at(HashTable *table, int index) {
    if (table->entries[index].state != HASHENTRY_ACTIVE) 
        return NULL; // no entry
    return table->entries[index].value;
}

/* Finds and returns the index of the key's entry in the given hashtable.
 * Returns: the index of the entry in the table, -1 if not present 
 *
 * table - the hashtable to search
 * key - the key of the entry to find the index of
 */
int hashtable_index(HashTable *table, unsigned long key) {
    if (table->free_count == 0) {
        // remove HASHENTRY_DELETED entries, so search is not O(n)
        for (int i = 0; i < table->size; i++) {
            if (table->entries[i].state == HASHENTRY_DELETED) {
                table->entries[i].state = HASHENTRY_FREE;
            }
        }
        // rehash entries so they are still accessible
        table->count = 0;
        for (int i = 0; i < table->size; i++) {
            if (table->entries[i].state != HASHENTRY_ACTIVE) continue;
            table->entries[i].state = HASHENTRY_FREE;
            hashtable_add(table, table->entries[i].key, table->entries[i].value);
        }
        table->free_count = table->size - table->count;
    }

    HashEntry *entries = table->entries;
    int index = key % table->size;
    int start = index;
    while (entries[index].state != HASHENTRY_FREE) {
        if (entries[index].state == HASHENTRY_ACTIVE && entries[index].key == key) return index;
        index = (index + 1) % table->size;
        if (index == start) return -1; // not found
    }
    return -1; // key not found
}

/* djb2: http://www.cse.yorku.ca/~oz/hash.html
 * by Dan Bernstein
 * Computes hash for a string.
 */
unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

/* Doubles the size of the table and properly rehashes all entries
 * into the new entry arrays. (does not resize beyond max_size)
 *
 * table - the table whose size to double.
 */
static void resize(HashTable *table) {
    int old_size = table->size;
    int new_size = old_size * 2;
    if (table->max_size != 0 && new_size > table->max_size) {
        new_size = table->max_size;
    }

    HashEntry *new_entries = calloc(new_size, sizeof(HashEntry));
    HashEntry *old_entries = table->entries;
    table->entries = new_entries;
    table->size = new_size;
    table->free_count = table->size;
    table->count = 0;

    // rehash old entries into new table
    for (int i = 0; i < old_size; i++) {
        if (old_entries[i].state != HASHENTRY_ACTIVE) continue; // no entry
        hashtable_add(table, old_entries[i].key, old_entries[i].value);
    }

    // delete old array
    free(old_entries);
}

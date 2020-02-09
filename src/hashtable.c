#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Hash a key and consider the amount of
 * slots in a table.
 *
 * @key: The null-terminated key to be hashed
 * @size: The number of slots in a table
 *
 * Returns: The hashed-key
 */
uint32_t hash(const char *key, int size)
{
	unsigned long int value = 0;
	unsigned int i = 0;
	unsigned int key_len = strlen(key);

	/* Do several rounds of multiplication */
	for (; i < key_len; ++i) {
		value = value * 37 + key[i];
	}

	/* Make sure value is 0 <= value < size */
	value = value % size;

	return(value);
}

/*
 * Create a new hashtable-entry and bind
 * both the key and the buffer to it. The
 * returned pointer can then be inserted into
 * the table.
 *
 * @key: The key of this entry
 * @buf: The data-buffer to attach
 * @size: The size of the buffer in bytes
 *
 * Returns: Either a pointer to a new entry
 * 	or NULL if an error occurred
 */
struct ht_entry *htPair(const char *key, const uint8_t *buf, int size)
{
	struct ht_entry *entry;

	entry = malloc(sizeof(struct ht_entry));
	if(entry == NULL) return (NULL);

	entry->key = NULL;
	entry->buf = NULL;
	entry->next = NULL;

	entry->key = malloc(strlen(key) + 1);
	if(entry->key == NULL) goto failed;
	strcpy(entry->key, key);

	entry->buf = malloc(size);
	if(entry->buf == NULL) goto failed;
	memcpy(entry->buf, buf, size);

	entry->size = size;

	return(entry);

failed:
	free(entry->key);
	free(entry->buf);
	free(entry);

	return(NULL);
}

/*
 * Create a new hash-table. Note that size corresponds to
 * the number of slots/buckets.
 *
 * @size: Number of slots
 *
 * Returns: Either a pointer to the table or NULL
 * 	if an error occurred
 */
struct ht_t *htCreate(int size)
{
	struct ht_t *table;
	int i = 0;

	table = malloc(sizeof(struct ht_t));
	if(table == NULL) return(NULL);

	table->entries = malloc(sizeof(struct ht_entry *) * size);
	if(table->entries == NULL) goto failed;

	for (; i < size; ++i) {
		table->entries[i] = NULL;
	}

	table->size = size;

	return(table);

failed:
	free(table->entries);
	free(table);

	return(NULL);
}

/* 
 * Destroy a hash-table and free allocated memory.
 *
 * @tbl: Pointer to the table to destroy
 */
void htDestroy(struct ht_t *tbl)
{
	int i;
	struct ht_entry *ptr, *next;

	for(i = 0; i < tbl->size; i++) {
		ptr = tbl->entries[i];

		while(ptr != NULL) {
			next = ptr->next;

			free(ptr->key);		
			free(ptr->buf);
			free(ptr);

			ptr = next;
		}
	}

	free(tbl->entries);
	free(tbl);
}

/*
 * Add a new entry to a hash-table.
 *
 * @table: Pointer to the hash-table
 * @key: The key to attach the data to
 * @buf: The buffer containing the data
 * @size: The size of the buffer in bytes 
 *
 * Returns: Either 0 on success or -1
 * 	if an error occurred
 */
int htSet(struct ht_t *tbl, const char *key, const uint8_t *buf, int size)
{
	struct ht_entry *entry, *prev;
	unsigned int slot = hash(key, tbl->size);

	/* Try to look up an entry set */
	entry = tbl->entries[slot];

	/* No entry means slot empty, insert immediately */
	if (entry == NULL) {
		tbl->entries[slot] = htPair(key, buf, size);
		if(tbl->entries[slot] == NULL) return(-1);

		return(0);
	}

	while (entry != NULL) {
		if (strcmp(entry->key, key) == 0) {
			free(entry->buf);
			
			entry->buf = malloc(size);
			if(entry->buf == NULL) return(-1);

			memcpy(entry->buf, buf, size);
			entry->size = size;
			return(0);
		}

		prev = entry;
		entry = prev->next;
	}

	prev->next = htPair(key, buf, size);
	if(prev->next == NULL) return(-1);

	return(0);
}

/*
 * Retrieve data from a hashtable. The pointer to the
 * buffer and the size of the buffer in bytes will be
 * written to the parameters.
 *
 * @tbl: Pointer to the table
 * @key: The key of the entry to get
 * @ptr: A pointer to write the buffer-address to
 * @size: A pointer to write the buffer-size to(can be NULL)
 *
 * Returns: Either 0 on success or -1
 * 	if an error occurred
 */
int htGet(struct ht_t *tbl, const char *key, uint8_t **ptr, int *size)
{
	struct ht_entry *entry;
	unsigned int slot = hash(key, tbl->size);

	/* Try to find a valid slot */
	entry = tbl->entries[slot];
	if (entry == NULL) return(-1);

	while(entry != NULL) {
		if (strcmp(entry->key, key) == 0) {
			*ptr = entry->buf;
			
			if(size != NULL) {
				*size = entry->size;
			}
			return(0);
		}

		entry = entry->next;
	}

	return(-1);
}

/*
 * Delete and remove an entry from the hashtable.
 *
 * @tbl: Pointer to the table
 * @key: The key of the entry
 */
void htDel(struct ht_t *tbl, const char *key)
{
	struct ht_entry *entry, *prev;
	int idx = 0;
	unsigned int bucket = hash(key, tbl->size);

	entry = tbl->entries[bucket];
	if (entry == NULL) return;

	while (entry != NULL) {
		if (strcmp(entry->key, key) == 0) {
			if (entry->next == NULL && idx == 0) {
				tbl->entries[bucket] = NULL;
			}

			if (entry->next != NULL && idx == 0) {
				tbl->entries[bucket] = entry->next;
			}

			if (entry->next == NULL && idx != 0) {
				prev->next = NULL;
			}

			if (entry->next != NULL && idx != 0) {
				prev->next = entry->next;
			}

			free(entry->key);
			free(entry->buf);
			free(entry);

			return;
		}

		prev = entry;
		entry = prev->next;

		++idx;
	}
}

/*
 * Dump info about a hashtable into
 * the terminal.
 *
 * @tbl: Pointer to the table to dump
 */
void htDump(struct ht_t *tbl)
{
	int i = 0;
	struct ht_entry *entry = NULL;

	for(; i < tbl->size; i++) {
		printf("[%2x] ", i);

		entry = tbl->entries[i];

		while(entry != NULL) {
			printf("%s ", entry->key);

			entry = entry->next;
		}	

		printf("\n");
	}
}

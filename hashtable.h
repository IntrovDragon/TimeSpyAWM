#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <stdint.h>


// Fixed size for hashtable | should fix this
#define Size 100

typedef struct{
    char* key;
    char* value;
} hs_item;


typedef struct{
    uint16_t maxNumber;
    uint16_t count;
    hs_item** items;
} hash_table;

hash_table* hs_create(int size);
hs_item* hs_search_item(hash_table* table, char* key);
int hs_insert_item(hash_table* table, char* key, char* value);
void hs_delete_item(hash_table* table, hs_item* i);
void hs_delete_table(hash_table* table);
unsigned int hs_hashfunction(char* key);

#endif

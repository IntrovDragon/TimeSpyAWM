#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <stdint.h>
#include "json.h"


// Fixed size for hashtable | should fix this
#define Size 100


JsonObject* hs_create(int size, Arena** arena);
JsonKeyValue* hs_search_item(JsonObject* table, char* key);
int hs_insert_item(JsonObject* table, char* key, char* value);
void hs_delete_item(JsonObject* table, JsonKeyValue* i);
void hs_delete_table(JsonObject* table);
unsigned int hs_hashfunction(char* key);

#endif

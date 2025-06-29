#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "hashtable.h"


/**
* Allocates and initializes a hash table with the specified size.
* @param size The maximum number of item the hash table can hold.
* @return A pointer to the newly created hash table.
*/

JsonObject* hs_create(int size, Arena** arena){
    // Allocate memory for a JsonObject structure
    JsonObject* table = arena_alloc(arena, sizeof(JsonObject));

    // Set the maximum number of item the hash table can hold
    table->maxNumber = size;

    // Initialize the count of item in the hash table to 0
    table->count = 0;

    // Allocate memory for an array of JsonKeyValue pointers, initialized to NULL
    table->item = arena_alloc(arena, sizeof(JsonKeyValue) * size);
    if(table->item == NULL){
        fprintf(stderr, "Error, couldnt allocate memory!\n");
        exit(1);
    }
    // Return the newly created JsonObject
    return table;
}


// Searches for an item in a hash table based on a given key.
// Returns the found item if the key exists, otherwise returns NULL.
JsonKeyValue* hs_search_item(JsonObject* table, char* key) {
    int keyCounter = hs_hashfunction(key);
    for (int counter = 0; counter < Size; counter++) {
        JsonKeyValue* jsonItem = &(table->item[keyCounter]);
        if (jsonItem->key != NULL && strcmp(jsonItem->key, key) == 0) {
            return jsonItem;
        }
        keyCounter = (keyCounter + 1) % Size;
    }
    return NULL;
}

// Inserts a key-value pair into the hash table if the key does not already exist.
// Returns 1 if an error occurs (e.g., memory allocation failure), otherwise returns 0.
int hs_insert_item(JsonObject* table, char* key, char* value){
    if(table->count >= Size){
        printf("Table is full!!\n");
        return 1;
    }

    JsonKeyValue* item = hs_search_item(table, key);
    if(item != NULL)
        return 1; // Means item exists!

    int keyCounter = hs_hashfunction(key);

    while(table->item[keyCounter].key != NULL){
        keyCounter++;
        keyCounter %= Size;
    }

    if(table->item[keyCounter].key == NULL){
        fprintf(stderr, "Error, couldnt allocate memory!\n");
        exit(1);
        return 1;
    }

    table->item[keyCounter].key = strdup(key);
    table->item[keyCounter].value = strdup(value);
    table->count++;

    return 0;
}


/**
* Deletes an item from the hash table based on the provided key.
*
* @param table The hash table from which the item will be deleted.
* @param item The item to be deleted.
*/
void hs_delete_item(JsonObject* table, JsonKeyValue* item){
    int keyCounter = 0;
    if(item == NULL){
        printf("Item is NULL!!!\n");
        return;
    }

    for(int counter = 0; counter < Size; counter++){ // iterates till he either finds the key or not

        if(table->item[keyCounter].key != NULL && item->key == table->item[keyCounter].key)
        {
            free(table->item[keyCounter].key);
            free(table->item[keyCounter].value);
            free(table->item);
            table->item[keyCounter].key = NULL; // to make it a null pointer again
            table->count--;
            return;
        }
        keyCounter = (keyCounter + 1) % Size;
    }
    printf("Item not found and thus not deleted.\n\n");
}

void hs_delete_table(JsonObject* table){
    for(int counter = 0; counter < Size; counter++){
        if(table->count == 0){
            free(table);
        }
        if(table->item[counter].key != NULL){
            free(table->item[counter].key);
            free(table->item[counter].value);
            free(table->item);
            table->item[counter].key = NULL; // to make it a null pointer again
            table->count--;
        }
    }
}

// This function takes a string 'key' as input and returns an unsigned integer hash value.
// It calculates a simple hash by summing the ASCII values of each character in the key,
// multiplying the sum by 31, and then taking the modulus with 'Size'.
// The purpose is to map keys to indices within a hash table array.
unsigned int hs_hashfunction(char* key){
    int keyCounter = 0;
    for(int i = 0; key[i] != 0; i++){
        switch(key[i]){
            case('"'):
                break;
            default:
                keyCounter += (int)key[i];
                break;
        }
    }
    keyCounter *= 31;
    keyCounter %= Size;

    return keyCounter;
}

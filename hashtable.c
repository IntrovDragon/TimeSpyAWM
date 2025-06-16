#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"



/**
* Allocates and initializes a hash table with the specified size.
* @param size The maximum number of items the hash table can hold.
* @return A pointer to the newly created hash table.
*/

hash_table* hs_create(int size){
    // Allocate memory for a hash_table structure
    hash_table* table = malloc(sizeof(hash_table));

    // Set the maximum number of items the hash table can hold
    table->maxNumber = size;

    // Initialize the count of items in the hash table to 0
    table->count = 0;

    // Allocate memory for an array of hs_item pointers, initialized to NULL
    table->items = calloc(size, sizeof(hs_item*));
    if(table->items == NULL){
        fprintf(stderr, "Error, couldnt allocate memory!\n");
        exit(1);
    }
    // Return the newly created hash_table
    return table;
}


// Searches for an item in a hash table based on a given key.
// Returns the found item if the key exists, otherwise returns NULL.
hs_item* hs_search_item(hash_table* table, char* key) {
    int keyCounter = hs_hashfunction(key);
    for (int counter = 0; counter < Size; counter++) {
        hs_item* item = table->items[keyCounter];
        if (item != NULL && strcmp(item->key, key) == 0) {
            return item;
        }
        keyCounter = (keyCounter + 1) % Size;
    }
    return NULL;
}

// Inserts a key-value pair into the hash table if the key does not already exist.
// Returns 1 if an error occurs (e.g., memory allocation failure), otherwise returns 0.
int hs_insert_item(hash_table* table, char* key, char* value){
    if(table->count >= Size){
        printf("Table is full!!\n");
        return 1;
    }

    hs_item* item = hs_search_item(table, key);
    if(item != NULL)
        return 1; // Means item exists!

    int keyCounter = hs_hashfunction(key);

    while(table->items[keyCounter] != NULL){
        keyCounter++;
        keyCounter %= Size;
    }

    table->items[keyCounter] = (hs_item*)malloc(sizeof(hs_item));
    if(table->items[keyCounter] == NULL){
        fprintf(stderr, "Error, couldnt allocate memory!\n");
        exit(1);
        return 1;
    }

    table->items[keyCounter]->key = strdup(key);
    table->items[keyCounter]->value = strdup(value);
    table->count++;

    return 0;
}


/**
* Deletes an item from the hash table based on the provided key.
*
* @param table The hash table from which the item will be deleted.
* @param item The item to be deleted.
*/
void hs_delete_item(hash_table* table, hs_item* item){
    int keyCounter = 0;
    if(item == NULL){
        printf("Item is NULL!!!\n");
        return;
    }

    for(int counter = 0; counter < Size; counter++){ // iterates till he either finds the key or not

        if(table->items[keyCounter] != NULL && item->key == table->items[keyCounter]->key)
        {
            free(table->items[keyCounter]->key);
            free(table->items[keyCounter]->value);
            free(table->items[keyCounter]);
            table->items[keyCounter] = NULL; // to make it a null pointer again
            table->count--;
            return;
        }
        keyCounter = (keyCounter + 1) % Size;
    }
    printf("Item not found and thus not deleted.\n\n");
}

void hs_delete_table(hash_table* table){
    for(int counter = 0; counter < Size; counter++){
        if(table->count == 0){
            free(table);
        }
        if(table->items[counter] != NULL){
            free(table->items[counter]->key);
            free(table->items[counter]->value);
            free(table->items[counter]);
            table->items[counter] = NULL; // to make it a null pointer again
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
        keyCounter += (int)key[i];
    }
    keyCounter *= 31;
    keyCounter %= Size;

    return keyCounter;
}

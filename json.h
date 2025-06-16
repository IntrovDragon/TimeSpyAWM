#ifndef JSON_H
#define JSON_H
#include <stdint.h>


typedef struct{
    char** tokens;
    uint64_t count;
    uint16_t index;
    uint64_t maxNumber;
} Token;

typedef struct{
    char* type;
    char* key;
    void* value;
} JsonKeyValue;

typedef struct{
    JsonKeyValue* item;
    uint64_t count;
    uint64_t maxNumber;
} JsonArray;

typedef struct{
    JsonKeyValue* item; // hashtable????
    uint64_t count;
    uint64_t maxNumber;
} JsonObject;

Token* token_tokenizer(char *string); // Takes JSON based string and returns tokens
Token* token_string_resizer(Token* token); // If expands array which holds tokens

JsonObject* parse_object(Token* token);
JsonKeyValue parse_key(Token* token);

// returns only the index since it has no memory allocated
int token_function_finder(Token* token, JsonKeyValue* item, int objectIndex);

#endif

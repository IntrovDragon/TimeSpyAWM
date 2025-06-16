#ifndef JSON_H
#define JSON_H
#include <stdint.h>


typedef struct{
    char** tokens;
    uint64_t count;
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
    uint64_t maxNumber;
} JsonObject;

Token* token_tokenizer(char *string); // Takes JSON based string and returns tokens
Token* token_string_resizer(Token* token); // If expands array which holds tokens

JsonObject* parse_object(char *string, uint32_t position);

#endif

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
    char* type;
    void* value;
} JsonValue;

// F*#king love c for this :D // Just have to rewrite a bit of code.... :/
typedef enum { JSON_KEY_VALUE_PAIR, JSON_VALUE} JsonType;
typedef struct{
    JsonType type;
    union {
        JsonKeyValue jsonKeyValue;
        JsonValue jsonValue;
    } item;
} JsonItem;

typedef struct{
    JsonItem* item;
    uint64_t count;
    uint64_t maxNumber;
} JsonArray;

typedef struct{
    JsonItem* item;
    uint64_t count;
    uint64_t maxNumber;
} JsonObject;


Token* token_tokenizer(char *string); // Takes JSON based string and returns tokens
Token* token_string_resizer(Token* token); // If expands array which holds tokens

JsonObject* parse_object(Token* token); // Takes tokens and turns them into object
JsonArray* parse_array(Token* token); // Takes tokens and return a array
JsonKeyValue parse_key(Token* token); // Takes tokens and return the json key value pair


JsonKeyValue get_key_value_object(JsonObject* object, char* key); // returns the key and value from a struct inside the Object

// returns only the index since it has no memory allocated
int token_function_finder(Token* token, JsonKeyValue* item, int objectIndex);

#endif

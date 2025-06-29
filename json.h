#ifndef JSON_H
#define JSON_H
#include <stdint.h>


typedef struct{
    void** tokens;
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
typedef enum { JSON_VALUE = 0, JSON_KEY_VALUE_PAIR } JsonType;
typedef struct{
    union {
        JsonKeyValue jsonKeyValue;
        JsonValue jsonValue;
    } item;
    JsonType type;
} JsonItem;

typedef struct{
    JsonValue* item;
    uint64_t count;
    uint64_t maxNumber;
} JsonArray;

typedef struct{
    JsonKeyValue* item;
    uint64_t count;
    uint64_t maxNumber;
} JsonObject;

// keeps track of the nested pointers. A object can hold more objects and same for arrays...
typedef struct{
    void** ptr;
    uint16_t count;
    uint16_t maxNumber;
} PointerList;

typedef struct Arena{
    void* start; // Pointer to the start of the memory block
    void* offset; // Pointer to the current position in the memory block
    int size; // Size of the memory block
    struct Arena* prev; // Pointer to the arena before in the linked list
} Arena;

typedef enum {OpenBrace = 0, CloseBrace, OpenBracket, CloseBracket, Comma, Colon, String, Integer, Float, Btrue, Bfalse} TokenType;

// Function Prototypes

Arena** arena_create(int size); // Creates a memory arena
void* arena_alloc(Arena** arena, int size); // Allocates memory from the arena
Arena** arena_resize(Arena** arena, int size); // Resizes the memory arena
void arena_destroy(Arena** arena); // Destroys the memory arena

JsonItem* json_init(char* jsonString, Arena** arena); // Initializes a JsonItem struct
void json_close(Arena** arena); // Frees the JsonItem struct

Token* token_tokenizer(char *string, Arena** arena); // Takes JSON based string and returns tokens
Token* token_print_tokens(Token* token); // Prints the tokens
Token* token_string_resizer(Token* token, Arena** arena); // If expands array which holds tokens

JsonObject* parse_object(Token* token, Arena** arena); // Takes tokens and turns them into object
JsonArray* parse_array(Token* token, Arena** arena); // Takes tokens and return a array
JsonKeyValue parse_key(Token* token, Arena** arena); // Takes tokens and return the json key value pair

JsonKeyValue get_key_value_object(JsonObject* object, char* key); // returns the key and value from a struct inside the Object


void token_function_finder(Token* token, JsonItem* item, Arena** arena); // Finds the function which should be called based on the token type

#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.c"
#include "json.h"


int main(){
      const char* json_array_of_objects =
        "[\n"
        "  {\"id\": 1, \"item\": \"Laptop\"},\n"
        "  {\"id\": 2, \"item\": \"Mouse\"},\n"
        "  {\"id\": 3, \"item\": \"Keyboard\"}\n"
        "]";

    printf("%s\n", json_array_of_objects);

    Arena** arena = arena_create(2024); // Create an arena with an initial size of 2048 bytes

    JsonItem* jsonItem = json_init((char*)json_array_of_objects, arena);

    json_close(arena);

    return 0;
}

Arena** arena_create(int size){
    if(size <= 0){
        fprintf(stderr, "Error: Size must be greater than 0!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }

    Arena** arena = calloc(1, sizeof(Arena));
    if(arena == NULL){
        fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }
    arena[0] = calloc(1, sizeof(Arena));
    if(arena[0] == NULL){
        fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }

    arena[0]->start = malloc(size);
    if(arena[0]->start == NULL){
        fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }
    arena[0]->size = size;
    arena[0]->offset = arena[0]->start;
    arena[0]->prev = NULL;
    return arena;
}

void* arena_alloc(Arena** arena, int size){
    printf("Allocating %d bytes\n", size);
    if(arena[0] == NULL || arena[0]->start == NULL){
        fprintf(stderr, "Error: Arena is NULL!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        return NULL;
    }
    if(size <= 0){
        fprintf(stderr, "Error: Size must be greater than 0!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        return NULL;
    }

    int modulo = (uintptr_t)arena[0]->offset % size;
    if(arena[0]->offset + size - modulo + size > arena[0]->start + arena[0]->size){
        if(size > arena[0]->size){
            arena = arena_resize(arena, size * 2); // Resize the arena to double the size if the requested size is larger than the current size
        }
        else{
            arena = arena_resize(arena, arena[0]->size);
        }
    }
    void* ptr;
    if(modulo != 0){
        ptr = arena[0]->offset + (size - modulo); // Align the pointer to the next multiple of size
        memset(ptr, 0, size); // Initialize the allocated memory to zero
    }
    else{
        ptr = arena[0]->offset;
        memset(ptr, 0, size); // Initialize the allocated memory to zero
    }
    arena[0]->offset = ptr + size;

    return ptr;
}

Arena** arena_resize(Arena** arena, int size){ // use pointer to another pointer to keep track of the active arena
    printf("Resizing arena to %d bytes\n", size);
    Arena* newArena = calloc(1, sizeof(Arena));
    if(arena == NULL || arena[0]->start == NULL){
        fprintf(stderr, "Error: Arena is NULL!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        return NULL;
    }
    if(size <= 0){
        fprintf(stderr, "Error: Size must be greater than 0!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        return NULL;
    }

    newArena->start = malloc(size);
    if(newArena->start == NULL){
        fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }
    newArena->offset = newArena->start;
    newArena->size = size;
    newArena->prev = arena[0]; // Set the previous arena to the current one
    arena[0] = newArena;

    return arena;
}

void arena_destroy(Arena** arena){
    if(arena == NULL){
        fprintf(stderr, "Error: Arena is NULL!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        return;
    }
    printf("endtest\n");
    free(arena[0]->start); // Free the memory block
    free(arena[0]); // Free the arena structure
    free(arena); // Free the pointer to the arena structure
}


JsonItem* json_init(char* jsonString, Arena** arena){
    if(jsonString == NULL){
        fprintf(stderr, "Error: JsonString is NULL!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        return NULL;
    }


    Token* token = token_tokenizer(jsonString, arena);
    if(token == NULL){
        fprintf(stderr, "Error: Tokenization failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        return NULL;
    }
    token_print_tokens(token);


    JsonItem* jsonItem = calloc(1, sizeof(JsonItem));
    if(jsonItem == NULL){
        fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);

    }
    jsonItem->type = JSON_VALUE;

  
    token_function_finder(token, jsonItem, arena);

    return jsonItem;
}


void json_close(Arena** arena){
    arena_destroy(arena);
}


Token* token_tokenizer(char *string, Arena** arena){
    Token* token = arena_alloc(arena, sizeof(Token));
    token->maxNumber = 120;
    token->tokens = (void**)arena_alloc(arena, sizeof(void**) * token->maxNumber);
    token->index = 0;

    int count = token->count;
    for(int i = 0; string[i] != '\0'; i++){
        switch(string[i]){
            case('{'):
                token->tokens[count] = arena_alloc(arena, sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = OpenBrace;
                count++;
                break;
            case('}'):
                token->tokens[count] = arena_alloc(arena, sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = CloseBrace;
                count++;
                break;
            case('['):
                token->tokens[count] = arena_alloc(arena, sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = OpenBracket;
                count++;
                break;
            case(']'):
                token->tokens[count] = arena_alloc(arena, sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = CloseBracket;
                count++;
                break;
            case(','):
                token->tokens[count] = arena_alloc(arena, sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = Comma;
                count++;
                break;
            case(':'):
                // printf("Colon!\n");
                token->tokens[count] = arena_alloc(arena, sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = Colon;
                count++;
                break;
            case('"'):
                token->tokens[count] = arena_alloc(arena, sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = String;
                count++;
                if(count == token->maxNumber){
                    token = token_string_resizer(token, arena);
                }
                token->tokens[count] = arena_alloc(arena, sizeof(char)*64);
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                int index = 0;
                i++;
                for(; string[i] != '\"'; index++){
                    if(string[index] == '\0'){
                        fprintf(stderr, "Error: String not closed, Line: %d, Function: %s\n", __LINE__, __func__);
                        exit(1);
                    }
                    *((char*)(token->tokens[count])+index) = string[i];
                    i++;
                    if(index >= 64){
                        fprintf(stderr, "Error: String too long!, Line: %d, Function: %s\n", __LINE__, __func__);
                        exit(1);
                    }
                }

                // Add the null terminator to the end of the string
                *((char*)(token->tokens[count])+index) = '\0';

                count++;
                break;

            case (int)'0' ... (int)'9':
                token->tokens[count] = arena_alloc(arena, sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = Integer;
                count++;
                if(count == token->maxNumber){
                    token = token_string_resizer(token, arena);
                }

                // Still buggy need to check if number is too big!
                token->tokens[count] = arena_alloc(arena, sizeof(int)); // each number has a max length of 64 bytes
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                for(int n = 0; string[i] <= '9' && string[i] >= '0' || string[i] == '.'; n++){
                    *((int*)(token->tokens[count])+n) = string[i] - '0'; // convert char to int
                    if(string[i] == '.'){
                        *((uint8_t*)(token->tokens[count-1])) = Float;
                    }
                    i++;
                }
                // Point is the only allowed symbole for now!
                if((string[i] < '0' || string[i] > '9' || string[i] == '.') && string[i] != ','&& string[i] != '{' && string[i] != '}' && string[i] != '[' && string[i] != ']' && string[i] != ':'){
                    fprintf(stderr, "Error: Integer seperated by unknown character!, Line: %d, Function: %s\n", __LINE__, __func__);
                    exit(1);
                }
                count++;
                break;
            case('t'):
                char buf[6];
                for(int n = 0; n < 4; n++){
                    buf[n] = string[i];
                    i++;
                }
                buf[4] = '\0';
                if(strcmp(buf, "true") != 0){
                    fprintf(stderr, "Error: unknown string to tokenize! Line: %d, Function: %s\n", __LINE__, __func__);
                    exit(1);
                }
                else{
                token->tokens[count] = arena_alloc(arena, sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = Btrue;
                }
                count++;
                break;
            case('f'):
                for(int n = 0; n < 5; n++){
                    buf[n] = string[i];
                    i++;
                }
                buf[5] = '\0';
                if(strcmp(buf, "false") != 0){
                    fprintf(stderr, "Error: unknown string to tokenize! Line: %d, Function: %s\n", __LINE__, __func__);
                    exit(1);
                }
                else{
                token->tokens[count] = arena_alloc(arena, sizeof(uint8_t));
                    if(token->tokens[count] == NULL){
                        fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    *((int*)(token->tokens[count])) = Btrue;
                }
                count++;
                break;
        }
        if(count == token->maxNumber){
            token = token_string_resizer(token, arena);
        }
    }
    token->count = count;
    return token;
}

Token* token_string_resizer(Token *token, Arena** arena){
    Token* resized = arena_alloc(arena, sizeof(Token));
    if(resized == NULL){
        fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }
    resized->maxNumber = token->maxNumber*  2;
    resized->count = token->count;
    resized->index = token->index;
    resized->tokens = (void**)arena_alloc(arena, sizeof(void*) * resized->maxNumber);
    if(resized->tokens == NULL){
        fprintf(stderr, "Realloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }

    if(memcpy(resized->tokens, token->tokens, sizeof(void*) * token->count) == NULL){
        fprintf(stderr, "Memcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }

    return resized;
}


JsonObject* parse_object(Token* token, Arena** arena){
    JsonObject* jsonObject = hs_create(Size, arena); //have to use Size currently else it will create a bug and crash!

    JsonItem* jsonItem = calloc(1,sizeof(JsonItem));
    if(jsonItem == NULL){
        fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }
    jsonItem->type = JSON_VALUE;

    JsonKeyValue* item = calloc(1,sizeof(JsonKeyValue));
    if(item == NULL){
        fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }

    token->index++;
    int braceCounter = 1;
    while(braceCounter > 0){
        switch(*((int*)token->tokens[token->index])){
            case(OpenBrace):
                braceCounter++;
                break;
            case(CloseBrace):
                braceCounter--;
                if(braceCounter == 0)
                    goto fin;
                break;
        }
        token_function_finder(token, jsonItem, arena);

        // if the token got used by another function like parse_array got called by parse_object he is one off and goes of bound! At least there is a chance for it...
        if(token->index >= token->count)
            break;

        if(jsonItem->type == JSON_KEY_VALUE_PAIR){
            if(jsonItem->item.jsonKeyValue.value != NULL){
                item->type = jsonItem->item.jsonKeyValue.type;
                item->key = jsonItem->item.jsonKeyValue.key;
                item->value = jsonItem->item.jsonKeyValue.value;

                jsonItem->item.jsonKeyValue.type = NULL;
                jsonItem->item.jsonKeyValue.key = NULL;
                jsonItem->item.jsonKeyValue.value = NULL;
                jsonItem->type = JSON_VALUE;
            }
            if(item->type != NULL && item->key != NULL && item->value != NULL){
                jsonObject->item[hs_hashfunction(item->key)].type = item->type;
                jsonObject->item[hs_hashfunction(item->key)].key = item->key;
                jsonObject->item[hs_hashfunction(item->key)].value = item->value;

                // Resets the item 
                item->type = NULL;
                item->key = NULL;
                item->value = NULL;
                jsonObject->count++;
            }
        }
    }
    fin:

    // free(item);
    // free(jsonItem);
    return jsonObject;
}

JsonArray* parse_array(Token* token, Arena** arena){
    JsonArray* jsonArray = arena_alloc(arena, sizeof(JsonArray));
    if(jsonArray == NULL){
        fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }
    JsonItem* jsonItem = calloc(1,sizeof(JsonItem));
    if(jsonItem == NULL){
        fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }

    jsonArray->maxNumber = 10; // need to make a resizer function!
    jsonArray->item = arena_alloc(arena, sizeof(JsonKeyValue) * jsonArray->maxNumber);
    if(jsonArray->item == NULL){
        fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }

    token->index++;

    int index = 0;
    int bracketCount = 1; // if bracket count is > 1 it means that he still goes through the array if = 0 he reached the CloseBracket
    while(bracketCount > 0){
        switch(*((int*)token->tokens[token->index])){
            case(OpenBracket):
                bracketCount++;
                break;
            case(CloseBracket):
                bracketCount--;
                if(bracketCount == 0)
                    goto fin;
                break;
        }
        token_function_finder(token, jsonItem, arena);

        // if the token got used by another function like parse_array got called by parse_object he is one off and goes of bound! At least there is a chance for it...
        if(token->index >= token->count)
            break;

        if(jsonItem->type == JSON_VALUE){
            if(jsonItem->item.jsonValue.type != NULL && jsonItem->item.jsonValue.value != NULL){

                jsonArray->item[index].type = jsonItem->item.jsonValue.type;
                jsonArray->item[index].value = jsonItem->item.jsonValue.value;

                // Resets the jsonItem 
                jsonItem->type = JSON_VALUE;
                jsonItem->item.jsonValue.type = NULL;
                jsonItem->item.jsonValue.value = NULL;
                index++;
            }
        }
    }
    fin:
    jsonArray->count = index;
    // free(jsonItem);
    return jsonArray;
}

void token_function_finder(Token *token, JsonItem* jsonItem, Arena** arena){
    int index = token->index;
    switch(*(int*)token->tokens[index]){
        case(OpenBrace):
            switch(jsonItem->type){
                case(JSON_VALUE):
                    jsonItem->item.jsonValue.type = arena_alloc(arena, sizeof("JsonObject"));
                    if(strcpy(jsonItem->item.jsonValue.type, "JsonObject") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    JsonObject* jsonObject = parse_object(token, arena);
                    if(jsonObject->count == 0)
                        break;
                    jsonItem->item.jsonValue.value = jsonObject;
                    break;
                case(JSON_KEY_VALUE_PAIR):
                    jsonItem->item.jsonKeyValue.type = arena_alloc(arena, sizeof("JsonObject"));
                    if(strcpy(jsonItem->item.jsonKeyValue.type, "JsonObject") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    jsonObject = parse_object(token, arena);
                    if(jsonObject->count == 0)
                        break;
                    jsonItem->item.jsonKeyValue.value = jsonObject;
                    break;
                default:
                    fprintf(stderr, "Error: Unknown Type!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                    break;
            }
            break;
        case(CloseBrace):
            break;
        case(OpenBracket):
            switch(jsonItem->type){
                case(JSON_VALUE):
                    jsonItem->item.jsonValue.type = arena_alloc(arena, sizeof("JsonArray"));
                    if(strcpy(jsonItem->item.jsonValue.type, "JsonArray") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    JsonArray* jsonArray = parse_array(token, arena);
                    if(jsonArray->item[jsonArray->count-1].value == NULL)
                        break;
                    jsonItem->item.jsonValue.value = jsonArray;
                    break;
                case(JSON_KEY_VALUE_PAIR):
                    jsonItem->item.jsonKeyValue.type = arena_alloc(arena, sizeof("JsonArray"));
                    if(strcpy(jsonItem->item.jsonKeyValue.type, "JsonArray") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    jsonArray = parse_array(token, arena);
                    if(jsonArray->item[jsonArray->count-1].value == NULL)
                        break;
                    jsonItem->item.jsonKeyValue.value = jsonArray;
                    break;
                default:
                    fprintf(stderr, "Error: Unknown Type!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                    break;
            }
            break;
        case(CloseBracket):
            break;
        case(Comma):
            // functions as a reset
            jsonItem->type = JSON_VALUE;
            break;
        case(Colon):
            // if he hits a Colon i know its a key value pair!
            jsonItem->type = JSON_KEY_VALUE_PAIR;

            // free(jsonItem->item.jsonValue.type);
            void* value = jsonItem->item.jsonValue.value;

            // since inside the jsonItem was a jsonValue i can change the value to key since the key gets always read first!
            jsonItem->item.jsonKeyValue.type = NULL;
            jsonItem->item.jsonKeyValue.key = value;
            jsonItem->item.jsonKeyValue.value = NULL;
            break;
        case(String):
            switch(jsonItem->type){
                case(JSON_VALUE):
                    jsonItem->item.jsonValue.type = arena_alloc(arena, sizeof("String"));
                    if(strcpy(jsonItem->item.jsonValue.type, "String") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    jsonItem->item.jsonValue.value = arena_alloc(arena, sizeof(token->tokens[index + 1]));
                    if(strcpy(jsonItem->item.jsonValue.value, token->tokens[index + 1]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    token->index++;
                    break;
                case(JSON_KEY_VALUE_PAIR):

                    jsonItem->item.jsonKeyValue.value = arena_alloc(arena, sizeof(token->tokens+1));
                    if(strcpy(jsonItem->item.jsonKeyValue.value, token->tokens[index+1]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    // free(jsonItem->item.jsonKeyValue.type);

                    jsonItem->item.jsonValue.type = arena_alloc(arena, sizeof("String"));
                    if(strcpy(jsonItem->item.jsonValue.type, "String") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    break;
                default:
                    fprintf(stderr, "Error: This enum shouldnt exist!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                    break;
            }
            break;
        case(Integer):
            switch(jsonItem->type){
                case(JSON_VALUE):
                    jsonItem->item.jsonValue.type = arena_alloc(arena, sizeof("Integer"));
                    if(strcpy(jsonItem->item.jsonValue.type, "Integer") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    jsonItem->item.jsonValue.value = arena_alloc(arena, sizeof(token->tokens[index + 1]));
                    if(strcpy(jsonItem->item.jsonValue.value, token->tokens[index + 1]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    token->index++;
                    break;
                case(JSON_KEY_VALUE_PAIR):

                    jsonItem->item.jsonKeyValue.value = arena_alloc(arena, sizeof(token->tokens[index+1]));
                    if(strcpy(jsonItem->item.jsonKeyValue.value, token->tokens[index+1]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    // free(jsonItem->item.jsonKeyValue.type);

                    jsonItem->item.jsonValue.type = arena_alloc(arena, sizeof("Integer"));
                    if(strcpy(jsonItem->item.jsonValue.type, "Integer") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    break;
                default:
                    fprintf(stderr, "Error: This enum shouldnt exist!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                    break;
            }
            break;
        case(Btrue):
            switch(jsonItem->type){
                case(JSON_VALUE):
                    jsonItem->item.jsonValue.type = arena_alloc(arena, sizeof("Boolean"));
                    if(strcpy(jsonItem->item.jsonValue.type, "Boolean") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    jsonItem->item.jsonValue.value = arena_alloc(arena, sizeof(token->tokens[index]));
                    if(strcpy(jsonItem->item.jsonValue.value, token->tokens[index]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    token->index++;
                    break;
                case(JSON_KEY_VALUE_PAIR):

                    jsonItem->item.jsonKeyValue.value = arena_alloc(arena, sizeof(token->tokens[index]));
                    if(strcpy(jsonItem->item.jsonKeyValue.value, token->tokens[index]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    // free(jsonItem->item.jsonKeyValue.type);

                    jsonItem->item.jsonValue.type = arena_alloc(arena, sizeof("Boolean"));
                    if(strcpy(jsonItem->item.jsonValue.type, "Boolean") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    break;
                default:
                    fprintf(stderr, "Error: This enum shouldnt exist!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                    break;
            }
            break;
        case(Bfalse):
            switch(jsonItem->type){
                case(JSON_VALUE):
                    jsonItem->item.jsonValue.type = arena_alloc(arena, sizeof("Boolean"));
                    if(strcpy(jsonItem->item.jsonValue.type, "Boolean") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    jsonItem->item.jsonValue.value = arena_alloc(arena, sizeof(token->tokens[index]));
                    if(strcpy(jsonItem->item.jsonValue.value, token->tokens[index]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    token->index++;
                    break;
                case(JSON_KEY_VALUE_PAIR):

                    jsonItem->item.jsonKeyValue.value = arena_alloc(arena, sizeof(token->tokens[index]));
                    if(strcpy(jsonItem->item.jsonKeyValue.value, token->tokens[index]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    // free(jsonItem->item.jsonKeyValue.type);

                    jsonItem->item.jsonValue.type = arena_alloc(arena, sizeof("Boolean"));
                    if(strcpy(jsonItem->item.jsonValue.type, "Boolean") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    break;
                default:
                    fprintf(stderr, "Error: This enum shouldnt exist!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                    break;
            }
            break;
        default:
            break;
    }
    token->index++;
}


JsonKeyValue get_key_value_object(JsonObject *object, char *key){
    JsonKeyValue jsonItem = object->item[hs_hashfunction(key)];
	return jsonItem;
}

Token* token_print_tokens(Token* token){
    for(int i = 0; i < token->count; i++){
        switch(*(uint8_t*)(token->tokens[i])){
            case(OpenBrace):
                printf("Open Brace\n");
                break;
            case(CloseBrace):
                printf("Close Brace\n");
                break;
            case(OpenBracket):
                printf("Open Bracket\n");
                break;
            case(CloseBracket):
                printf("Close Bracket\n");
                break;
            case(Comma):
                printf("Comma\n");
                break;
            case(Colon):
                printf("Colon\n");
                break;
            case(String):
                printf("String: %s\n", (char*)token->tokens[i+1]);
                i++;
                break;
            case(Integer):
                printf("Integer: %s\n", (char*)token->tokens[i+1]);
                i++;
                break;
            case(Float):
                printf("Float: %s\n", (char*)token->tokens[i+1]);
                i++;
                break;
            case(Btrue):
                printf("Boolean: True\n");
                break;
            case(Bfalse):
                printf("Boolean: False\n");
                break;
            default:
                printf("Unknown Token: %d\n", *(int*)token->tokens[i]);
        }
    }
    return token;
}

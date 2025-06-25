#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.c"
#include "json.h"


int main(){
const char* json_array_of_objects =
    "["
    "  {"
    "    \"id\": 1,"
    "    \"name\": \"Laptop\","
    "    \"price\": 1200.50,"
    "    \"available\": true"
    "  },"
    "  {"
    "    \"id\": 2,"
    "    \"name\": \"Mouse\","
    "    \"price\": 25.00,"
    "    \"available\": false"
    "  },"
    "  {"
    "    \"id\": 3,"
    "    \"name\": \"Keyboard\","
    "    \"price\": 75.99,"
    "    \"available\": true"
    "  }"
    "]";
    printf("%s\n", json_array_of_objects);

    Token* token = token_tokenizer(json_array_of_objects);

    PointerList* pointerList = calloc(1, sizeof(PointerList));
    pointerList->maxNumber = 100;
    pointerList->ptr = malloc(sizeof(void*)*pointerList->maxNumber);

    JsonValue* itemValue= calloc(1, sizeof(JsonValue));
    itemValue->value = parse_array(token, pointerList);

    JsonArray* jsonArray = itemValue->value;
    JsonObject* jsonObject =(JsonObject*)jsonArray->item[0].value;
    
    printf("Item Type: %s\n", (char*)jsonArray->item[0].type);
    printf("Item Value: %s\n", (char*)jsonArray->item[0].value);
    
    JsonKeyValue item = get_key_value_object(jsonObject, "id");
    
    printf("Type: %s, Key: %s, Value: %s\n", item.type, item.key, (char*)item.value);
    
    for(int i = 0; i < jsonArray->count; i++){
        printf("Item Type: %s\n", (char*)jsonArray->item[i].type);
        printf("Item Value: %s\n", (char*)jsonArray->item[i].value);
    }
    printf("\n");

    // Todo: Write a better token writer func!
    for(int i = 0; i < token->count; i++){
        switch(*(int*)token->tokens[i]){
            case(String):
                i++;
                printf("String: %s\n", (char*)token->tokens[i]);
                break;
            case(Integer):
                i++;
                printf("Integer: %s\n", (char*)token->tokens[i]);
                break;
            case(Float):
                i++;
                printf("Float: %s\n", (char*)token->tokens[i]);
                break;
            case(Btrue):
                printf("Bool: True\n");
                break;
            case(Bfalse):
                printf("Bool: False\n");
                break;
            default:
                printf("Anything else: %d\n", *(int*)(token->tokens[i]));
                break;
        }
        free(token->tokens[i]);
    }
    free(token);
    for(int i = 0; i < pointerList->count; i++){
       free(pointerList->ptr[i]); 
    }

    return 0;
}



Token* token_tokenizer(char *string){
    Token* token = calloc(1, sizeof(Token));
    token->maxNumber = 4;
    token->tokens = (void**)malloc(sizeof(void**) * token->maxNumber);
    token->index = 0;

    int count = token->count;
    for(int i = 0; string[i] != '\0'; i++){
        switch(string[i]){
            case('{'):
                // printf("Open Curly Bracket!\n");
                token->tokens[count] = malloc(sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = OpenBrace;
                count++;
                break;
            case('}'):
                // printf("Close Curly Bracket!\n");
                token->tokens[count] = malloc(sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = CloseBrace;
                count++;
                break;
            case('['):
                // printf("Open Square Bracket!\n");
                token->tokens[count] = malloc(sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = OpenBracket;
                count++;
                break;
            case(']'):
                // printf("Close Square Bracket!\n");
                token->tokens[count] = malloc(sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = CloseBracket;
                count++;
                break;
            case(','):
                // printf("Comma!\n");
                token->tokens[count] = malloc(sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = Comma;
                count++;
                break;
            case(':'):
                // printf("Colon!\n");
                token->tokens[count] = malloc(sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = Colon;
                count++;
                break;
            case('"'):
                token->tokens[count] = malloc(sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = String;
                count++;
                if(count == token->maxNumber){
                    token = token_string_resizer(token);
                }

                token->tokens[count] = malloc(sizeof(char*) * 64);
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                int index = 0;
                i++;
                for(; string[i] != '\"'; index++){
                    if(string[index] == '\n' || string[index] == '\0'){
                        fprintf(stderr, "Error: String not closed, Line: %d, Function: %s\n", __LINE__, __func__);
                        exit(1);
                    }
                    *((char*)(token->tokens[count])+index) = string[i];
                    i++;
                }

                // moves \0 one behind; that way " is included at the end
                *((char*)(token->tokens[count])+index) = '\0';

                count++;
                break;

            case (int)'0' ... (int)'9':
                token->tokens[count] = malloc(sizeof(uint8_t));
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                *((int*)(token->tokens[count])) = Integer;
                count++;
                if(count == token->maxNumber){
                    token = token_string_resizer(token);
                }

                token->tokens[count] = malloc(sizeof(char*) * 64);
                if(token->tokens[count] == NULL){
                    fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                    exit(1);
                }
                char number[64];
                for(int n = 0; string[i] <= '9' && string[i] >= '0' || string[i] == '.'; n++){
                    *((char*)(token->tokens[count])+n) = string[i];
                    if(string[i] == '.'){
                        *((int*)(token->tokens[count-1])) = Float;
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
                token->tokens[count] = malloc(sizeof(uint8_t));
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
                    token->tokens[count] = malloc(sizeof(uint8_t));
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
            token = token_string_resizer(token);
        }
    }
    token->count = count;
    return token;
}

Token* token_string_resizer(Token *token){
    Token* resized = calloc(1, sizeof(Token));
    if(resized == NULL){
        fprintf(stderr, "Malloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }
    resized->maxNumber = token->maxNumber*  2;
    resized->count = token->count;
    resized->index = token->index;
    resized->tokens = (void**)realloc(token->tokens, sizeof(char*) * resized->maxNumber);
    if(resized->tokens == NULL){
        fprintf(stderr, "Realloc failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
        exit(1);
    }
    free(token);
    return resized;
}


JsonObject* parse_object(Token* token, PointerList* pointerList){
    JsonObject* jsonObject = hs_create(Size); //have to use Size currently else it will create a bug and crash!

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
        token_function_finder(token, jsonItem, pointerList);

        // if the token got used by another function like parse_array got called by parse_object he is one off and goes of bound! At least there is a chance for it...
        if(token->index >= token->count)
            break;

        // printf("Type: %s\n", jsonItem->item.jsonValue.type);
        // printf("Value: %s\n", (char*)jsonItem->item.jsonValue.value);

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

                pointerList->ptr[pointerList->count] = jsonObject->item[hs_hashfunction(item->key)].type;
                pointerList->count++;
                pointerList->ptr[pointerList->count] = jsonObject->item[hs_hashfunction(item->key)].key;
                pointerList->count++;
                pointerList->ptr[pointerList->count] = jsonObject->item[hs_hashfunction(item->key)].value;
                pointerList->count++;

                // printf("In Object Type: %s, Key: %s, Value: %s\n", item->type, item->key, (char*)item->value);
                // Resets the item 
                item->type = NULL;
                item->key = NULL;
                item->value = NULL;
                jsonObject->count++;
            }
        }
    }
    fin:
    // i have to look into memcpy... maybe he copies only the pointer and doesnt create new to a new space in the heap!?!??!
    free(item);
    free(jsonItem);
    return jsonObject;
}

JsonArray* parse_array(Token* token, PointerList* pointerList){
    JsonArray* jsonArray = calloc(1, sizeof(JsonArray));
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
    jsonArray->item = malloc(sizeof(JsonKeyValue) * jsonArray->maxNumber);
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
        token_function_finder(token, jsonItem, pointerList);

        // if the token got used by another function like parse_array got called by parse_object he is one off and goes of bound! At least there is a chance for it...
        if(token->index >= token->count)
            break;

        if(jsonItem->type == JSON_VALUE){
            if(jsonItem->item.jsonValue.type != NULL && jsonItem->item.jsonValue.value != NULL){
                // printf("test\n");
                // printf("Type: %s\n", jsonItem->item.jsonValue.type);
                // printf("Value: %s\n", (char*)jsonItem->item.jsonValue.value);

                jsonArray->item[index].type = jsonItem->item.jsonValue.type;
                jsonArray->item[index].value = jsonItem->item.jsonValue.value;

                pointerList->ptr[pointerList->count] = jsonArray->item[index].type;
                pointerList->count++;
                pointerList->ptr[pointerList->count] = jsonArray->item[index].value;
                pointerList->count++;

                // printf("%s, %s, %s\n", jsonItem->type, jsonItem->key, (char*)jsonItem->value);
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
    return jsonArray;
}

// need to rewrite this section... pretty inefficient
// maybe i could turn the token char** into enums then i could use switch case :D
void token_function_finder(Token *token, JsonItem* jsonItem, PointerList* pointerList){
    int index = token->index;
    switch(*(int*)token->tokens[index]){
        case(OpenBrace):
            switch(jsonItem->type){
                case(JSON_VALUE):
                    jsonItem->item.jsonValue.type = malloc(sizeof("JsonObject"));
                    if(strcpy(jsonItem->item.jsonValue.type, "JsonObject") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    JsonObject* jsonObject = parse_object(token, pointerList);
                    if(jsonObject->count == 0)
                        break;
                    jsonItem->item.jsonValue.value = jsonObject;
                    break;
                case(JSON_KEY_VALUE_PAIR):
                    jsonItem->item.jsonKeyValue.type = malloc(sizeof("JsonObject"));
                    if(strcpy(jsonItem->item.jsonKeyValue.type, "JsonObject") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    jsonObject = parse_object(token, pointerList);
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
                    jsonItem->item.jsonValue.type = malloc(sizeof("JsonArray"));
                    if(strcpy(jsonItem->item.jsonValue.type, "JsonArray") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    JsonArray* jsonArray = parse_array(token, pointerList);
                    if(jsonArray->item[jsonArray->count-1].value == NULL)
                        break;
                    jsonItem->item.jsonValue.value = jsonArray;
                    break;
                case(JSON_KEY_VALUE_PAIR):
                    jsonItem->item.jsonKeyValue.type = malloc(sizeof("JsonArray"));
                    if(strcpy(jsonItem->item.jsonKeyValue.type, "JsonArray") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    jsonArray = parse_array(token, pointerList);
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
            jsonItem->type = JSON_VALUE;
            break;
        case(Colon):
            // if he hits a Colon i know its a key value pair!
            jsonItem->type = JSON_KEY_VALUE_PAIR;

            free(jsonItem->item.jsonValue.type);
            void* value = jsonItem->item.jsonValue.value;

            // since the jsonItem was before a jsonValue i can change the value to key since the key gets always read first!
            jsonItem->item.jsonKeyValue.type = NULL;
            jsonItem->item.jsonKeyValue.key = value;
            jsonItem->item.jsonKeyValue.value = NULL;
            break;
        case(String):
            switch(jsonItem->type){
                case(JSON_VALUE):
                    jsonItem->item.jsonValue.type = malloc(sizeof("String"));
                    if(strcpy(jsonItem->item.jsonValue.type, "String") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    jsonItem->item.jsonValue.value = malloc(sizeof(token->tokens[index + 1]));
                    if(strcpy(jsonItem->item.jsonValue.value, token->tokens[index + 1]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    token->index++;
                    break;
                case(JSON_KEY_VALUE_PAIR):

                    jsonItem->item.jsonKeyValue.value = malloc(sizeof(token->tokens+1));
                    if(strcpy(jsonItem->item.jsonKeyValue.value, token->tokens[index+1]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    free(jsonItem->item.jsonKeyValue.type);

                    jsonItem->item.jsonValue.type = malloc(sizeof("String"));
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
                    jsonItem->item.jsonValue.type = malloc(sizeof("Integer"));
                    if(strcpy(jsonItem->item.jsonValue.type, "Integer") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    jsonItem->item.jsonValue.value = malloc(sizeof(token->tokens[index + 1]));
                    if(strcpy(jsonItem->item.jsonValue.value, token->tokens[index + 1]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    token->index++;
                    break;
                case(JSON_KEY_VALUE_PAIR):

                    jsonItem->item.jsonKeyValue.value = malloc(sizeof(token->tokens+1));
                    if(strcpy(jsonItem->item.jsonKeyValue.value, token->tokens[index+1]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    free(jsonItem->item.jsonKeyValue.type);

                    jsonItem->item.jsonValue.type = malloc(sizeof("Integer"));
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
                    jsonItem->item.jsonValue.type = malloc(sizeof("Boolean"));
                    if(strcpy(jsonItem->item.jsonValue.type, "Boolean") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    jsonItem->item.jsonValue.value = malloc(sizeof(token->tokens[index]));
                    if(strcpy(jsonItem->item.jsonValue.value, token->tokens[index]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    token->index++;
                    break;
                case(JSON_KEY_VALUE_PAIR):

                    jsonItem->item.jsonKeyValue.value = malloc(sizeof(token->tokens));
                    if(strcpy(jsonItem->item.jsonKeyValue.value, token->tokens[index]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    free(jsonItem->item.jsonKeyValue.type);

                    jsonItem->item.jsonValue.type = malloc(sizeof("Boolean"));
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
                    jsonItem->item.jsonValue.type = malloc(sizeof("Boolean"));
                    if(strcpy(jsonItem->item.jsonValue.type, "Boolean") == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    jsonItem->item.jsonValue.value = malloc(sizeof(token->tokens[index]));
                    if(strcpy(jsonItem->item.jsonValue.value, token->tokens[index]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }

                    token->index++;
                    break;
                case(JSON_KEY_VALUE_PAIR):

                    jsonItem->item.jsonKeyValue.value = malloc(sizeof(token->tokens));
                    if(strcpy(jsonItem->item.jsonKeyValue.value, token->tokens[index]) == 0){
                        fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                        exit(1);
                    }
                    free(jsonItem->item.jsonKeyValue.type);

                    jsonItem->item.jsonValue.type = malloc(sizeof("Boolean"));
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


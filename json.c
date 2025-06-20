#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.c"
#include "json.h"


int main(){
    char *jsonString = "{\"name\": \"Alice\",\n\"age\": 30,\n\"isStudent\": false,\n\"courses\": [\"Math\", \"Science\"]\n}";

    Token* token = token_tokenizer(jsonString);

    JsonObject* jsonObject = parse_object(token);
    JsonKeyValue item = get_key_value_object(jsonObject, "name");
    printf("Type: %s, Key: %s, Value: %s\n\n", item.type, item.key, (char*)item.value);

    for(int i = 0; i < token->count; i++){
        printf("%s\n", token->tokens[i]);
        free(token->tokens[i]);
    }
    free(token);

    return 0;
}



Token* token_tokenizer(char *string){
    Token* token = calloc(1, sizeof(Token));
    token->maxNumber = 4;
    token->tokens = (char**)malloc(sizeof(char*) * token->maxNumber);
    token->index = 0;

    int count = token->count;
    for(int i = 0; string[i] != '\0'; i++){
        switch(string[i]){
            case('{'):
                // printf("Open Curly Bracket!\n");
                token->tokens[count] = malloc(sizeof("OpenBrace"));
                strcpy(token->tokens[count], "OpenBrace");
                count++;
                break;
            case('}'):
                // printf("Close Curly Bracket!\n");
                token->tokens[count] = malloc(sizeof("CloseBrace"));
                strcpy(token->tokens[count], "CloseBrace");
                count++;
                break;
            case('['):
                // printf("Open Square Bracket!\n");
                token->tokens[count] = malloc(sizeof("OpenBracket"));
                strcpy(token->tokens[count], "OpenBracket");
                count++;
                break;
            case(']'):
                // printf("Close Square Bracket!\n");
                token->tokens[count] = malloc(sizeof("CloseBracket"));
                strcpy(token->tokens[count], "CloseBracket");
                count++;
                break;
            case(','):
                // printf("Comma!\n");
                token->tokens[count] = malloc(sizeof("Comma"));
                strcpy(token->tokens[count], "Comma");
                count++;
                break;
            case(':'):
                // printf("Colon!\n");
                token->tokens[count] = malloc(sizeof("Colon"));
                strcpy(token->tokens[count], "Colon");
                count++;
                break;
            case('"'):
                token->tokens[count] = malloc(sizeof("String"));
                strcpy(token->tokens[count], "String");
                count++;
                if(count == token->maxNumber){
                    token = token_string_resizer(token);
                }

                token->tokens[count] = malloc(sizeof(char*) * 64);
                int index = 0;
                token->tokens[count][index] = string[i];

                index++;
                i++;
                for(; string[i] != '\"'; index++){
                    if(string[index] == '\n' || string[index] == '\0'){
                        fprintf(stderr, "Error: String not closed, Line: %d, Function: %s\n", __LINE__, __func__);
                        exit(1);
                    }
                    token->tokens[count][index] = string[i];
                    i++;
                }

                // moves \0 one behind; that way " is included at the end
                token->tokens[count][index] = string[i];
                token->tokens[count][index + 1] = '\0';

                count++;
                break;

            case (int)'0' ... (int)'9':
                token->tokens[count] = malloc(sizeof("Integer"));
                strcpy(token->tokens[count], "Integer");
                count++;
                if(count == token->maxNumber){
                    token = token_string_resizer(token);
                }

                token->tokens[count] = malloc(sizeof(char*) * 64);

                for(int n = 0; string[i] <= '9' && string[i] >= '0'; n++){
                    token->tokens[count][n] = string[i];
                    i++;
                }
                if(string[i + 1] >= '0' && string[i + 1] <= '9'){
                        fprintf(stderr, "Error: Integer seperated by unknown character!, Line: %d, Function: %s\n", __LINE__, __func__);
                        exit(1);
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
    resized->maxNumber = token->maxNumber*  2;
    resized->count = token->count;
    resized->index = token->index;
    resized->tokens = (char**)realloc(token->tokens, sizeof(char*) * resized->maxNumber);
    if(resized->tokens == NULL){
        fprintf(stderr, "Realloc not worked!\n");
        exit(1);
    }
    free(token);
    return resized;
}


JsonObject* parse_object(Token* token){
    JsonObject* jsonObject = hs_create(Size); //have to use Size currently else it will create a bug and crash!
    int index = 0;
    JsonKeyValue* item = calloc(1,sizeof(JsonKeyValue));

    while(strcmp(token->tokens[token->index], "CloseBrace") != 0){
        token_function_finder(token, item, index);

        // if the token got used by another function like parse_array got called by parse_object he is one off and goes of bound! At least there is a chance for it...
        if(token->index >= token->count)
            break;

        if(item->type != NULL && item->key != NULL && item->value != NULL){
            if(memcpy(&jsonObject->item[hs_hashfunction(item->key)], item, sizeof(JsonKeyValue)) == 0){
                fprintf(stderr, "memcpy not worked!\n");
                exit(1);
            }
            // printf("%s, %s, %s\n", item->type, item->key, (char*)item->value);
            // Resets the item 
            item->type = NULL;
            item->key = NULL;
            item->value = NULL;
        }
    }
    printf("Test2\n");

    free(item);
    return jsonObject;
}

JsonArray* parse_array(Token* token){
    JsonArray* jsonArray = calloc(1, sizeof(JsonArray));
    JsonKeyValue* item = calloc(1,sizeof(JsonKeyValue));

    jsonArray->maxNumber = 10; // need to make a resizer function!
    jsonArray->item = malloc(sizeof(JsonKeyValue) * jsonArray->maxNumber);


    int index = 0;
    int bracketCount = 1; // if bracket count is > 1 it means that he still goes through the array if = 0 he reached the CloseBracket
    while(bracketCount > 0){
        bracketCount = token_function_finder(token, item, bracketCount);

        printf("objectIndex: %d \n", bracketCount);

        if(item->type != NULL && item->value != NULL){
            printf("test\n");
            if(memcpy(&jsonArray->item[index], item, sizeof(JsonKeyValue)) == 0){
                fprintf(stderr, "memcpy not worked!\n");
                exit(1);
            }
            // printf("%s, %s, %s\n", item->type, item->key, (char*)item->value);
            // Resets the item 
            item->type = NULL;
            item->key = NULL;
            item->value = NULL;
            index++;
        }
        printf("Token: %s \n", token->tokens[token->index]);
    }
    return jsonArray;
}

// need to rewrite this section... pretty inefficient
// maybe i could turn the token char** into enums then i could use switch case :D
int token_function_finder(Token *token, JsonKeyValue* item, int objectIndex){
    int index = token->index;
    if(strcmp(token->tokens[index], "OpenBrace") == 0){

    }
    else if(strcmp(token->tokens[index], "CloseBrace") == 0){

    }
    else if(strcmp(token->tokens[index], "OpenBracket") == 0){
        // objectIndex++;
        // printf("objectIndex: %d \n", objectIndex);
        token->index++;
        parse_array(token);
    }
    else if(strcmp(token->tokens[index], "CloseBracket") == 0){
        // printf("objectIndex: %d \n", objectIndex);
        objectIndex--;
        printf("Test\n");
    }
    else if(strcmp(token->tokens[index], "Comma") == 0){
    }
    else if(strcmp(token->tokens[index], "Colon") == 0){

    }
    else if(strcmp(token->tokens[index], "String") == 0){























        // If this is true i know that i am dealing with the key and not the value!
        // printf("Token 2: %s\n",  token->tokens[index+1]);
        // printf("Token 3: %s\n\n",  token->tokens[index+2]);
        if(strcmp(token->tokens[index+2], "Colon") == 0){
            // printf("Key after colon: %s\n",  token->tokens[index+1]);
            // printf("%d\n", hs_hashfunction(token->tokens[index+1]));
            item->key = malloc(sizeof(char*) * 64);
            if(strcpy(item->key, token->tokens[index+1]) == 0){
                fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                exit(1);
                        }
        }
        // need to test the code later without this... might be unneccessary :/
        else if(strcmp(token->tokens[index+1], "Comma") == 0){
            printf("Test\n");
            item->type = malloc(sizeof("String"));
            if(strcpy(item->type, "String") == 0){
                fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                exit(1);
            }

            // printf("Value: %s\n",  token->tokens[index+1]);
            item->value = malloc(sizeof(token->tokens[index+1]));
            if(strcpy(item->value, token->tokens[index+1]) == 0){
                fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                exit(1);
            }
        }
        else{
            item->type = malloc(sizeof("String"));
            if(strcpy(item->type, "String") == 0){
                fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                exit(1);
            }

            // printf("Value: %s\n",  token->tokens[index+1]);
            item->value = malloc(sizeof(token->tokens[index+1]));
            if(strcpy(item->value, token->tokens[index+1]) == 0){
                fprintf(stderr, "Error: strcpy failed!, Line: %d, Function: %s\n", __LINE__, __FUNCTION__);
                exit(1);
            }
        }

    }
    else if(strcmp(token->tokens[index], "Integer") == 0){

    }
    // he doesnt understand it if its a actual string value
    // else{
    //     printf("%s\n", token->tokens[index]);
    //     fprintf(stderr, "Error: unknown token, Line: %d, Function: %s", __LINE__, __FUNCTION__);
    //     exit(1);
    // }
    token->index++;
    return objectIndex;
}


JsonKeyValue get_key_value_object(JsonObject *object, char *key){
    JsonKeyValue jsonKeyValue = object->item[hs_hashfunction(key)];
	return jsonKeyValue;
}


#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(){
    char *jsonString = "{\"name\": \"Alice\",\n\"age\": 30,\n\"isStudent\": false,\n\"courses\": [\"Math\", \"Science\"]\n}";

    Token* token = token_tokenizer(jsonString);

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

    int count = token->count;
    for(int i = 0; string[i] != '\0'; i++){
        switch(string[i]){
            case('{'):
                // printf("Open Curly Braket!\n");
                token->tokens[count] = malloc(sizeof("OpenBrace"));
                strcpy(token->tokens[count], "OpenBrace");
                count++;
                break;
            case('}'):
                // printf("Close Curly Braket!\n");
                token->tokens[count] = malloc(sizeof("CloseBrace"));
                strcpy(token->tokens[count], "CloseBrace");
                count++;
                break;
            case('['):
                // printf("Open Square Braket!\n");
                token->tokens[count] = malloc(sizeof("OpenBraket"));
                strcpy(token->tokens[count], "OpenBraket");
                count++;
                break;
            case(']'):
                // printf("Close Square Braket!\n");
                token->tokens[count] = malloc(sizeof("CloseBraket"));
                strcpy(token->tokens[count], "CloseBraket");
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
                if(string[i] >= '0' && string[i] <= '9'){
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
    resized->tokens = (char**)realloc(token->tokens, sizeof(char*) * resized->maxNumber);
    if(resized->tokens == NULL){
        fprintf(stderr, "Realloc not worked!\n");
        exit(1);
    }
    free(token);
    return resized;
}


JsonObject* parse_object(char *string, uint32_t position){
    JsonObject* jsonObject = malloc(sizeof(JsonObject));

    return jsonObject;
}

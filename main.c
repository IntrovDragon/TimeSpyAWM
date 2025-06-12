#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "timespy.c"

/* FYI the first created new line happens because of the awesome-client command in the awesome.sh file */

int main(){

    ApplicationTracker* applicationTracker = getOpenApplications("./awesome.sh");

    Application* application = applicationTracker->application;

    FILE *fptr;
    fptr = fopen ("DB.json", "r");
    if(fptr == NULL){
        printf("File does not exist!\n");
        fptr = fopen ("DB.json", "w");
        fprintf(fptr, "{\n");
        fprintf(fptr, "}");
    }

    int count = applicationTracker->count;

    printf("Application: %s\n", application[0].name);

    checkApplication("DB.json", application, count);
    
    addApplication("DB.json", application, count);
    fclose(fptr);

    for(int i = 0; i < count; i++){
        if(application[i].name != NULL)
            free(application[i].name);
    }

    return 0;
}

/* Potentional bug still exist where if there a two windows open on the same tag he will register both, but how should this then work with xdotool? */

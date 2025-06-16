#include "timespy.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>



ApplicationTracker* application_get_open(char* script){
    // File gets a pointer which can access it
    FILE *pp;
    pp = popen(script, "r");
    if(pp == NULL){
        fprintf(stderr, "Couldnt get input from awesome.sh");
        exit(1);
    }

    ApplicationTracker* applicationTracker = calloc(1, sizeof(ApplicationTracker));

    applicationTracker->application = calloc(10, sizeof(Application));

    Application* application = applicationTracker->application;

    // counts the found applications from awesome.sh
    int count = 0;

    // problem, what if a applications is open twice?? duplicated problem...
    while (1) {
        char *line;
        char buf[100];
        line = fgets(buf, sizeof buf, pp);
        if (line == NULL) break;

        int size = strcspn(line, "\n");
        line[size] = '\0';

        application[count].name = (char*)malloc(size + 1);

        for(int i = 0; i < count; i++){
            if(strcmp(application[i].name, line) == 0){
                count--;
            }
        }

        // copy line to application
        if(strcpy(application[count].name, line) == NULL){
            fprintf(stderr, "strcpy failed to copy line into application[count].name");
        }
        application[count].size = size;

        count++;
        if(count >= 10){
            fprintf(stderr, "Buffer overflow! More applications than handable");
            exit(1);
        }
    }
    applicationTracker->count = count;
    pclose(pp);
    return applicationTracker;
}

void addTime(char* file, Application* application, int count);

void application_update_time(char* file, Application* application, int count){
    FILE *fptr;
    fptr = fopen(file, "r");
    char *line;
    int applicationnum = 0;

    // Firstly looking that the application doesnt exist already
    while(1){
        char buf[100];
        line = fgets(buf, sizeof buf, fptr);

        while(line == NULL){
            applicationnum++;
            if(applicationnum == count){
                fclose(fptr);
                return;
            }

            rewind(fptr);
            line = fgets(buf, sizeof buf, fptr);
        }

        char *unLine = (char *)malloc((strlen(line) + 1) * sizeof(char));
        strcpy(unLine, line);
        if(unLine == NULL)
            exit(1);

        // Deletes whitespaces
        for(int i = 0; line[0] == ' '; i++){
            line++;
        }

        int x = 0;
        while(line[x] != '\0'){
            if(line[x] == ':')
                line[x] = '\0';
            x++;
        }

        // if one of the open application is found inside the data template, retrieve the time and add it up to it
        if(strcmp(application[applicationnum].name, line) == 0){
            int findNum = 0;
            char buf2[10];
            while(unLine[findNum] != ':'){
                findNum++;
            }
            findNum++;

            int index = 0;

            for(int i = 0; unLine[findNum + i] != ','; i++){
                if(unLine[findNum + i] == '\0') break;

                if(unLine[findNum + i] != ' ' && unLine[findNum + i] != '\n'){
                    buf2[index] = unLine[findNum + i];
                    index++;
                }
            }
            buf2[index] = '\0';
            application[applicationnum].time = strtol(buf2, NULL, 10) + 30;
        }
    }
}


void application_update_file(char* file, Application* application, int count){
    FILE *fptr;
    fptr = fopen(file, "r");
    char *line;
    int applicationnum = 0;

    // Keeps track at which line the program is in the file
    int linenum = 1;

    // Checks where the last line is
    int endline = 0;
    while(1){
        char buf[100];
        line = fgets(buf, sizeof buf, fptr);

        if(line == NULL) break;
        
        endline++;
    }

    rewind(fptr);
    // For add every not existing application
    while(1){
        for(int i = 0; i < count; i++){
            if(strcmp(application[i].name, "0") != 0){
                applicationnum++;
            }
        }
        // since there are new applications we can instantly return!!!
        if(applicationnum == 0){
            fclose(fptr);
            return;
        }

        if(endline > 2 && applicationnum > 0){
            FILE *tmpfptr;
            tmpfptr = fopen("DBtemp.json", "w");

            while(linenum < endline){
                char buf[100];
                line = fgets(buf, sizeof buf, fptr);
                if(line == NULL) break;

                char *unLine = (char *)malloc((strlen(line) + 1) * sizeof(char));
                strcpy(unLine, line);
                if(unLine == NULL)
                    exit(1);

                // Deletes whitespaces
                for(int i = 0; line[0] == ' '; i++){
                    line++;
                }

                int x = 0;
                while(line[x] != '\0'){
                    if(line[x] == ':')
                        line[x] = '\0';
                    x++;
                }

                // add the add the data template and open found applications together in a new file

                bool foundApp = false;
                for(int i = 0; i < count; i++){
                    if(strcmp(application[i].name, line) == 0){
                        fprintf(tmpfptr, "%s: %d,\n", application[i].name, application[i].time);
                        strcpy(application[i].name, "0");
                        foundApp = true;
                    }
                }
                if(foundApp == false){
                    fprintf(tmpfptr, "%s",unLine);
                } 

                linenum++;
            }
            for(int i = 0; i < count; i++){
                if(strcmp(application[i].name, "0") != 0){
                    fprintf(tmpfptr, "%s: %d,\n", application[i].name, application[i].time);
                }
            }
            fprintf(tmpfptr, "}");
            fclose(tmpfptr);
            fclose(fptr);

            // delete the old file and rename the new file after the old >> meaning that its updating the old one
            if(remove("DB.json") == 0){
                // printf("Delete DB.json!\n");
            }
            if(rename("DBtemp.json", "DB.json") == 0){
                // printf("Successfully renamed the file!\n");
            }
            return;
        }
        else{
            fclose(fptr);
            fptr = fopen("DB.json", "w");
            fprintf(fptr, "{\n");
    
            for(int i = 0; i < count; i++){
                fprintf(fptr, "%s: 0,\n", application[i].name);
            }
            fprintf(fptr, "}");
            fclose(fptr);
            return;
        }
    }
}

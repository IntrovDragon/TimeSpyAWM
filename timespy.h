#ifndef TIMESPY_H
#define TIMESPY_H

#include <stdint.h>

typedef struct{
    char* name;
    uint8_t size;
    uint16_t time;
} Application;

typedef struct{
    Application* application;
    uint8_t count;
} ApplicationTracker;

typedef struct{
    char* name;
    Application* application; // Because it belongs to a webbrowers which is a Application
    uint16_t time;
} WebPage;

// accept the input from script and put them into the Applicationarray
ApplicationTracker* application_get_open(char* script);
// updates the time by comparing input with the file and add the new time to Application struct
void application_update_time(char* file, Application* application, int count);
// updates the file by adding the new applications / changed time spend
void application_update_file(char* file, Application* application, int count);

#endif

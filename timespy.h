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

ApplicationTracker* getOpenApplications(char* script);

void addTime(char* file, Application* application, int count);

void checkApplication(char* file, Application* application, int count);

void addApplication(char* file, Application* application, int count);

#endif

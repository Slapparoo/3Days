#pragma once
#define HISTORY_SIZE 128
char* rl(char* prompt);
typedef struct {
    char *displayText;
    char *completion;
} CCompletion;
extern CCompletion* (*rlACGen)(const char *buffer,long srcoff,const char* text,long *length);
void InitRL();

#pragma once
#define HISTORY_SIZE 128
char* rl(char* prompt);
void AddHistory(char* text);
int HistorySize();
void HistoryResize(long sz);
typedef struct {
    char *displayText;
    char *completion;
} CCompletion;
extern CCompletion* (*rlACGen)(const char *buffer,long srcoff,const char* text,long *length);
void InitRL();

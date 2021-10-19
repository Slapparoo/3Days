#include <assert.h>
#include "rl.h"
#if _WIN64
#include <windows.h>
#else
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gc.h"
#include "alloc.h"
// https://gist.github.com/fleroviux/8343879d95a72140274535dc207f467d
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200

typedef struct {
    long start, end, cur;
    char* history[HISTORY_SIZE];
} CLIFO;
static CLIFO History;
void InitRL() {
    AddGCRoot(&History,sizeof(History));
    #ifdef TARGET_WIN32
    DWORD omode;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &omode);
    omode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), omode);
    #endif
}
int HistoryIsEmpty() {
    return History.cur == 0 && History.start == History.end;
}
int HistorySize() {
    if (History.start < History.end) return History.end - History.start;

    if (HistoryIsEmpty()) return 0;

    //(---->end] [start------->)HISTORY_SIZE
    return HISTORY_SIZE - History.start + History.end;
}
void AddHistory(char* text) {
    char* dup = strcpy(GC_MALLOC(strlen(text) + 1), text);
    History.history[History.end] = dup;

    if(History.end == History.start&&!HistoryIsEmpty()) {
        History.end = History.start = ++History.start % HISTORY_SIZE;
    } else {
        History.end++;
        History.end %= HISTORY_SIZE;
    }

    History.cur = HistorySize() - 1;
}
char* HistoryPeek(int h) {
    if (HistorySize() <= h) return NULL;

    return History.history[(History.start + h) % HISTORY_SIZE];
}
void HistoryResize(long sz) {
    if (HistorySize() >= sz) return;

    History.end = (History.start + sz) % HISTORY_SIZE;
}
#ifdef _WIN64
static void WriteStdout(char* str) {
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), str, strlen(str), NULL, NULL);
}
static int ReadStdin() {
    unsigned char chr;
    DWORD read;
    ReadConsole(GetStdHandle(STD_INPUT_HANDLE), &chr, 1, &read, NULL);

    if (read != 1) return -1;

    return chr;
}
#else
static void WriteStdout(char* str) {
    write(STDOUT_FILENO, str, strlen(str));
}
static int ReadStdin() {
    unsigned char r;

    if (read(STDIN_FILENO, &r, 1) != 1)
        return -1;

    return r;
}
#endif
static void _GetCursorPos(int* x, int* y) {
    WriteStdout("\x1b[6n");
    while(ReadStdin()!=0x1b);
    assert(ReadStdin() == '[');
    int parseNumber() {
        int num = 0;
        int chr;
loop:

        switch (chr = ReadStdin()) {
        case '0'...'9':
            num *= 10;
            num += chr - '0';
            goto loop;

        default:
            return num;
        }
    }
    *y = parseNumber();
    *x = parseNumber();
}
static int IsIdentChar(char chr) {
    switch (chr) {
    case 'a'...'z':
    case 'A'...'Z':
    case '0'...'9':
    case '_':
        return 1;

    default:
        return 0;
    }
}

#ifdef _WIN64
typedef struct {
    DWORD imode, omode;
} COldTAttrs;
void SetTerminalAttrs(COldTAttrs* old) {
    DWORD imode, origImode;
    GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &imode);
    origImode = imode;
    imode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT|ENABLE_PROCESSED_INPUT);
    imode |= ENABLE_VIRTUAL_TERMINAL_INPUT ;
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), imode);
    DWORD omode, origOmode;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &omode);
    origOmode = omode;
    omode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), omode);
    old->omode = origOmode;
    old->imode = origImode;
}
void RestoreTermianlAttrs(COldTAttrs* old) {
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), old->imode);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), old->omode);
}
#else
typedef struct {
    struct termios imode;
} COldTAttrs;
void SetTerminalAttrs(COldTAttrs* old) {
    struct termios attrs;
    tcgetattr(STDIN_FILENO, &attrs);
    old->imode = attrs;
    attrs.c_lflag &= ~(ICANON | ISIG | IEXTEN | ECHO);
    attrs.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    attrs.c_cflag |= (CS8);
    attrs.c_oflag &= ~(OPOST);
    attrs.c_cc[VMIN] = 0;
    attrs.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &attrs);
}
void RestoreTermianlAttrs(COldTAttrs* old) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old->imode);
}
#endif
CCompletion* (*rlACGen)(const char *start,long offset,const char* text,long *length);
void RefreshLineShrink(long offset,long oldsize,long newsize) {
    char buffer[128];
    long right=(newsize-offset);
    WriteStdout("\x1b[?25l"); //Turn offf cursor
    if(right>0) {
        sprintf(buffer,"\x1b[%liC",right);
        WriteStdout(buffer);
    } else if(right<0) {
        sprintf(buffer,"\x1b[%liD",-right);
        WriteStdout(buffer);
    }
    memset(buffer,' ',oldsize-newsize);
    buffer[oldsize-newsize]=0;
    WriteStdout(buffer);
    WriteStdout("\x1b[?25h"); //Turn on cursor
}
char* rl(char* prompt) {
    fflush(stdout);
    int bufferHeight=1;
    static int firstRun;

    COldTAttrs old;
    SetTerminalAttrs(&old);
    int cursor_x, cursor_y;
    char buffer[2048];
    //Used when "coming back from" history
    char _buffer[2048];
    buffer[0] = 0;
    _GetCursorPos(&cursor_x, &cursor_y);
    int cursor_pos = 0;
    int hist = HistorySize();
    goto redraw;
loop:
    ;
    int chr = ReadStdin();
    long width,height;
    if (chr == -1) goto loop;

    if (chr == 0x1b) {
        if ((chr = ReadStdin()) == '[') {
            if ((chr = ReadStdin()) == 'A') {
                if(hist==HistorySize()) strcpy(_buffer,buffer);
                //Up
                if (hist - 1 >= 0) {
                    hist--;
                    char *htext=HistoryPeek(hist);
                    if(strlen(buffer)>strlen(htext))
                        RefreshLineShrink(cursor_pos,strlen(buffer),strlen(htext));
                    strcpy(buffer, htext);
                }

                cursor_pos=0;
                goto redraw;
            } else if (chr == 'B') {
                //Down
                if (hist + 1 < HistorySize()) {
                    hist++;
                    char *htext=HistoryPeek(hist);
                    if(strlen(buffer)>strlen(htext))
                        RefreshLineShrink(cursor_pos,strlen(buffer),strlen(htext));
                    strcpy(buffer, htext);
                } else if(hist+1==HistorySize()) {
                    hist++;
                    if(strlen(buffer)>strlen(_buffer))
                        RefreshLineShrink(cursor_pos,strlen(buffer),strlen(_buffer));
                    strcpy(buffer,_buffer);
                }

                cursor_pos=0;
                goto redraw;
            } else if (chr == 'D') {
                //Left
                if (cursor_pos > 0)
                    cursor_pos--;

                goto movecursor;
            } else if (chr == 'C') {
                //Right
                if (cursor_pos + 1 <= strlen(buffer))
                    cursor_pos++;

                goto movecursor;
            } else if (chr == '1') {
                if ((chr = ReadStdin()) == ';') {
                    if ((chr = ReadStdin()) == '5') {
                        if ((chr = ReadStdin()) == 'C') {
                            //Ctrl + Right
                            long len = strlen(buffer);

                            while (cursor_pos + 1 <= len && !IsIdentChar(buffer[cursor_pos])) cursor_pos++;

                            while (cursor_pos + 1 <= len && IsIdentChar(buffer[cursor_pos])) cursor_pos++;

                            goto movecursor;
                        } else if (chr == 'D') {
                            //Ctrl + Left
                            while (cursor_pos > 0 && !IsIdentChar(buffer[cursor_pos])) cursor_pos--;

                            while (cursor_pos > 0 && IsIdentChar(buffer[cursor_pos])) cursor_pos--;

                            goto movecursor;
                        }
                    }
                }
            }
        }
    } else if (chr == 0x7f) {
        if (!cursor_pos) goto loop;
        cursor_pos--;
        //Clear deleted charactor
        long len = strlen(buffer + cursor_pos);
        //Backspace
        memmove(&buffer[cursor_pos], &buffer[cursor_pos + 1], strlen(buffer + cursor_pos) + 1);
        goto redraw;
    } else if (chr == 13) {
        goto end;
    } else if (chr == ('c' & 0x1f)) {
        RestoreTermianlAttrs(&old);
        abort();
    } else if (chr == 9) {
        long p = cursor_pos - 1;

        if (p < 0) goto loop;

        while (p - 1 >= 0 && IsIdentChar(buffer[p])) p--;

        if (!IsIdentChar(buffer[p])) p++;

        char word[cursor_pos - p + 1];
        strncpy(word, buffer + p, cursor_pos - p);
        word[cursor_pos - p] = 0;
        long wordlen = cursor_pos - p;
        //Tab
        if (rlACGen) {
            long i = 0;
            long maxlen = 0;
            long count;
            CCompletion *comps=rlACGen(buffer,p,word,&count);
            for(; i!=count; i++) {
                CCompletion *ent=&comps[i];
                if(strlen(ent->completion)>maxlen) maxlen=strlen(ent->completion);
            }

            if (!i) {
                TD_FREE(comps);
                goto loop;
            }

            long j = 0;
            WriteStdout("\n");

            for (; j != count; j++) {
                WriteStdout("\x1b[1G\x1b[1M"); //Move to x 1 and delete line
                char fmted[1024];
                sprintf(fmted, "%s\n", comps[j].displayText);
                WriteStdout(fmted);
                TD_FREE(comps[j].displayText);
            }

            WriteStdout("\x1b[1G"); //Move to x 1
            _GetCursorPos(&cursor_x, &cursor_y);
            //Find common root text across entries
            char common[maxlen + 1];
            strcpy(common, comps[0].completion);
            j = 1;

            for (; j < i; j++) {
                long idx = 0;

                while (common[idx] == comps[j].completion[idx]) idx++;

                common[idx] = 0;
                TD_FREE(comps[j].completion);
            }

            if (strlen(common)) {
                long start = cursor_pos - wordlen;
                long end = cursor_pos - wordlen + strlen(common);
                memmove(&buffer[end], &buffer[cursor_pos], strlen(&buffer[cursor_pos]) + 1);
                strncpy(&buffer[start], common, strlen(common));
                cursor_pos += strlen(common) - wordlen;
            }
            TD_FREE(comps);
        }

        goto redraw;
    } else if (chr != (chr & 0x1f)) {
        memmove(&buffer[cursor_pos + 1], &buffer[cursor_pos], strlen(buffer + cursor_pos) + 1);
        buffer[cursor_pos++] = chr;
        goto redraw;
    }

    goto loop;
redraw:
    ;
    #ifndef _WIN64
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    width=ws.ws_col;
    height=ws.ws_row;
    #else
    //https://stackoverflow.com/questions/6812224/getting-terminal-size-in-c-for-windows
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns, rows;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    #endif
    char mbuffer[32];
    WriteStdout("\x1b[?25l");  //Tunr off cursor
    sprintf(mbuffer, "\x1b[%i;%iH", cursor_y, 1);
    WriteStdout(mbuffer);
    sprintf(mbuffer, "\x1b[%iM", bufferHeight);
    WriteStdout(mbuffer);
    long len=strlen(buffer)+strlen(prompt);
    if(len>=width) {
      long lines=len/width;
      if(lines+1>bufferHeight&&cursor_y+bufferHeight+1>height) {
        sprintf(mbuffer, "\x1b[%liL", lines-bufferHeight+1);
        cursor_y-=lines-bufferHeight+1;
        if(cursor_y<0) cursor_y=1;
        WriteStdout(mbuffer);
        bufferHeight=lines+1;
        goto redraw;
      }
    }
    char *cat=TD_MALLOC(len+1);
    strcat(cat,prompt);
    strcat(cat,buffer);
    char *catptr=cat;

    while(*catptr) {
      char buffer[width+1];
      if(strlen(catptr)<width) strcpy(buffer,catptr);
      else strncpy(buffer,catptr,width);
      WriteStdout(buffer);
      catptr+=strlen(buffer);
      if(*catptr)
      WriteStdout("\x1b[1E");
    }
    TD_FREE(cat);

    WriteStdout("\x1b[?25h"); //Tunr on cursor
movecursor:
    sprintf(mbuffer, "\x1b[%i;%liH", cursor_y+len/width, (strlen(prompt)+cursor_pos)%width+1); //Move to end of prompt
    WriteStdout(mbuffer);

    goto loop;
end:
    RestoreTermianlAttrs(&old);
    printf("\n");
    return strcpy(GC_MALLOC(strlen(buffer) + 1), buffer);
}

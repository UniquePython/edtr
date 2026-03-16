#define _POSIX_C_SOURCE 200809L

#include "editor_conf.h"
#include "editor.h"

#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

EditorConfig gEC;

void configureEditor(void)
{
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    gEC.nrows = ws.ws_row;
    gEC.ncols = ws.ws_col;
    gEC.cx = 0;
    gEC.cy = 0;
}

void editorOpen(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        die("fopen: while trying to open file for reading in editorOpen()");

    char *line = NULL;
    size_t linesize = 0;
    ssize_t read;

    while ((read = getline(&line, &linesize, file)) != -1)
    {
        line[strcspn(line, "\r\n")] = '\0';

        Line text = {strdup(line), strlen(line)};
        text.cap = strlen(line) + 1;

        gEC.lines = realloc(gEC.lines, sizeof(Line) * (gEC.nlines + 1));
        gEC.lines[gEC.nlines] = text;
        gEC.nlines++;
    }

    free(line);
    fclose(file);
}

void editorInsertChar(char c)
{
    lineInsertChar(&gEC.lines[gEC.cy], gEC.cx, c);
    gEC.cx++;
}

void editorDeleteChar(void)
{
    if (gEC.cx != 0 && gEC.cx <= gEC.lines[gEC.cy].len)
    {
        lineDeleteChar(&gEC.lines[gEC.cy], gEC.cx - 1);
        gEC.cx--;
    }
}

void editorInsertNewline(void)
{
    Line *currLine = &gEC.lines[gEC.cy];
    char *rest = currLine->chars + gEC.cx;
    int restLen = currLine->len - gEC.cx;
    currLine->len = gEC.cx;
    editorInsertLine(gEC.cy + 1, rest, restLen);
    gEC.cy++;
    gEC.cx = 0;
}

void editorInsertLine(int at, char *chars, int len)
{
    gEC.lines = realloc(gEC.lines, sizeof(Line) * (gEC.nlines + 1));
    memmove(&gEC.lines[at + 1], &gEC.lines[at], sizeof(Line) * (gEC.nlines - at));

    gEC.lines[at].chars = strndup(chars, len);
    gEC.lines[at].len = len;
    gEC.lines[at].cap = len + 1;
    gEC.nlines++;
}
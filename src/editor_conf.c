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
    gEC.rowOffset = 0;
    gEC.colOffset = 0;
}

void editorOpen(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        die("fopen: while trying to open file for reading in editorOpen()");

    gEC.filename = filename;

    gEC.colOffset = 0;
    gEC.rowOffset = 0;

    char *line = NULL;
    size_t linesize = 0;
    ssize_t read;

    while ((read = getline(&line, &linesize, file)) != -1)
    {
        line[strcspn(line, "\r\n")] = '\0';

        Line text = {strdup(line), (int)strlen(line), (int)strlen(line) + 1};
        text.cap = strlen(line) + 1;

        gEC.lines = realloc(gEC.lines, sizeof(Line) * (gEC.nlines + 1));
        gEC.lines[gEC.nlines] = text;
        gEC.nlines++;
    }

    free(line);
    fclose(file);

    if (gEC.nlines == 0)
        editorInsertLine(0, "", 0);
}

char *editorRowsToString(int *outlen)
{
    int len = 0;
    for (int i = 0; i < gEC.nlines; i++)
        len += gEC.lines[i].len + 1;

    char *buffer = malloc(len);
    int buflen = 0;

    if (!buffer)
        die("malloc: while allocating buffer in editorRowsToString()");

    for (int i = 0; i < gEC.nlines; i++)
    {
        memcpy(buffer + buflen, gEC.lines[i].chars, gEC.lines[i].len);
        buffer[buflen + gEC.lines[i].len] = '\n';
        buflen += gEC.lines[i].len + 1;
    }

    *outlen = buflen;

    return buffer;
}

void editorSave(void)
{
    int buflen;
    char *buffer = editorRowsToString(&buflen);

    FILE *file = fopen(gEC.filename, "w");
    if (!file)
        die("fopen: while opening file for writing in editorSave()");

    fwrite(buffer, 1, buflen, file);
    fclose(file);
    free(buffer);

    gEC.modified = false;
}

void editorScroll(void)
{
    if (gEC.cy < gEC.rowOffset)
        gEC.rowOffset = gEC.cy;

    if (gEC.cy >= gEC.rowOffset + gEC.nrows - 1)
        gEC.rowOffset = gEC.cy - gEC.nrows + 2;

    if (gEC.cx < gEC.colOffset)
        gEC.colOffset = gEC.cx;

    if (gEC.cx >= gEC.colOffset + gEC.ncols)
        gEC.colOffset = gEC.cx - gEC.ncols + 1;
}

void editorInsertChar(char c)
{
    if (gEC.cy >= gEC.nlines)
        return;
    lineInsertChar(&gEC.lines[gEC.cy], gEC.cx, c);
    gEC.cx++;

    gEC.modified = true;
}

void editorDeleteChar(void)
{
    if (gEC.cy >= gEC.nlines)
        return;
    if (gEC.cx != 0 && gEC.cx <= gEC.lines[gEC.cy].len)
    {
        lineDeleteChar(&gEC.lines[gEC.cy], gEC.cx - 1);
        gEC.cx--;
    }

    gEC.modified = true;
}

void editorInsertNewline(void)
{
    if (gEC.cy >= gEC.nlines)
        return;
    Line *currLine = &gEC.lines[gEC.cy];
    char *rest = currLine->chars + gEC.cx;
    int restLen = currLine->len - gEC.cx;
    currLine->len = gEC.cx;
    editorInsertLine(gEC.cy + 1, rest, restLen);
    gEC.cy++;
    gEC.cx = 0;

    gEC.modified = true;
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
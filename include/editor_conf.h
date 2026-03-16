#ifndef EDITOR_CONF_H
#define EDITOR_CONF_H

#include "line.h"

#include <stdbool.h>

typedef struct
{
    int nrows, ncols;

    int nlines;
    Line *lines;

    int cx, cy;

    int rowOffset, colOffset;

    const char *filename;

    bool modified;
} EditorConfig;

extern EditorConfig gEC;

void configureEditor(void);
void editorOpen(const char *filename);
char *editorRowsToString(int *outlen);
void editorSave(void);

void editorScroll(void);

void editorInsertChar(char c);
void editorDeleteChar();

void editorInsertLine(int at, char *chars, int len);
void editorInsertNewline(void);

#endif
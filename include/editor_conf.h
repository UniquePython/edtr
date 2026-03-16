#ifndef EDITOR_CONF_H
#define EDITOR_CONF_H

#include "line.h"

typedef struct
{
    int nrows, ncols;

    int nlines;
    Line *lines;

    int cx, cy;
} EditorConfig;

extern EditorConfig gEC;

void configureEditor(void);
void editorOpen(const char *filename);
void editorInsertChar(char c);
void editorDeleteChar();

void editorInsertLine(int at, char *chars, int len);
void editorInsertNewline(void);

#endif
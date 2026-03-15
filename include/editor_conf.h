#ifndef EDITOR_CONF_H
#define EDITOR_CONF_H

typedef struct
{
    char *chars;
    int len;
} Line;

typedef struct
{
    int nrows;
    int ncols;
    int nlines;
    Line *lines;
} EditorConfig;

extern EditorConfig gEC;

void configureEditor(void);
void editorOpen(const char *filename);

#endif
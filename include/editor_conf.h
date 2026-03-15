#ifndef EDITOR_CONF_H
#define EDITOR_CONF_H

typedef struct
{
    char *chars;
    int len;
} Line;

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

#endif
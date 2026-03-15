#ifndef EDITOR_CONF_H
#define EDITOR_CONF_H

#include <unistd.h>
#include <sys/ioctl.h>

typedef struct
{
    int rows;
    int cols;
} EditorConfig;

extern EditorConfig gEC;

static inline void configureEditor(void)
{
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    gEC.rows = ws.ws_row;
    gEC.cols = ws.ws_col;
}

#endif
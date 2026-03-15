#define _POSIX_C_SOURCE 200809L

#include "editor_conf.h"

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
    {
        perror("fopen: while trying to open file for reading in editorOpen()");
        exit(1);
    }

    char *line = NULL;
    size_t linesize = 0;
    ssize_t read;

    while ((read = getline(&line, &linesize, file)) != -1)
    {
        line[strcspn(line, "\r\n")] = '\0';

        Line text = {strdup(line), strlen(line)};

        gEC.lines = realloc(gEC.lines, sizeof(Line) * (gEC.nlines + 1));
        gEC.lines[gEC.nlines] = text;
        gEC.nlines++;
    }

    free(line);
    fclose(file);
}
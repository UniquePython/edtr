#include "line.h"
#include "editor.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void lineInsertChar(Line *line, int at, char c)
{
    if (line->len + 1 >= line->cap)
    {
        int newcap = (int)(line->cap * 2);
        char *newchars = realloc(line->chars, newcap);
        if (!newchars)
            die("realloc: while increasing capacity of current line in lineInsertChar()");

        line->chars = newchars;
        line->cap = newcap;
    }

    memmove(&line->chars[at + 1], &line->chars[at], line->len - at);
    line->chars[at] = c;
    line->len++;
}

void lineDeleteChar(Line *line, int at)
{
    memmove(&line->chars[at], &line->chars[at + 1], line->len - at);
    line->len--;
}
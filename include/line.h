#ifndef LINE_H
#define LINE_H

typedef struct
{
    char *chars;
    int len;
    int cap;
} Line;

void lineInsertChar(Line *line, int at, char c);
void lineDeleteChar(Line *line, int at);

#endif
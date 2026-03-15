#ifndef APPEND_BUF_H
#define APPEND_BUF_H

typedef struct
{
    char *buf;
    int len;
} AppendBuffer;

void abAppend(AppendBuffer *ab, const char *s, int len);
void abFree(AppendBuffer *ab);

#endif
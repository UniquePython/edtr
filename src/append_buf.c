#include "append_buf.h"

#include <stdlib.h>
#include <string.h>

void abAppend(AppendBuffer *ab, const char *s, int len)
{
    char *newbuf = realloc(ab->buf, ab->len + len);
    if (newbuf == NULL)
    {
        perror("realloc: while increasing length of AppendBuffer in abAppend()");
        exit(1);
    }
    ab->buf = newbuf;
    memcpy(ab->buf + ab->len, s, len);
    ab->len += len;
}

void abFree(AppendBuffer *ab)
{
    free(ab->buf);
}
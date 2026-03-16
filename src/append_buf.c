#include "append_buf.h"
#include "editor.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void abAppend(AppendBuffer *ab, const char *s, int len)
{
    char *newbuf = realloc(ab->buf, ab->len + len);
    if (newbuf == NULL)
        die("realloc: while increasing length of AppendBuffer in abAppend()");

    ab->buf = newbuf;
    memcpy(ab->buf + ab->len, s, len);
    ab->len += len;
}

void abFree(AppendBuffer *ab)
{
    free(ab->buf);
}
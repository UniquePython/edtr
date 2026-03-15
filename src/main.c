#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string.h>

#include "editor.h"
#include "editor_conf.h"
#include "append_buf.h"

typedef enum
{
    KEY_UP = 1000,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT
} KEY;

#define NEXTLINE "\r\n"
#define CLEAR_SCREEN "\x1b[2J"
#define CUR_HIDE "\x1b[?25l"
#define CUR_SHOW "\x1b[?25h"
#define CUR_TOP_LEFT "\x1b[H"
#define CUR_MOVE "\x1b[%d;%dH"

void drawRows(AppendBuffer *ab)
{
    for (int i = 0; i < gEC.nrows; i++)
        if (i < gEC.nrows - 1)
            if (i < gEC.nlines)
            {
                int len = gEC.lines[i].len;
                if (len > gEC.ncols)
                    len = gEC.ncols;
                abAppend(ab, gEC.lines[i].chars, len);
                abAppend(ab, "\r\n", 2);
            }
            else
                abAppend(ab, "~" NEXTLINE, 3);
        else
            abAppend(ab, "~", 1);
}

void refreshScreen(void)
{
    AppendBuffer ab = {NULL, 0};

    abAppend(&ab, CLEAR_SCREEN, strlen(CLEAR_SCREEN));
    abAppend(&ab, CUR_HIDE, strlen(CUR_HIDE));
    abAppend(&ab, CUR_TOP_LEFT, strlen(CUR_TOP_LEFT));

    drawRows(&ab);

    char buf[16];
    snprintf(buf, sizeof(buf), CUR_MOVE, gEC.cy + 1, gEC.cx + 1);
    abAppend(&ab, buf, strlen(buf));

    abAppend(&ab, CUR_SHOW, strlen(CUR_SHOW));

    write(STDOUT_FILENO, ab.buf, ab.len);

    abFree(&ab);
}

int editorReadKey(void)
{
    char c;
    read(STDIN_FILENO, &c, 1);

    if (c == '\x1b')
    {
        char seq[2];
        if (read(STDIN_FILENO, seq, 2) == 2)
        {
            if (seq[0] == '[')
            {
                switch (seq[1])
                {
                case 'A':
                    return KEY_UP;
                    break;

                case 'B':
                    return KEY_DOWN;
                    break;

                case 'C':
                    return KEY_RIGHT;
                    break;

                case 'D':
                    return KEY_LEFT;
                    break;

                default:
                    return -1;
                    break;
                }
            }
        }
        return -1;
    }
    return (int)c;
}

void editorMoveCursor(int key)
{
    switch (key)
    {
    case KEY_UP:
        gEC.cy--;
        if (gEC.cy < 0)
            gEC.cy = 0;
        if (gEC.cy >= gEC.nrows)
            gEC.cy = gEC.nrows - 1;
        break;

    case KEY_RIGHT:
        gEC.cx++;
        if (gEC.cx < 0)
            gEC.cx = 0;
        if (gEC.cx >= gEC.ncols)
            gEC.cx = gEC.ncols - 1;
        break;

    case KEY_DOWN:
        gEC.cy++;
        if (gEC.cy < 0)
            gEC.cy = 0;
        if (gEC.cy >= gEC.nrows)
            gEC.cy = gEC.nrows - 1;
        break;

    case KEY_LEFT:
        gEC.cx--;
        if (gEC.cx < 0)
            gEC.cx = 0;
        if (gEC.cx >= gEC.ncols)
            gEC.cx = gEC.ncols - 1;
        break;

    default:
        break;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (tcgetattr(STDIN_FILENO, &gOrigTerm) != 0)
    {
        perror("tcgetattr: while storing original terminal state in main()");
        exit(1);
    }

    if (atexit(disableRawMode) != 0)
    {
        fprintf(stderr, "Failed to register cleanup function.\n");
        return EXIT_FAILURE;
    }

    configureEditor();

    enableRawMode();

    editorOpen(argv[1]);

    refreshScreen();

    int key;
    while ((key = editorReadKey()) != 'q')
    {
        editorMoveCursor(key);
        refreshScreen();
    }

    return EXIT_SUCCESS;
}

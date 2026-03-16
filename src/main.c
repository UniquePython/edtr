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

#define CTRL_KEY(k) ((k) & 0x1f)

#define NEXTLINE "\r\n"
#define CLEAR_SCREEN "\x1b[2J"
#define CUR_HIDE "\x1b[?25l"
#define CUR_SHOW "\x1b[?25h"
#define CUR_TOP_LEFT "\x1b[H"
#define CUR_MOVE "\x1b[%d;%dH"
#define INV_VIDEO "\x1b[7m"
#define RESET "\x1b[m"

void drawRows(AppendBuffer *ab)
{
    for (int i = 0; i < gEC.nrows - 1; i++)
    {
        int fileRow = gEC.rowOffset + i;

        if (fileRow < gEC.nlines)
        {
            int len = gEC.lines[fileRow].len - gEC.colOffset;
            if (len < 0)
                len = 0;
            if (len > gEC.ncols)
                len = gEC.ncols;

            char *start = gEC.lines[fileRow].chars + gEC.colOffset;
            abAppend(ab, start, len);
        }
        else
            abAppend(ab, "~", 1);

        abAppend(ab, NEXTLINE, strlen(NEXTLINE));
    }
}

void drawStatusBar(AppendBuffer *ab)
{
    abAppend(ab, INV_VIDEO, strlen(INV_VIDEO));

    char *modified = gEC.modified ? "[modified]" : "";

    int leftlen = snprintf(NULL, 0, "%s %s", gEC.filename, modified);
    int rightlen = snprintf(NULL, 0, "%d / %d", gEC.cy + 1, gEC.nlines);

    char *left = malloc(leftlen + 1);
    char *right = malloc(rightlen + 1);

    snprintf(left, leftlen + 1, "%s %s", gEC.filename, modified);
    snprintf(right, rightlen + 1, "%d / %d", gEC.cy + 1, gEC.nlines);

    int padding = gEC.ncols - leftlen - rightlen;

    abAppend(ab, left, leftlen);
    while (padding-- > 0)
        abAppend(ab, " ", 1);
    abAppend(ab, right, rightlen);

    abAppend(ab, RESET, strlen(RESET));

    free(left);
    free(right);
}

void refreshScreen(void)
{
    editorScroll();

    AppendBuffer ab = {NULL, 0};

    abAppend(&ab, CLEAR_SCREEN, strlen(CLEAR_SCREEN));
    abAppend(&ab, CUR_HIDE, strlen(CUR_HIDE));
    abAppend(&ab, CUR_TOP_LEFT, strlen(CUR_TOP_LEFT));

    drawRows(&ab);
    drawStatusBar(&ab);

    char buf[64];
    snprintf(buf, sizeof(buf), CUR_MOVE, gEC.cy - gEC.rowOffset + 1, gEC.cx - gEC.colOffset + 1);
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
        if (gEC.cx > gEC.lines[gEC.cy].len)
            gEC.cx = gEC.lines[gEC.cy].len;
        break;

    case KEY_RIGHT:
        gEC.cx++;
        if (gEC.cx > gEC.lines[gEC.cy].len)
            gEC.cx = gEC.lines[gEC.cy].len;
        break;

    case KEY_DOWN:
        gEC.cy++;
        if (gEC.cy >= gEC.nlines)
            gEC.cy = gEC.nlines - 1;
        if (gEC.cx > gEC.lines[gEC.cy].len)
            gEC.cx = gEC.lines[gEC.cy].len;
        break;

    case KEY_LEFT:
        gEC.cx--;
        if (gEC.cx < 0)
            gEC.cx = 0;
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
    while (1)
    {
        key = editorReadKey();

        if (key == KEY_UP || key == KEY_DOWN || key == KEY_LEFT || key == KEY_RIGHT)
            editorMoveCursor(key);
        else if (key >= 32 && key < 127)
            editorInsertChar((char)key);
        else if (key == 127)
            editorDeleteChar();
        else if (key == '\r')
            editorInsertNewline();
        else if (key == CTRL_KEY('s'))
            editorSave();
        else if (key == CTRL_KEY('q'))
            break;

        refreshScreen();
    }

    return EXIT_SUCCESS;
}

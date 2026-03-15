#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string.h>

#include "editor.h"
#include "editor_conf.h"
#include "append_buf.h"

#define NEXTLINE "\r\n"
#define CLEAR_SCREEN "\x1b[2J"
#define CUR_HIDE "\x1b[?25l"
#define CUR_SHOW "\x1b[?25h"
#define CUR_TOP_LEFT "\x1b[H"

EditorConfig gEC;

void drawRows(AppendBuffer *ab)
{
    for (int i = 0; i < gEC.rows; i++)
        if (i < gEC.rows - 1)
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

    abAppend(&ab, CUR_SHOW, strlen(CUR_SHOW));

    write(STDOUT_FILENO, ab.buf, ab.len);

    abFree(&ab);
}

int main(void)
{
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

    refreshScreen();

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        printf("%c -> %d" NEXTLINE, c, (int)c);
        if (c == 'q')
            break;
    }

    return EXIT_SUCCESS;
}

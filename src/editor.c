#include "editor.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

termios_t gOrigTerm;

void enableRawMode(void)
{
    termios_t currTerm;
    if (tcgetattr(STDIN_FILENO, &currTerm) != 0)
    {
        perror("tcgetattr: while reading terminal state in enableRawMode()");
        exit(1);
    }

    currTerm.c_lflag &= ~(ECHO | ICANON | ISIG);
    currTerm.c_iflag &= ~IXON;
    currTerm.c_oflag &= ~OPOST;

    currTerm.c_cc[VMIN] = 1;
    currTerm.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &currTerm) != 0)
    {
        perror("tcsetattr: while setting modified terminal state in enableRawMode()");
        exit(1);
    }
}

void disableRawMode(void)
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, &gOrigTerm) != 0)
    {
        perror("tcsetattr: while resetting orignal terminal state in disableRawMode()");
        return;
    }
}
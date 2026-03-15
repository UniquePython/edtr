#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define NEXTLINE "\r\n"

typedef struct termios termios_t;

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

    enableRawMode();

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        printf("%c -> %d" NEXTLINE, c, (int)c);
        if (c == 'q')
            break;
    }

    return EXIT_SUCCESS;
}

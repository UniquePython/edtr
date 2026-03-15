#ifndef EDITOR_H
#define EDITOR_H

#include <termios.h>

typedef struct termios termios_t;

extern termios_t gOrigTerm;

void enableRawMode(void);
void disableRawMode(void);

#endif
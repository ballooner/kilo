#include <ctype.h>
#include <stdio.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

struct termios originalTerminalParameters;

void leaveRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTerminalParameters);
}

void enterRawMode()
{
    tcgetattr(STDIN_FILENO, &originalTerminalParameters);
    atexit(leaveRawMode);
 
    struct termios terminalParameters = originalTerminalParameters;
   
    terminalParameters.c_lflag &= ~(ECHO | ICANON | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminalParameters);
}

int main()
{
    enterRawMode();

    char c;
    while ((read(STDIN_FILENO, &c, 1) == 1) && c != 'q')
    {
        if (iscntrl(c))
        {
            printf("%d\n", c);
        } else 
        {
            printf("%c: %d\n", c, c);
        }
    }

    return 0;
}

/** includes **/

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

/** data **/

struct termios originalTerminalParameters;

/** terminal **/

void die(const char *s)
{
    perror(s);
    exit(1);
}

void leaveRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTerminalParameters) == -1)
        die("tcsetattr");
}

void enterRawMode()
{
    if (tcgetattr(STDIN_FILENO, &originalTerminalParameters) == -1)
        die("tcgetattr");

    
    atexit(leaveRawMode);
 
    struct termios terminalParameters = originalTerminalParameters;

    terminalParameters.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    terminalParameters.c_oflag &= ~(OPOST);
    terminalParameters.c_cflag |= (CS8);
    terminalParameters.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    terminalParameters.c_cc[VMIN] = 0;
    terminalParameters.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminalParameters) == -1)
        die("tcsetattr");
}

/** init **/

int main()
{
    enterRawMode();

    while (1)
    {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
            die("read");

        if (iscntrl(c))
        {
            printf("%d\r\n", c);
        } else 
        {
            printf("%c: %d\r\n", c, c);
        }

        if (c == 'q') break;
    }

    return 0;
}

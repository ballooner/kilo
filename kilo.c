/** includes **/

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

/** defines **/

#define CTRL_KEY(k) ((k) & 0x1f)

/** data **/

struct editorConfig 
{
    int screenrows;
    int screencols;
    struct termios originalTerminalParameters;
};

struct editorConfig editorState;

/** terminal **/

void die(const char *s)
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

void leaveRawMode()
{
    if (tcsetattr(STDIN_FILENO, 
                  TCSAFLUSH, 
                  &editorState.originalTerminalParameters) == -1)
        die("tcsetattr");
}

void enterRawMode()
{
    if (tcgetattr(STDIN_FILENO, &editorState.originalTerminalParameters) == -1)
        die("tcgetattr");

    
    atexit(leaveRawMode);
 
    struct termios terminalParameters = editorState.originalTerminalParameters;

    terminalParameters.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    terminalParameters.c_oflag &= ~(OPOST);
    terminalParameters.c_cflag |= (CS8);
    terminalParameters.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    terminalParameters.c_cc[VMIN] = 0;
    terminalParameters.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminalParameters) == -1)
        die("tcsetattr");
}

char editorReadKey()
{
    int nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN) die("read");
    }

    return c;
}

int getWindowSize(int *rows, int *cols)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        return -1;
    } else 
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

/** output **/

void editorDrawRows()
{
    for (int y = 0; y < editorState.screenrows; y++)
    {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

void editorRefreshScreen()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    editorDrawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);
}

/** input **/

void editorProcessKeypress()
{
    char c = editorReadKey();

    switch (c)
    {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);

            exit(0);
            break;
    }
}

/** init **/

void initEditor()
{
    if (getWindowSize(&editorState.screenrows, &editorState.screencols) == -1)
        die("getWindowSize");
}

int main()
{
    enterRawMode();
    initEditor();

    while (1)
    {
        editorRefreshScreen();
        editorProcessKeypress();
    }

    return 0;
}

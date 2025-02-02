#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <sstream>
#include <unistd.h> 
#include <termios.h>
#include <stdlib.h>
#include <iostream>
#include <sys/ioctl.h>
#include <vector>
#include "settings.h"
#include "snake.h"

#define CTRL_KEY(k) ((k) & 0x1f)

void disableRawMode();
void enableRawMode();
void die (const char * s);
void refreshScreen();
void screenInit();
int readKeypress();
int processKeypress();

struct screenConfig S;

int main() {
    enableRawMode();
    screenInit();
    refreshScreen();

    Snake snake{S.screenRows, S.screenCols};
    //    std::cout<<"Initial direction: "<<snake.getDirection()<<"\r\n"a
    while(1) {
         int direction = processKeypress();
         switch (direction){
            case -1:
                write(STDOUT_FILENO, "\x1b[2J", 4);
                write(STDOUT_FILENO, "\x1b[H", 3);
                exit(0);
            case UP:
            case DOWN:
            case LEFT:
            case RIGHT:
                snake.setDirection(direction);
                break; 
        };
        snake.move();
        usleep(100000);
    }
       return 0;
}

int readKeypress() {
      char key = '\0';
      if(read(STDIN_FILENO, &key, 1)==-1 && errno != EAGAIN) die("read");
      
    if (key == '\x1b') {
        char seq[2];

        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        if ( seq[0] == '[') {
            switch(seq[1]) {
                case 'A' : return UP;
                case 'B' : return DOWN;
                case 'C' : return RIGHT;
                case 'D' : return LEFT;
            }
        }
        return '\x1b';
    } else {
        return key;
    }
}

int processKeypress() {
    int key = readKeypress();

    switch (key) {
        case CTRL_KEY('q'):
            return -1;
        case UP:
        case DOWN:
        case LEFT:
        case RIGHT:
            return key;
        default:
            return -2;
    }
}

void refreshScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void enableRawMode() {

    if(tcgetattr(STDIN_FILENO, &S.orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = S.orig_termios;
    //do not print the letters i type in the terminal
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    //How many bytes we want before read returns
    raw.c_cc[VMIN] = 0;


    //how fast we want read to return
    //raw.c_cc[VTIME] = 1;
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw)==-1) die("tcsetattr");
}


void disableRawMode() {

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &S.orig_termios)==-1) {
        die("tcsetattr");
    };
}


int getCursorPosition(int *rows, int *cols) {
    if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

    std::vector<char>buf(32);
    unsigned int i = 0;


 
    while (i < buf.size()) {
        if(read(STDIN_FILENO, &buf[i], 1) != 1) break;

        //if we reach the end of the STDIN
        if(buf[i] == 'R') break;
        ++i;
    }
    buf[i] = '\0';



    // example : <esc>[24;80 
    if(buf[0] != '\x1b' || buf[1] != '[') return -1;
    std::istringstream ss(&buf[2]);
    ss >> *rows;
    ss.ignore(1);
    ss >> *cols;


    return 0;
}

int getWindowSize(int *rows, int *cols){
    struct winsize ws;

    if ( ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B",12) != 12) return -1;
        return getCursorPosition( rows, cols);
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
            return 0;
  }
}


void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
    std::cerr<< s;
    exit(1);

}

void screenInit() {
    if (getWindowSize(&S.screenRows, &S.screenCols) == -1) die("getWindowSize");
}

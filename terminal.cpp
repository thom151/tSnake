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
#include "terminal.h"

#define CTRL_KEY(k) ((k) & 0x1f)
struct screenConfig S;

void displayStart() {
    std::string buf = "";
    for (int i{0}; i<S.screenRows; i++) { 
        buf += "\r\n";
         if (i == S.screenRows/3) {
            std::string welcome = "=== SNAKE ===\r\n\r\n";
            std::string directions ="Use arrows keys ↑ ↓ ← → to move\r\n";
            std::string startDirection = "Press CTRL + S to start\r\n";
            std::string exitDirection = "Press CTRL + Q to quit\r\n";

            int spaces = (S.screenCols - welcome.length())/2;
            for(; spaces > 0; --spaces) {
               buf += " ";  
            }

            buf += welcome;

            int directionSpaces = (S.screenCols - (directions.length() - 8))/2;
            for(; directionSpaces > 0; --directionSpaces) {
                buf += " ";
            }

            buf += directions;
    
            int startSpaces = (S.screenCols - startDirection.length())/2;
            for(; startSpaces > 0; --startSpaces) {
               buf += " ";  
            }

            buf += startDirection;

            int exitSpaces = (S.screenCols - exitDirection.length())/2;
            for(; exitSpaces > 0; --exitSpaces) {
               buf += " ";  
            }


            buf += exitDirection;

            
           write(STDOUT_FILENO, buf.c_str(), buf.length());
        }

       

    }

    while(true) {
        int key = readKeypress();
        switch(key) {
            case CTRL_KEY('q'):
                write(STDOUT_FILENO, "\x1b[2J", 4);
                write(STDOUT_FILENO, "\x1b[H", 3);
                exit(0);
                return;
            case CTRL_KEY('s'):
                return;
            default:
                continue;
        };
    }
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
    } else if (key == '\n') {
        return '\n'; 
    }else {
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


    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw)==-1) die("tcsetattr");
}


void disableRawMode() {

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &S.orig_termios)==-1) {
        die("tcsetattr");
    };

    std::cout<<"YOU GOT "<<snakePoints<<" POINTS\r\n";
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

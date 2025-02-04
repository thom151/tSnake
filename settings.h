#ifndef SETTINGS_H
#define SETTINGS_H
#include <termios.h>
#include <iostream>
enum Direction{
    UP = 1000,
    DOWN,
    LEFT,
    RIGHT,
    MAX,
};

struct screenConfig {
    int cx, cy;
    int screenRows;
    int screenCols;
    struct termios orig_termios;
};

struct appendBuffer {
    std::string apBuf;
};

extern struct screenConfig S;
extern int snakePoints;

#endif

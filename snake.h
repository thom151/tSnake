
#ifndef SNAKE_H
#define SNAKE_H

#include <iostream>
#include <random>
#include <string>
#include <unistd.h>
#include "settings.h"


class Snake {
private:
    int m_hx;
    int m_hy;
    int m_direction;

public:
    Snake(const int screen_rows,const int screen_cols) 
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<>hx(0, screen_cols -1 );
        std::uniform_int_distribution<>hy(0, screen_rows -1 );
        std::uniform_int_distribution<>direction(UP, RIGHT);

        m_hx = hx(gen);
        m_hy = hy(gen);
        m_direction = direction(gen);
        setPosition();
         
    }

    void setPosition() {
        if (m_hx < 0 || m_hy < 0) return;
        std::string positionCommand = "\x1b[" + std::to_string(m_hy) + ";" + std::to_string(m_hx) + "H";
        write(STDOUT_FILENO, positionCommand.c_str(), positionCommand.length());   
    }

  
    void move() {
        switch (m_direction) {
            case UP:
                --m_hy;
                setPosition();
                break;
            case DOWN:
                ++m_hy;
                setPosition();
                break;
            case LEFT:
                --m_hx;
                setPosition();
                break;
            case RIGHT:
                ++m_hx;
                setPosition();
                break; 
        }
    }

    void setDirection(int move) {
        m_direction = move; 
    }

    int getDirection() {
        return m_direction;
    }

};


#endif

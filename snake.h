
#ifndef SNAKE_H
#define SNAKE_H

#include <iostream>
#include <random>
#include <string>
#include <unistd.h>
#include <vector>
#include "settings.h"


struct Coordinates{
    int x;
    int y;
};

class Snake {
private:
    int m_hx;
    int m_hy;
    int m_direction;
    int m_length;
    std::vector<Coordinates> m_body;

public:
    Snake(const int screen_rows,const int screen_cols)
    : m_length{3}
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
        for(int i=0; i<m_length; ++i) {
            Coordinates cell = {};
            cell.x = m_hx;
            cell.y = m_hy;
            switch (m_direction) {
                case LEFT:
                    cell.x -= i;
                    break;
                case RIGHT:
                    cell.x += i;
                    break;
                case UP:
                    cell.y += i;
                    break;
                case DOWN:
                    cell.y -= i;
                    break;
            }
            m_body.push_back(cell);
        }
         
    }

    void setPosition() {
        if (m_hx < 0 || m_hy < 0) return;
        std::string positionCommand = "\x1b[" + std::to_string(m_hy) + ";" + std::to_string(m_hx) + "H";
        write(STDOUT_FILENO, positionCommand.c_str(), positionCommand.length()); 
    }
    
    void erasePreviousSnake() {
        Coordinates tail = m_body.back();
        std::string clearCommand = "\x1b[" + std::to_string(tail.y) + ";" + std::to_string(tail.x) + "H ";
        write(STDOUT_FILENO, clearCommand.c_str(), clearCommand.length());
    }
    void move() {

        erasePreviousSnake();
        Coordinates newHead = m_body[0];
        switch (m_direction) {
            case UP:
                --newHead.y;
                setPosition();
                break;
            case DOWN:
                ++newHead.y;
                setPosition();
                break;
            case LEFT:
                --newHead.x;
                setPosition();
                break;
            case RIGHT:
                ++newHead.x;
                setPosition();
                break; 
        }

        m_body.insert(m_body.begin(), newHead);
        m_body.pop_back();

        m_hx = newHead.x;
        m_hy = newHead.y;
    }

    void setDirection(int move) {
        m_direction = move; 
    }

    int getDirection() {
        return m_direction;
    }
  
    void drawSnake() {
        for(int i=0; i<m_length; ++i) {
            std::string positionCommand = "\x1b[" + std::to_string(m_body[i].y) + ";" + std::to_string(m_body[i].x) + "H";
            write(STDOUT_FILENO, positionCommand.c_str(), positionCommand.length());


            if (i==0) {
                write(STDOUT_FILENO, "O", 1);
            } else {
                write(STDOUT_FILENO, "X", 1);
            }
        }
    }
   
    void refreshScreen() {
        write(STDOUT_FILENO, "\x1b[2J", 4); // Clear the scren
        write(STDOUT_FILENO, "\x1b[H", 3);  // Move cursor to top-left corner
    }
};


#endif

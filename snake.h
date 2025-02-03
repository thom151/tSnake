
#ifndef SNAKE_H
#define SNAKE_H

#include <iostream>
#include <random>
#include <string>
#include <unistd.h>
#include <unordered_set>
#include <vector>
#include "settings.h"
#include "apple.h"


struct Coordinates{
    int x;
    int y;

    bool operator==(const Coordinates& c) const {
        return x == c.x && y == c.y;
    }
};

namespace std {
    template <>
    struct hash<Coordinates> {
        size_t operator()(const Coordinates& c) const {
            return hash<int>()(c.x) ^ (hash<int>()(c.y) << 1);
        }
    };
}

class Snake {
private:
    int m_hx;
    int m_hy;
    int m_direction;
    int m_length;
    int m_points;
    int screenRows;
    int screenCols;
    std::vector<Coordinates> m_body;
    std::unordered_set<Coordinates> m_bodySet;
    

public:
    Snake(const int screen_rows,const int screen_cols)
    : m_length{3}, m_points{0}, screenRows{screen_rows}, screenCols{screen_cols}
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
            m_bodySet.insert(cell);

        }
         
    }

    bool ateApple(Apple& apple) {
        if (apple.getXpos() == m_hx && apple.getYpos() == m_hy) {
            return true;
        }
        return false;
    } 

    bool hitWall() {
        return (m_hx == screenCols || m_hx == 0 || m_hy == screenRows|| m_hy == 0);
    }

    void grow() {
        Coordinates newTail = m_body.back();

        switch(m_direction) {
            case UP:
                newTail.y += 1;
                break;
            case DOWN:
                newTail.y -= 1;
                break;
            case LEFT:
                newTail.x += 1;
                break;
            case RIGHT:
                newTail.x -= 1;
                break;

        };

        m_body.push_back(newTail);
        m_bodySet.insert(newTail);
        ++m_length;
    }

    void increasePoints() {
        ++m_points;
        return;
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
        m_bodySet.erase(tail);
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

        Coordinates tail = m_body.back();

          if (m_bodySet.count(newHead)) {
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);

        }


        m_body.insert(m_body.begin(), newHead);
        m_bodySet.insert(newHead);
        m_bodySet.erase(tail);
        m_body.pop_back();

        m_hx = newHead.x;
        m_hy = newHead.y;
    }

    void setDirection(int move) {
        switch(move) {
            case RIGHT: if (m_direction == LEFT) return; break; 
            case LEFT : if (m_direction == RIGHT) return; break;
            case UP   : if (m_direction == DOWN) return; break;
            case DOWN : if (m_direction == UP) return; break;
            default: return;
        };
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

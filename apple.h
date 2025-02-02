#ifndef APPLE_H
#define APPLE_H

#include <random>
#include <unistd.h>

class Apple {
    private:
        int m_x;
        int m_y;
public:
    Apple(const int screen_rows, const int screen_cols) 
    {
    
        
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<>hx(0, screen_cols -1 );
        std::uniform_int_distribution<>hy(0, screen_rows -1 );

        m_x = hx(gen);
        m_y = hy(gen);
    }
    

    void setAndDraw() {
        if (m_x < 0 || m_y < 0) return;
        std::string positionCommand = "\x1b[" + std::to_string(m_y) + ";" + std::to_string(m_x) + "H";
        write(STDOUT_FILENO, positionCommand.c_str(), positionCommand.length()); 
        write(STDOUT_FILENO, "0", 1);
    }


};

#endif

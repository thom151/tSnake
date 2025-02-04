#include "terminal.h"
#include "settings.h"
#include "snake.h"
#include "apple.h"
#include "terminal.h"


int snakePoints = 0;
int main() {
    enableRawMode();
    screenInit();
    refreshScreen();
    
    displayStart(); 
    refreshScreen();
    Snake snake{S.screenRows, S.screenCols};
    Apple apple{S.screenRows, S.screenCols};
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
        if(snake.ateApple(apple)) {
            apple.reset();
            snake.grow();
            snake.increasePoints();
            snakePoints = snake.getPoints();
        }

        if (snake.hitWall()) {
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
        }

        snake.drawSnake();
        apple.setAndDraw();

        snake.displayPoints();
        usleep(100000/2);
    }

       return 0;
}




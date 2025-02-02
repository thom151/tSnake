# Compiler and flags
CC = g++
CFLAGS = -Wall -Wextra -std=c++17

# Targets and files
TARGET = main
SRC = main.cpp
HEADERS = snake.h settings.h
# Build target
$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean up build files
clean:
	rm -f $(TARGET)


#ifndef TERMINAL_H
#define TERMINAL_H

#define CTRL_KEY(k) ((k) & 0x1f)

void disableRawMode();
void enableRawMode();
void die (const char * s);
void refreshScreen();
void screenInit();
int readKeypress();
int processKeypress();
void displayStart();

#endif

#include "display.h"

void win_init()
{
    setlocale(LC_ALL, "");
    initscr();

    // cbreak();
    raw();                // dont stop on click
    noecho();             // dont print what user press
    keypad(stdscr, TRUE); // to detect arrow keys
}

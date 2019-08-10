#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>

#include "set_color.h"

void set_color_row(WINDOW* win, int row, const char* str, int num)
{
	wattron(win, COLOR_PAIR(num));
	wmove(win, row, 0);
	wprintw(win, "%s", str);
}

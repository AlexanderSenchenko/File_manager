#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>

#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <ncurses.h>

#include "init_file_manager.h"

void sig_winch(int signo)
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char*) &size);
	resizeterm(size.ws_row, size.ws_col);
}

int init_file_manager()
{
	WINDOW* win_left;
	WINDOW* win_right;

	int starty1 = 0;
	int startx1 = 0;
	int starty2 = 0;
	int startx2;

	int row, col;

	initscr();
	signal(SIGWINCH, sig_winch);
	cbreak();
	noecho();
	curs_set(FALSE);
	refresh();

	startx2 = get_startx2();
	
	get_row_col(&row, &col);

	if (init_win(&win_left, row, col, starty1, startx1) == -1) {
		getch();
		return -1;
	}

	if (init_win(&win_right, row, col, starty2, startx2) == -1) {
		getch();
		return -1;
	}

	getch();

	delwin(win_right);
	delwin(win_left);

	endwin();

	return 0;
}

static int init_win(WINDOW** win, int row, int col, int starty, int startx)
{
	*win = newwin(row, col, starty, startx);
	if (*win == NULL)
		return -1;

	box(*win, '|', '-');

	wrefresh(*win);

	return 0;
}

static int get_startx2()
{
	int max_y, max_x;

	getmaxyx(stdscr, max_y, max_x);

	return ((max_x % 2) == 0) ? max_x / 2 : (max_x / 2) + 1;
}

static void get_row_col(int* row, int* col)
{
	int max_y, max_x;

	getmaxyx(stdscr, max_y, max_x);

	*row = max_y;
	*col = (max_x / 2) - 1;
}

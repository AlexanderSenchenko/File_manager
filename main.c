#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>

#include <dirent.h>

#include "init_file_manager.h"
#include "dir.h"

#define MOVE_UP -1
#define MOVE_DOWN 1

void act_mv(WINDOW*, int*, struct dirent**, int);
void set_color_row(WINDOW*, int, const char*, int);

void act_mv(WINDOW* win, int* row, struct dirent** namelist, int way_move)
{
	set_color_row(win, *row, namelist[*row]->d_name, 2);

	(*row) += way_move;

	set_color_row(win, *row, namelist[*row]->d_name, 1);

	wrefresh(win);
}

void set_color_row(WINDOW* win, int row, const char* str, int num)
{
	wattron(win, COLOR_PAIR(num));
	wmove(win, row, 0);
	wprintw(win, "%s", str);
	wrefresh(win);
}

int main()
{
	WINDOW* win[2];

	int act;
	int curr_win = 0;

	struct dirent** namelist[2];
	int n[2];

	int row[2] = { 0, 0 };

	init_file_manager(&(win[0]), &(win[1]));

	read_dir(win[0], &(namelist[0]), &(n[0]));
	read_dir(win[1], &(namelist[1]), &(n[1]));

	

	wattron(win[curr_win], COLOR_PAIR(1));
	wmove(win[curr_win], row[curr_win], 0);
	wprintw(win[curr_win], "%s", namelist[curr_win][row[curr_win]]->d_name);
	wrefresh(win[curr_win]);

	while ((act = wgetch(stdscr)) != KEY_BACKSPACE) {
		switch (act) {
			case KEY_ENTER:
				break;
			case KEY_STAB:
				break;
			case KEY_UP:
				if (row[curr_win] > 0)
					act_mv(win[curr_win], &(row[curr_win]),
						namelist[curr_win], MOVE_UP);
				break;
			case KEY_DOWN:
				if (row[curr_win] < n[curr_win] - 1)
					act_mv(win[curr_win], &(row[curr_win]),
						namelist[curr_win], MOVE_DOWN);
				break;
			default:
				break;
		}
	}

	free_namelist(namelist[0], n[0]);
	free_namelist(namelist[1], n[1]);

	delwin(win[0]);
	delwin(win[1]);

	endwin();

	return 0;
}

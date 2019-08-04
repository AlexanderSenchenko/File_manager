#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>

#include <dirent.h>
// #include <sys/types.h>

#include "init_file_manager.h"
#include "dir.h"

int main()
{
	WINDOW* win_left;
	WINDOW* win_right;

	int act;

	struct dirent** namelist_left;
	int n_left;

	struct dirent** namelist_right;
	int n_right;

	init_file_manager(&win_left, &win_right);

	read_dir(win_left, &namelist_left, &n_left);
	read_dir(win_right, &namelist_right, &n_right);

	while ((act = wgetch(stdscr)) != KEY_BACKSPACE) {
		switch (act) {
			case KEY_ENTER:
				break;
			case KEY_STAB:
				break;
			default:
				break;
		}
	}

	free_namelist(namelist_left, n_left);
	free_namelist(namelist_right, n_right);

	delwin(win_right);
	delwin(win_left);

	endwin();

	return 0;
}

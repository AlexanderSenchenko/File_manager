#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>

#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "init_file_manager.h"
#include "dir.h"
#include "act.h"
#include "set_color.h"

#define MOVE_UP -1
#define MOVE_DOWN 1

int main()
{
	WINDOW* win[2];

	int act;
	int i;
	int curr_win = 0;

	struct dirent** namelist[2];
	int n[2];

	int row[2] = { 0, 0 };

	char** cwd;
	cwd = malloc(sizeof(char*) * 2);

	init_file_manager(&(win[0]), &(win[1]));

	read_dir(win[0], &(namelist[0]), &(n[0]), &(cwd[0]));
	read_dir(win[1], &(namelist[1]), &(n[1]), &(cwd[1]));

	#ifndef DEBUG
	set_color_row(win[curr_win], row[curr_win],
			namelist[curr_win][row[curr_win]], COLOR_TEXT);
	wrefresh(win[curr_win]);
	#else
	printf("%s\n\r", namelist[curr_win][row[curr_win]]->d_name);
	#endif

	while ((act = wgetch(stdscr)) != KEY_F(10)) {
		switch (act) {
			case 10: // Enter
				act_enter(win[curr_win], &(row[curr_win]),
						&(namelist[curr_win]),
						&(n[curr_win]),
						&(cwd[curr_win]));
				break;
			case 9: // Tab
				act_tab(win, row, namelist, &curr_win, cwd);
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
			case KEY_F(1):
				act_copy(win, row, namelist, n, curr_win, cwd);
				break;
			default:
				break;
		}
	}

	for (i = 0; i < 2; i++) {
		if (cwd[i] != NULL) {
			free(cwd[i]);
		}
	}
	free(cwd);

	free_namelist(namelist[0], n[0]);
	free_namelist(namelist[1], n[1]);

	delwin(win[0]);
	delwin(win[1]);

	endwin();

	return 0;
}

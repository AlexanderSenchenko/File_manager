#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>

#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "set_color.h"

void set_color_row(WINDOW* win, int row, struct dirent* info, int num)
{
	int max_y, max_x;
	int i;
	struct stat sb;
	int ret;

	getmaxyx(win, max_y, max_x);

	wattron(win, COLOR_PAIR(num));

	wmove(win, row, 0);
	for (i = 0; i < max_x; i++)
		wprintw(win, " ");

	wmove(win, row, 0);
	wprintw(win, " %s", info->d_name);

	#if 1
	ret = stat(info->d_name, &sb);
	if (ret) {
		perror("stat");
		return;
	}	

	wmove(win, row, max_x / 2);
	wprintw(win, "|%ld", sb.st_size);
	#endif
}

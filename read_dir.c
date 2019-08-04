#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>

#include <dirent.h>

#include "dir.h"

int read_dir(WINDOW* win, struct dirent*** namelist, int* n)
{
	*n = scandir(".", namelist, 0, alphasort);

	if (*n < 0) return -1;

	output_dir(win, *namelist, *n);

	return 0;
}

static int output_dir(WINDOW* win, struct dirent** namelist, int n)
{
	int i;

	wmove(win, 0, 0);
	for (i = 0; i < n; ++i)
		wprintw(win, "%s\n", namelist[i]->d_name);

	wrefresh(win);
}

void free_namelist(struct dirent** namelist, int n)
{
	while (n--)
		free(namelist[n]);
	free(namelist);
}


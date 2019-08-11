#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>

#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dir.h"

int read_dir(WINDOW* win, struct dirent*** namelist, int* n, char** cwd)
{
	*n = scandir(".", namelist, 0, alphasort);

	if (*n < 0)
		return -1;

	*cwd = getcwd(NULL, 0);
	if (!cwd) {
		perror("getcwd");
		return -1;
	}

	#ifndef DEBUG
	output_dir(win, *namelist, *n);
	#endif

	return 0;
}

static int output_dir(WINDOW* win, struct dirent** namelist, int n)
{
	int i;
	int max_y, max_x;
	struct stat sb;
	int ret;

	getmaxyx(win, max_y, max_x);

	for (i = 0; i < n; ++i) {
		wmove(win, i, 1);
		wprintw(win, "%s", namelist[i]->d_name);

		#if 0
		ret = stat(namelist[i]->d_name, &sb);
		if (ret) {
			perror("stat");
			break;
		}

		wmove(win, i, (max_x / 2) + 1);
		wprintw(win, "%ld", sb.st_size);
		#endif
	}

	wmove(win, 0, max_x / 2);
	wvline(win, '|', max_y);

	wrefresh(win);
}

void free_namelist(struct dirent** namelist, int n)
{
	while (n--) {
		if (namelist[n] != NULL)
			free(namelist[n]);
		namelist[n] = NULL;
	}
	free(namelist);
}


#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>

#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "act.h"
#include "dir.h"
#include "set_color.h"

extern const int COLOR_TEXT;
extern const int UNCOLOR_TEXT;

void act_copy(WINDOW** win, int* row, struct dirent*** namelist, int curr_win,
								char** cwd)
{
	const char* name = namelist[curr_win][row[curr_win]]->d_name;

	struct stat sb;
	int ret;

	int fd_read;
	int fd_write;

	ret = stat(name, &sb);
	if (ret) {
		perror("stat");
		return;
	}

	if ((sb.st_mode & S_IFMT) != S_IFREG) {
		#ifdef DEBUG
		printf("%s not file\n", name);
		#endif
		return;
	}

	if ((sb.st_mode & S_IRUSR) != S_IRUSR) {
		#ifdef DEBUG
		printf("Owner not have read permission\n");
		#endif
		return;
	}

	
}

void act_mv(WINDOW* win, int* row, struct dirent** namelist, int way_move)
{
	#ifndef DEBUG
	set_color_row(win, *row, namelist[*row], UNCOLOR_TEXT);
	#endif

	(*row) += way_move;

	#ifndef DEBUG
	set_color_row(win, *row, namelist[*row], COLOR_TEXT);

	wrefresh(win);
	#else
	printf("%s\n\r", namelist[*row]->d_name);
	#endif
}

void act_tab(WINDOW** win, int* row, struct dirent*** namelist, int* curr_win)
{
	int ret_chdir;

	#ifndef DEBUG
	set_color_row(win[*curr_win], row[*curr_win],
			namelist[*curr_win][row[*curr_win]], UNCOLOR_TEXT);

	wrefresh(win[*curr_win]);
	#endif

	*curr_win ^= 1;

	#ifndef DEBUG
	set_color_row(win[*curr_win], row[*curr_win],
			namelist[*curr_win][row[*curr_win]], COLOR_TEXT);

	wrefresh(win[*curr_win]);
	#else
	printf("%s\n\r", namelist[*curr_win][row[*curr_win]]->d_name);
	#endif
}

int act_enter(WINDOW* win, int* row, struct dirent*** namelist, int* n,
							char** cwd)
{
	const char* ptr_buff;
	int ret_check_dir;
	int ret_chdir;

	ret_chdir = chdir(*cwd);
	if (ret_chdir) {
		perror("chdir");
		return -1;
	}

	ptr_buff = (*namelist)[*row]->d_name;
	ret_check_dir = check_dir(ptr_buff);

	if (!ret_check_dir) {
		ret_chdir = chdir(ptr_buff);
		if (ret_chdir) {
			perror("chdir");
			return -1;
		}

		if (*cwd != NULL)
			free(*cwd);

		free_namelist(*namelist, *n);

		#ifndef DEBUG
		wattron(win, COLOR_PAIR(UNCOLOR_TEXT));
		wclear(win);
		#endif

		*row = 0;
		
		read_dir(win, namelist, n, cwd);

		#ifndef DEBUG
		set_color_row(win, *row, (*namelist)[*row],
							COLOR_TEXT);
		wrefresh(win);
		#else
		printf("%s\n\r", (*namelist)[*row]->d_name);
		#endif
	}

	return 0;
}

static int check_dir(const char* str)
{
	struct stat sb;
	int ret;

	ret = stat(str, &sb);
	return ((sb.st_mode & S_IFMT) == S_IFDIR) ? 0 : 1;
}

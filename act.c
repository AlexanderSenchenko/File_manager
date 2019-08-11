#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>

#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fcntl.h>

#include "act.h"
#include "dir.h"
#include "set_color.h"

extern const int COLOR_TEXT;
extern const int UNCOLOR_TEXT;

extern int START_ROW_COMM_STR;

void act_copy(WINDOW** win, int* row, struct dirent*** namelist, int curr_win,
								char** cwd)
{
	const char* name = namelist[curr_win][row[curr_win]]->d_name;

	struct stat sb;
	int ret;

	int fd_read;
	int fd_write;

	int ret_chdir;

	int permission = 0;

	#ifndef DEBUG
	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);
	#endif

	ret = stat(name, &sb);
	if (ret) {
		#if 0
		wmove(stdscr, START_ROW_COMM_STR, 0);
		whline(stdscr, ' ', max_x);
		wmove(stdscr, START_ROW_COMM_STR, 0);
		wrefresh(stdscr);
		#endif
		perror("stat");
		return;
	}

	if ((sb.st_mode & S_IFMT) != S_IFREG) {
		printf("%s not file\n", name);
		return;
	}

	if ((permission = (sb.st_mode & S_IRUSR)) != S_IRUSR) {
		#ifdef DEBUG
		printf("Owner not have read permission\n");
		#endif
		return;
	}

	fd_read = open(name, O_RDONLY);
	if (fd_read == -1) {
		perror("open");
		return;
	}

	ret_chdir = chdir(cwd[curr_win ^ 1]);
	if (ret_chdir) {
		perror("chdir");
		close(fd_read);
		return;
	}

	fd_write = open(name, O_WRONLY | O_CREAT);

	close(fd_write);
	close(fd_read);
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

void act_tab(WINDOW** win, int* row, struct dirent*** namelist, int* curr_win,
							char** cwd)
{
	int ret_chdir;

	#ifndef DEBUG
	set_color_row(win[*curr_win], row[*curr_win],
			namelist[*curr_win][row[*curr_win]], UNCOLOR_TEXT);

	wrefresh(win[*curr_win]);
	#endif

	*curr_win ^= 1;

	ret_chdir = chdir(cwd[*curr_win]);
	if (ret_chdir) {
		perror("chdir");
		return;
	}

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

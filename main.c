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

#define MOVE_UP -1
#define MOVE_DOWN 1

void act_mv(WINDOW*, int*, struct dirent**, int);
void set_color_row(WINDOW*, int, const char*, int);
int check_dir(const char*);
void act_tab(WINDOW**, int*, struct dirent***, int*);
void act_enter(WINDOW*, int*, struct dirent***, int*);

void act_mv(WINDOW* win, int* row, struct dirent** namelist, int way_move)
{
	#ifndef DEBUG
	set_color_row(win, *row, namelist[*row]->d_name, UNCOLOR_TEXT);
	#endif

	(*row) += way_move;

	#ifndef DEBUG
	set_color_row(win, *row, namelist[*row]->d_name, COLOR_TEXT);

	wrefresh(win);
	#else
	printf("%s\n\r", namelist[*row]->d_name);
	#endif
}

void set_color_row(WINDOW* win, int row, const char* str, int num)
{
	wattron(win, COLOR_PAIR(num));
	wmove(win, row, 0);
	wprintw(win, "%s", str);
}

int check_dir(const char* str)
{
	struct stat sb;
	int ret;

	ret = stat(str, &sb);
	return ((sb.st_mode & S_IFMT) == S_IFDIR) ? 0 : 1;
}

void act_tab(WINDOW** win, int* row, struct dirent*** namelist, int* curr_win)
{
	#ifndef DEBUG
	set_color_row(win[*curr_win], row[*curr_win],
			namelist[*curr_win][row[*curr_win]]->d_name,
			UNCOLOR_TEXT);

	wrefresh(win[*curr_win]);
	#endif

	*curr_win ^= 1;

	#ifndef DEBUG
	set_color_row(win[*curr_win], row[*curr_win],
			namelist[*curr_win][row[*curr_win]]->d_name,
			COLOR_TEXT);

	wrefresh(win[*curr_win]);
	#else
	printf("%s\n\r", namelist[*curr_win][row[*curr_win]]->d_name);
	#endif
}

void act_enter(WINDOW* win, int* row, struct dirent*** namelist, int* n)
{
	const char* ptr_buff;
	int ret_check_dir;
	int ret_chdir;

	ptr_buff = (*namelist)[*row]->d_name;
	ret_check_dir = check_dir(ptr_buff);

	if (!ret_check_dir) {
		free_namelist(*namelist, *n);

		#ifndef DEBUG
		wattron(win, COLOR_PAIR(UNCOLOR_TEXT));
		wclear(win);
		#endif

		*row = 0;

		ret_chdir = chdir(ptr_buff);
		if (ret_chdir) {
			endwin();
			exit(1);
		}

		read_dir(win, namelist, n);

		#ifndef DEBUG
		set_color_row(win, *row, (*namelist)[*row]->d_name,
							COLOR_TEXT);
		wrefresh(win);
		#else
		printf("%s\n\r", (*namelist)[*row]->d_name);
		#endif
	}
}

int main()
{
	WINDOW* win[2];

	int act;
	int curr_win = 0;
	int ret_check;
	int ret_chdir;
	const char* ptr_buff;

	struct dirent** namelist[2];
	int n[2];

	int row[2] = { 0, 0 };

	init_file_manager(&(win[0]), &(win[1]));

	read_dir(win[0], &(namelist[0]), &(n[0]));
	read_dir(win[1], &(namelist[1]), &(n[1]));

	#ifndef DEBUG
	set_color_row(win[curr_win], row[curr_win],
			namelist[curr_win][row[curr_win]]->d_name, COLOR_TEXT);
	wrefresh(win[curr_win]);
	#else
	printf("%s\n\r", namelist[curr_win][row[curr_win]]->d_name);
	#endif

	while ((act = wgetch(stdscr)) != KEY_BACKSPACE) {
		switch (act) {
			case 10: // Enter
				act_enter(win[curr_win], &(row[curr_win]),
						&(namelist[curr_win]),
						&(n[curr_win]));
				break;
			case 9: // Tab
				act_tab(win, row, namelist, &curr_win);
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

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

#include <string.h>

#include <errno.h>

#include <pthread.h>

#include "act.h"
#include "dir.h"
#include "set_color.h"

#define SIZE_BUF 64

extern const int COLOR_TEXT;
extern const int UNCOLOR_TEXT;

extern int START_ROW_COMM_STR;

off_t len;
off_t len_read;

pthread_mutex_t mutex;

void act_copy(WINDOW** win, int* row, struct dirent*** namelist, int* n,
						int curr_win, char** cwd)
{
	const char* name = namelist[curr_win][row[curr_win]]->d_name;

	struct stat sb;

	int fd_read;
	int fd_write;

	int permission = 0;

	ssize_t ret_read;
	ssize_t ret_write;
	char buf[SIZE_BUF];
	memset(buf, 0, SIZE_BUF);

	off_t curr_len;

	#ifndef DEBUG
	pthread_t tid;
	void* status;
	#endif

	if (stat(name, &sb)) {
		#ifndef DEBUG
		output_to_comm_str("Error: stat");
		#else
		perror("stat");
		#endif
		return;
	}

	len = sb.st_size;
	curr_len = len;
	len_read = 0;

	if ((sb.st_mode & S_IFMT) != S_IFREG) {
		#ifndef DEBUG
		output_to_comm_str("Error: not file");
		#else
		printf("%s not file\n", name);
		#endif
		return;
	}

	if ((permission = (sb.st_mode & S_IRUSR)) != S_IRUSR) {
		#ifndef DEBUG
		output_to_comm_str("Owner not have read permission");
		#else
		wprintw(stdscr, "Owner not have read permission");
		#endif
		return;
	}

	if ((fd_read = open(name, O_RDONLY)) == -1) {
		#ifndef DEBUG
		output_to_comm_str("Error: file not open");
		#else
		perror("open");
		#endif
		return;
	}

	if (chdir(cwd[curr_win ^ 1])) {
		#ifndef DEBUG
		output_to_comm_str("Error: chdir");
		#else
		perror("chdir");
		#endif
		close(fd_read);
		return;
	}

	permission = sb.st_mode & S_IRWXU;
	permission |= sb.st_mode & S_IRWXG;
	permission |= sb.st_mode & S_IRWXO;

	fd_write = open(name, O_WRONLY | O_CREAT | O_EXCL, permission);
	if (fd_write == -1) {
		#ifndef DEBUG
		output_to_comm_str("Error: open or creat");
		#else
		perror("open(not open or creat)");
		#endif
		close(fd_read);

		if (chdir(cwd[curr_win])) {
			#ifndef DEBUG
			output_to_comm_str("Fatal error. Press any kay to continue...");
			getch();
			#else
			perror("chdir");
			#endif
			close(fd_read);
			close(fd_write);
			exit(EXIT_FAILURE);
		}
		return;
	}

	#ifndef DEBUG
	pthread_create(&tid, NULL, progress_bar, NULL);
	pthread_mutex_init(&mutex, NULL);
	#endif
	
	while ((curr_len != 0) && ((ret_read = read(fd_read, buf, SIZE_BUF)) != 0)) {
		if (ret_read == -1) {
			if (errno == EINTR)
				continue;
			#ifndef DEBUG
			output_to_comm_str("Fatal error: read");
			pthread_cancel(tid);
			#else
			perror("read");
			#endif
			break;
		}

		curr_len -= ret_read;

		#ifndef DEBUG
		pthread_mutex_lock(&mutex);
		len_read += ret_read;
		pthread_mutex_unlock(&mutex);
		#endif

		ret_write = write(fd_write, buf, ret_read);
		if (ret_write == -1) {
			#ifndef DEBUG
			output_to_comm_str("Error: write");
			pthread_cancel(tid);
			#else
			perror("write");
			#endif
			break;	
		}

		memset(buf, 0, ret_read);
	}

	#ifndef DEBUG
	pthread_join(tid, &status);
	pthread_mutex_destroy(&mutex);
	#endif

	close(fd_read);
	close(fd_write);

	free_namelist(namelist[curr_win ^ 1], n[curr_win ^ 1]);
	wclear(win[curr_win ^ 1]);	
	read_dir(win[curr_win ^ 1], &(namelist[curr_win ^ 1]),
					&(n[curr_win ^ 1]), NULL);

	if (chdir(cwd[curr_win])) {
		#ifndef DEBUG
		output_to_comm_str("Fatal error. Press any kay to continue...");
		getch();
		#else
		perror("chdir");
		#endif
		exit(EXIT_FAILURE);
	}
}

void* progress_bar(void* ptr)
{
	off_t share;
	float precent;
	int count_cells = 0;
	int i;
	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);

	while(1) {
		pthread_mutex_lock(&mutex);
		share = len_read;
		pthread_mutex_unlock(&mutex);

		precent = (float) share;
		precent = (precent / len) * 100;

		while ((count_cells * 2) <= precent)
			count_cells++;

		wmove(stdscr, START_ROW_COMM_STR, 0);
		whline(stdscr, ' ', max_x);
		
		wmove(stdscr, START_ROW_COMM_STR, 0);
		wprintw(stdscr, "[");
		
		whline(stdscr, '*', count_cells);
		wmove(stdscr, START_ROW_COMM_STR, count_cells + 1);
		whline(stdscr, ' ', 50 - count_cells);

		wmove(stdscr, START_ROW_COMM_STR, 51);
		wprintw(stdscr, "] ");
		wprintw(stdscr, "%.0f", precent);

		wrefresh(stdscr);
		if (share == len)
			break;
	}

	return NULL;
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

static void output_to_comm_str(const char* str)
{
	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);

	wmove(stdscr, START_ROW_COMM_STR, 0);
	whline(stdscr, ' ', max_x);
	wmove(stdscr, START_ROW_COMM_STR, 0);
	wprintw(stdscr, "%s", str);
	wrefresh(stdscr);
}

static int check_dir(const char* str)
{
	struct stat sb;
	int ret;

	ret = stat(str, &sb);
	return ((sb.st_mode & S_IFMT) == S_IFDIR) ? 0 : 1;
}

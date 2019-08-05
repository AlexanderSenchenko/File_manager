#ifndef INIT_FILE_MANAGER_H
#define INIT_FILE_MANAGER_H

#define COLOR_TEXT 1
#define	UNCOLOR_TEXT 2

int init_file_manager(WINDOW**, WINDOW**);
static int init_win(WINDOW**, int, int, int, int);
static int get_startx2();
static void get_row_col(int*, int*);

#endif

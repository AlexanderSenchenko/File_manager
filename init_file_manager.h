#ifndef INIT_FILE_MANAGER_H
#define INIT_FILE_MANAGER_H

extern const int COLOR_TEXT;
extern const int UNCOLOR_TEXT;

int init_file_manager(WINDOW**, WINDOW**);
static int init_menu();
static int init_command_str();
static int init_win(WINDOW**, int, int, int, int);
static int get_startx2();
static void get_row_col(int*, int*);

#endif

#ifndef ACT_H
#define ACT_H

void act_copy(WINDOW**, int*, struct dirent***, int*, int, char**);
void act_mv(WINDOW*, int*, struct dirent**, int);
void act_tab(WINDOW**, int*, struct dirent***, int*, char**);
int act_enter(WINDOW*, int*, struct dirent***, int*, char**);
static void output_to_comm_str(const char*);
static int check_dir(const char*);

#endif

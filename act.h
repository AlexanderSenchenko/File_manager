#ifndef ACT_H
#define ACT_H

void act_mv(WINDOW*, int*, struct dirent**, int);
void act_tab(WINDOW**, int*, struct dirent***, int*);
int act_enter(WINDOW*, int*, struct dirent***, int*);
static int check_dir(const char*);

#endif

#ifndef DIR_H
#define DIR_H

int read_dir(WINDOW*, struct dirent***, int*, char**);
static int output_dir(WINDOW*, struct dirent**, int);
void free_namelist(struct dirent**, int);

#endif

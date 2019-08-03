NCURSES_CFLAGS := -D_GNU_SOURSE -I/usr/include/ncursesw
NCURSES_LIBS := -lncursesw -ltinfo

all:
	gcc init_file_manager.c -c -g $(NCURSES_CFLAGS)
	gcc main.c -c -g
	gcc main.o init_file_manager.o -o main $(NCURSES_LIBS)

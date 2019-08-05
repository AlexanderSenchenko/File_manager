NCURSES_CFLAGS := -D_GNU_SOURSE -I/usr/include/ncursesw
NCURSES_LIBS := -lncursesw -ltinfo
DEBUG := -DDEBUG

all:
	gcc init_file_manager.c -c -g $(NCURSES_CFLAGS)
	gcc read_dir.c -c -g $(NCURSES_CFLAGS)
	gcc main.c -c -g $(NCURSES_CFLAGS)
	gcc main.o init_file_manager.o read_dir.o -o main $(NCURSES_LIBS)

.PHONY: debug
debug:
	gcc init_file_manager.c -c -g $(NCURSES_CFLAGS) $(DEBUG)
	gcc read_dir.c -c -g $(NCURSES_CFLAGS) $(DEBUG)
	gcc main.c -c -g $(NCURSES_CFLAGS) $(DEBUG)
	gcc main.o init_file_manager.o read_dir.o -o main $(NCURSES_LIBS)

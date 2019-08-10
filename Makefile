NCURSES_CFLAGS := -D_GNU_SOURSE -I/usr/include/ncursesw
NCURSES_LIBS := -lncursesw -ltinfo
DEBUG := -DDEBUG

all:
	gcc set_color.c -c -g $(NCURSES_CFLAGS)
	gcc act.c -c -g $(NCURSES_CFLAGS)
	gcc init_file_manager.c -c -g $(NCURSES_CFLAGS)
	gcc read_dir.c -c -g $(NCURSES_CFLAGS)
	gcc main.c -c -g $(NCURSES_CFLAGS)
	gcc main.o init_file_manager.o read_dir.o act.o set_color.o -o main $(NCURSES_LIBS)

.PHONY: debug
debug:
	gcc set_color.c -c -g $(NCURSES_CFLAGS)
	gcc act.c -c -g $(NCURSES_CFLAGS) $(DEBUG)
	gcc init_file_manager.c -c -g $(NCURSES_CFLAGS) $(DEBUG)
	gcc read_dir.c -c -g $(NCURSES_CFLAGS) $(DEBUG)
	gcc main.c -c -g $(NCURSES_CFLAGS) $(DEBUG)
	gcc main.o init_file_manager.o read_dir.o act.o set_color.o -o main $(NCURSES_LIBS)

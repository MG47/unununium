/* LICENSE INFO: TBD*/

/* tiper - text editor */

/* */

#include "utilbox.h"

#define VERSION 0
#define REVISION 1
#define TOOL_NAME "tiper"

static FILE *stream;
static int maxrow, maxcol;
static int row, col;
static void usage()
{
	printf("tiper:\n");
	printf("Usage: "
		"./tiper [OPTIONS] [FILE]\n"
		"OPTIONS:\n"
		"-h - Usage\n"
		"-v - Version\n"); 
}

static void print_version()
{
	printf("%s version:%d.%d\n",TOOL_NAME, VERSION, REVISION);
}

static void signal_handler(int signo)
{
	if (signo == SIGINT) {
		endwin();
		fflush(stream);
		exit(EXIT_FAILURE);
	}
}

static FILE *parse_file(char *filename)
{
	/* TODO parse path and filename */
	stream = fopen(filename, "w+");
	if (!stream) {
		printf("error: %s\n", strerror(errno));
		return NULL;
	}
	return stream;
}

static int init_console()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, maxcol, maxrow);
	getyx(stdscr, col, row);
	printw("Tiper Text Editor %d.%d\n", TIPER_VERSION, TIPER_REVISION);
	refresh();
	return 0;
}

int tiper_main(int argc, char **argv)
{
	int opt;
	char *file_string;

	printf("\nTiper Text Editor :\n\n");
	if (argc < 2) {
		usage();
		return 0;
	}

	while ((opt = getopt(argc, argv, "hv")) != -1) {
		switch (opt) {
		case 'h':
			usage();
			return 0;
		case 'v':
			print_version();
			return 0;
		}
	}

	file_string = argv[1];

	stream = parse_file(file_string);
	if (!stream)
		return 0;

	if (signal(SIGINT, signal_handler) == SIG_ERR) {
		printf("Error: Cannot handle signal SIGNINT");
		return 0;
	}

	if (init_console())
		return 0;

	int read;

	while (1) {
	// threaded:
	// open console window 
	// read char, write char
	// update cursor
	// input and interpret keys
		// save load exit find 
	// close resources	 	
		read = getch();
		switch(read) {
		case KEY_LEFT:
			if (col > 0) {
				col--;
				move(row, col);
			}
			break;
		case KEY_RIGHT:
			if (col < maxcol) {
				col++;
				move(row, col);
			}
			break;
		case KEY_UP:
			if (row > 0) {
				row--;
				move(row, col);
			}
			break;
		case KEY_DOWN:
			if (row < maxrow) {
				row++;
				move(row, col);
			}
			break;
		}
		continue;
		printw("%c", read);
		refresh();
	}
	return 0;
}



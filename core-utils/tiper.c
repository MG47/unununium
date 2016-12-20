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

/* TODO Change this */

/*TODO Test this*/
#ifndef CTRL
#define CTRL(x) ((x) & 0x1f)
#endif

struct file_buffer {
	char **buf;
	int buffer_lines;
};

static struct file_buffer buffer;

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
		erase();
		refresh();
		endwin();
		exit(EXIT_FAILURE);
	}
}
#if 0
static void resize_handler(int sig)
{
	// handle SIGWINCH
}
#endif

static FILE *parse_file(char *filename)
{
	int i, new_file = 0;
	stream = fopen(filename, "r+");
	if (!stream) {
		stream = fopen(filename, "w+");
		if (!stream) {
			printf("error: %s\n", strerror(errno));
			return NULL;
		}
		new_file = 1;
	}

	i = 0;
	buffer.buf = (char **)malloc(300 * sizeof(char *));
	for (i = 0; i < 3000; i++) {
		buffer.buf[i] = malloc(sizeof(char) * 20);
	}

	// TODO fix this
#if 0
	if (new_file)
		return stream;
#endif
	i = 0;
	while (fgets(buffer.buf[i], 30, stream) != NULL)
		i++;

	buffer.buffer_lines = i;
	
	if (buffer.buffer_lines > 24) {
		printf("error: tiper can only handle files with less than %d lines currently\n", 24);
		exit(EXIT_FAILURE);
	}

	printf("lines in file %d\n", buffer.buffer_lines);

	return stream;
}

static int init_console()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, maxrow, maxcol);
	attron(A_REVERSE);
	mvprintw(maxrow-5, maxcol/2, "Tiper Text Editor %d.%d\n", TIPER_VERSION, TIPER_REVISION);
	mvprintw(maxrow-4, maxcol/2, "Shortcuts");
	mvprintw(maxrow-3, maxcol/2, "Save and Exit: Ctrl+X");
	attroff(A_REVERSE);
	refresh();
	return 0;
}

static void print_contents() 
{
	int i;
	for (i = 0; i < buffer.buffer_lines; i++) 
		mvprintw(i, 0, "%s", buffer.buf[i]);
	refresh();
}

#if 0
static int process_input(int read)
{
	return 0;
}
#endif
static int save_and_exit()
{
	erase();
	refresh();
	endwin();

// TODO change this 
	int i;
	for (i = 0; i < buffer.buffer_lines; i++)
		fputs(buffer.buf[i], stream);
	fflush(stream);

	for (i = 0; i < buffer.buffer_lines; i++)
		free(buffer.buf[i]);
	free(buffer.buf);

	exit(EXIT_SUCCESS);
}

int tiper_main(int argc, char **argv)
{
	int opt;
	char *file_string;	
	int read;

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

#if 0
	if (signal(SIGWINCH, resize_handler) == SIG_ERR) {
		printf("Error: Cannot handle signal SIGNINT");
		return 0;
	}
#endif

	init_console();
	move(0, 0);
	row = 0;
	col = 0;
	print_contents();

	while (1) {	
	// read char, write char
	// update cursor
	// input and interpret keys
		// save load exit find 
	// close resources	 	
		read = getch();
//		process_input(read);
		switch (read) {
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
			if (row > 1) {
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
		case KEY_BACKSPACE:
			if (col > 0) {
				col --;
				move(row, col);
			}
			break;
		case CTRL('x'):
			save_and_exit();
			break;			
		case KEY_F(1):
			break;
		case KEY_F(2):
			break;
		default:
			getyx(stdscr, row, col);
			mvaddch(row, col, read);
			buffer.buf[row][col] = read;
		}
			refresh();
	}
	return 0;
}



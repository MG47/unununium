/* LICENSE INFO: TBD*/

/* less - opposite of more */

/* */

#include "utilbox.h"

#define TOOL_NAME "less"


static FILE *stream;
static unsigned int maxrow, maxcol;
static unsigned int row, col;

#define BUFFER_LINES 300
#define BUFFER_COLUMNS 80

#define DEBUG_ON

/* TODO Change this */

/*TODO Test this*/
#if 0
#ifndef CTRL
#define CTRL(x) ((x) & 0x1f)
#endif
#endif

struct file_buffer {
	char **buf;
	unsigned int buffer_lines;
};

static struct file_buffer buffer;
static char *file_string;	


static void usage()
{
	printf("less:\n");
	printf("Usage: "
		"./less [OPTIONS] [FILE]\n"
		"OPTIONS:\n"
		"-h - Usage\n"
		"-v - Version\n"); 
}

static void print_version()
{
	printf("%s version:%d.%d\n",TOOL_NAME, LESS_VERSION, LESS_REVISION);
}

static void clear_screen()
{
	const char* CLEAR_SCREEN_ANSI =  "\e[2J\e[H";
	write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 7);
}

static void exit_less()
{
	erase();
	refresh();
	endwin();
	clear_screen();
	exit(EXIT_SUCCESS);	
}

static void signal_handler(int signo)
{
	if (signo == SIGINT) {
		erase();
		refresh();
		endwin();
		clear_screen();
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
	unsigned int i;
	int fd;

	stream = fopen(filename, "r+");
	if (!stream) { 
		perror("Error");
		return NULL;
	}

	/* TODO need to fix allocation */
	i = 0;
	buffer.buf = (char **)malloc(BUFFER_LINES * sizeof(char *));

	for (i = 0; i < BUFFER_LINES; i++) {
		buffer.buf[i] = malloc(sizeof(char) * BUFFER_COLUMNS);
	}

	i = 0;
	/* TODO Add maxcolumn check */
	while (fgets(buffer.buf[i], BUFFER_COLUMNS, stream) != NULL)
		i++;

	buffer.buffer_lines = i + 1;

	return stream;
}

static void init_console()
{
	int max_length;
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, max_length, maxcol);
	maxrow = max_length - 1;
//	scrollok(stdscr,TRUE);
	mvprintw(maxrow, 0, ":");
	refresh();
}

static void print_contents() 
{
	clear();
	unsigned int i;
	unsigned int offset;

	offset = row - maxrow;
	for (i = 0; ((i < maxrow) && ((offset + i) < buffer.buffer_lines)); i++) 
		mvprintw(i, 0, "%s", buffer.buf[offset + i]);
	if ((offset + i) < (buffer.buffer_lines - 1)) {
		mvprintw(i, 0, ": ");
	} else {
		attron(A_REVERSE);
		mvprintw(i, 0, "(END)");
	}
	attron(A_REVERSE);
#ifdef DEBUG_ON
	mvprintw(i, maxcol/2, "row %d   maxrow %d  MAX LINES %d ", row, maxrow, buffer.buffer_lines);
#endif
	move(i + 1, 0);
	attroff(A_REVERSE);
	refresh();
}

static void process_input(int read)
{
	switch (read) {
	case KEY_UP:
		if (row > maxrow)
			row--;
		print_contents();
		break;
	case KEY_DOWN:
		if (row < buffer.buffer_lines - 1)
			row++;
		print_contents();
		break;
	default:
		if (read == 'q')
			exit_less();
	}
}

int less_main(int argc, char **argv)
{
	int opt;
	int read;

	printf("\nless :\n");
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

/* TODO : Add sigwinch handler */
#if 0
	if (signal(SIGWINCH, resize_handler) == SIG_ERR) {
		printf("Error: Cannot handle signal SIGNINT");
		return 0;
	}
#endif

	init_console();
	row = maxrow;
	print_contents();

	while (1) {	
		read = getch();
		process_input(read);
		refresh();
	}
	return 0;
}



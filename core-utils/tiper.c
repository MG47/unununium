/* LICENSE INFO: TBD*/

/* tiper - text editor */

/* */

#include "utilbox.h"

#define TOOL_NAME "tiper"

static FILE *stream;
static unsigned int maxrow, maxcol;
static unsigned int row, col;

#define BUFFER_LINES 300
#define BUFFER_COLUMNS 80

/* TODO Change this */

/*TODO Test this*/
#ifndef CTRL
#define CTRL(x) ((x) & 0x1f)
#endif

struct file_buffer {
	char **buf;
	unsigned int buffer_lines;
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
	printf("%s version:%d.%d\n",TOOL_NAME, TIPER_VERSION, TIPER_REVISION);
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
	unsigned int i;
	int new_file = 0;
	int fd;

	stream = fopen(filename, "r+");
		printf("creating new file");
	if (!stream) { 
		fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
		stream = fdopen(fd, "r+");
		if (!stream) {
			perror("Error");
			return NULL;
		}
		new_file = 1;
		printf("created new file");
	}

	i = 0;
	buffer.buf = (char **)malloc(BUFFER_LINES * sizeof(char *));
	for (i = 0; i < BUFFER_LINES; i++) {
		buffer.buf[i] = malloc(sizeof(char) * BUFFER_COLUMNS);
	}

	if (new_file) {
		buffer.buffer_lines = 1;
		return stream;
	}

	i = 0;
	while (fgets(buffer.buf[i], BUFFER_COLUMNS, stream) != NULL)
		i++;

	buffer.buffer_lines = i;
	if (buffer.buffer_lines > 24) {
		printf("error: tiper can only handle files with less than %d lines currently\n", 24);
		exit(EXIT_FAILURE);
	}

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
	mvprintw(maxrow-3, maxcol/2, "Tiper Text Editor (%d.%d)\n", TIPER_VERSION, TIPER_REVISION);
	mvprintw(maxrow-2, maxcol/2, "Shortcuts");
	mvprintw(maxrow-1, maxcol/2, "Save and Exit: Ctrl+X");
	attroff(A_REVERSE);
	refresh();
	return 0;
}

static void print_contents() 
{
	unsigned int i;
	for (i = 0; i < buffer.buffer_lines; i++) 
		mvprintw(i, 0, "%s", buffer.buf[i]);
	refresh();
}

static void save_and_exit()
{
	erase();
	refresh();
	endwin();

// TODO change this 
	unsigned int i;

//	buffer.buf[0] = "hello";
	fseek(stream, 0, SEEK_SET);
	for (i = 0; i < buffer.buffer_lines; i++) {
		fputs(buffer.buf[i], stream);
	}
	fflush(stream);

	for (i = 0; i < BUFFER_LINES; i++)
		free(buffer.buf[i]);
	free(buffer.buf);

	exit(EXIT_SUCCESS);
}

static void insert_char_at(char *str, int index, char ch)
{
	char *src;
	int i;

	src = str;
	src[strlen(src) + 1] = '\0';
	for (i = strlen(src); i > index; i--)
		src[i] = src[i - 1];
	src[index] = ch;
}

static void insert_newline_at(int line_num, int col)
{
	unsigned int lines_to_end = buffer.buffer_lines - line_num;
	unsigned int chars_to_end = (strlen(buffer.buf[row]) - col) + 1;

	buffer.buf[buffer.buffer_lines] = malloc(sizeof(char) * BUFFER_COLUMNS);
	buffer.buffer_lines++;

	memmove(buffer.buf[line_num + 1], buffer.buf[line_num], lines_to_end); 
	strncpy(&buffer.buf[line_num + 1][0], &buffer.buf[line_num][col], chars_to_end);

	insert_char_at(buffer.buf[row], col, '\n');
	//todo check this
	insert_char_at(buffer.buf[row], col + 1, '\0');
}

static void remove_char(char *str, char remove) 
{
	char *src, *dst;
	for (src = dst = str; *src != '\0'; src++) {
		*dst = *src;
		if (*dst != remove) 
			dst++;
	}
	*dst = '\0';
}

static void process_input(int read)
{
	switch (read) {
	case KEY_LEFT:
		if (col > 0) {
			col--;
			move(row, col);
		}
		break;
	case KEY_RIGHT:
	// * TODO find length
		if (col < maxcol && col < (strlen(buffer.buf[row]) - 1)) {
			col++;
			move(row, col);
		}
		break;
	case KEY_UP:
		if (row > 0)
			row--;
		if (col > (strlen(buffer.buf[row]) - 1))
			col = (strlen(buffer.buf[row]) - 1);
		move(row, col);
		break;
	case KEY_DOWN:
		if (row <= buffer.buffer_lines)
			row++;
		// TODO Android shell has a different behaviour for key_down.
//		if (col > (strlen(buffer.buf[row]) - 1))
//			col = (strlen(buffer.buf[row]) - 1);
		col = 0;
		move(row, col);
		break;
	case KEY_BACKSPACE:
		if (col > 0) {
			remove_char(buffer.buf[row], buffer.buf[row][col -1]);
			mvdelch(row, (col-1));
		}
	break;
	case KEY_DC:
		//delete key
		if (col <= (strlen(buffer.buf[row]) - 1)) {
			remove_char(buffer.buf[row], buffer.buf[row][col]);
			mvdelch(row, col);
		}
		break;
	case KEY_ENTER:
	case 10:
		insert_newline_at(row, col);
		row++;
		col = 0;
		move(row, col);
//		print_contents();
		break;
	case CTRL('f'):
		break;			
	case CTRL('x'):
		save_and_exit();
		break;			
	case KEY_F(1):
		break;
	case KEY_F(2):
		break;
	default:
		buffer.buf[row][col++] = read;
		addch(read);
	}
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
	print_contents();
	move(0, 0);

	while (1) {	
		getyx(stdscr, row, col);
		read = getch();
		process_input(read);
		refresh();
	}
	return 0;
}



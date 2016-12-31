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

static void clear_screen()
{
	const char* CLEAR_SCREEN_ANSI =  "\e[2J\e[H";
	write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 7);
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
	int new_file = 0;
	int fd;

	stream = fopen(filename, "r+");
	if (!stream) { 
		fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
		stream = fdopen(fd, "r+");
		if (!stream) {
			perror("Error");
			return NULL;
		}
		new_file = 1;
	}

	/* TODO need to fix allocation */
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

	buffer.buffer_lines = i + 1;
	if (buffer.buffer_lines > 24) {
		printf("error: tiper can only handle files with less than %d lines currently\n", 24);
		exit(EXIT_FAILURE);
	}

	return stream;
}

static void init_console()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, maxrow, maxcol);
	attron(A_REVERSE);
	mvprintw(maxrow-2, maxcol/4, "Tiper Text Editor (%d.%d)\n", TIPER_VERSION, TIPER_REVISION);
	mvprintw(maxrow-1, maxcol/4, "Shortcuts");
	mvprintw(maxrow-2, maxcol/2, "Save and Exit: Ctrl+x");
	mvprintw(maxrow-1, maxcol/2, "Save: Ctrl+i");
	mvprintw(maxrow-2, 3*maxcol/4, "Exit: Ctrl+c");
	attroff(A_REVERSE);
	refresh();
}

static void print_line(int line_no) 
{
	mvprintw(line_no, 0, "%s", buffer.buf[line_no]);
	refresh();
}

static void print_contents() 
{
	clear();
	unsigned int i;
	for (i = 0; i < buffer.buffer_lines; i++) 
		mvprintw(i, 0, "%s", buffer.buf[i]);
	refresh();
}

static void save_to_file()
{
	//todo add newline at end
	unsigned int i;
	fseek(stream, 0, SEEK_SET);
	for (i = 0; i < buffer.buffer_lines; i++) {
		fputs(buffer.buf[i], stream);
	}
	fflush(stream);
}

static void save_and_exit()
{
	erase();
	refresh();
	endwin();

	unsigned int i;

	fseek(stream, 0, SEEK_SET);
	for (i = 0; i < buffer.buffer_lines; i++) {
		fputs(buffer.buf[i], stream);
	}
	fflush(stream);

	clear_screen();
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

static void insert_newline()
{
	unsigned int lines_to_end = (buffer.buffer_lines - 1) - row;
	unsigned int chars_to_end = (strlen(buffer.buf[row]) + 1) - col ;

	// TODO fix this
	if (!buffer.buf)
		buffer.buf[buffer.buffer_lines] = malloc(sizeof(char) * BUFFER_COLUMNS);

	unsigned int i;
	for (i = 0; i < lines_to_end; i++) {
		strcpy(buffer.buf[buffer.buffer_lines - i], buffer.buf[buffer.buffer_lines - i - 1]);
	}

	buffer.buffer_lines++;

	strncpy(&buffer.buf[row + 1][0], &buffer.buf[row][col], chars_to_end);
	buffer.buf[row][col] = '\n';
	buffer.buf[row][col + 1] = '\0';
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

static void remove_line()
{
	unsigned int lines_to_end = (buffer.buffer_lines - 1) - row;

	remove_char(buffer.buf[row - 1], '\n');
	strcat(buffer.buf[row - 1], buffer.buf[row]);

	unsigned int i;
	// TODO confirm this
	for (i = 0; i < lines_to_end; i++) {
		strcpy(buffer.buf[row + i], buffer.buf[row + i + 1]);
	}

	free(buffer.buf[buffer.buffer_lines - 1]);
	buffer.buffer_lines--;
}

static void process_input(int read)
{
	switch (read) {
	case KEY_LEFT:
		if (col > 0) {
			col--;
		} else {
			if (row > 0) {
				row--;
				col = strlen(buffer.buf[row]) - 1;
			} 
		}
		move(row, col);
		break;
	case KEY_RIGHT:
		if (col < maxcol && col < (strlen(buffer.buf[row]) - 1)) {
			col++;
		} else {
			if (row < (buffer.buffer_lines - 1)) {
				row++;
				col = 0;
			}
		}
		move(row, col);
		break;
	case KEY_UP:
		if (row > 0)
			row--;
		if (col > (strlen(buffer.buf[row]) - 1))
			col = (strlen(buffer.buf[row]) - 1);
		move(row, col);
		break;
	case KEY_DOWN:
		if (row < (buffer.buffer_lines - 1))
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
			mvdelch(row, col - 1);
			col--;
			move(row, col);
		} else {
			if (row > 0) {
				remove_line();
				print_contents();
				row--;
				col = (strlen(buffer.buf[row]));
				move(row, col);
			}
		}
		break;
	case KEY_DC:
		//delete key
		// TODO fix newline check
		if (col <= (strlen(buffer.buf[row]) - 2)) {
			remove_char(buffer.buf[row], buffer.buf[row][col]);
			delch();
		} else {
			if (row < (buffer.buffer_lines - 1)) {
				unsigned int oldrow = row;
				unsigned int oldcol = col;
				row++;
				col = 0;
				remove_line();
				print_contents();
				move(oldrow, oldcol);	
			}
		}
		break;
	case KEY_ENTER:
	case 10:
		insert_newline();
		print_contents();
		row++;
		col = 0;
		move(row, col);
		break;
	case KEY_HOME:
	case KEY_END:
		break;
	case CTRL('f'):
		break;			
	case CTRL('i'):
		save_to_file();
		break;			
	case CTRL('x'):
		save_and_exit();
		break;			
	case KEY_F(1):
		break;
	case KEY_F(2):
		break;
	default:
		if (col < maxcol) {
			insert_char_at(buffer.buf[row], col, read);
			print_line(row);
			col++;
			move(row, col);
		}
	}
}

int tiper_main(int argc, char **argv)
{
	int opt;
	char *file_string;	
	int read;

	printf("\nTiper Text Editor :\n");
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



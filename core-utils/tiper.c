/* LICENSE INFO: TBD*/

/* tiper - text editor */

/* */

#include "utilbox.h"

#define TOOL_NAME "tiper"

#define DEBUG_ON

static FILE *stream;

//TODO Make a struct for these
static unsigned int maxrow, maxcol;
static unsigned int row, col;
static unsigned int current_page;

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
/* Lock buffer */
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

	if (signo = SIGWINCH) {
		//TODO Handle resize gracefully
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

	/* TODO need to fix allocation for scrolling*/
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
	/* TODO : add critical maxcolumn check */
	while (fgets(buffer.buf[i], BUFFER_COLUMNS, stream) != NULL)
		i++;

	buffer.buffer_lines = i + 1;
#if 0
	if (buffer.buffer_lines > 24) {
		printf("error: tiper can only handle files with less than %d lines currently\n", 24);
		exit(EXIT_FAILURE);
	}
#endif
	return stream;
}

static void print_cursor_info()
{
	unsigned int page_offset = (current_page * maxrow);
	unsigned int line_offset = page_offset + row;
	mvprintw(maxrow + 1, 0, "Page %u: %u (%u), %u", current_page, line_offset, row, col);
	mvprintw(maxrow + 2, 0, "MAX LINES: %u", buffer.buffer_lines - 1);
	refresh();
}

static void print_menu()
{
	attron(A_REVERSE);
	mvprintw(maxrow + 1, maxcol / 4, "Tiper Text Editor (%d.%d)\n", TIPER_VERSION, TIPER_REVISION);
	mvprintw(maxrow + 2, maxcol / 4, "Shortcuts");
	mvprintw(maxrow + 1, maxcol / 2, "Save and Exit: Ctrl+x");
	mvprintw(maxrow + 2, maxcol / 2, "Save: Ctrl+i");
	mvprintw(maxrow + 1,  (maxcol * 3.0) / 4, "Exit: Ctrl+c");
	attroff(A_REVERSE);
}

static void init_console()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	int max_length;
	getmaxyx(stdscr, max_length, maxcol);
	maxrow = max_length - 3;
	print_menu();
	refresh();
}

static void print_contents(unsigned int page_no) 
{
	clear();
	unsigned int i;
	unsigned int page_offset = page_no * maxrow;
	unsigned int line_offset = page_offset % maxrow;

	for (i = line_offset; i < (line_offset + maxrow); i++) {
		//TODO bounds check 
		if (!((i + page_offset) < buffer.buffer_lines))
			break;
		mvprintw(i, 0, "%s", buffer.buf[i + page_offset]);
	}
	refresh();
	print_menu();
}

static void save_to_file()
{
	//TODO add newline at end
	//TODO mvprintw saved to file//
	unsigned int i;
	fseek(stream, 0, SEEK_SET);
	for (i = 0; i < buffer.buffer_lines; i++) {
		fputs(buffer.buf[i], stream);
	}
	fflush(stream);
}

static void save_and_exit()
{
	save_to_file();
	erase();
	refresh();
	endwin();
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

static void insert_newline(unsigned int line_offset)
{
	//TODO add fix for scrolling
	unsigned int lines_to_end = (buffer.buffer_lines - 1) - line_offset;
	unsigned int chars_to_end = (strlen(buffer.buf[line_offset]) + 1) - col ;

	// TODO fix this
	if (!buffer.buf)
		buffer.buf[buffer.buffer_lines] = malloc(sizeof(char) * BUFFER_COLUMNS);

	unsigned int i;
	for (i = 0; i < lines_to_end; i++) {
		strcpy(buffer.buf[buffer.buffer_lines - i], buffer.buf[buffer.buffer_lines - i - 1]);
	}

	buffer.buffer_lines++;

	strncpy(&buffer.buf[line_offset + 1][0], &buffer.buf[line_offset][col], chars_to_end);
	buffer.buf[line_offset][col] = '\n';
	buffer.buf[line_offset][col + 1] = '\0';
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

static void remove_char_at(char *src, int index)
{
	int i, len;
	len = strlen(src);
	for (i = index; i <= len; i++) {
		src[i] = src[i + 1];
	}
}

static void remove_newline(unsigned int line_offset)
{
	unsigned int i;
	unsigned int lines_to_end = (buffer.buffer_lines - 1) - line_offset;

	remove_char(buffer.buf[line_offset - 1], '\n');
	strcat(buffer.buf[line_offset - 1], buffer.buf[line_offset]);

	// TODO confirm this
	for (i = 0; i < lines_to_end; i++) {
		strcpy(buffer.buf[line_offset + i], buffer.buf[line_offset + i + 1]);
	}

	free(buffer.buf[buffer.buffer_lines - 1]);
	buffer.buffer_lines--;
}

static void process_input(int read)
{
	unsigned int page_offset = (current_page * maxrow);
	unsigned int line_offset = page_offset + row;

	switch (read) {
	case KEY_LEFT:
	//fix column bug
		if (col > 0) {
			col--;
		} else {
			if (row) {
				row--;
				line_offset = page_offset + row;
				col = strlen(buffer.buf[line_offset]) - 1;
			} else {
				if (current_page) {
					row = maxrow - 1;
					line_offset = page_offset + row;
					col = strlen(buffer.buf[line_offset]) - 1;
					current_page--;
				}
			}
			print_contents(current_page);
		}
		move(row, col);
		break;
	case KEY_RIGHT:
	//fix column bug
		if (col < maxcol && col < (strlen(buffer.buf[line_offset]) - 1)) {
			col++;
		} else {
			if (line_offset < (buffer.buffer_lines - 2)) {
				if (row < maxrow - 1) {
					row++;
				} else {
					row = 0;
					current_page++;
					print_contents(current_page);
				} 
				col = 0;
			}
		}
		move(row, col);
		break;
	case KEY_UP:
		if (row) {
			row--;
			line_offset = page_offset + row;
			if (col > (strlen(buffer.buf[line_offset]) - 1))
				col = (strlen(buffer.buf[line_offset]) - 1);
		} else {
			if (current_page) {
				row = maxrow - 1;
				line_offset = page_offset + row;
				if (col > (strlen(buffer.buf[line_offset]) - 1))
					col = (strlen(buffer.buf[line_offset]) - 1);
				current_page--;
				print_contents(current_page);
			}
		}
		move(row, col);
		break;
	case KEY_DOWN:
		if (line_offset < (buffer.buffer_lines - 2)) {
			if (row < maxrow - 1) {
				row++;
			} else {
				row = 0;
				current_page++;
				print_contents(current_page);
			} 
		}
/*Android shell has a different behaviour for key_down.*/
#ifdef ANDROID_SHELL
		col = 0;
#else
		if (col > (strlen(buffer.buf[row]) - 1))
			col = (strlen(buffer.buf[row]) - 1);
#endif
		move(row, col);
		break;
	case KEY_BACKSPACE:
		//TODO fix last line bug
		if (col > 0) {
			remove_char_at(buffer.buf[line_offset], (col - 1));
			mvdelch(row, col - 1);
			col--;
		} else {
			/* TODO fix max column limit */
			if ((strlen(buffer.buf[line_offset]) + (strlen(buffer.buf[line_offset - 1])))  >= maxcol)
				break;
			if (line_offset) {
				col = (strlen(buffer.buf[line_offset - 1])) - 1;
				remove_newline(line_offset);
				if (row > 0) {
					row--;
				} else {
					current_page--;
					row = maxrow - 1;
				}
				print_contents(current_page);
			}
		}
		move(row, col);
		break;
	case KEY_DC:
		//TODO fix last line bug
		if (col < (strlen(buffer.buf[line_offset]) - 1)) {
			remove_char_at(buffer.buf[line_offset], col);
			delch();
		} else {
			if (line_offset < buffer.buffer_lines - 2) {
				/* TODO fix max column limit */
				if ((strlen(buffer.buf[line_offset]) + (strlen(buffer.buf[line_offset + 1])))  >= maxcol)
					break;		
				remove_newline(line_offset + 1);
			}
		}
		print_contents(current_page);
		break;
	case KEY_ENTER:
	case 10:
		insert_newline(line_offset);
		if (row == (maxrow - 1)) {
			current_page++;
			row = 0;
		} else {
			row++;
		}
		print_contents(current_page);
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
			unsigned int page_offset = current_page * maxrow; 
			insert_char_at(buffer.buf[page_offset + row], col, read);
			mvprintw(row, 0, "%s", buffer.buf[page_offset + row]);
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
		/* TODO : create a file named 'untitled' instead of usage */
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

	if (signal(SIGWINCH, signal_handler) == SIG_ERR) {
		printf("Error: Cannot handle signal SIGNINT");
		return 0;
	}

	current_page = 0;
	init_console();
	move(row, col);
	print_contents(current_page);
	move(0, 0);

	while (1) {	
#ifdef DEBUG_ON	
		print_cursor_info();
#endif
		move(row, col);
		read = getch();
		process_input(read);
		refresh();

	}
	return 0;
}



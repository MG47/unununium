/* LICENSE INFO: TBD*/

/* tiper - text editor */

/* */

#include "utilbox.h"

#define VERSION 0
#define REVISION 1
#define TOOL_NAME "tiper"

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

static FILE *parse_file(char *filename)
{
	FILE *stream;
	/* TODO parse path and filename */
	stream = fopen(filename, "a+");
	if (!stream) {
		printf("error: %s\n", strerror(errno));
		return NULL;
	}

	return stream;
}

static int init_console()
{
	return 0;
}

int tiper_main(int argc, char **argv)
{
	int opt;
	char *file_string;
	FILE *stream;

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
	// open dir/parse/create file function, check permissions

	stream = parse_file(file_string);
	if (!stream)
		return 0;

	if (init_console())
		return 0;

	char *read;
	read = malloc(sizeof(char *) * 10);

	while (1) {
	// threaded:
	// open console window 
	// read char, write char
	// update cursor
	// input and interpret keys
		// save load exit find 
	// close resources	 	
		read[0] = fgetc(stdin);
		printf("read %s\n", read);
		fputs(read, stream);break;
	}
	return 0;
}



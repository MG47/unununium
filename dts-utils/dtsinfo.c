/* LICENSE INFO: TBD*/

/* dtsinfo - show information about device tree and associated firmware in the system */

/* */
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define SYSFS_CLASS_PATH "/sys/class/"
#define SYSFS_DEVICE_PATH "/sys/devices/"

#define OPT_HELP 1
#define OPT_CLASSES 2

void usage()
{
	printf("Help:\n");
	printf("dtsinfo:\n");
	printf("Prints information about device tree and associated firmware");
}

void print_classes()
{
	DIR *dir;
	struct dirent *dirp;

	printf("classes:\n");

	dir = opendir(SYSFS_CLASS_PATH);
	if (!dir) {
		printf("error: %s\n", strerror(errno));
		return;
	}

	while ((dirp = readdir(dir)) != NULL) {
		if ((strcmp(dirp->d_name, ".")) && strcmp((dirp->d_name), ".."))
			printf("%s\n", dirp->d_name);
	}

	closedir(dir);
}

int main(int argc, char **argv)
{
	char ch;
	int flags = 0;

	printf("Device Tree Information:\n");

	while ((ch = getopt(argc, argv, "ch")) != -1) {

		switch (ch) {
		case 'c':
			flags |= OPT_CLASSES;
			break;

		case 'h':
			flags |= OPT_HELP;
			break;
		}
	}

	if (flags & OPT_HELP)
		usage();

	if (!flags || (flags & OPT_CLASSES))
		print_classes();

	return 0;
}

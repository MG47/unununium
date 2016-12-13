/* LICENSE INFO: TBD*/

/* dtsinfo - show information about device tree and associated firmware in the system */

/* */
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SYSFS_CLASS_PATH "/sys/class/"
#define SYSFS_DEVICE_PATH "/sys/devices/"
#define SYSFS_DEVICETREE_PATH "/sys/firmware/devicetree/base"

#define OPT_HELP 1
#define OPT_CLASSES 2

void usage()
{
	printf("dtsinfo:\n");
	printf("Usage: "
            "./dtsinfo [-h -c]\n"
            "-h - Usage\n"
            "-c - Print classes\n"); 
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

void print_devicetree()
{
	DIR *dir;
	struct dirent *dirp;
	int i = 0;
	char *buf[FILENAME_MAX];

	printf("Device Tree:\n\n");

	dir = opendir(SYSFS_DEVICETREE_PATH);
	if (!dir) {
		printf("error: %s\n", strerror(errno));
		return;
	}

	while ((dirp = readdir(dir)) != NULL) {
		if ((strcmp(dirp->d_name, ".")) && (strcmp((dirp->d_name), ".."))) {
			buf[i] = malloc(FILENAME_MAX * sizeof(char *));
			sprintf(buf[i], "%s", dirp->d_name);
			printf("%s\n", buf[i]);
			i++;
		}
	}

	int count = i;
	for (i = 0; i < count; i++) {
		if (!(strcmp(buf[i], "model"))) {
//			printf("Found it");
				break;
		}
	}

	closedir(dir);
}

int main(int argc, char **argv)
{
    int opt;
	int flags = 0;

    while ((opt = getopt(argc, argv, "ch")) != -1) {
        switch (opt) {
		case 'c':
			flags |= OPT_CLASSES;
			break;

		case 'h':
			flags |= OPT_HELP;
			break;

		/*
			TODO 
			add gpio check flags
		*/
		}
	}

	if (flags & OPT_HELP)
		usage();

	if (flags & OPT_CLASSES)
		print_classes();

	if (!flags)
		print_devicetree();

	return 0;
}

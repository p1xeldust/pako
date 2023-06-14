#include <stdio.h>

void print_help(const char* version) {
	printf("[\e[33mPako!\e[39m] v%s\n\n",version);
	printf("Usage: pako <command> [flags]\n\nPackage manager functions:\n\n   -i	install a pako package.\n   -r	remove installed package\n   -l	list installed packages\n   -h	get some help like this help message\n\n");

}

/*
 * Written by Paul Goldstein, May 2023.
 */
 
#include <iostream>
#include <cstring>

#include "functions.h"

#define VERSION "0.1-dev_build"

int main(int argc, char* argv[]) {
	if(argc < 2) {
		printf("No arguments provided. Use -h to print help message\n");
		exit(1);
	}

	if(!strcmp(argv[1],"-h")) {
		print_help(VERSION);
	}
	if(!strcmp(argv[1],"-i")) {
		install(argc, argv);
	}
}


/* Written by Tim Kientzle, March 2009.
 * 
 * Released into the public domain.
 * 
 * Modified by Paul Goldstein, May 2023. 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <filesystem>

#include <sys/stat.h>

static int parseoct(const char *p, size_t n) {
	int i = 0;

	while (*p < '0' || *p > '7') {
		++p;
		--n;
	}
	while (*p >= '0' && *p <= '7' && n > 0) {
		i *= 8;
		i += *p - '0';
		++p;
		--n;
	}
	return i;
}

static int is_end_of_archive(const char *p) {
	int n;
	for (n = 511; n >= 0; --n)
		if (p[n] != '\0')
			return 0;
	return 1;
}

static void create_dir(char *pathname, int mode) {
	char *p;
	int r;

	if (pathname[strlen(pathname) - 1] == '/')
		pathname[strlen(pathname) - 1] = '\0';

	r = mkdir(pathname, mode);

	if (r != 0) {
		
		p = strrchr(pathname, '/');
		if (p != NULL) {
			*p = '\0';
			create_dir(pathname, 0755);
			*p = '/';
			r = mkdir(pathname, mode);
		}
	}
}

static FILE * create_file(char *pathname, int mode) {

	FILE *f;
	f = fopen(pathname, "w+");
	if (f == NULL) {
		char *p = strrchr(pathname, '/');
		if (p != NULL) {
			*p = '\0';
			create_dir(pathname, 0755);
			*p = '/';
			f = fopen(pathname, "w+");
		}
	}
	return (f);
}

static int verify_checksum(const char *p) {

	int n, u = 0;
	for (n = 0; n < 512; ++n) {
		if (n < 148 || n > 155)
			u += ((unsigned char *)p)[n];
		else
			u += 0x20;
	}
	return (u == parseoct(p + 148, 8));
}


static int untar(FILE *a, const char *path) {

	char buff[512];
	FILE *f = NULL;
	size_t bytes_read;
	int filesize;
	
	for (;;) {
		bytes_read = fread(buff, 1, 512, a);
		if (bytes_read < 512) {
			return 1;
		}
		if (is_end_of_archive(buff)) {
			return 0;
		}
		if (!verify_checksum(buff)) {
			return 1;
		}
		filesize = parseoct(buff + 124, 12);
		f = create_file(buff, parseoct(buff + 100, 8));
		while (filesize > 0) {
			bytes_read = fread(buff, 1, 512, a);
			if (bytes_read < 512)
				return 1;
			if (filesize < 512)
				bytes_read = filesize;
			if (f != NULL) {
				if (fwrite(buff, 1, bytes_read, f) != bytes_read) {
					fclose(f);
					f = NULL;
				}
			}
			filesize -= bytes_read;
		}
		if (f != NULL) {
			fclose(f);
			f = NULL;
		}
	}
}

int unpack(const char *argv) {

	FILE *a;
	a = fopen(argv, "r");
	
	if (a == NULL)
		exit(1);
	std::string defpath = std::filesystem::current_path();
	std::string filepath = argv;
	int filename_length = filepath.find_last_of("/");
		if (filename_length >= 0)
			filepath = filepath.substr(0, filename_length);
		chdir(filepath.c_str());
	if(!untar(a, argv)) {
		return 1;
	}
	fclose(a);
	chdir(defpath.c_str());
	return 0;
}


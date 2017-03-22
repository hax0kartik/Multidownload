#pragma once

#include <stdio.h>  // Basic functions
#include <stdlib.h> // malloc and free
#include <string.h> // String manipulation
#include <dirent.h> // For opendir() and readdir()
#include <unistd.h> // rmdir()
#include <3ds.h>    // Main 3ds lib (libctru)
#include <string>

/*
Notes

According to Wikipedia, the max FAT32 path length is 255 UTF-16 characters, so 255 * 2 = 510 (because the 16 in UTF-16 means 16 bits = 2 bytes)

all malloc / realloc statements create the length needed +1, because there needs to be room for the end character

Colours { red = error related, blue = button related, green = path related, white = other }
*/

extern char current_path[511];
extern char current_file[511];
extern int selected;
extern char **file_arr;
extern char **disp_file_arr;
extern char *isfile_arr;
extern std::string ab;
extern int size_of_file_array;
extern int scroll;
extern PrintConsole topScreen;
extern PrintConsole instructionscreen;
extern PrintConsole debugscreen;
extern int MAX_DIR_NAME_SIZE;
extern int MAX_FILES_ON_SCREEN;
extern int MAX_PATH_SIZE;
int xplorer(void);
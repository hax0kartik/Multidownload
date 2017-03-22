#include "dir.h"
#include "sort.h"
#include <errno.h>
// Gets the upper directory of the current_path
void get_ud(void) {
    consoleSelect(&debugscreen);

    char path_to_iterate[MAX_PATH_SIZE];
    strcpy(path_to_iterate, current_path);

    char looking_for[] = "/";
    char *token;
    char dummy1[MAX_PATH_SIZE];
    char dummy2[MAX_PATH_SIZE];

    // Sometimes there is random stuff in here for some reason, so clean both before starting
    strcpy(dummy1, "");
    strcpy(dummy2, "");

    token = strtok(path_to_iterate, looking_for);

    while( token != NULL ) {
        // add token (for example "sdmc:" into dummy1
        strcat(dummy1, token);
        strcat(dummy1, "/");

        if(strstr(dummy1, current_path) != NULL) {
            // dummy2 happens after this, so will have 1 less token
            strcpy(current_path, dummy2);
            break;
        }

        strcat(dummy2, token);
        strcat(dummy2, "/");

        // get the next token288342890277634069
        token = strtok(NULL, looking_for);
    }
    printf("\x1b[32mnew path: %s\x1b[0m\n", current_path);
}

// Fills file array with all files in a given directory
void get_all_in_dir(char dir_to_show[]) {
    // Bottom screen is for debug info. Anything printed in this func will print to bottom screen
    consoleSelect(&debugscreen);

    // 2 of each for 2 iterations
    DIR *d;
    DIR *nd;
    d = opendir(dir_to_show);
    nd = opendir(dir_to_show);

    if (d) {
        // Get rid of strings in file name array
		if(current_file[0]!=0)
        for (int i = 0; i < size_of_file_array; i++) { free(file_arr[i]); }

        struct dirent *dir;
        struct dirent *ndir;
        int count = 0;
        selected  = 0;
        scroll    = 0;

        // While readdir returns something other than NULL. The variable dir will change each loop
        while ((dir = readdir(d)) != NULL) {
            // Count how many files there are
            count++;
        }

        size_of_file_array = count;
        count = 0;

        // Create a 2D string array using malloc
        // Create an array of pointers the size of the amount of files in the chosen directory
        file_arr =(char**)realloc(file_arr, (size_of_file_array+1) * sizeof(char*));
        // Set each pointer inside the array to point to a char
        for (int i = 0; i < size_of_file_array; i++) { file_arr[i] =(char*) malloc((MAX_DIR_NAME_SIZE+1) * sizeof(char)); }
        isfile_arr =(char*) realloc(isfile_arr, (size_of_file_array+1) * sizeof(bool));

        // Iterate over dir again, this time adding filenames to created 2D array
        while ((ndir = readdir(nd)) != NULL) {
            // Get d_name from the dir struct and copy into array
            strcpy(file_arr[count], ndir->d_name);
            // If d_type is a file
            isfile_arr[count] = (ndir->d_type == 8);
            count++;
        }
    }
    closedir(d);
    closedir(nd);
    bubble_sort_files();
}
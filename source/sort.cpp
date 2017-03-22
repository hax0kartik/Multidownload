#include "sort.h"

// Sort given array (yay for bubble sort)
void bubble_sort_files(void) {
    bool swapped = true;
    char temp[MAX_DIR_NAME_SIZE];
    int inttemp;
    int pass;
    int pass2;

    for (pass=size_of_file_array-1; pass>0; pass--) {
        for (pass2=0; pass2<pass; pass2++) {
            if (strcasecmp(file_arr[pass2], file_arr[pass2+1]) > 0) {
                swapped = true;

                // Swap values in file name array
                strcpy(temp, file_arr[pass2]);
                strcpy(file_arr[pass2], file_arr[pass2+1]);
                strcpy(file_arr[pass2+1], temp);

                // Swap values in isfile array
                inttemp = isfile_arr[pass2];
                isfile_arr[pass2] = isfile_arr[pass2+1];
                isfile_arr[pass2+1] = inttemp;

            }
        }
        if (!swapped) { break; }
    }
}

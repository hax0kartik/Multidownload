// Print all 'things' in the file name array
#include "common.h"

void print_all_values_in_filear(void) {

    int max_files_to_print;

    if (size_of_file_array < MAX_FILES_ON_SCREEN) { max_files_to_print = size_of_file_array; }
    else { max_files_to_print = MAX_FILES_ON_SCREEN; }
    consoleSelect(&topScreen);
    printf("\x1b[0;0H");

        int i;
    if (max_files_to_print > 0) {
        if (scroll > 0) { printf("/\\\n"); } // Print up arrow showing you can scroll up
   else { printf("    \n"); } 
        // Print directories first
        for (i=0; i<max_files_to_print; i++) {
            // If it is a dir
            if (!isfile_arr[i+scroll]) {
                // print as white text on black background
                if (i == selected) { printf("\x1b[31m\x1b[47;30m%-47.47s\x1b[0m\n", file_arr[i+scroll]); }
                // Else, just print it without highlight
                else { printf("\x1b[31;1m%-47.47s\x1b[0m\n", file_arr[i+scroll]); }
				
            }

            else {
                if (i == selected) { printf("\x1b[32m\x1b[47;30m%-47.47s\x1b[0m\n", file_arr[i+scroll]); }
                else { printf("\x1b[32;1m%-47.47s\x1b[0m\n", file_arr[i+scroll]); }
				
            }
        }
	
      if ( (size_of_file_array > MAX_FILES_ON_SCREEN) && (selected+scroll != size_of_file_array-1) ) { printf("\n\n\\/ "); }else printf("\n\n    ");
    }
else { printf("\n\n\t\t\x1b[47;30m- Folder is empty -\x1b[0m"); }
}
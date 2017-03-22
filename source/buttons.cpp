#include "buttons.h"
#include "dir.h"
#include "draw.h"

void up(void) {
    if (size_of_file_array == 0){ ; }

    // if selected is 0 and there is no scroll, skip to the bottom
    else if (selected+scroll == 0) {
        // If there is a need for scroll
        if (size_of_file_array > MAX_FILES_ON_SCREEN) {
            // arrays are 0 indexed, so max files -1 = highest index of array shown
            selected = MAX_FILES_ON_SCREEN-1;
            // scroll will be max size it can be
            scroll = size_of_file_array-MAX_FILES_ON_SCREEN;
        }
        else { selected = size_of_file_array-1; }
    }

    else if (scroll > 0) { scroll--; }

    else { selected--; }
}

void down(void) {
    if (size_of_file_array == 0){ ; }

    // If selected+scroll are at the largest index of the file array (arrays are 0 indexed!)
    else if (selected+scroll == size_of_file_array-1) {
        selected = 0;
        scroll = 0;
    }

    // If selected = highest index and selected+scroll < the max index for the array
    else if ((selected == MAX_FILES_ON_SCREEN-1) && (selected+scroll < size_of_file_array-1)) { scroll++; }

    else { selected++; }
}

void left(void) {
    if (size_of_file_array == 0){ ; }
    // If selected+scroll are at the largest index of the file array (arrays are 0 indexed!)
    else {
        selected = 0;
        scroll = 0;
    }
}

void right(void){
    if (size_of_file_array == 0){ ; }
    else {
        // If there is a need for scroll
        if (size_of_file_array > MAX_FILES_ON_SCREEN) {
            // arrays are 0 indexed, so max files -1 = highest index of array shown
            selected = MAX_FILES_ON_SCREEN-1;
            // scroll will be max size it can be
            scroll = size_of_file_array-MAX_FILES_ON_SCREEN;
        }
        else { selected = size_of_file_array-1; }
    }
}

void a_pressed(void) {
    consoleSelect(&debugscreen);

    if (size_of_file_array == 0){ ; }

    else {
        // If it is actually a directory
        if (!isfile_arr[selected+scroll]) {
            printf("path is dir\n");
            strcat(current_path, file_arr[selected+scroll]);
            strcat(current_path, "/");
            printf("\x1b[32mnew path: %s\x1b[0m\n", current_path);
            get_all_in_dir(current_path);
			consoleSelect(&topScreen);
			printf("\x1b[2J");
            print_all_values_in_filear();
        }   
	}

}

void b_pressed(void) {
    consoleSelect(&debugscreen);
    
    if (!strcmp(current_path, "sdmc:/")) { printf("\x1b[32mcurrently in sdmc:/\x1b[0m\n"); }
    		
    // move up a directory
    else { get_ud(); }
	consoleSelect(&topScreen);
			printf("\x1b[2J");
}

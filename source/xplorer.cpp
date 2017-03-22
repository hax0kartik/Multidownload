#include "common.h"
#include "dir.h"
#include "draw.h"
#include "buttons.h"

// contains current path
char current_path[511];
char current_file[511];
// selected file
int selected = 0;
// A pointer to an array which is full of pointers which will point to strings (only way to create a dynamic/resizable array)
char **file_arr;
// Same as file_arr, will be used "alongside" file_arr to check whether something is a file or directory
char *isfile_arr;
// Size of file name array
int size_of_file_array;
// Used to offset what is printed from the file array, to allow "scrolling"
int scroll = 0;
// One PrintConsole for each screen
PrintConsole topScreen, instructionscreen, debugscreen;
// Max len of file/directory name
int MAX_DIR_NAME_SIZE = 261;
// What it says on the tin
int MAX_FILES_ON_SCREEN = 26;
// ^
int MAX_PATH_SIZE = 511;

int xplorer(void) {
	
    // Initialize console for both screens using the two different PrintConsole
    consoleInit(GFX_BOTTOM, &debugscreen);
    consoleInit(GFX_BOTTOM, &instructionscreen);
	consoleInit(GFX_TOP, &topScreen);
    consoleSetWindow(&instructionscreen, 0, 0, 40, 8);
    consoleSetWindow(&debugscreen, 0, 9, 40, 21);
    consoleSelect(&instructionscreen);
    printf("A - Change Directory/Select file\nB - go up a directory\nDPAD/Circle pad - up and down\nSELECT - use the current location as download location.");
    printf("\n----------------------------------------");

    consoleSelect(&debugscreen);
    printf("Started xplorer...\n");

    // Initial allocation for these 2 arrays
    file_arr = (char**)malloc(1 * sizeof(char*));
	printf("Ok\n");
    isfile_arr = (char*) malloc(1 * sizeof(bool));
	printf("OK 1\n");
    // For when it is first realloc(ed)
    size_of_file_array = 1;

    strcpy(current_path, "sdmc:/");
    get_all_in_dir(current_path); 
	printf("Ok 3");
	// Fill file name array with file names
    print_all_values_in_filear();      // Print all in /
    u64 min=0;
    // Main loop
	 while (aptMainLoop()) {

        hidScanInput();

        u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
        if(kDown)
			min=osGetTime();
        if ((kDown & KEY_UP)|((kHeld & KEY_UP)&&(osGetTime() - min>500))) {
            up();
            print_all_values_in_filear();
        }

        else if ((kDown & KEY_DOWN)||((kHeld & KEY_DOWN)&&(osGetTime() - min>650))) {
            down();
            print_all_values_in_filear();
        }

        else if (kDown & KEY_LEFT) {
            left();
            print_all_values_in_filear();
        }

        else if (kDown & KEY_RIGHT) {
            right();
            print_all_values_in_filear();
        }

        else if (kDown & KEY_A) {
            a_pressed();
			if(current_file[0]!=0)
			break;	
        }

        else if (kDown & KEY_B) {
            b_pressed();
            get_all_in_dir(current_path);
            print_all_values_in_filear();
        }
		
		else if(kDown & KEY_START)
			break;
		else if(kDown & KEY_SELECT)
			break;
       gfxSwapBuffers();
    }

    // Clear up allocated memory
    for (int i=0; i<size_of_file_array; i++) { free(file_arr[i]); }
    free(isfile_arr);
    return 0;
}

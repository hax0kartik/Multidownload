#include "util.hpp"
#include <iostream>
#include <string>
#include <3ds.h>
using namespace std;
char mybuf[1024];
string tl(int buttons,string texgen,const char * right)
{
	SwkbdState swkbd;
	SwkbdButton button = SWKBD_BUTTON_NONE;
	swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, buttons, -1);
	swkbdSetHintText(&swkbd, texgen.c_str());
	if(buttons==2){
	swkbdSetButton (&swkbd, SWKBD_BUTTON_LEFT,"Cancel", true);
	swkbdSetButton (&swkbd, SWKBD_BUTTON_RIGHT,right, true);
	}
	else
	swkbdSetButton (&swkbd, SWKBD_BUTTON_LEFT,right, true);
	swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
	button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));
	return mybuf;
}
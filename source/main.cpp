#include <3ds.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "gfx.hpp"
#include "fs.hpp"
#include "download.hpp"
#include "util.hpp"
#include "common.h"
#include "qrdec.h"
u32 __stacksize__ =0x40000;

using namespace std;
touchPosition touch;
PrintConsole top,bottom;
bool touchInBox(touchPosition touch, int x, int y, int w, int h)
{
	int tx=touch.px;
	int ty=touch.py;
	u32 kDown = hidKeysDown();
	if (kDown & KEY_TOUCH && tx > x && tx < x+w && ty > y && ty < y+h) {
		return true;
	} else {
		return false;
	}
}

int main()
{
	gfxInitDefault();
	menu();
	consoleSelect(&top);
	hidInit();
	Result ret;
	httpcInit(0x1000);
	while (aptMainLoop()) {

		hidScanInput();
		hidTouchRead(&touch);

        u32 kDown=hidKeysDown();
		if (kDown & KEY_START) {
			break;
		}
	    if(touchInBox(touch,5,25,300,25))
		{
			string b=tl(2,"Enter URL","Download");
			consoleSelect(&top);
			cout<<"\x1b[2J";
			ret=http_download(b,ab);
			if(ret==0)
			cout<<"\x1b[33;1mDownload :\x1b[32;1m Success\x1b[37;1m"<<endl;
			else
			cout<<"\x1b[33;1mDownload :\x1b[31;1m Fail\x1b[37;1m"<<endl;	
			cout<<"This app will automatically return to this app's menu in 5 seconds"<<endl;
			svcSleepThread(5000000000);
			ab.clear();
			menu();	
		}
		else if(touchInBox(touch,5,60,300,25))
		{
			consoleSelect(&top);
			cout<<"Xplorer";
			xplorer();
			cout<<"\x1b[2J";
			std::string path (current_path);
			fs f;
			f.writefile("sdmc:/multi.cfg", path);
			cout<<"Current path:"<<current_path;
			menu();
		}
		else if(touchInBox(touch,5,100,300,30))
		{
			consoleSelect(&bottom);
			cout<<"\x1b[2J";
			string ac=qr_dec();
			cout<<"\x1b[2J";
			consoleInit(GFX_TOP,&top);
			consoleSelect(&top);
			draw_border(GFX_TOP,0,255,255);
			consoleSetWindow(&top, 1, 1, 47, 28);
			ret=http_download(ac,ab);
			if(ret==0)
			cout<<"\x1b[33;1mDownload :\x1b[32;1m Success\x1b[37;1m"<<endl;
			else
			cout<<"\x1b[33;1mDownload :\x1b[31;1m Fail\x1b[37;1m"<<endl;	
			cout<<"This app will automatically return to this app's menu in 5 seconds"<<endl;
			svcSleepThread(5000000000);
			ab.clear();
			menu();
		}
		gfxFlushBuffers();
		gspWaitForVBlank();
		gfxSwapBuffers();
	}
httpcExit();	
hidExit();
gfxExit();
return 0;
}
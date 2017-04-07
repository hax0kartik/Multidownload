#include "gfx.hpp"
#include "fs.hpp"
#include <iostream>
#include <iomanip>
#include <3ds.h>
#include <cstdio>
using namespace std;
string ab;
void menu()
{
	extern PrintConsole top,bottom;
	consoleInit(GFX_BOTTOM, &bottom);
    consoleInit(GFX_TOP, &top);
	consoleSelect(&top);
	draw_border(GFX_TOP,0,255,255);
	draw_border(GFX_BOTTOM,255,255,0);
	consoleSetWindow(&top, 1, 1, 47, 28);
	consoleSetWindow(&bottom, 1, 1, 38, 28);
	cout<<"\x1b[14;12H\x1b[1;37mWelcome to Multidownload++"<<endl;
	cout<<"\x1b[15;17HMade by:-Kartik"<<endl;
	ab=rf("sdmc:/multi.cfg");
	if(ab=="error")ab="Not set";
	cout<<"\x1b[16;11HDownload Location:- "<< ab <<endl;
	consoleSelect(&bottom);
	string Enterurl="Enter URL";
	string border_url (Enterurl.size()+4,'-');
	cout<<"\x1b[1;12H"<<border_url<<endl;
	cout<<"\x1b[2;12H| "<<Enterurl<<" |"<<endl;
	cout<<"\x1b[3;12H"<<border_url<<endl<<endl<<endl;
	string changelocation="Change Download Location";
	string border_location (changelocation.size()+4,'-');
	cout<<"\x1b[7;5H"<<border_location<<endl;
	cout<<"\x1b[8;5H| "<<changelocation<<" |"<<endl;
	cout<<"\x1b[9;5H"<<border_location<<endl<<endl<<endl;
	string qr_code="Scan QR code";
	string border_qr_code (qr_code.size()+4,'-');
	cout<<"\x1b[13;11H"<<border_qr_code<<endl;
	cout<<"\x1b[14;11H| "<<qr_code<<" |"<<endl;
	cout<<"\x1b[15;11H"<<border_qr_code<<endl<<endl<<endl;
}
void drawpixel(u8* screen, u32 x,u32 y,u8 r,u8 g, u8 b)
{
	u32 color = RGB8_to_565(r, g, b);
	int idx = (x * 240) + (239 - y);
	((u16*)screen)[idx] = (u16)color;
}
void drawline(u8 *screen,u32 x,u32 y,u8 r,u8 g, u8 b,int width,int j)
{  
	if(j==1)
    for(int i=x;i<=width;i++)
	drawpixel(screen,i,y,r,g,b);
    else
	for(int i=y;i<=width;i++)
	drawpixel(screen,x,i,r,g,b);	
}
void draw_border(gfxScreen_t screen,u8 r ,u8 g,u8 b)
{   
	int height;
    u8 *fb;
    if(screen==GFX_TOP){
	fb=gfxGetFramebuffer(GFX_TOP ,GFX_LEFT,NULL, NULL);		
	height = 400;}
	else {
	fb=gfxGetFramebuffer(GFX_BOTTOM,GFX_LEFT,NULL, NULL);			
	height = 320;}
    drawline(fb,0,0,r,g,b,height,1);
	drawline(fb,0,1,r,g,b,height,1);
	drawline(fb,0,238,r,g,b,height,1);
	drawline(fb,0,239,r,g,b,height,1);
	drawline(fb,0,0,r,g,b,240,0);
	drawline(fb,1,0,r,g,b,240,0);
	drawline(fb,height-2,0,r,g,b,240,0);
	drawline(fb,height-1,0,r,g,b,240,0);
}


void pBar::update(double newProgress) {
	currentProgress += newProgress;
	amountOfFiller = (int)((currentProgress / neededProgress)*(double)pBarLength);
	}
void pBar::length_set(int size){
	neededProgress = size;
}
void pBar::print() {
	currUpdateVal %= pBarUpdater.length();
	printf("\x1b[14;0H\x1b[37;1mDownloaded %.2f MBs out of %.2f MBs.",(currentProgress/1048576),(neededProgress/1048576));
	cout << "\x1b[15;0H" 
	<< firstPartOfpBar; //Print out first part of pBar
	for (int a = 0; a < amountOfFiller; a++) { //Print out current progress
		cout << pBarFiller;
	}
	cout << pBarUpdater[currUpdateVal];
	for (int b = 0; b < pBarLength - amountOfFiller; b++) { //Print out spaces
		cout << " ";
	}
	cout << lastPartOfpBar //Print out last part of progress bar
	<< "\x1b[16;16H(" << (int)(100*(currentProgress/neededProgress)) << "%)" //This just prints out the percent
	<< flush;
	currUpdateVal += 1;
	}


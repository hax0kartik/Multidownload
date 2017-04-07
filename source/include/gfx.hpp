#include <3ds.h>
#include <string>
#ifndef gfx_hpp
#define gfx_hpp
void drawpixel(u8* screen, u32 x,u32 y,u8 r,u8 g, u8 b);
void drawline(u8 *screen,u32 x,u32 y,u8 r,u8 g, u8 b,int width,int j);
void draw_border(gfxScreen_t screen,u8 r ,u8 g,u8 b);
void menu();
class pBar {
public:
	void update(double newProgress);
	void print(); 
	void length_set(int size);
	std::string firstPartOfpBar = "[", 
	lastPartOfpBar = "]",
	pBarFiller = "=",
	pBarUpdater = ">";
	double neededProgress = 0;
private:
	int amountOfFiller,
	pBarLength = 35, //I would recommend NOT changing this
	currUpdateVal = 0; //Do not change
	double currentProgress = 0; //Do not change
};
#endif
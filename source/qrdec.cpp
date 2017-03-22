#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <3ds.h>
#include <sys/dirent.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <math.h>
#include <stdbool.h>
#include "gfx.hpp"
extern "C"{
#include "quirc.h"
}
#define WIDTH 400
#define HEIGHT 240
#define SCREEN_SIZE WIDTH * HEIGHT * 2
#define BUF_SIZE SCREEN_SIZE * 2
#define WAIT_TIMEOUT 300000000ULL
char qurl[1024];

void writePictureToIntensityMap(void *fb, void *img, u16 width, u16 height){
	u8 *fb_8 = (u8*) fb;
	u16 *img_16 = (u16*) img;
	for(u32 i = 0; i < width * height; i++){
		u16 data = img_16[i];
		uint8_t b = ((data >> 11) & 0x1F) << 3;
		uint8_t g = ((data >> 5) & 0x3F) << 2;
		uint8_t r = (data & 0x1F) << 3;
		fb_8[i] = (r + g + b) / 3;
	}
}

void bhm_line(void *fb,int x1,int y1,int x2,int y2,u32 c)
{
	int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;
	dx=x2-x1;
	dy=y2-y1;
	dx1=fabs(dx);
	dy1=fabs(dy);
	px=2*dy1-dx1;
	py=2*dx1-dy1;
	if(dy1<=dx1){
		if(dx>=0){
			x=x1;
			y=y1;
			xe=x2;
		} else {
			x=x2;
			y=y2;
			xe=x1;
		}
		drawpixel((u8*)fb,x,y,255,255,255);
		for(i=0;x<xe;i++){
			x=x+1;
			if(px<0){
				px=px+2*dy1;
			} else {
				if((dx<0 && dy<0) || (dx>0 && dy>0)){
					y=y+1;
				} else {
					y=y-1;
				}
				px=px+2*(dy1-dx1);
			}
			drawpixel((u8*)fb,x,y,255,255,255);
		}
	} else {
		if(dy>=0){
			x=x1;
			y=y1;
			ye=y2;
		} else {
			x=x2;
			y=y2;
			ye=y1;
		}
		drawpixel((u8*)fb,x,y,255,255,255);
		for(i=0;y<ye;i++){
			y=y+1;
			if(py<=0){
				py=py+2*dx1;
			} else {
				if((dx<0 && dy<0) || (dx>0 && dy>0)){
					x=x+1;
				} else {
					x=x-1;
				}
				py=py+2*(dx1-dy1);
			}
			drawpixel((u8*)fb,x,y,255,255,255);
		}
	}
}


void cleanup() {
	camExit();
}

void writePictureToFramebufferRGB565(void *fb, void *img, u16 x, u16 y, u16 width, u16 height) {
	u8 *fb_8 = (u8*) fb;
	u16 *img_16 = (u16*) img;
	int i, j, draw_x, draw_y;
	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
		    draw_y = y + height - j;
			draw_x = x + i;
			u16 data = img_16[j * width + i];
			uint8_t b = ((data >> 11) & 0x1F) << 3;
			uint8_t g = ((data >> 5) & 0x3F) << 2;
            uint8_t r = (data & 0x1F) << 3;	
		   drawpixel(fb_8,i,j,r,g,b);	
		}
	}
}

// TODO: Figure out how to use CAMU_GetStereoCameraCalibrationData
void takePicture3D(u16 *buf){
	u32 bufSize;
	Handle camReceiveEvent = 0;

	CAMU_GetMaxBytes(&bufSize, WIDTH, HEIGHT);
	CAMU_SetTransferBytes(PORT_CAM1, bufSize, WIDTH, HEIGHT);
	
	CAMU_ClearBuffer(PORT_CAM1);
	CAMU_StartCapture(PORT_CAM1);
	CAMU_SetReceiving(&camReceiveEvent, (u8*)buf, PORT_CAM1, WIDTH * HEIGHT * 2, (s16) bufSize);
	svcWaitSynchronization(camReceiveEvent, WAIT_TIMEOUT);
	
	svcCloseHandle(camReceiveEvent);
	//CAMU_Activate(SELECT_NONE);
}

char * qr_dec() 
{
	// Initializations
	// Save current stack frame for easy exit
	u32 kDown;
	u32 kHeld;
	printf("Initializing camera\n");
	gfxSetScreenFormat(GFX_TOP,GSP_RGB565_OES);
	// Enable double buffering to remove screen tearing
    gfxFlushBuffers();
	gfxSwapBuffers();
	printf("camInit: 0x%08X\n", (unsigned int) camInit());
    CAMU_Activate(SELECT_OUT1);
	printf("CAMU_SetSize: 0x%08X\n", (unsigned int) CAMU_SetSize(SELECT_OUT1, SIZE_CTR_TOP_LCD, CONTEXT_A));
	printf("CAMU_SetOutputFormat: 0x%08X\n", (unsigned int) CAMU_SetOutputFormat(SELECT_OUT1, OUTPUT_RGB_565, CONTEXT_A));
    CAMU_SetFrameRate(SELECT_OUT1, FRAME_RATE_30);
	printf("CAMU_SetNoiseFilter: 0x%08X\n", (unsigned int) CAMU_SetNoiseFilter(SELECT_OUT1, true));
	printf("CAMU_SetAutoExposure: 0x%08X\n", (unsigned int) CAMU_SetAutoExposure(SELECT_OUT1, true));
	printf("CAMU_SetAutoWhiteBalance: 0x%08X\n", (unsigned int) CAMU_SetAutoWhiteBalance(SELECT_OUT1, true));
	//printf("CAMU_SetEffect: 0x%08X\n", (unsigned int) CAMU_SetEffect(SELECT_OUT1_OUT2, EFFECT_MONO, CONTEXT_A));

	printf("CAMU_SetTrimming: 0x%08X\n", (unsigned int) CAMU_SetTrimming(PORT_CAM1, false));
	//printf("CAMU_SetTrimmingParamsCenter: 0x%08X\n", (unsigned int) CAMU_SetTrimmingParamsCenter(PORT_CAM1, 512, 240, 512, 384));
    struct quirc *qr;
	qr = quirc_new();
	if (!qr){
		printf("Failed to allocate memory");
		return 0;
	}

	if (quirc_resize(qr, WIDTH, HEIGHT) < 0){
		printf("Failed to allocate video memory");
		return 0;
	}
     
	u16 *buf =(u16*) malloc(BUF_SIZE);

	gfxFlushBuffers();
	gfxSwapBuffers();
    struct quirc_data data;
	// Main loop
	while (aptMainLoop()) {
		// Read which buttons are currently pressed or not
		hidScanInput();
		kDown = hidKeysDown();
		// If START button is pressed, break loop and quit
		if (kDown & KEY_START) {
			break;
		}
		takePicture3D(buf);
		writePictureToFramebufferRGB565(gfxGetFramebuffer(GFX_TOP ,GFX_LEFT,NULL, NULL), buf, 0, 0, WIDTH, HEIGHT);
		int w=WIDTH, h=HEIGHT;
		u8 *image = (u8*)quirc_begin(qr, &w, &h);
		writePictureToIntensityMap(image, buf, WIDTH, HEIGHT);
		quirc_end(qr);

		int num_codes = quirc_count(qr);
		
		for (int i = 0; i < num_codes; i++){
			struct quirc_code code;
			
			quirc_decode_error_t err;

			quirc_extract(qr, i, &code);
		                
			err = quirc_decode(&code, &data);
			if (!err){
				CAMU_Activate(SELECT_NONE);
				printf("\x1b[32;1mDECODE SUCCESSFUL %s\n\x1b[37;1m",data.payload);
				printf("\x1b[32;1mSuccesful 3\n\x1b[37;1m");
				free(buf);
				printf("\x1b[32;1mSuccesful 4\n\x1b[37;1m");
				cleanup();
				quirc_destroy(qr);
				printf("\x1b[32;1mSuccesful 5\n\x1b[37;1m");
				printf("\x1b[32;1mSuccesful 1\x1b[37;1m");
				strcpy(qurl,(char*)data.payload);
				return qurl;
            }                                                              
		gspWaitForVBlank();
		gfxSwapBuffers();
	 }
   }
 return "ERROR";
}
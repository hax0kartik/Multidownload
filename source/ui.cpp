#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include "ui.h"

uiFuncs_s ui;

void uiSetScreenTop(func_t callback_func, void *args)
{
    ui.topfunc = callback_func;
    ui.topargs = args;
}

void uiSetScreenBottom(func_t callback_func, void *args)
{
    ui.botfunc = callback_func;
    ui.botargs = args;
}

void uitop_func()
{
    ui.topfunc(ui.topargs);
}

void uibot_func()
{
    ui.botfunc(ui.botargs);
}

void uiThread(void *arg)
{
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    if(ui.debug) { consoleInit(GFX_BOTTOM, NULL); printf("Console Attached"); consoleDebugInit(debugDevice_CONSOLE);}
    else {consoleDebugInit(debugDevice_SVC); fprintf(stderr, "DEBUG init");}
    while(aptMainLoop() && ui.run)
    {
        svcWaitSynchronization(ui.event, U64_MAX);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        if(!ui.debug)C2D_TargetClear(bottom, C2D_Color32(32, 38, 100, 0xFF));
        if(!ui.debug)C2D_SceneBegin(bottom);
	 	if(!ui.debug && ui.botfunc)uibot_func();
        C2D_TargetClear(top, C2D_Color32(32, 38, 100, 0xFF));
        C2D_SceneBegin(top);
        if(ui.topfunc)uitop_func();
		C3D_FrameEnd(0);
    }
}
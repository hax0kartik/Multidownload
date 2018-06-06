#pragma once
#include <3ds.h>    
#include <citro3d.h>
#include <citro2d.h>

typedef void (*func_t)(void *args);

typedef struct  
{
    func_t topfunc;
    func_t botfunc;
    void *topargs;
    void *botargs;
    Thread uiThreadHandle;
    Handle event;
    bool run;
    bool debug = false;
}uiFuncs_s;

extern uiFuncs_s ui;
void uiThread(void *arg);
void uiSetScreenTop(func_t callback_func, void *args);
void uiSetScreenBottom(func_t callback_func, void *args);
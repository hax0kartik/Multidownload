#include <3ds.h>
#include <stdio.h>
#include <utility>
#include <tuple>
#include <iostream>
#include "ui.h"
#include "utils.h"
#include "download.hpp"
#include "xplorer.h"

int __stacksize__ = 64 * 1024;

using namespace std;
C2D_TextBuf text_buf;
extern void stringToC2D(const char* string, C2D_Text *text);
bool isWithinTouchbox(touchPosition *pos, int x1, int y1, int w, int h)
{
    return (pos->px > x1 && pos->py > y1 && pos->px < w + x1 && pos->py < h + y1);
}

std::pair<C2D_Image, C2D_Image> sceneInit(void)
{
    //printf("sceneInit()\n");
    
    FILE *file = fopen("romfs:/gfx/backgrounds.t3x", "rb");
    C2D_SpriteSheet sheet = C2D_SpriteSheetLoadFromHandle(file);
    fclose(file);

    C2D_Image bot_image = C2D_SpriteSheetGetImage(sheet, 1);
    C2D_Image top_image = C2D_SpriteSheetGetImage(sheet, 0);

    return std::make_pair(top_image, bot_image);
}

void drawImage(C2D_Image *args)
{
    C2D_DrawImageAt(*args, 0.0f, 0.0f, 0.5f, nullptr, 1.0f, 1.0f);
}

void drawImageAndDownload(std::tuple <C2D_Image, C2D_Text*> *args)
{
    auto arg = *args;
    C2D_DrawImageAt(std::get<0>(arg), 0.0f, 0.0f, 0.5f, nullptr, 1.0f, 1.0f);
    C2D_DrawText(std::get<1>(arg), C2D_WithColor | C2D_AtBaseline, 100.0f, 234.0f, 0.5f, 0.4f, 0.4f, C2D_Color32f(1.0f,1.0f,1.0f,1.0f));
} 

void drawProgressBar(std::tuple <float, C2D_Text *> *args)
{
    auto args_2 = *args;
    C2D_DrawRectSolid(2.0f, 100.0f, 0.0f, std::get<0>(args_2), 40.0f, 0xFFFFFFFF);
    C2D_DrawText(std::get<1>(args_2), C2D_WithColor, 8.0f, 150.0f, 0.4f, 0.4f, 0.4f, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));
}

int progress_callback(void *clientp,double dltotal,double dlnow,double ultotal,double ulnow)
{
    if(dltotal == 0) return 0;
    auto p = static_cast<std::tuple<float, C2D_Text *>*>(clientp);
    string tmp = "Downloaded " + std::to_string((u32)dlnow) + " out of " + std::to_string((u32)dltotal) + " bytes";
    stringToC2D(tmp.c_str(), std::get<1>(*p));
    *p = make_tuple((dlnow/dltotal) * 316, std::get<1>(*p));
    return 0;
}
int main()
{
    romfsInit();
    gfxInitDefault();

    text_buf = C2D_TextBufNew(0x8000);
    C2D_TextBuf text_buf2 = C2D_TextBufNew(0x2000);
    C2D_Text *text = new C2D_Text[2];

    std::pair<C2D_Image, C2D_Image>images = sceneInit();
    
    const char *downloaded = "Downloaded 0 out of 0 bytes";
    stringToC2D(downloaded, &text[1]);
    auto p1 = make_tuple((1/100) * 316, &text[1]);
    auto to_pass_1 = &p1;

    const char * dl = "Download Location: ";
    std::string dlloc = "/";
    C2D_TextBufClear(text_buf2);
    C2D_TextParse(&text[0], text_buf2, std::string(dl + dlloc).c_str());
    C2D_TextOptimize(&text[0]);
    auto p = std::make_tuple(images.first, &text[0]);
    auto to_pass = &p;

    uiSetScreenTop((func_t)drawImageAndDownload, to_pass);
    uiSetScreenBottom((func_t)drawImage, &images.second);
   // ui.debug = true;
    ui.run = true;
    ui.uiThreadHandle = threadCreate(uiThread, nullptr, 8 * 1024, 0x20, -2, true);
   // ui.hid_func = ini_func;
    utils util;
    touchPosition touchpos;
    while(aptMainLoop())
    {
        hidScanInput();
        if(keysDown() & KEY_START) break;
        touchRead(&touchpos);
        if(isWithinTouchbox(&touchpos, 74, 14, 172, 60))
        {
            util.InitKeyboard();
            if(!util.ShowKeyboard("Enter A URL")) continue;
            std::string url = util.GetKeyboardData();
            DownloadManager dm;
            auto ret = dm.Initialize();
            printf("Download Begin\n");
            dm.SetURL(url);
            dm.SetDownloadLocation(dlloc);
            dm.ExtractFileIfArchive(true);
            uiSetScreenBottom((func_t)drawProgressBar, to_pass_1);
            dm.SetProgressMeterCallback(progress_callback, to_pass_1);
            auto res = dm.DownloadDirectly();
            // We'll reuse the url string
            url = "Download :" + res.second;
            stringToC2D(url.c_str(), &text[1]);
            *to_pass_1 = make_tuple(316, &text[1]);
            //DownloadManager::DownloadURL(url);
        }
        if(isWithinTouchbox(&touchpos, 30, 86, 260, 60))
        {
            dlloc = open_xplorer();
            uiSetScreenBottom((func_t)drawImage, &images.second);
            C2D_TextBufClear(text_buf2);
            C2D_TextParse(&text[0], text_buf2, std::string(dl + dlloc).c_str());
            C2D_TextOptimize(&text[0]);
            *to_pass = std::make_tuple(images.first, &text[0]);
            uiSetScreenTop((func_t)drawImageAndDownload, to_pass);

        }
        //printf("You are within Download Location\n");
        if(isWithinTouchbox(&touchpos, 54, 158, 214, 60))
        {
            Result res;
            if((res = util.qrInit()) != 0) {printf("Result : %08lX", res);}
            util.qrScan();
            std::string url = util.getDecodedURL();
            *to_pass = std::make_tuple(images.first, &text[0]);
            uiSetScreenTop((func_t)drawImageAndDownload, to_pass);
            // TODO
        }
        //printf("You are within Qr Code\n");
    }

    gfxExit();
}
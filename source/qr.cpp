#include "qr.hpp"
#include "ui.h"
#include <tuple>

#define RESULT(x) if(x != 0) return x;
#define WAIT_TIMEOUT 1000000000ULL

uint16_t rgb565_from_triplet(uint8_t red, uint8_t green, uint8_t blue)
{
  red   >>= 3;
  green >>= 2;
  blue  >>= 3;
  return (red << 11) | (green << 5) | blue;
}

void DrawImageOnScreen(std::tuple <C2D_Image, u16 **, Handle *, bool *> *pair)
{
    auto arg = *pair;
    auto image = std::get<0>(arg);
    auto camera_image = std::get<1>(arg);
    auto lock = std::get<2>(arg);
    auto done = std::get<3>(arg);
    if(*done == true) return;
    svcWaitSynchronization(*lock, U64_MAX);
   // printf("Mutex created by uiThread\n");
    for(u32 x = 0; x < 400; x++)
    {
        u16 *camImage = *camera_image;
        for(u32 y = 0; y < 240; y++)
        {
            u32 dstPos = ((((y >> 3) * (512 >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) * 2;
            u32 srcPos = (y * 400 + x);
            memcpy(&((u8*)image.tex->data)[dstPos], &camImage[srcPos], 2);
        }
    }
    svcReleaseMutex(*lock);
    C2D_DrawImageAt(image, 0.0f, 0.0f, 0.5f, NULL, 1.0f, 1.0f);
}

static void qrThread(std::tuple<u16 **, char **, struct quirc *, Handle *>*args)
{
    u16 **cameraBuffer_p = std::get<0>(*args);
    char **url = std::get<1>(*args);
    struct quirc *context =  std::get<2>(*args);
    Handle *done = std::get<3>(*args);
    fprintf(stderr, "QUIRC\n");
    while(true)
    {
        u16 *cameraBuffer = *cameraBuffer_p;
        int w,h;
        u8 *image = (u8*)quirc_begin(context, &w, &h);
        for (ssize_t x = 0; x < w; x++)
        {
            for (ssize_t y = 0; y < h; y++)
            {
                u16 px = cameraBuffer[y * 400 + x];
                image[y * w + x] = (u8)(((((px >> 11) & 0x1F) << 3) + (((px >> 5) & 0x3F) << 2) + ((px & 0x1F) << 3)) / 3);
            }
        }
        quirc_end(context);
        if (quirc_count(context) > 0)
        {
            struct quirc_code code;
            struct quirc_data scan_data;
            quirc_extract(context, 0, &code);  
            if (!quirc_decode(&code, &scan_data))
            {
                *url = (char*)malloc(scan_data.payload_len);
                memcpy(*url, &scan_data.payload, scan_data.payload_len);
                svcSignalEvent(*done);
                break;
            }
        }
    }
    threadExit(0);
}

Result qr::qrInit(void)
{
    svcCreateEvent(&this->events[2], RESET_ONESHOT);
    this->context = quirc_new();
    quirc_resize(this->context, 400, 240);
    this->cameraBuffer = (u16*)calloc(1, this->TOPSCREEN_HEIGHT * this->TOPSCREEN_WIDTH * sizeof(u16));
    if(!this->cameraBuffer) return -1;
    this->tex = new C3D_Tex[1];
    static const Tex3DS_SubTexture subt3x = { 512, 256, 0.0f, 1.0f, 1.0f, 0.0f };
    this->image = (C2D_Image){ this->tex, &subt3x };
    C3D_TexInit(this->image.tex, 512, 256, GPU_RGB565);
    C3D_TexSetFilter(this->image.tex, GPU_LINEAR, GPU_LINEAR);
    
    RESULT(camInit());
   // printf("Result Call 1: %08lx", this->res);
    RESULT(CAMU_SetSize(SELECT_OUT1, SIZE_CTR_TOP_LCD, CONTEXT_A));
    //printf("Result Call 2: %08lx", this->res);
    RESULT(CAMU_SetOutputFormat(SELECT_OUT1, OUTPUT_RGB_565, CONTEXT_A));
    //printf("Result Call 3: %08lx", this->res);
    RESULT(CAMU_SetFrameRate(SELECT_OUT1, FRAME_RATE_30));
    //printf("Result Call 4: %08lx", this->res);
    RESULT(CAMU_SetNoiseFilter(SELECT_OUT1, true));
    //printf("Result Call 5: %08lx", this->res);
    RESULT(CAMU_SetAutoExposure(SELECT_OUT1, true));
    //printf("Result Call 6: %08lx", this->res);
    RESULT(CAMU_SetAutoWhiteBalance(SELECT_OUT1, true));
    //printf("Result Call 7: %08lx", this->res);
    //
    RESULT(CAMU_GetBufferErrorInterruptEvent(&this->events[1], PORT_CAM1));
    RESULT(CAMU_SetTrimming(PORT_CAM1, false));
    //printf("Result Call 8: %08lx", this->res);
    RESULT(CAMU_GetMaxBytes((u32*)&this->transferUnit, 400, 240));
   // printf("Result Call 9: %08lx", this->res);
    //printf("TransferUnit %lX", this->transferUnit);
    RESULT(CAMU_SetTransferBytes(PORT_CAM1, this->transferUnit, 400, 240));
    //printf("Result Call 10: %08lx", this->res);
    RESULT(CAMU_Activate(SELECT_OUT1));
    //printf("Result Call 11: %08lx", this->res);
    RESULT(CAMU_ClearBuffer(PORT_CAM1));
    //printf("Result Call 12: %08lx", this->res);
    RESULT(CAMU_StartCapture(PORT_CAM1));
    //printf("Result Call 13: %08lx", this->res);
    return 0;
}

Result qr::qrScan(void)
{
    //Create one more thread for qr decode
    this->events[0] = 0;
    svcCreateMutex(&this->lock, false);
    CAMU_SetReceiving(&this->events[0], this->cameraBuffer, PORT_CAM1, 400 * 240 * sizeof(u16), (s16)this->transferUnit);
    svcWaitSynchronization(this->events[0], WAIT_TIMEOUT);
    auto pair = std::make_tuple(this->image, &this->cameraBuffer, &this->lock, this->done);
    auto to_pass = &pair;
    auto to_pass_to_qrThread = std::make_tuple(&this->cameraBuffer, &this->url, this->context, &this->events[2]);
    this->qrThreadHandle = threadCreate((ThreadFunc)qrThread, &to_pass_to_qrThread, 64 * 1024, 0x32, -2, true);
    uiSetScreenTop((func_t)DrawImageOnScreen, to_pass);
    while(!this->done)
    {
        s32 index = 0;
        bool busy;
        svcWaitSynchronizationN(&index, this->events, 3, false, U64_MAX);
       // printf("Index: %d\n", index);
        switch(index)
        {
            case 0:
                svcCloseHandle(this->events[0]);
                this->events[0] = 0;
                break;
            case 1:
                svcCloseHandle(this->events[0]);
                this->events[0] = 0;
                CAMU_ClearBuffer(PORT_CAM1);
                CAMU_StartCapture(PORT_CAM1);
                break;
            case 2:
                for (int i = 0; i < 3; i++) svcCloseHandle(i);
                CAMU_StopCapture(PORT_CAM1);
                while(R_SUCCEEDED(CAMU_IsBusy(&busy, PORT_CAM1)) && busy) svcSleepThread(1000000);
                CAMU_ClearBuffer(PORT_CAM1);
                CAMU_Activate(SELECT_NONE);
                camExit();
                svcReleaseMutex(this->lock);
                svcCloseHandle(this->lock);
                free(this->cameraBuffer);
                free(this->tex);
                this->done = true;
                break;
        }
        svcReleaseMutex(this->lock);
        svcWaitSynchronization(this->lock, U64_MAX);
        CAMU_SetReceiving(&this->events[0], this->cameraBuffer, PORT_CAM1, 400 * 240 * sizeof(u16), (s16)this->transferUnit);
    }
    return 0;
}
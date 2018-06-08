#include "qr.hpp"

Result qr::qrInit(void)
{
    this->context = quirc_new();
    quirc_resize(this->context, 400, 240);
    this->cameraBuffer = new u16[this->TOPSCREEN_HEIGHT * this->TOPSCREEN_WIDTH]();
    if(!this->cameraBuffer) return -1;
    this->tex = new C3D_Tex[1];
    static const Tex3DS_SubTexture subt3x = { 512, 256, 0.0f, 1.0f, 1.0f, 0.0f };
    this->image = (C2D_Image){ this->tex, &subt3x };
    C3D_TexInit(this->image.tex, 512, 256, GPU_RGB565);
    C3D_TexSetFilter(this->image.tex, GPU_LINEAR, GPU_LINEAR);
}
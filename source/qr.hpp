#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

extern "C"
{
    #include "quirc/quirc.h"
}
class qr
{
    public:
    Result qrInit(void);
    private:
    struct quirc *context;
    u16 *cameraBuffer;
    int TOPSCREEN_HEIGHT = 400;
    int TOPSCREEN_WIDTH = 240;
    C3D_Tex *tex;
    C2D_Image image;
};
#include "mygba.h"

void vblintroFunc(void) {
    if (!run_in_mb)
        ham_SyncMixer();
    ham_CopyObjToOAM();
    newframeintro4711 = 1;
    frameintro4711s++;
}

void ham_intro(void)
{
    ham_InitText(0);
    ham_SetBgMode(0);
    ham_LoadObjPal16(logo25_palette,0);

    u8 hamPaddle = ham_CreateObj(logo25_h_Bitmap,2,1,0,0,0,0,0,0,0,0,0x4a,0x46);
    u8 hamBall = ham_CreateObj(logo25_ball_Bitmap,0,0,0,0,0,0,0,0,0,0,10,10);
    u8 hamLogo = ham_CreateObj(logo25_Bitmap,0,3,0,0,0,0,0,0,0,0,0x38,0x30);
    u8 hamWhat = ham_CreateObj(undefined,0,3,0,0,0,0,0,0,0,0,0x78,0x30);
}

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
  uchar spriteH;
  uchar spriteBall;
  uchar spriteLogo1;
  uchar spriteLogo2;
  uchar ballY;
  uchar ballX;
  uchar logoY;

  ham_InitText(0);
  ham_SetBgMode(0);
  ham_LoadObjPal16(logo25_Palette,0);
  logoY = 70;
  spriteH = ham_CreateObj(logo25_h_Bitmap,2,1,0,0,0,0,0,0,0,0,0x4a,70);
  spriteBall = ham_CreateObj(logo25_ball_Bitmap,0,0,0,0,0,0,0,0,0,0,10,10);
  spriteLogo1 = ham_CreateObj(logo25_Bitmap,0,3,0,0,0,0,0,0,0,0,0x38,0x30);
  spriteLogo2 = ham_CreateObj(logo25_Bitmap + 0x800,0,3,0,0,0,0,0,0,0,0,0x78,0x30);
  ballX = 248;
  ballY = 159;
  ham_DrawText(0,0x13,"MULTIBOOT");
  run_in_mb = 1;
  ham_DrawText(0x19,0x13,"v%d.%d",2,8);
  ham_StartIntHandler(0,vblintroFunc);
  while( true ) {
    do {
    } while (newframeintro4711 == 0);
    if (run_in_mb == 0) {
      ham_UpdateMixer();
    }
    if (frameintro4711s < 0x1c) {
      ballX = ballX + '\x03';
    }
    else {
      ballX = ballX + 0xfd;
    }
    if ((frameintro4711s == 0x19) && (run_in_mb == 0)) {
      ham_PlaySample(0);
    }
    if (frameintro4711s - 0x15 < 9) {
      logoY = logoY + 2;
    }
    ballY = ballY + 0xfe;
    if (frameintro4711s - 0x1f < 9) {
      logoY = logoY + 0xfe;
    }
    ham_SetObjXY(spriteBall,ballX,ballY);
    ham_SetObjY(spriteH,logoY);
    if (frameintro4711s == 0x3a) {
      ham_DrawText(0x19,0x13,"     ");
      ham_DrawText(0,0x13,"         ");
      ham_DeleteObj(spriteBall);
    }
    if (0x3c < frameintro4711s) {
      ham_FadePal(1,1);
    }
    if (0x5a < frameintro4711s) break;
    newframeintro4711 = 0;
  }
  if (run_in_mb == 0) {
    ham_DeInitSample(0);
    ham_DeInitMixer();
  }
  ham_DeInitText();
  ham_DeleteObj(spriteLogo2);
  ham_DeleteObj(spriteLogo1);
  ham_DeleteObj(spriteH);
  ham_StopIntHandler(0);
  return;
}

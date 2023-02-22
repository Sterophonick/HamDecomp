#include <stdbool.h>
#include "mygba.h"

u8 run_in_mb = 0;
u16 frameintro4711s = 0;
u8 newframeintro4711 = 0;
extern u32 __malloc_heap_end;
u8 intro_done = 0;

extern u16 logo25_ball_Bitmap[];
extern u8 logo25_Palette[];
extern u16 logo25_h_Bitmap[];
extern u16 logo25_Bitmap[];

void vblintroFunc(void) {
    if (!run_in_mb) //Why only sync mixer when not MULTIBOOT?
        ham_SyncMixer();
    ham_CopyObjToOAM();
    newframeintro4711 = 1;
    frameintro4711s++;
}

void ham_intro(void)
{
  u8 spriteH;
  u8 spriteBall;
  u8 spriteLogo1;
  u8 spriteLogo2;
  u8 ballY;
  u8 ballX;
  u8 logoY;

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
  run_in_mb = 1; // always set to multiboot????
  ham_DrawText(0x19,0x13,"v%d.%d",2,8); //print version
  ham_StartIntHandler(0,vblintroFunc);
  while( true ) {

    //???
    do {
    } while (newframeintro4711 == 0);

    //Do no play sound in multiboot
    //What was the purpose here?
    if (run_in_mb == 0) {
      ham_UpdateMixer();
    }

    if (frameintro4711s < 0x1c) {
      ballX += 3; //ball coming towards
    }
    else {
      ballX -= 3; //ball go wheee
    }

    //Play sample when ball go clack
    if ((frameintro4711s == 0x19) && (run_in_mb == 0)) {
      ham_PlaySample(0);
    }

    //logo go down
    if (frameintro4711s - 0x15 < 9) {
      logoY = logoY + 2;
    }

    //ball y position
    ballY = ballY - 2;

    //logo go up
    if (frameintro4711s - 0x1f < 9) {
      logoY = logoY + 0xfe;
    }

    //Set sprite positions
    ham_SetObjXY(spriteBall,ballX,ballY);
    ham_SetObjY(spriteH,logoY);

    //Clear sprits + text
    if (frameintro4711s == 0x3a) {
      ham_DrawText(0x19,0x13,"     ");
      ham_DrawText(0,0x13,"         ");
      ham_DeleteObj(spriteBall);
    }

    //Fade out
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

void ham_ResetAll(void)
{
  if (intro_done) {
    ham_InitIntrTable();
    ham_ResetBg();
    ham_ResetObj();
    ham_InitWin();

    //?
    R_DISCNT &= 0xf078;
    R_DISCNT |= 1;
  }
  return;
}

void ham_Init(void)
{
  __malloc_heap_end = 0x2040000; //restrict malloc to ewram
  intro_done = 1;
  ham_ResetAll();
  ham_intro();
  ham_ResetAll();
  memset((void *)0x6000000,0,0x18000); //clear VRAM
  R_INTMST = 0; //Disable IRQs
  return;
}

u32 ham_GetHAMlibVersion(void)
{
    return 0x20008; //v2.8
}

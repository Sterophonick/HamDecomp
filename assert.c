#include "mygba.h"

void AssertImplementation
               (bool a_Assertion,char *a_Message,char *a_Expression,char *a_FileName,
               u32 a_LineNumber)

{
  s32 iVar1;
  u32 uVar2;
  u16 *vramPtr;
  
  if ((a_Assertion == false) && (_hamlib_assert_to_dbgshow == '\0')) {
    //Resets display controllers
    R_DISCNT = 0x403;
    ham_RotBgEx(2,0,0,0,0,0,0x100,0x100);

    iVar1 = 0x4aff;
    vramPtr = (u16 *)&MEM_VRAM;
    while ((uVar2 & 0xffff) != 0xffff) {
      uVar2 = iVar1 - 1;
      *vramPtr = 0;
      iVar1 = (int)(uVar2 * 0x10000) >> 0x10;
      vramPtr = vramPtr + 1;
    }
    AssertDrawStringMode3(10,10,0xffff,"Assertion failed:");
    AssertDrawStringMode3(0x1e,0x14,0xff,a_Expression);
    AssertDrawStringMode3(10,0x1e,0xffff,"In File: ");
    AssertDrawStringMode3(0x1e,0x28,0xff,a_FileName);
    AssertDrawStringMode3(10,0x32,0xffff,"At Line: ");
    AssertDrawNumberMode3(0x1e,0x3c,0xff,a_LineNumber);
    AssertDrawStringMode3(10,0x46,0xffff,"Message:");
    AssertDrawStringMode3(0x1e,0x50,0xff,a_Message);
    AssertDrawStringMode3(10,0x8c,0xffff,"Press any key to ignore, but remember that");
    AssertDrawStringMode3(10,0x96,0xffff,"VMode and VRam recovery is unlikely");
    while ((~R_CTRLINPUT & 0x3ff) == 0);
  }
  return;
}

void AssertDrawStringMode3(uint param_1,int param_2,undefined2 param_3,byte *param_4)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  undefined1 *puVar5;
  undefined2 *puVar6;
  
  uVar2 = (uint)*param_4;
  if (uVar2 != 0) {
    param_2 = param_2 * 0xf0;
    uVar4 = param_1;
    do {
      if (uVar2 == 0x2e) {
        uVar3 = 0x24;
LAB_00010328:
        uVar2 = 0;
        do {
          puVar6 = (undefined2 *)((param_2 + uVar4 + uVar2) * 2 + 0x6000000);
          uVar1 = 0;
          puVar5 = font.17 + uVar3 * 0x14;
          do {
            uVar1 = uVar1 + 1;
            if (puVar5[uVar2] == '\0') {
              *puVar6 = param_3;
            }
            puVar5 = puVar5 + 4;
            puVar6 = puVar6 + 0xf0;
          } while (uVar1 < 5);
          uVar2 = uVar2 + 1;
        } while (uVar2 < 4);
        uVar4 = uVar4 + 5;
        if (0xeb < uVar4) {
          param_2 = param_2 + 0x5a0;
          uVar4 = param_1;
        }
      }
      else {
        if (uVar2 == 0x3a) {
          uVar3 = 0x25;
          goto LAB_00010328;
        }
        if (uVar2 == 0x2c) {
          uVar3 = 0x26;
          goto LAB_00010328;
        }
        if (uVar2 == 0x28) {
          uVar3 = 0x27;
          goto LAB_00010328;
        }
        if (uVar2 == 0x29) {
          uVar3 = 0x28;
          goto LAB_00010328;
        }
        if (uVar2 == 0x2f) {
          uVar3 = 0x29;
          goto LAB_00010328;
        }
        if (uVar2 == 0x5c) {
          uVar3 = 0x2a;
          goto LAB_00010328;
        }
        if (uVar2 == 0x2b) {
          uVar3 = 0x2c;
          goto LAB_00010328;
        }
        uVar3 = uVar2;
        if (uVar2 == 0x2d) goto LAB_00010328;
        if (uVar2 == 0x3d) {
          uVar3 = 0x2e;
          goto LAB_00010328;
        }
        if (uVar2 == 0x2a) {
          uVar3 = 0x2f;
          goto LAB_00010328;
        }
        if (uVar2 == 0x26) {
          uVar3 = 0x30;
          goto LAB_00010328;
        }
        if (uVar2 == 0x7c) {
          uVar3 = 0x31;
          goto LAB_00010328;
        }
        if (uVar2 == 0x3c) {
          uVar3 = 0x32;
          goto LAB_00010328;
        }
        if (uVar2 == 0x3e) {
          uVar3 = 0x33;
          goto LAB_00010328;
        }
        if (uVar2 == 0x5f) {
          uVar3 = 0x34;
          goto LAB_00010328;
        }
        if (uVar2 != 0x20) {
          uVar3 = uVar2 - 0x41;
          if ((0x19 < uVar2 - 0x41) && (uVar3 = uVar2 - 0x61, 0x19 < uVar2 - 0x61)) {
            if (uVar2 - 0x30 < 10) {
              uVar3 = uVar2 - 0x16;
            }
            else {
              uVar3 = 0x2b;
            }
          }
          goto LAB_00010328;
        }
        uVar4 = uVar4 + 5;
      }
      param_4 = param_4 + 1;
      uVar2 = (uint)*param_4;
    } while (uVar2 != 0);
  }
  return;
}


void AssertDrawNumberMode3(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  char cVar2;
  int iVar3;
  char *pcVar4;
  char *pcVar5;
  char acStack34 [2];
  undefined4 uStack4;
  
  uStack4 = 0x103bc;
  acStack34[1] = 0;
  pcVar5 = acStack34;
  iVar3 = param_4;
  pcVar4 = pcVar5;
  if (0 < param_4) {
    do {
      iVar1 = iVar3 / 10;
      pcVar5 = pcVar4 + -1;
      *pcVar4 = (char)iVar3 + (char)iVar1 * -10 + '0';
      iVar3 = iVar1;
      pcVar4 = pcVar5;
    } while (0 < iVar1);
  }
  if (param_4 < 0) {
    cVar2 = '-';
  }
  else {
    cVar2 = '+';
  }
  *pcVar5 = cVar2;
  AssertDrawStringMode3(param_1,param_2,param_3,pcVar5);
  return;
}

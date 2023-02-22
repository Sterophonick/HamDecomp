#include "mygba.h"

void ham_InitMixer(u16 bitrate)
{
    float uVar1; //float?

    //reset samples
    ham_mixer.first_sample = 0;
    ham_mixer.last_sample = 0;

    ham_mixer.bitrate = bitrate;

    //what the fuck?
    uVar1 = __floatsidf(bitrate);
    uVar1 = __divdf3((int)uVar1,(int)((ulonglong)uVar1 >> 0x20),0x404ddd0e,0x56041893);
    __adddf3((int)uVar1,(int)((ulonglong)uVar1 >> 0x20),0x3ff00000,0);
    ham_mixer._12_2_ = __fixunsdfsi();
    ham_mixer._14_2_ = __divsi3(0x1000000,bitrate);

    //oh god oh no
    ham_mixer._4_4_ = malloc((size_t)ham_mixer._12_2_);
    ham_mixer._8_4_ = malloc((size_t)ham_mixer._12_2_);
}

void ham_DeInitMixer(void)
{
    R_TIM0CNT = 0;
    ham_DeInitDSound();
    free(ham_mixer._4_4);
    free(ham_mixer._8_4);
}

void ham_SyncMixer(void)
{
}

void ham_UpdateMixer(void)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  char *pcVar4;
  char *pcVar5;
  uint *puVar6;
  uint *puVar7;
  uint uVar8;
  uint uVar9;
  char *pcVar10;
  int iVar11;

  iVar3 = ham_mixer[16] + -1;
  if (iVar3 < 0) {
    iVar3 = -iVar3;
  }
  ham_mixer[16] = (char)iVar3;
  _DAT_040000b4 = *(uint *)(ham_mixer + ham_mixer[16] * 4 + 4) & 0x7ffffff;
  _DAT_040000b8 = ham_mixer._12_2_ >> 2;
  _DAT_040000b0 = &mixempty;
  _DAT_040000ba = _DAT_040000ba & 0x881f | 0x8500;
  bVar1 = false;
  puVar6 = ham_mixer._20_4_;
  if (ham_mixer._20_4_ != (uint *)0x0) {
    do {
      uVar8 = puVar6[4];
      uVar9 = (uint)*(ushort *)((int)puVar6 + 0xe);
      pcVar10 = *(char **)(ham_mixer + ham_mixer[16] * 4 + 4);
      pcVar4 = (char *)(puVar6[1] + uVar8 + 0x2000000);
      iVar3 = 0;
      if (*puVar6 < uVar8 + uVar9) {
        for (iVar3 = (uint)*(ushort *)puVar6 - (uint)*(ushort *)(puVar6 + 4);
            iVar3 = iVar3 * 0x10000 >> 0x10, iVar3 != 0; iVar3 = iVar3 + -1) {
          pcVar10[iVar3] = pcVar10[iVar3] + pcVar4[iVar3];
        }
        puVar7 = (uint *)puVar6[6];
        if (puVar7 == (uint *)0x0) {
          bVar1 = true;
        }
        ham_KillSample(puVar6);
      }
      else {
        iVar11 = (int)(short)ham_mixer._12_2_;
        if (0 < iVar11) {
          do {
            for (cVar2 = *(char *)((int)puVar6 + 10); cVar2 != '\0'; cVar2 = cVar2 + -1) {
              *pcVar10 = *pcVar10 + *pcVar4;
              iVar11 = (iVar11 + -1) * 0x10000 >> 0x10;
              pcVar10 = pcVar10 + 1;
            }
            if ((uint)(iVar3 * 0x10000) < 0x1000001) {
              pcVar5 = pcVar4 + 1;
              *pcVar10 = *pcVar10 + *pcVar4;
              uVar8 = (uint)*(ushort *)(puVar6 + 3);
            }
            else {
              *pcVar10 = *pcVar10 + *pcVar4;
              uVar8 = 0xffffff00;
              pcVar5 = pcVar4;
            }
            pcVar10 = pcVar10 + 1;
            iVar11 = (iVar11 + -1) * 0x10000 >> 0x10;
            iVar3 = (int)((iVar3 + uVar8) * 0x10000) >> 0x10;
            pcVar4 = pcVar5;
          } while (0 < iVar11);
          uVar8 = puVar6[4];
          uVar9 = (uint)*(ushort *)((int)puVar6 + 0xe);
        }
        puVar6[4] = uVar8 + uVar9;
        puVar7 = (uint *)puVar6[6];
        if (puVar7 == (uint *)0x0) {
          bVar1 = true;
        }
      }
      puVar6 = puVar7;
    } while (!bVar1);
  }
  return;
}

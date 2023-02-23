#include "mygba.h"


void ham_InitSound(void)
{
    R_SNDCNT3 |= 0x80;

    R_SNDCNT2 &= 0xCCF8;
    R_SNDCNT2 |= 0x3305;

    R_SNDCNT1 &= 0xFF88;
    R_SNDCNT1 |= 0xFF77;
}

void ham_InitDSound(void)
{
    R_SNDCNT2 &= 0xF8F3;
    R_SNDCNT2 |= 0x8B0C;

    R_SNDCNT3 |= 0x80;
}

void ham_DeInitDSound(void)
{
    R_SNDCNT2 &= 0xCCFF;
    R_SNDCNT3 &= 0xFF7F;
}

void ham_DeInitSample(sample_info *sample)
{
    free(sample);
}

//end my life
undefined4 * ham_InitSample(undefined4 param_1,undefined4 param_2,undefined2 param_3)

{
  undefined uVar1;
  undefined2 uVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  undefined8 uVar8;

  puVar3 = (undefined4 *)malloc(0x20);
  *(undefined2 *)(puVar3 + 2) = param_3;
  uVar8 = __floatsidf(param_3);
  __divdf3((int)uVar8,(int)((ulonglong)uVar8 >> 0x20),0x404ddd0e,0x56041893);
  uVar2 = __fixunsdfsi();
  iVar7 = (int)_DAT_00011034;
  *(undefined2 *)((int)puVar3 + 0xe) = uVar2;
  uVar4 = __floatsisf(iVar7);
  if (iVar7 < 0) {
    uVar4 = __addsf3(uVar4,0x47800000);
  }
  iVar6 = (int)*(short *)((int)puVar3 + 0xe);
  uVar5 = __floatsisf(iVar6);
  if (iVar6 < 0) {
    uVar5 = __addsf3(uVar5,0x47800000);
  }
  uVar4 = __divsf3(uVar4,uVar5);
  __subsf3(uVar4,0x3f800000);
  uVar1 = __fixunssfsi();
  *(undefined *)((int)puVar3 + 10) = uVar1;
  uVar4 = __floatsisf(iVar7);
  if (iVar7 < 0) {
    uVar4 = __addsf3(uVar4,0x47800000);
  }
  uVar5 = __floatsisf(iVar6);
  if (iVar6 < 0) {
    uVar5 = __addsf3(uVar5,0x47800000);
  }
  uVar4 = __divsf3(uVar4,uVar5);
  uVar4 = __subsf3(uVar4,0x3f800000);
  uVar5 = __floatsisf(*(undefined *)((int)puVar3 + 10));
  uVar4 = __subsf3(uVar4,uVar5);
  __mulsf3(uVar4,0x43800000);
  uVar2 = __fixunssfsi();
  *puVar3 = param_2;
  *(undefined2 *)(puVar3 + 3) = uVar2;
  puVar3[1] = param_1;
  puVar3[4] = 0;
  *(undefined *)(puVar3 + 5) = 0;
  return puVar3;
}

void ham_ChangeSampleBitrate(int param_1,undefined2 param_2)

{
  undefined uVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  undefined8 uVar7;

  *(undefined2 *)(param_1 + 8) = param_2;
  uVar7 = __floatsidf(param_2);
  __divdf3((int)uVar7,(int)((ulonglong)uVar7 >> 0x20),0x404ddd0e,0x56041893);
  uVar2 = __fixunsdfsi();
  *(undefined2 *)(param_1 + 0xe) = uVar2;
  iVar5 = (int)_DAT_00011034;
  uVar3 = __floatsisf(iVar5);
  if (iVar5 < 0) {
    uVar3 = __addsf3(uVar3,0x47800000);
  }
  iVar5 = (int)*(short *)(param_1 + 0xe);
  uVar4 = __floatsisf(iVar5);
  if (iVar5 < 0) {
    uVar4 = __addsf3(uVar4,0x47800000);
  }
  uVar3 = __divsf3(uVar3,uVar4);
  __subsf3(uVar3,0x3f800000);
  uVar1 = __fixunssfsi();
  *(undefined *)(param_1 + 10) = uVar1;
  iVar6 = (int)_DAT_00011034;
  uVar3 = __floatsisf(iVar6);
  if (iVar6 < 0) {
    uVar3 = __addsf3(uVar3,0x47800000);
  }
  uVar4 = __floatsisf(iVar5);
  if (iVar5 < 0) {
    uVar4 = __addsf3(uVar4,0x47800000);
  }
  uVar3 = __divsf3(uVar3,uVar4);
  uVar3 = __subsf3(uVar3,0x3f800000);
  uVar4 = __floatsisf(*(undefined *)(param_1 + 10));
  uVar3 = __subsf3(uVar3,uVar4);
  __mulsf3(uVar3,0x43800000);
  uVar2 = __fixunssfsi();
  *(undefined2 *)(param_1 + 0xc) = uVar2;
  return;
}

void ham_PlaySample(sample_info *sample)

{
  int prevSample; //?

  sample->playing = 1; //sample+0x14

  //the fuck?
  iVar1 = _DAT_00011040;
  if (_DAT_0001103c != 0) {
    sample->next = 0; //sample+0x18
    sample->prev = prevSample; //sample+0x1c
    *(int *)(_DAT_00011040 + 0x18) = sample;
    _DAT_00011040 = sample;
    return;
  }
  _DAT_0001103c = sample;
  _DAT_00011040 = sample;
  sample->prev = 0; //sample+0x1c
  sample->next = 0; //sample+0x18
  return;
}


#include "mygba.h"

void ham_SetFxMode(u16 source_layer,u16 target_layer,u16 fx_mode)
{
  ASSERT(fx_mode < 4, "No such FX_MODE. Set to 0-3","!(fx_mode>3)");
  R_BLDCNT = source_layer | (u16)((u32)((int)(short)target_layer << 0x10) >> 8) | fx_mode * 0x40;
  return;
}

void ham_SetFxAlphaLevel(u16 source_intensity,u16 target_intensity)uint
{
  R_BLDALPHA = (u16)(((u32)target_intensity << 0x10) >> 8) | source_intensity;
  return;
}

void ham_SetFxBrightnessLevel(u16 intensity)
{
  R_BLDY = intensity;
  return;
}


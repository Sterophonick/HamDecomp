void ham_InitObj(void)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  uVar3 = 0;
  do {
    iVar1 = uVar3 * 0xc;
    *(undefined4 *)(ham_obj + iVar1) = 0;
    *(undefined4 *)(ham_obj + iVar1 + 4) = 0;
    *(undefined4 *)(ham_obj + iVar1 + 8) = 0;
    ham_SetObjX(uVar3,0xf1);
    ham_SetObjY(uVar3,0xf1);
    uVar2 = uVar3 + 1;
    iVar1 = uVar3 * 8;
    uVar3 = uVar2 & 0xff;
    *(undefined2 *)(iVar1 + 0x7000000) = 0xf1;
    *(undefined2 *)(&DAT_07000002 + iVar1) = 0xf1;
    *(undefined2 *)(&DAT_07000004 + iVar1) = 0;
  } while ((uVar2 & 0x80) == 0);
  uVar3 = (uint)ham_obj_first;
  _DAT_04000000 = _DAT_04000000 | 0x1040;
  ham_obj[uVar3 * 0xc] = 0;
  ham_obj[uVar3 * 0xc + 9] = 0xff;
  return;
}


void ham_ResetObj(void)

{
  ham_obj_first = 0;
  ham_InitMemObj();
  ham_InitObj();
  ham_CopyObjToOAM();
  return;
}


ushort ham_GetObjGfxSlot(u8 objno)

{
  return *(ushort *)(ham_obj + (objno & 0xff) * 0xc + 6) & 0x3ff;
}

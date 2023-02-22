void ham_InitBg(ushort bgno,ushort active,ushort prio,ushort mosaic)

{
  int iVar1;
  ushort uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  byte *pbVar7;
  short *psVar8;

  uVar3 = (uint)bgno;
  iVar1 = uVar3 * 0x14;
  *(ushort *)(ham_bg + iVar1 + 10) = active;
  *(ushort *)(ham_bg + iVar1 + 0xc) = prio;
  *(ushort *)(ham_bg + iVar1 + 8) = mosaic;
  uVar4 = _DAT_04000208 & 1;
  if ((_DAT_04000208 & 1) == 0) {
    if ((*(int *)(ham_bg + iVar1 + 4) != 0) && (uVar4 = *(uint *)(ham_bg + iVar1), uVar4 != 0)) {
      uVar4 = 1;
    }
    AssertImplementation
              (uVar4,
               "ham_InitBg: You need to set up the map and tile info (.mi and .ti) before running in itbg!"
               ,"ham_bg[bgno].ti!=0 && ham_bg[bgno].mi!=0",&DAT_000108fc,0x3c);
  }
  else {
    _DAT_04000208 = 0;
    iVar5 = 0;
    if ((*(int *)(ham_bg + iVar1 + 4) != 0) && (iVar5 = *(int *)(ham_bg + iVar1), iVar5 != 0)) {
      iVar5 = 1;
    }
    AssertImplementation
              (iVar5,
               "ham_InitBg: You need to set up the map and tile info (.mi and .ti) before running in itbg!"
               ,"ham_bg[bgno].ti!=0 && ham_bg[bgno].mi!=0",&DAT_000108fc,0x3c);
    _DAT_04000208 = _DAT_04000208 | 1;
  }
  uVar2 = _DAT_04000208 & 1;
  if ((_DAT_04000208 & 1) == 0) {
    if ((*(char *)(*(int *)(ham_bg + uVar3 * 0x14) + 1) != '\x01') ||
       (**(short **)(ham_bg + uVar3 * 0x14 + 4) != 0)) {
      uVar2 = 1;
    }
    AssertImplementation
              (uVar2,
               "ham_InitBg: You cannot set a rotation map and 16 color tile sets in the GBA! use 256 col."
               ,"!(ham_bg[bgno].mi->map_rot==1 && ham_bg[bgno].ti->col_mode==0)",&DAT_000108fc,0x3d)
    ;
  }
  else {
    _DAT_04000208 = 0;
    uVar6 = 0;
    if ((*(char *)(*(int *)(ham_bg + uVar3 * 0x14) + 1) != '\x01') ||
       (**(short **)(ham_bg + uVar3 * 0x14 + 4) != 0)) {
      uVar6 = 1;
    }
    AssertImplementation
              (uVar6,
               "ham_InitBg: You cannot set a rotation map and 16 color tile sets in the GBA! use 256 col."
               ,"!(ham_bg[bgno].mi->map_rot==1 && ham_bg[bgno].ti->col_mode==0)",&DAT_000108fc,0x3d)
    ;
    _DAT_04000208 = _DAT_04000208 | 1;
  }
  iVar1 = uVar3 * 2;
  *(ushort *)(&DAT_04000008 + iVar1) = *(ushort *)(&DAT_04000008 + iVar1) & 0xfffd;
  iVar5 = uVar3 * 0x14;
  *(ushort *)(&DAT_04000008 + iVar1) =
       *(ushort *)(&DAT_04000008 + iVar1) | *(ushort *)(ham_bg + iVar5 + 0xc);
  *(ushort *)(&DAT_04000008 + iVar1) = *(ushort *)(&DAT_04000008 + iVar1) & 0xfff3;
  psVar8 = *(short **)(ham_bg + iVar5 + 4);
  *(ushort *)(&DAT_04000008 + iVar1) =
       *(ushort *)(&DAT_04000008 + iVar1) | (ushort)*(byte *)(psVar8 + 2) << 2;
  *(ushort *)(&DAT_04000008 + iVar1) = *(ushort *)(&DAT_04000008 + iVar1) & 0xe0ff;
  pbVar7 = *(byte **)(ham_bg + iVar5);
  *(ushort *)(&DAT_04000008 + iVar1) = *(ushort *)(&DAT_04000008 + iVar1) | (ushort)*pbVar7 << 8;
  *(ushort *)(&DAT_04000008 + iVar1) = *(ushort *)(&DAT_04000008 + iVar1) & 0xff7f;
  *(ushort *)(&DAT_04000008 + iVar1) = *(ushort *)(&DAT_04000008 + iVar1) | *psVar8 << 7;
  *(ushort *)(&DAT_04000008 + iVar1) = *(ushort *)(&DAT_04000008 + iVar1) & 0x3fff;
  *(ushort *)(&DAT_04000008 + iVar1) =
       *(ushort *)(&DAT_04000008 + iVar1) | *(short *)(pbVar7 + 2) << 0xe;
  *(ushort *)(&DAT_04000008 + iVar1) = *(ushort *)(&DAT_04000008 + iVar1) & 0xffbf;
  *(ushort *)(&DAT_04000008 + iVar1) =
       *(ushort *)(&DAT_04000008 + iVar1) | *(short *)(ham_bg + iVar5 + 8) << 6;
  _DAT_04000000 =
       _DAT_04000000 & ((ushort)(0xfeff << (uVar3 & 0xff)) | 0xff) |
       (ushort)(((uint)*(ushort *)(ham_bg + iVar5 + 10) << 8) << (uVar3 & 0xff));
  return;
}

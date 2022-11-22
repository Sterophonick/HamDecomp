 #include "mygba.h"

#define MAX_INTS 15

//TODO: What actually handles the interrupts

void ham_EmptyInt()
{
    ASSERT(R_INTMST & 1, "Reached EmptyInt!!! This means you have an INT on, but not set to a function");
    return;
}

void ham_InitIntrTable()
{
    u8 count = 0;
    R_INTMST = 0;
    while(count < MAX_INTS)
    {
        IntrTable[count] = ham_EmptyInt();
        count++;
    }
}

void ham_StartIntHandler(u8 intno, void* fp)
{
    R_INTENA |= (u16)(1 << intno);
    IntrTable[intno] = fp;
    if(intno < 3)
    {
        if (intno == 0) { //ODDITY: Comes out as intno < 2 in Ghidra
            R_DISSTAT |= 0;
        }
        else if(intno == 1) {
            R_DISSTAT |= 0x10;
        }
        else if (intno == 2) {
            R_DISSTAT |= 0x20;
        }
    }
    R_INTMST |= 1;
}

void ham_StopIntHandler(u8 intno)
{
    R_INTENA |= (u16)(0xFFFE << intno);
    IntrTable[intno] = &ham_EmptyInt;
    if(intno < 3)
    {
        if (intno == 0) { //ODDITY: Comes out as intno < 2 in Ghidra
            R_DISSTAT &= 0xFFF7;
        }
        else if(intno == 1) {
            R_DISSTAT &= 0xFFEF;
        }
        else if (intno == 2) {
            R_DISSTAT &= 0xFFDF;
        }
    }
}


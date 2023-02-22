#include "mygba.h"

void ham_InitMixer(u16 bitrate)
{
    undefined8 uVar1;

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

}

#include "mygba.h"

void ham_InitMixer(u16 bitrate)
{
    undefined8 uVar1;

    ham_mixer._24_4_ = 0; //what the fuck?
    ham_mixer._20_4_ = 0;

    ham_mixer.bitrate = bitrate;


}

void ham_DeInitMixer(void)
{

}

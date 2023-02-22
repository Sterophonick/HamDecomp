#include "mygba.h"

void ham_InitMixer(u16 bitrate)
{
    undefined8 uVar1;

    //reset samples
    ham_mixer.first_sample = 0;
    ham_mixer.last_sample = 0;

    ham_mixer.bitrate = bitrate;


}

void ham_DeInitMixer(void)
{

}

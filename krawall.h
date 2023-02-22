/***********************************************************
* KRAWALL Soundsystem
* Copyright (c) 2001,2002 Sebastian Kienzl <seb@riot.org>
************************************************************
* $Id: krawall.h,v 1.3 2002/12/09 02:03:35 schleuss Exp $
************************************************************/

// this is generated, do not edit.
// see documentation for detailed information

#ifndef __KRAWALL_H__
#define __KRAWALL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mtypes.h"

typedef unsigned int chandle;

#if __GNUC__ == 2
	#define kramWorker kramWorkerSTUB
	#define kramPlay kramPlaySTUB
	#define kramPlayExt kramPlayExtSTUB
	#define kramStop kramStopSTUB
	#define kramSetFreq kramSetFreqSTUB
	#define kramSetVol kramSetVolSTUB
	#define kramSetPan kramSetPanSTUB
	#define kramSetPos kramSetPosSTUB
	#define LONG_CALL
#elif __GNUC__ == 3
	#define LONG_CALL __attribute__ ((long_call))
#else
	#error "This version of Krawall is for GCC2/3 only!!"
#endif

//! Init function.
/*!
Call this function once at startup.
\param stereo Whether Krawall should operate stereo or not
*/
void kragInit( int stereo );
#define KRAG_INIT_MONO 0
#define KRAG_INIT_STEREO 1

//! Start music
/*!
\param m Pointer to module
\param mode is one or more of:
			- KRAP_MODE_LOOP	Loop module
			- KRAP_MODE_SONG	Enable song-mode
			- KRAP_MODE_JINGLE	Play module as jingle
\param song Song of module to play
\sa krapStop()
*/
void krapPlay( const Module *m, int mode, int song );
#define KRAP_MODE_LOOP 1
#define KRAP_MODE_SONG 2
#define KRAP_MODE_JINGLE 4

//! Stop music
/*!
Immediately stops playback of music.
\sa krapPlay()
*/
void krapStop();

//! Install callback
/*!
Installs a callback. The callback should return as quickly as possible.
When the callback gets called the first numeric parameter
describes the event, the second numeric parameter (if any)
is the parameter to the event.
The events are as following:
	- KRAP_CB_FADE		Destination volume has been reached
	- KRAP_CB_DONE		Module is done (also when KRAP_MODE_LOOP)
	- KRAP_CB_MARK		Mark-Effect Zxx (xx in param 2)
	- KRAP_CB_SONG		Song-boundary hit (+++-Marker)
	- KRAP_CB_JDONE		Jingle is done
\sa krapSetMusicVol
*/
void krapCallback( void (*func)( int, int ) );
#define KRAP_CB_FADE 1	// fading done (see krapSetMusicVol)
#define KRAP_CB_DONE 2	// module done (also when looping)
#define KRAP_CB_MARK 3	// mark-effect occured in pattern (op in param 2)
#define KRAP_CB_SONG 4	// played over a song-boundary (++)
#define KRAP_CB_JDONE 5 // jingle done

//! Pause music
/*!
Pauses all currently active channels. You still can play SFX's.
The paused channels will be frozen until krapUnpause() gets called.
\param sfx If true pause sfx as well; if false pause music only
\sa krapUnpause()
*/
void krapPause( int sfx );

//! Unpause music
/*!
Reactivates all channels that have been paused with krapPause()
\sa krapPause()
*/
void krapUnpause();

//! Get Pause status
/*!
Returns whether playback is currently paused or not
\return True if paused
\sa krapPause()
\sa krapUnpause()
*/
int krapIsPaused();

//! Set music volume
/*!
You can either set the music volume immediately or fade slowly
to the specified volume. The fadespeed depends on the speed
of the currently active module.
If module is paused then volume is always set immediately.
If a callback is installed it will get triggered when fading is done.
The volume given will directly scale the global volume set in the S3M.
\param vol Music volume (0..128)
\param fade If true fade, if false set immediately
\sa kramSetSFXVol
\sa krapCallback
*/
void krapSetMusicVol( unsigned int vol, int fade );


//! Worker procedure
/*!
This is where the actual work is done, you *MUST* call
this once per frame after kraInit() to get sound
\sa kragInit()
\return True if actual work has been done
*/
int kramWorker() LONG_CALL;

//! Get number of currently active channels
/*!
Returns number of currently active channels.
\return Number of currently active channels
*/
int kramGetActiveChannels();

//! Set quality mode
/*!
This sets the quality mode of the mixing routines.
KRAM_QM_NORMAL is the default, KRAM_QM_MARKED only plays the marked samples (see docs)
in HQ and KRAM_QM_HQ plays everything in HQ.
*/
void kramQualityMode( int );
#define KRAM_QM_NORMAL 	0
#define KRAM_QM_MARKED 	1
#define KRAM_QM_HQ 		2

//! Play a sample
/*!
Plays a sample with it's C2 (neutral) frequency.
\param s Pointer to sample
\param sfx Whether sample to play is an SFX
\param c Old handle, will be recycled if given
\sa kramPlayExt()
\return Channel handle
*/
chandle kramPlay( const Sample *s, int sfx, chandle c ) LONG_CALL;

//! Play a sample Ext
/*!
Just like kramPlay, but all of the attribs can be specified.
\param s Pointer to sample
\param sfx Whether sample to play is an SFX
\param c Old handle, will be recycled if given
\param freq Frequency in hertz to play sample at
\param vol Volume to play sample with (0..64)
\param pan Panning to play sample with (-64..64)
\sa kramPlay()
\return Channel handle
*/
chandle kramPlayExt( const Sample *s, int sfx, chandle c, unsigned int freq, unsigned int vol, int pan ) LONG_CALL;

//! Stop a channel
/*!
Stops playback of a channel.
Note that if the channel has already stopped this call will not do anything and return false.
\param c Channel handle
\return true if successful
*/
int kramStop( chandle c ) LONG_CALL;

//! Set frequency
/*!
Sets frequency of an active channel.
Note that if the channel has already stopped this call will not do anything and return false.
\param c Channel handle
\param freq Frequency in hertz
\return true if successful
*/
int kramSetFreq( chandle c, unsigned int freq ) LONG_CALL;

//! Set volume
/*!
Sets volume of an active channel.
Note that if the channel has already stopped this call will not do anything and return false.
\param c Channel handle
\param vol Volume (0..64)
\return true if successful
*/
int kramSetVol( chandle c, unsigned int vol ) LONG_CALL;

//! Set panning
/*!
Sets the panning-position of an active channel.
Note that if the channel has already stopped this call will not do anything and return false.
\param c Channel handle
\param pan Panning (-64..0..64), KRAM_SP_LEFT, KRAM_SP_RIGHT, KRAM_SP_CENTER
\return true if successful
*/
int kramSetPan( chandle c, int pan ) LONG_CALL;
#define KRAM_SP_LEFT -64
#define KRAM_SP_CENTER 0
#define KRAM_SP_RIGHT 64
//@}

//! Set Position
/*!
Sets the sample-position of an active channel.
Note that if the channel has already stopped this call will not do anything and return false.
\param c Channel handle
\param pos Sample offset to set
\return true if successful
*/
int kramSetPos( chandle c, unsigned int pos ) LONG_CALL;

//! Set SFX volume
/*!
Sets the volume of all active and future sfx.
\param vol Volume (0..128)
*/
void kramSetSFXVol( unsigned int vol );

//! Set Master Clip volume
/*!
Sets the clipping curve's steepness, you will mainly use this for
some additional gain.
\param vol 128 is normal, everything below/above changes the clipping curve
*/
void kramSetMasterVol( unsigned int vol );

//! Directsound Interrupt
/*!
This function resets the DMA and must be tied to the
Timer1-IRQ.
*/
void kradInterrupt();

//! Activate Krawall
/*!
You only need to call this if you have called kradDeactivate().
*/
void kradActivate();

//! Deactivate Krawall
/*!
You might want to deactivate Krawall in order to write savegames
and stuff like that. Calling this will stop all DMA-operations
(and thus sound-output) until resumed by kradActivate().
*/
void kradDeactivate();



#ifdef __cplusplus
}
#endif

#endif


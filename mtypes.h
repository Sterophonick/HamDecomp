/***********************************************************
* KRAWALL Soundsystem
* Copyright (c) 2001,2002 Sebastian Kienzl <seb@riot.org>
************************************************************
* $Id: mtypes.h,v 1.3 2002/12/09 02:03:35 schleuss Exp $
************************************************************/

#ifndef __MTYPES_H__
#define __MTYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef __attribute__ ((packed)) struct {
	unsigned long 	loopLength;
	unsigned long 	end;
	unsigned short 	c2Freq;
	unsigned char  	volDefault;
	unsigned char  	looped;
	unsigned char	hq;
	signed char  	data[1];
} Sample;

typedef __attribute__ ((packed)) struct {
	unsigned short 	index[ 16 ];
	unsigned char 	data[1];
} Pattern;

typedef __attribute__ ((packed)) struct {
	unsigned char 	numOrders;
	unsigned char 	order[ 256 ];

	unsigned char 	channels;
	signed char 	channelPan[ 32 ];

	unsigned char 	songIndex[ 64 ];

	unsigned char 	volGlobal;

	unsigned char 	initSpeed;
	unsigned char 	initTempo;

	unsigned char 	flagVolOpt;
	unsigned char 	flagVolSlides;
	unsigned char 	flagAmigaLimits;

	const Pattern* 	patterns[1];
} Module;

extern const Sample* const samples[];

#ifdef __cplusplus
}
#endif

#endif


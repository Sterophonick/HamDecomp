//
// HAM (c) Emanuel Schleussinger 2001/2002
// all rights reserved.
//
// Includes Doxygen compatible documentation
// http://www.doxygen.org
//


/*
----------------------------------------------------------

    mygba.h     
                the definitive gba header file
	            part of the HAM project

                by tubooboo@ngine.de
                http://www.ngine.de/ham.html

    You are VERY welcome to contribute fixes/enhancements to this.
    Just email me on tubooboo@ngine.de and tell me about suggestions
    and/or fixes on HAM.

    Posts on the message board also welcome.

    Whats included:
    ---------------
	- setup section at the top of this file.
	  should be mainly self-configuring via the makefile, so
	  generally no need to touch it
    - Structured Memory Map defines
    - All kinds of GBA Hardware registers
      and nice little functions for you to
      use with them
      - Display
      - BG incl rotation (see "whats missing")
      - OBJ (sprites) incl rotation
      - Sound
      - Palettes
      - Window
      - Mosaic
      - Controller
      - Timer
      - DMA
      - Interrupts

    - some C style structs for you to use, suchas 
      for BGs,OBJs... these are accessed using the 
      functions in mygba_sys.c ...check them out.

    - some rather useful TOOL defines, have a look
	  at what you find useful.

    Whats still missing:
    --------------------

    - Serial IO

    Guide to attribute names:
    -------------------------

    R_       --> Register location
	          absolute adress of a register.
		  includes typecasting for direct c writes.
                  example: R_DISPCNT = value;

    V_       --> Value
	          A register related value used by macros 
                  and functions. Register Masks are also
                  treated as values, this is currently in-
                  consistent with C_ (constants), which they
                  should be really. Anyone want to clean up? :-)

    C_       --> Constant
                  Helpers that will never need change. holds
                  values for bitfields that are hard to remember
                  Also note, C_'s might get declared only once 
                  for a set of registers. If so, two rules apply:
                   - the C_ declaration always is in the first
                     register of the set.
                   - the number of the register (example BG0CNT,
                     BG1CNT) is set to C_BGXCNT.  

    F_       --> Function
	          a function returns a value suchas true

    M_       --> Macro
	          A macro does stuff on its own, suchas 
		  switching a display mode or enabling
		  sprites. This is what you will be using 
                  most of the time. Example:
                  M_INTMST_ENABLE will enable interrupts

    TOOL_    --> Tool function
                  Usually a collection of macros and/or
                  functions. Propably not the fastest on
                  earth, but very convienient. These are
                  found at the end of the file. For example
                  TOOL_INIT_ALL(1) sets up the display for
                  BGMode 1 and inits sound and interrupts.


    Notes:
    ------
    Some of the functions/macros are not the fastest on
    earth, i know that. Most of them are built for 
    convienience rather than speed, and will suffice
    in the most cases. 

    Also, please do not mail me about the fact that 
    using REG= A | B | C | D is faster than what 
    is being done here. I know that ;-) First of all, that 
    only applies sometimes (if you need to set whole bitfields), 
    and second of all, you can still do it on your own using
    the R_ defines. I do the same in my code, but it proves
    handy to have the base cases covered. 
    
    Disclaimer:
    -----------
    I'd like to be greeted in your demo if you use this
    header :) 

    Be fair.

    Also, this source file and any other part of HAM must NOT:

     - be changed and redistributed as new versions, I 
       would like to keep the main distribution point on
       my end. Help with any suggestions please, though. 
       You WILL be mentioned for any help you did on HAM,
       and also, you can join in the team.

---------------------------------------------------------
*/


#ifndef MYGBA_H
#define MYGBA_H


/* 
---------------------------------------------------
---------------------------------------------------	

      HAM Control defines.

      These defines control how this file is parsed
      and inserted into your code. Have a look at the
      individual docs for each define. Switch off the
      defines you do not need by setting them to 0.
      
      You can find the areas that setup these sections
      at the end of this file.
      
      !!!! IMPORTANT !!!!
      be VERY careful that these are only overrides for
      the values defined in system/master.mak . If you 
      don't know what you are doing, do not play with it.
      
---------------------------------------------------
---------------------------------------------------	
*/

/*
 HAM_DEBUGGER
 When set, enables the HAM machine monitor. Also turns
 on or off the ASSERT functionality. Basically, build with
 this until you are done :)
 Note: HAM_DEBUGGER is always set in the freeware version of HAMlib, and switched
 on as a default in the HAM standard makefile. It can only be fully switched off
 in the registered version (this will, for example, disable checking for assertions.)
*/
#define HAM_DEBUGGER            1

/*
 HAM_WITH_LIBHAM
 Needs to be set for libham support (already set if using standard HAM makefiles)
*/
#define HAM_WITH_LIBHAM         1




/* 
---------------------------------------------------
---------------------------------------------------	

      BASE Includes 

---------------------------------------------------
---------------------------------------------------	
*/

#include <stdarg.h>
#include <stdlib.h>
#include "string.h"

/* 
---------------------------------------------------
---------------------------------------------------	

 HAM internal switches

---------------------------------------------------
---------------------------------------------------	
*/

// enable/disable GBA multiboot

// this define forces the program to be linked to
// execute in EXWRAM. this means, you want to do that
// if your program should run as a slave GBA program, 
// or using the MBV2 cable directly. Set this to 0 to
// force compilation to ROM space. HAM_MULTIBOOT is 
// set in system\master.mak 
// This can also be achieved by deselecting MBV2 support
// in the installer.
#ifdef HAM_MULTIBOOT

#ifndef MULTIBOOT
#define MULTIBOOT volatile const int __gba_multiboot = 1;  
#endif

#else

#define MULTIBOOT 

#endif

// this enables the so-called MBV2 console mode. you 
// only want to enable this if you have an MBV2 cable
// and run the MB.exe that came with it with the -c
// flag. This will give you printf capability over
// the serial line onto your console. HAM_DEBUGGER is
// set in system\master.mak
// This option only makes sense if you have a MBV2.
#ifdef HAM_DEBUGGER
#define HAM_DEBUG_MODE 1 
#endif


/* 
---------------------------------------------------
---------------------------------------------------	

      BASE Defines 

---------------------------------------------------
---------------------------------------------------	
*/

#define HAM_CURRENT_VERSION_MAJOR 2
#define HAM_CURRENT_VERSION_MINOR 80


// linker related

/*
--------------------------
IN_<X>WRAM defines. 

These defines should be written at the end of C variable 
declaration to force them into a particular memory space, 
as IWRAM or EWRAM. 

An example: 
char myarray[3] MEM_IN_EWRAM = {0,0,0};

This (due to how the GCC linker works) can ONLY work for initialized
data! This will NOT work:

char myarray[3] MEM_IN_EWRAM ;


You can use this to dictate where functions 
go in your memory. Just set the location in the 
prototype.

An example:
void myfunction(void) MEM_FUNC_IN_IWRAM;
--------------------------
*/
#define MEM_IN_EWRAM __attribute__ ((section (".ewram"))) = {0}
#define MEM_IN_IWRAM __attribute__ ((section (".iwram"))) = {0}
#define MEM_FUNC_IN_IWRAM __attribute__ ((section (".iwram"), long_call))


// HAM typedefs

// bool is a standard type in cplusplus, but not in c.    
#ifndef __cplusplus
typedef     unsigned char           bool;
#endif
typedef     unsigned char           u8;
typedef     unsigned short int      u16;
typedef     unsigned int            u32;
typedef     unsigned long long int  u64;

typedef     signed char             s8;
typedef     signed short int        s16;
typedef     signed int              s32;
typedef     signed long long int    s64;

typedef     volatile unsigned char           vu8;
typedef     volatile unsigned short int      vu16;
typedef     volatile unsigned int            vu32;
typedef     volatile unsigned long long int  vu64;

typedef     volatile signed char             vs8;
typedef     volatile signed short int        vs16;
typedef     volatile signed int              vs32;
typedef     volatile signed long long int    vs64;

// fixed point math typedefs

typedef     s16     sfp16;  //1:7:8 fixed point
typedef     s32     sfp32;  //1:19:8 fixed point
typedef     u16     ufp16;  //8:8 fixed point
typedef     u32     ufp32;  //24:8 fixed point

// alias type typedefs 

#define FIXED s32

// boolean defines

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

// some non HAM conform defines for debuglib
// a bit dirty to put them here, but bear with
// them and ignore them if you dont use MBV2 and debuglib.
// they won't take any space, so... :)

#define REG_BASE		0x04000000
#define REG_SIOCNT		(REG_BASE+0x128)
#define REG_SIODATA8	(REG_BASE+0x12a)
#define REG_RCNT		(REG_BASE+0x134)

/*
--------------------------------------------------------
 conversion defines
--------------------------------------------------------
*/
#define RGB(r,g,b) ((((b)>>3)<<10)+(((g)>>3)<<5)+((r)>>3))
#define RGB_SET(r,g,b) (((b)<<10)+((g)<<5)+(r));

#define RGB_GET_R_VALUE(rgb)    ((rgb & 0x001f) << 3)
#define RGB_GET_G_VALUE(rgb)    (((rgb >> 5) & 0x001f) << 3)
#define RGB_GET_B_VALUE(rgb)    (((rgb >> 10) & 0x001f) << 3)

#define PI 3.1415926
#define RADIAN(n) (((n)*PI)/180)

// be aware that SFP32 format is 1:15:16
// be aware that SFP16 format is 1:7:8

#define CONV_U8_TO_UFP16(n)     ((ufp16)(n<<8))
#define CONV_U8_TO_UFP32(n)     ((ufp32)(n<<8))
#define CONV_U16_TO_UFP32(n)    ((ufp32)(n<<8))
#define CONV_S8_TO_SFP16(n)     ((sfp16)(n<<8))
#define CONV_S8_TO_SFP32(n)     ((sfp32) ( ((n & 0x7F)<<8) | ((n & 0x80)<<24) ) )
#define CONV_S16_TO_SFP32(n)    ((sfp32)((n) << 16))

#define CONV_UFP16_TO_U8(n)     ((u8)((n)>>8))
#define CONV_UFP32_TO_U8(n)     ((u8)((n)>>8))
#define CONV_UFP32_TO_U16(n)    ((u16)((n)>>8))
#define CONV_SFP16_TO_S8(n)     ((s8)((n)>>8))
#define CONV_SFP32_TO_S8(n)     ((s8) (((n) & 0x7F0000)>>16)   | (((n) & 0x80000000)>>24))
#define CONV_SFP32_TO_S16(n)    ((s16)((n) >> 16))

#define CONV_FLOAT_TO_SFP16(n)  ((sfp16)((n)*256))
#define CONV_FLOAT_TO_SFP32(n)  ((sfp32)((n)*65536))

#define MUL_SFP32(m,n)			(((m)>>8)*((n)>>8))
#define DIV_SFP32(m,n)			(((m)<<8)/((n)>>8))

#define SIZEOF_8BIT(x)          (sizeof(x))
#define SIZEOF_16BIT(x)         (sizeof(x)/2)
#define SIZEOF_32BIT(x)         (sizeof(x)/4)

#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000
#define BIT_SET(value,bit) ((value) & (bit))

#define NOT_BIT0  0xFFFE
#define NOT_BIT1  0xFFFD
#define NOT_BIT2  0xFFFB
#define NOT_BIT3  0xFFF7
#define NOT_BIT4  0xFFEF
#define NOT_BIT5  0xFFDF
#define NOT_BIT6  0xFFBF
#define NOT_BIT7  0xFF7F
#define NOT_BIT8  0xFEFF
#define NOT_BIT9  0xFDFF
#define NOT_BIT10 0xFBFF
#define NOT_BIT11 0xF7FF
#define NOT_BIT12 0xEFFF
#define NOT_BIT13 0xDFFF
#define NOT_BIT14 0xBFFF
#define NOT_BIT15 0x7FFF
#define NOT_BIT_SET(value,bit) (!((value) & (bit)))


/* 
---------------------------------------------------
---------------------------------------------------

    BASE Globals

    System wide external variables/arrays 
    (initialized in mygba_sys.c)
---------------------------------------------------
---------------------------------------------------	
*/
#ifdef HAM_WITH_LIBHAM
extern u32 IntrTable[];
#endif //HAM_WITH_LIBHAM
/*
---------------------------------------------------
      BG defines
      These are helper vals for the BG functions
      in HAMlib
---------------------------------------------------	
*/

#define BG_0            (0)
#define BG_1            (1)
#define BG_2            (2)
#define BG_3            (3)

#define BG_PRIO_0       (0)
#define BG_PRIO_1       (1)
#define BG_PRIO_2       (2)
#define BG_PRIO_3       (3)

#define BG_ACTIVE_ON    (1)
#define BG_ACTIVE_OFF   (0)

#define BG_MOSAIC_ON    (1)
#define BG_MOSAIC_OFF   (0)

#define BG_CBB_ONLY_MODE_OFF  0
#define BG_CBB_ONLY_MODE_ON   1

#define BG_COLORS_16          0
#define BG_COLORS_256         1

#define BG_ZOOM_DEFAULT       0x100

#define MAP_NORMAL            0
#define MAP_ROTATION_OFF      0
#define MAP_ROTATION_ON       1

/*
   Helper defines for non rotation maps
   Mapsize is specified in TILES
*/
#define MAP_SIZE_32X32        0  // (256*256 screen)
#define MAP_SIZE_64X32        1  // (512*256 screen)
#define MAP_SIZE_32X64        2  // (256*512 screen)
#define MAP_SIZE_64X64        3  // (512*512 screen)

#define MAP_U16SIZE_32X32     1024
#define MAP_U16SIZE_64X32     2048
#define MAP_U16SIZE_32X64     2048
#define MAP_U16SIZE_64X64     4096

/*
   Helper defines for rotation maps
   Mapsize is specified in TILES
*/
#define MAP_SIZE_ROT_16X16    0  // (128*128 screen)
#define MAP_SIZE_ROT_32X32    1  // (256*256 screen)
#define MAP_SIZE_ROT_64X64    2  // (512*512 screen)
#define MAP_SIZE_ROT_128X128  3  // (1024*1024 screen)


/* 
---------------------------------------------------
      WIN defines
      These are helper vals for the Window functions
      in HAMlib
---------------------------------------------------	
*/

#define WIN_BG0 0x01
#define WIN_BG1 0x02
#define WIN_BG2 0x04
#define WIN_BG3 0x08
#define WIN_OBJ 0x10


/* 
---------------------------------------------------
      OBJ defines
      These are helper vals for the OBJ functions
      in HAMlib
---------------------------------------------------	
*/

#define OBJ_MODE_NORMAL             0x00
#define OBJ_MODE_SEMITRANSPARENT    0x01
#define OBJ_MODE_OBJWINDOW          0x02

#define OBJ_SIZE_8X8                 0,0
#define OBJ_SIZE_16X16               0,1
#define OBJ_SIZE_32X32               0,2
#define OBJ_SIZE_64X64               0,3

#define OBJ_SIZE_16X8                1,0
#define OBJ_SIZE_32X8                1,1
#define OBJ_SIZE_32X16               1,2
#define OBJ_SIZE_64X32               1,3

#define OBJ_SIZE_8X16                2,0
#define OBJ_SIZE_8X32                2,1
#define OBJ_SIZE_16X32               2,2
#define OBJ_SIZE_32X64               2,3

/*
---------------------------------------------------
      Palette color defines
      These are helper vals for the Palette functions
      named ham_SetBgPalCol() and ham_SetObjPalCol()
      in HAMlib.
      Thanks to Peter Schraut for these values.
---------------------------------------------------	
*/
#define COLOR_BLACK        0x0000
#define COLOR_MAROON       0x0010
#define COLOR_GREEN        0x0200
#define COLOR_OLIVE        0x0210
#define COLOR_NAVY         0x4000
#define COLOR_PURPLE       0x4010
#define COLOR_TEAL         0x4200
#define COLOR_GRAY         0x4210
#define COLOR_SILVER       0x6318
#define COLOR_RED          0x001F
#define COLOR_LIME         0x03E0
#define COLOR_YELLOW       0x03FF
#define COLOR_BLUE         0x7C00
#define COLOR_FUCHSIA      0x7C1F
#define COLOR_WHITE        0x7FFF
#define COLOR_MONEYGREEN   0x6378
#define COLOR_SKYBLUE      0x7B34
#define COLOR_CREAM        0x7BFF
#define COLOR_MEDGRAY      0x5294

/*
---------------------------------------------------
      Special FX defines
      These are helper vals for the ham_Fx functions
      in HAMlib
---------------------------------------------------	
*/

// different Blending Modes available
#define FX_MODE_OFF         0
#define FX_MODE_ALPHABLEND  1
#define FX_MODE_LIGHTEN     2
#define FX_MODE_DARKEN      3

// Feed this Macro 0 or 1 to select / deselect a certain Layer
// this is a helper macro that can be used with ham_SetFxMode()
#define FX_LAYER_SELECT(BG0,BG1,BG2,BG3,OBJ,BD) \
        ( \
         (BG0)       | \
        ((BG1) << 1) | \
        ((BG2) << 2) | \
        ((BG3) << 3) | \
        ((OBJ) << 4) | \
        ((BD)  << 5)    \
        )
/* 
---------------------------------------------------
      GFX defines
      Just some general stuff on GBA GFX hardware
      that is not related to any registers.
---------------------------------------------------	
*/

#define GBA_SCREEN_WIDTH 240
#define GBA_SCREEN_HEIGHT 160

#define GFX_MODE3_WIDTH  240
#define GFX_MODE4_WIDTH  240
#define GFX_MODE5_WIDTH  160
#define GFX_MODE3_HEIGHT 160
#define GFX_MODE4_HEIGHT 160
#define GFX_MODE5_HEIGHT 120

/* 
---------------------------------------------------
      DMA defines
      Just some general stuff on GBA DMA hardware
      that is not related to any specific registers.
---------------------------------------------------	
*/


#define DMA_STARTAT_NOW			0
#define DMA_STARTAT_VBL			1
#define DMA_STARTAT_HBL			2
#define DMA_STARTAT_SOUND_FIFO	3


#define DMA_TRANSFER_16BIT 0
#define DMA_TRANSFER_32BIT 1

/* 
---------------------------------------------------

      Music defines 
	  (GBC/GBA specific frequency table)
      Very experimental, only for playing around
	  Actually thought of removing these, but if you
	  wanna do some silly music, you might like them :)
---------------------------------------------------	
*/

#define SND_FREQU_A_1 1750
#define SND_FREQU_H_1 1782
#define SND_FREQU_C_2 1797
#define SND_FREQU_CIS_2 1811
#define SND_FREQU_D_2 1824 
#define SND_FREQU_DIS_2 1837
#define SND_FREQU_E_2 1849
#define SND_FREQU_F_2 1860
#define SND_FREQU_FIS_2 1870
#define SND_FREQU_G_2 1880
#define SND_FREQU_GIS_2 1890
#define SND_FREQU_A_2 1899
#define SND_FREQU_CIS_2 1811


/* 
---------------------------------------------------

    Interrupt table enumeration

---------------------------------------------------	
*/

#define INT_TYPE_VBL    0
#define INT_TYPE_HBL    1
#define INT_TYPE_VCNT   2
#define INT_TYPE_TIM0   3
#define INT_TYPE_TIM1   4
#define INT_TYPE_TIM2   5
#define INT_TYPE_TIM3   6
#define INT_TYPE_SIO    7
#define INT_TYPE_DMA0   8
#define INT_TYPE_DMA1   9
#define INT_TYPE_DMA2   10
#define INT_TYPE_DMA3   11
#define INT_TYPE_KEY    12
#define INT_TYPE_CART   13

/*
---------------------------------------------------

Save RAM types supported by HAMlib. See the Save RAM
documentation for details

---------------------------------------------------	
*/
#define RAM_TYPE_SRAM_256K   0x00
#define RAM_TYPE_EEPROM_4K   0x01
#define RAM_TYPE_EEPROM_64K  0x02
#define RAM_TYPE_UNDEFINED   0xFF


/*
 #########################################################################################
 #########################################################################################
 #########################################################################################

  External system includes

 #########################################################################################
 #########################################################################################
 #########################################################################################
*/

// since v2.52, Krawall is included in HAM :)
#include "krawall.h"
// since HAM v3 tUME support is built into HAM!
#include "tumeric.h"


/*
 #########################################################################################
 #########################################################################################
 #########################################################################################

    A G B   Hardware  Macros

    main section of this document. Implements most of the hardware functionality in
	the GBA. This is the real meat of HAM, study them closely by looking the sample
	programs and how they use these macros.

    Not all of the macros in here have been tested, but most of them should  work 
	just ok. Any feedback is highly appreciated.

 #########################################################################################
 #########################################################################################
 #########################################################################################  
*/


/* 
---------------------------------------------------
---------------------------------------------------	

      Memory Map related 

---------------------------------------------------
---------------------------------------------------	
*/

/*
------------------------------
Memory Map Base locations
------------------------------
*/


//Memory locations

#define MEM_SYSROM			0x00000000	// System ROM, for BIOS Calls
#define MEM_EXRAM			0x02000000	// External WRAM, slow, also used for Multiboot uploads
#define MEM_RAM				0x03000000	// Fast CPU internal RAM
#define MEM_IO				0x04000000	// Register Base, all HW Registers are in here.
#define MEM_PAL				0x05000000	// Palette Base
#define MEM_PAL_BG			0x05000000	// Palette for BG
#define MEM_PAL_OBJ			0x05000200	// Palette for OBJ
#define MEM_VRAM			0x06000000	// GBA Video RAM
#define MEM_BG			    0x06000000	// GBA Video RAM (in BG mode)
#define MEM_OBJ             0x06010000  // OBJ memoryspace (32 kBytes)
#define MEM_OAM				0x07000000	// Object control space for sprites
#define MEM_ROM0			0x08000000	// Rom Space 0 (fastest, 0 wait)
#define MEM_ROM1			0x0A000000	// Rom Space 1 (1 wait)
#define MEM_ROM2			0x0C000000	// Rom Space 2 (slowest, 2 wait)
#define MEM_EEPROM			0x0D000000	// Gamepak EEPROM, if any.
#define MEM_SRAM			0x0E000000	// Gamepak SRAM, if any.

// Base block locations

#define MEM_PAL_BG_16(x)    (MEM_PAL_BG	 + ((x)<<5))
#define MEM_PAL_OBJ_16(x)   (MEM_PAL_OBJ + ((x)<<5))

//this define returns a pointer to either the bg palette (0) or the obj palette (1)
#define MEM_PAL_256(x)      (MEM_PAL_BG	 + ((x)<<9))
//the following defines return the memory address of a color palette index
#define MEM_PAL_BG_COL_256(x)   (MEM_PAL_BG	 + ((x)<<1))
#define MEM_PAL_OBJ_COL_256(x)  (MEM_PAL_OBJ + ((x)<<1))

#define MEM_CHR_BB(x)       (MEM_BG + ((x)<<14))
#define MEM_SCR_BB(x)       (MEM_BG + ((x)<<11))
#define MEM_HAMBG_SLOT(x)   (MEM_BG + ((x)<<11))
#define MEM_OAM_ENTRY(x)	(MEM_OAM+ ((x)<<3))
#define MEM_HAMOBJ_SLOT(x)  (MEM_OBJ+ ((x)<<5))

#define MEM_SYSROM_SIZE		    0x00004000
#define MEM_EXRAM_SIZE		    0x00040000
#define MEM_RAM_SIZE		    0x00008000
#define MEM_PAL_SIZE		    0x00000400
#define MEM_VRAM_SIZE		    0x00018000
#define MEM_BG_MODE0_SIZE		0x00010000
#define MEM_BG_MODE1_SIZE		0x00010000
#define MEM_BG_MODE2_SIZE		0x00010000
#define MEM_BG_MODE3_BUFSIZE    0x00014000
#define MEM_BG_MODE4_BUFSIZE    0x0000A000
#define MEM_BG_MODE5_BUFSIZE    0x0000A000
#define MEM_BG_SIZE		        0x00010000
#define MEM_BG_SIZE		        0x00010000
#define MEM_OBJ_SIZE            0x00008000
#define MEM_OAM_SIZE		    0x00000400
#define MEM_ROM0_SIZE		    0x02000000
#define MEM_ROM1_SIZE		    0x02000000
#define MEM_ROM2_SIZE		    0x02000000
#define MEM_SRAM_SIZE		    0x00010000

// Misc important mem locations

#define MEM_SNDFIFO_A			(MEM_IO + 0xA0)
#define MEM_SNDFIFO_B			(MEM_IO + 0xA4)


// Memory Pointers

#define MEM_SYSROM_PTR			 (u8*)  0x00000000	// System ROM, for BIOS Calls
#define MEM_EXRAM_PTR			 (u8*)  0x02000000	// External WRAM, slow, also used for Multiboot uploads
#define MEM_RAM_PTR				 (u8*)  0x03000000	// Fast CPU internal RAM
#define MEM_IO_PTR				 (u8*)  0x04000000	// Register Base, all HW Registers are in here.
#define MEM_PAL_PTR				 (u16*) 0x05000000	// Palette Base
#define MEM_PAL_BG_PTR			 (u16*) 0x05000000	// Palette for BG start
#define MEM_PAL_OBJ_PTR			 (u16*) 0x05000200	// Palette for OBJ start
#define MEM_PAL_COL_PTR(x)		 (u16*) (0x05000000+(x<<1))	// Palette color pointer
#define MEM_VRAM_PTR			 (u16*) 0x06000000	// GBA Video RAM
#define MEM_BG_PTR	             (u16*) 0x06000000	// GBA Video RAM
#define MEM_BG_MODE0_PTR	     (u16*) 0x06000000	// GBA Video RAM
#define MEM_BG_MODE1_PTR	     (u16*) 0x06000000	// GBA Video RAM
#define MEM_BG_MODE2_PTR	     (u16*) 0x06000000	// GBA Video RAM
#define MEM_BG_MODE3_PTR	     (u16*) 0x06000000	// GBA Video RAM
#define MEM_BG_MODE4_PTR(buf)    (u16*) (0x06000000+ (buf) * 0xA000)	// GBA Video RAM
#define MEM_BG_MODE5_PTR(buf)    (u16*) (0x06000000+ (buf) * 0xA000)	// GBA Video RAM
#define MEM_OBJ_PTR              (u16*) 0x06010000   // OBJ memoryspace (32 kBytes)
#define MEM_OAM_PTR				 (u8*)  0x07000000	// Object control space for sprites
#define MEM_ROM0_PTR			 (u8*)  0x08000000	// Rom Space 0 (fastest, 0 wait)
#define MEM_ROM1_PTR			 (u8*)  0x0A000000	// Rom Space 1 (1 wait)
#define MEM_ROM2_PTR			 (u8*)  0x0C000000	// Rom Space 2 (slowest, 2 wait)
#define MEM_SRAM_PTR			 (u8*)  0x0E000000	// Gamepak SRAM, if any.

// Base block pointers to Screen and Character BBs

#define MEM_CHR_BB_PTR(x)       ((u8*) MEM_BG + ((x)*0x4000))
#define MEM_SCR_BB_PTR(x)       ((u8*) MEM_BG + ((x)*0x800))

// Pointers to OAM entries
#define MEM_OAM_ENTRY_PTR(x)	 ((u16*) MEM_OAM_ENTRY(x))
#define MEM_OAM_ATR0_PTR(x)		 ((u16*) (MEM_OAM_ENTRY(x)+0))
#define MEM_OAM_ATR1_PTR(x)		 ((u16*) (MEM_OAM_ENTRY(x)+2))
#define MEM_OAM_ATR2_PTR(x)		 ((u16*) (MEM_OAM_ENTRY(x)+4))
#define MEM_OAM_ROTATR_PTR(set,atrno) ((u16*) (MEM_OAM_ENTRY(set<<2)+(atrno<<3)+6))

// Base block pointers for the HAM BG memory manager

#define MEM_HAMBG_SLOT_PTR(x)       ((u16*) MEM_HAMBG_SLOT(x))
#define MEM_HAMOBJ_SLOT_PTR(x)      ((u16*) MEM_HAMOBJ_SLOT(x))

// General access for memory locations

#define ACCESS_8(location)		*(volatile u8 *)  (location)
#define ACCESS_16(location)		*(volatile u16 *) (location)
#define ACCESS_32(location)		*(volatile u32 *) (location)

/* 
---------------------------------------------------
---------------------------------------------------	

      Hardware Register related 

---------------------------------------------------
---------------------------------------------------	
*/

#if 0
------------------------------
Display Registers
------------------------------
#endif

/* 
----------------------------------------------------
 IO 0x000 --	DISCNT		Display Control Register

	Controls Display features such as video modes
	and visibility of display components
----------------------------------------------------	
*/

#define R_DISCNT		*(volatile u16 *) MEM_IO 		// Display Control register

/* Masks */

#define V_DISCNT_BGMODE_MSK		0xFFF8			// BGMode, can be set to 1-5
#define V_DISCNT_BUFSEL_MSK		0xFFEF			// Display buffer selecton (0 or 1)
#define V_DISCNT_HBLOBJ_MSK		0xFFDF			// Enable OBJ processing in HBL (0=enable)
#define V_DISCNT_OBJMAP_MSK		0xFFBF			// OBJ mapping format (0=2d 1=1d)
#define V_DISCNT_FRCBLK_MSK		0xFF7F			// Force image system to show blank screen
#define V_DISCNT_BG0_MSK		0xFEFF			// Enable BG0 display
#define V_DISCNT_BG1_MSK		0xFDFF			// Enable BG1 display
#define V_DISCNT_BG2_MSK		0xFBFF			// Enable BG2 display
#define V_DISCNT_BG3_MSK		0xF7FF			// Enable BG3 display
#define V_DISCNT_OBJ_MSK		0xEFFF			// Enable OBJ display
#define V_DISCNT_WIN0_MSK		0xDFFF			// Enable Window 0 display
#define V_DISCNT_WIN1_MSK		0xBFFF			// Enable Window 1 display
#define V_DISCNT_WINO_MSK		0x7FFF			// Enable OBJ Window display

/* Functions */

#define F_DISCNT_BGMODE_GET     (R_DISCNT & 0x7)		//returns the current BGMode 
#define F_DISCNT_BUFSEL_GET     ((R_DISCNT & 0x10)>>4)	//returns the current BGMode4/5 buffer 

/* Macros */

#define M_DISCNT_BGMODE_SET(x)	(R_DISCNT &= V_DISCNT_BGMODE_MSK);\
								(R_DISCNT |= x);								// Set BGMODE(x)
#define M_DISCNT_BUFSEL(x)		(R_DISCNT &= V_DISCNT_BUFSEL_MSK); \
                                (R_DISCNT |= (x<<4));                           // Display buffer X
#define M_DISCNT_BUFSEL_0		(R_DISCNT &= V_DISCNT_BUFSEL_MSK);				// Display buffer 0
#define M_DISCNT_BUFSEL_1		(R_DISCNT |= 0x0010);							// Display buffer 1
#define M_DISCNT_HBLOBJ_ENA		(R_DISCNT &= V_DISCNT_HBLOBJ_MSK)				// Enable OBJ processing
#define M_DISCNT_HBLOBJ_DIS		(R_DISCNT |= 0x0020);							// Disable OBJ processing
#define M_DISCNT_OBJMAP_2D		(R_DISCNT &= V_DISCNT_OBJMAP_MSK);				// OBJ mapping format (0=2d 1=1d)
#define M_DISCNT_OBJMAP_1D		(R_DISCNT |= 0x0040);							// OBJ mapping format (0=2d 1=1d)
#define M_DISCNT_FRCBLK_DIS		(R_DISCNT &= V_DISCNT_FRCBLK_MSK); 				// Image system normal operation
#define M_DISCNT_FRCBLK_ENA		(R_DISCNT |= 0x0080);							// Image system forced to blank
#define M_DISCNT_BG0_OFF		(R_DISCNT &= V_DISCNT_BG0_MSK);					// Disable BG0 display
#define M_DISCNT_BG0_ON			(R_DISCNT |= 0x0100);							// Enable BG0 display
#define M_DISCNT_BG1_ON			(R_DISCNT |= 0x0200);							// Enable BG1 display
#define M_DISCNT_BG1_OFF		(R_DISCNT &= V_DISCNT_BG1_MSK);					// Disable BG1 display
#define M_DISCNT_BG2_ON			(R_DISCNT |= 0x0400);							// Enable BG2 display
#define M_DISCNT_BG2_OFF		(R_DISCNT &= V_DISCNT_BG2_MSK);					// Disable BG2 display
#define M_DISCNT_BG3_ON			(R_DISCNT |= 0x0800);							// Enable BG3 display
#define M_DISCNT_BG3_OFF		(R_DISCNT &= V_DISCNT_BG3_MSK);					// Disable BG3 display
#define M_DISCNT_OBJ_ON			(R_DISCNT |= 0x1000);							// Enable OBJ display
#define M_DISCNT_OBJ_OFF		(R_DISCNT &= V_DISCNT_OBJ_MSK);					// Disable OBJ display
#define M_DISCNT_WIN0_ON		(R_DISCNT |= 0x2000);							// Enable WIN0 display
#define M_DISCNT_WIN0_OFF		(R_DISCNT &= V_DISCNT_WIN0_MSK);				// Disable WIN0 display
#define M_DISCNT_WIN1_ON		(R_DISCNT |= 0x4000);							// Enable WIN1 display
#define M_DISCNT_WIN1_OFF		(R_DISCNT &= V_DISCNT_WIN1_MSK);				// Disable WIN1 display
#define M_DISCNT_WINO_ON		(R_DISCNT |= 0x8000);							// Enable WINO display
#define M_DISCNT_WINO_OFF		(R_DISCNT &= V_DISCNT_WINO_MSK);				// Disable WINO display

// this macro sets the window no specified to ON or OFF (1 or 0)
#define M_DISCNT_WINX_SET(winno,active) (R_DISCNT &= ((0xDFFF<<winno) | 0xFF ) ); \
                                        (R_DISCNT |= (active<<13)<<winno);
// this macro turns a specific BG display ON or OFF (1 or 0)
#define M_DISCNT_BGX_SET(x,active)      (R_DISCNT &= ((0xFEFF<<x) | 0xFF ) ); \
                                        (R_DISCNT |= (active<<8)<<x);



/* 
--------------------------------------------------
  IO 0x006 --	VCNT		Vertical Line Counter
--------------------------------------------------
*/

#define R_VCNT			*(volatile u16 *)(MEM_IO + 0x06)		// Vertical counter(0-227)

/* Masks */
#define V_VCNT_MSK			0x00FF			        // Mask for the value.

/* Functions */
#define F_VCNT_CURRENT_SCANLINE R_VCNT

/* 
---------------------------------------------------
  IO 0x004 --	DISSTAT		Display Status Register

	Controls Display features such as blanking,
	and Display interrupts.
---------------------------------------------------	
*/

#define R_DISSTAT		*(volatile u16 *)(MEM_IO + 0x04)		// Display Status register

/* Constants */
#define V_DISSTAT_VBLSTAT		0x0001			// Vertical Blank Status, 1 if Vblank is active
#define V_DISSTAT_HBLSTAT		0x0002			// Horizontal Blank Status, 1 if Hblank is active
#define V_DISSTAT_VCNTEVAL		0x0004			// 1 if V_DISSTAT_VCNT matches current scanline

/* Masks */
#define V_DISSTAT_VBLIRQR_MSK	0xFFF7			// Set VBL IRQ Request
#define V_DISSTAT_HBLIRQR_MSK	0xFFEF			// Set HBL IRQ Request
#define V_DISSTAT_VCNTIRQR_MSK	0xFFDF			// Set VCNT match IRQ Request
#define V_DISSTAT_VCNT_MSK		0x00FF			// Set scanline to check for with V_DISSTAT_VCNTEVAL

/* Functions */
#define F_DISSTAT_VBL_ACTIVE	(R_DISSTAT & V_DISSTAT_VBLSTAT)				// true if in VBL
#define F_DISSTAT_HBL_ACTIVE	(R_DISSTAT & V_DISSTAT_HBLSTAT)				// true if in HBL
#define F_DISSTAT_VCNT_ACTIVE	(R_DISSTAT & V_DISSTAT_VCNTEVAL)			// true if scanline matches VCNT
#define F_DISSTAT_VCNT_GET		((R_DISSTAT & V_DISSTAT_VCNT_MSK)>>8)		// returns VCNT


/* Macros */
#define M_DISSTAT_VBLIRQR_ON			(R_DISSTAT |= 0x0008);					//turns on VBL IRQ
#define M_DISSTAT_VBLIRQR_OFF			(R_DISSTAT &= V_DISSTAT_VBLIRQR_MSK );	//turns off VBL IRQ
#define M_DISSTAT_HBLIRQR_ON			(R_DISSTAT |= 0x0010);					//turns on HBL IRQ
#define M_DISSTAT_HBLIRQR_OFF			(R_DISSTAT &= V_DISSTAT_HBLIRQR_MSK );	//turns off HBL IRQ
#define M_DISSTAT_VCNTIRQR_ON			(R_DISSTAT |= 0x0020);					//turns on VCNT IRQ
#define M_DISSTAT_VCNTIRQR_OFF			(R_DISSTAT &= V_DISSTAT_VCNTIRQR_MSK );	//turns off VCNT IRQ
#define M_DISSTAT_VCNT_SET(scanline)	(R_DISSTAT &= V_DISSTAT_VCNT_MSK);\
										(R_DISSTAT |= (scanline<<8));			//sets VCNT to scanline(x)



/* 
---------------------------------------------------
  IO 0x008 --	BG0CNT		Background 0 control register

  Controls the display behaviour of BG0
---------------------------------------------------	
*/

#define R_BG0CNT		*(volatile u16 *)(MEM_IO + 0x08)

/* Constants */
#define C_BGXCNT_SCRSIZE_256X256 0x00
#define C_BGXCNT_SCRSIZE_512X256 0x01
#define C_BGXCNT_SCRSIZE_256X512 0x02
#define C_BGXCNT_SCRSIZE_512X512 0x03


/* Masks */
#define V_BG0CNT_SCRSIZE_MSK    0x3FFF  //screensize, can be set to %00-%11
#define V_BG0CNT_SCRBB_MSK      0xE0FF  //screen base block in VRAM (0-31, 2kb incr.)
#define V_BG0CNT_CHRBB_MSK      0xFFF3  //character base block in VRAM (0-3, 16k incr.)
#define V_BG0CNT_PRIO_MSK       0xFFFD  //this bg's priority (0=highest 3=lowest)

/* Functions */

/* Macros */
#define M_BG0CNT_COLMODE_SET_16     (R_BG0CNT &= 0xFF7F); //set mode to 16 colrs / 16 pals
#define M_BG0CNT_COLMODE_SET_256    (R_BG0CNT |= 0x0080); //set mode to 256 colrs / 1 pal
#define M_BG0CNT_MOSAIC_SET_OFF     (R_BG0CNT &= 0xFFBF); //disable mosaic
#define M_BG0CNT_MOSAIC_SET_ON      (R_BG0CNT |= 0x0040); //enable mosaic
#define M_BG0CNT_SCRSIZE_SET(size)  (R_BG0CNT &= V_BG0CNT_SCRSIZE_MSK);\
									(R_BG0CNT |= (size<<14));	//sets scrsize to size
#define M_BG0CNT_SCRBB_SET(block)   (R_BG0CNT &= V_BG0CNT_SCRBB_MSK);\
								    (R_BG0CNT |= (block<<8));	//sets scrbb to block
#define M_BG0CNT_CHRBB_SET(block)   (R_BG0CNT &= V_BG0CNT_CHRBB_MSK);\
								    (R_BG0CNT |= (block<<2));	//sets chrbb to block
#define M_BG0CNT_PRIO_SET(prio)     (R_BG0CNT &= V_BG0CNT_PRIO_MSK);\
								    (R_BG0CNT |= prio);	        //sets prio to prio :)

/* 
---------------------------------------------------
  IO 0x00A --	BG1CNT		Background 1 control register

  Controls the display behaviour of BG1

    This BG cannot scale/rotate
    Also, for this mode there is no wrapping transparency

---------------------------------------------------	
*/

#define R_BG1CNT		*(volatile u16 *)(MEM_IO + 0x0A)

/* Constants */

/* Masks */
#define V_BG1CNT_SCRSIZE_MSK    0x3FFF  //screensize, can be set to %00-%11
#define V_BG1CNT_SCRBB_MSK      0xE0FF  //screen base block in VRAM (0-31, 2kb incr.)
#define V_BG1CNT_CHRBB_MSK      0xFFF3  //character base block in VRAM (0-3, 16k incr.)
#define V_BG1CNT_PRIO_MSK       0xFFFD  //this bg's priority (0=highest 3=lowest)

/* Functions */

/* Macros */
#define M_BG1CNT_COLMODE_SET_16     (R_BG1CNT &= 0xFF7F); //set mode to 16 colrs / 16 pals
#define M_BG1CNT_COLMODE_SET_256    (R_BG1CNT |= 0x0080); //set mode to 256 colrs / 1 pal
#define M_BG1CNT_MOSAIC_SET_OFF     (R_BG1CNT &= 0xFFBF); //disable mosaic
#define M_BG1CNT_MOSAIC_SET_ON      (R_BG1CNT |= 0x0040); //enable mosaic
#define M_BG1CNT_SCRSIZE_SET(size)  (R_BG1CNT &= V_BG1CNT_SCRSIZE_MSK);\
									(R_BG1CNT |= (size<<14));	//sets scrsize to size
#define M_BG1CNT_SCRBB_SET(block)   (R_BG1CNT &= V_BG1CNT_SCRBB_MSK);\
								    (R_BG1CNT |= (block<<8));	//sets scrbb to block
#define M_BG1CNT_CHRBB_SET(block)   (R_BG1CNT &= V_BG1CNT_CHRBB_MSK);\
								    (R_BG1CNT |= (block<<2));	//sets chrbb to block
#define M_BG1CNT_PRIO_SET(prio)     (R_BG1CNT &= V_BG1CNT_PRIO_MSK);\
								    (R_BG1CNT |= prio);	        //sets prio to prio :)


/* 
---------------------------------------------------
  IO 0x00C --	BG2CNT		Background 2 control register

  Controls the display behaviour of BG2

  This BG can scale/rotate in BGMode 1 and 2

  ---------------------------------------------------	
*/

#define R_BG2CNT		*(volatile u16 *)(MEM_IO + 0x0C)

/* Constants */

/* Masks */
#define V_BG2CNT_SCRSIZE_MSK    0x3FFF  //screensize, can be set to %00-%11
#define V_BG2CNT_SCRBB_MSK      0xE0FF  //screen base block in VRAM (0-31, 2kb incr.)
#define V_BG2CNT_CHRBB_MSK      0xFFF3  //character base block in VRAM (0-3, 16k incr.)
#define V_BG2CNT_PRIO_MSK       0xFFFD  //this bg's priority (0=highest 3=lowest)

/* Functions */

/* Macros */
#define M_BG2CNT_COLMODE_SET_16     (R_BG2CNT &= 0xFF7F); //set mode to 16 colrs / 16 pals
#define M_BG2CNT_COLMODE_SET_256    (R_BG2CNT |= 0x0080); //set mode to 256 colrs / 1 pal
#define M_BG2CNT_MOSAIC_SET_OFF     (R_BG2CNT &= 0xFFBF); //disable mosaic
#define M_BG2CNT_MOSAIC_SET_ON      (R_BG2CNT |= 0x0040); //enable mosaic
#define M_BG2CNT_SCRSIZE_SET(size)  (R_BG2CNT &= V_BG2CNT_SCRSIZE_MSK);\
									(R_BG2CNT |= (size<<14));	//sets scrsize to size
#define M_BG2CNT_SCRBB_SET(block)   (R_BG2CNT &= V_BG2CNT_SCRBB_MSK);\
								    (R_BG2CNT |= (block<<8));	//sets scrbb to block
#define M_BG2CNT_CHRBB_SET(block)   (R_BG2CNT &= V_BG2CNT_CHRBB_MSK);\
								    (R_BG2CNT |= (block<<2));	//sets chrbb to block
#define M_BG2CNT_PRIO_SET(prio)     (R_BG2CNT &= V_BG2CNT_PRIO_MSK);\
								    (R_BG2CNT |= prio);	        //sets prio to prio :)
#define M_BG2CNT_WRAP_SET_OFF       (R_BG2CNT &= 0xDFFF); //wrap around display off (transparent)
#define M_BG2CNT_WRAP_SET_ON        (R_BG2CNT |= 0x2000); //wrap around display on 



/* 
---------------------------------------------------
  IO 0x00E --	BG3CNT		Background 3 control register

  Controls the display behaviour of BG3

  This BG can scale/rotate in BGMode 1 and 2

  ---------------------------------------------------	
*/

#define R_BG3CNT		*(volatile u16 *)(MEM_IO + 0x0E)

/* Constants */

/* Masks */
#define V_BG3CNT_SCRSIZE_MSK    0x3FFF  //screensize, can be set to %00-%11
#define V_BG3CNT_SCRBB_MSK      0xE0FF  //screen base block in VRAM (0-31, 2kb incr.)
#define V_BG3CNT_CHRBB_MSK      0xFFF3  //character base block in VRAM (0-3, 16k incr.)
#define V_BG3CNT_PRIO_MSK       0xFFFD  //this bg's priority (0=highest 3=lowest)

/* Functions */

/* Macros */
#define M_BG3CNT_COLMODE_SET_16     (R_BG3CNT &= 0xFF7F); //set mode to 16 colrs / 16 pals
#define M_BG3CNT_COLMODE_SET_256    (R_BG3CNT |= 0x0080); //set mode to 256 colrs / 1 pal
#define M_BG3CNT_MOSAIC_SET_OFF     (R_BG3CNT &= 0xFFBF); //disable mosaic
#define M_BG3CNT_MOSAIC_SET_ON      (R_BG3CNT |= 0x0040); //enable mosaic
#define M_BG3CNT_SCRSIZE_SET(size)  (R_BG3CNT &= V_BG3CNT_SCRSIZE_MSK);\
									(R_BG3CNT |= (size<<14));	//sets scrsize to size
#define M_BG3CNT_SCRBB_SET(block)   (R_BG3CNT &= V_BG3CNT_SCRBB_MSK);\
								    (R_BG3CNT |= (block<<8));	//sets scrbb to block
#define M_BG3CNT_CHRBB_SET(block)   (R_BG3CNT &= V_BG3CNT_CHRBB_MSK);\
								    (R_BG3CNT |= (block<<2));	//sets chrbb to block
#define M_BG3CNT_PRIO_SET(prio)     (R_BG3CNT &= V_BG3CNT_PRIO_MSK);\
								    (R_BG3CNT |= prio);	        //sets prio to prio :)
#define M_BG3CNT_WRAP_SET_OFF       (R_BG3CNT &= 0xDFFF); //wrap around display off (transparent)
#define M_BG3CNT_WRAP_SET_ON        (R_BG3CNT |= 0x2000); //wrap around display on 



/* 
---------------------------------------------------
  IO 0x00x --	BGXCNT		Background X control register

  These are convienience macros for manipulating
  any of the 4 BG control registers.

  The first parameter in these macros is always x,
  the BGs number.
---------------------------------------------------	
*/
#define R_BGXCNT(x)                 (ACCESS_16(MEM_IO+8+(x*2)))

#define M_BGXCNT_COLMODE_SET(x,mode)    (R_BGXCNT(x) &= 0xFF7F); \
                                        (R_BGXCNT(x) |= (mode<<7));
#define M_BGXCNT_WRAP_SET(x,wrap)       (R_BGXCNT(x) &= 0xDFFF); \
                                        (R_BGXCNT(x) |= (wrap<<13));
#define M_BGXCNT_MOSAIC_SET(x,mode)     (R_BGXCNT(x) &= 0xFFBF); \
                                        (R_BGXCNT(x) |= (mode<<6));             
#define M_BGXCNT_SCRSIZE_SET(x,size)    (R_BGXCNT(x) &= V_BG0CNT_SCRSIZE_MSK);\
		    						    (R_BGXCNT(x) |= (size<<14));	        //sets scrsize to size
#define M_BGXCNT_SCRBB_SET(x,block)     (R_BGXCNT(x) &= V_BG0CNT_SCRBB_MSK);\
								        (R_BGXCNT(x) |= (block<<8));	        //sets scrbb to block
#define M_BGXCNT_CHRBB_SET(x,block)     (R_BGXCNT(x) &= V_BG0CNT_CHRBB_MSK);\
								        (R_BGXCNT(x) |= (block<<2));	        //sets chrbb to block
#define M_BGXCNT_PRIO_SET(x,prio)       (R_BGXCNT(x) &= V_BG0CNT_PRIO_MSK);\
								        (R_BGXCNT(x) |= prio);	                //sets prio to prio :)


/* 
---------------------------------------------------
  IO 0x10 --	R_BG0SCRLX  BG0 Horizontal offset
                scrolling register
---------------------------------------------------	
*/
#define R_BG0SCRLX		*(volatile u16 *)(MEM_IO + 0x10)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG0SCRLX_SET(xoff)    (R_BG0SCRLX = xoff);

/* 
---------------------------------------------------
  IO 0x14 --	R_BG1SCRLX  BG1 Horizontal offset
                scrolling register
---------------------------------------------------	
*/
#define R_BG1SCRLX		*(volatile u16 *)(MEM_IO + 0x14)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG1SCRLX_SET(xoff)    (R_BG1SCRLX = xoff);
/* 
---------------------------------------------------
  IO 0x18 --	R_BG2SCRLX  BG2 Horizontal offset
                scrolling register
---------------------------------------------------	
*/
#define R_BG2SCRLX		*(volatile u16 *)(MEM_IO + 0x18)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG2SCRLX_SET(xoff)    (R_BG2SCRLX = xoff);
/* 
---------------------------------------------------
  IO 0x1C --	R_BG3SCRLX  BG3 Horizontal offset
                scrolling register
---------------------------------------------------	
*/
#define R_BG3SCRLX		*(volatile u16 *)(MEM_IO + 0x1C)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG3SCRLX_SET(xoff)    (R_BG3SCRLX = xoff);


/* 
---------------------------------------------------
  IO 0x12 --	R_BG0SCRLY  BG0 Vertical offset
                scrolling register
---------------------------------------------------	
*/
#define R_BG0SCRLY		*(volatile u16 *)(MEM_IO + 0x12)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG0SCRLY_SET(xoff)    (R_BG0SCRLY = xoff);


/* 
---------------------------------------------------
  IO 0x16 --	R_BG1SCRLY  BG1 Vertical offset
                scrolling register
---------------------------------------------------	
*/
#define R_BG1SCRLY		*(volatile u16 *)(MEM_IO + 0x16)

/* Constants */
/* Masks */
#define V_BG1SCRLY_MSK  0xFE00
/* Functions */
/* Macros */
#define M_BG1SCRLY_SET(xoff)    (R_BG1SCRLY = xoff);

/* 
---------------------------------------------------
  IO 0x1A --	R_BG2SCRLY  BG2 Vertical offset
                scrolling register
---------------------------------------------------	
*/
#define R_BG2SCRLY		*(volatile u16 *)(MEM_IO + 0x1A)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG2SCRLY_SET(xoff)    (R_BG2SCRLY = xoff);

/* 
---------------------------------------------------
  IO 0x1E --	R_BG3SCRLY  BG3 Vertical offset
                scrolling register
---------------------------------------------------	
*/
#define R_BG3SCRLY		*(volatile u16 *)(MEM_IO + 0x1E)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG3SCRLY_SET(xoff)    (R_BG3SCRLY = xoff);


/* 
---------------------------------------------------
  IO 0x00x --	R_BGXROT..  rotation and scaling

  These are convienience macros for manipulating
  any of the 2 ROTDX control registers.

  NOTE: this will ONLY work with BG2 and BG3
  
  The first parameter in these macros is always x,
  the BGs number.
---------------------------------------------------	
*/
#define R_BGXROTDX(x)        (ACCESS_16(MEM_IO+((x)*0x10)))
#define R_BGXROTDMX(x)       (ACCESS_16(MEM_IO+0x2+((x)*0x10)))
#define R_BGXROTDY(x)        (ACCESS_16(MEM_IO+0x4+((x)*0x10)))
#define R_BGXROTDMY(x)       (ACCESS_16(MEM_IO+0x6+((x)*0x10)))
#define R_BGXROTX(x)         (ACCESS_32(MEM_IO+0x8+((x)*0x10)))
#define R_BGXROTY(x)         (ACCESS_32(MEM_IO+0xC+((x)*0x10)))

/* Macros */
#define M_BGXROTDX_SET(x,val)    R_BGXROTDX(x)  = (val);
#define M_BGXROTDMX_SET(x,val)   R_BGXROTDMX(x) = (val);
#define M_BGXROTDY_SET(x,val)    R_BGXROTDY(x)  = (val);
#define M_BGXROTDMY_SET(x,val)   R_BGXROTDMY(x) = (val);
#define M_BGXROTX_SET(x,val)     R_BGXROTX(x)   = (val);
#define M_BGXROTY_SET(x,val)     R_BGXROTY(x)   = (val);

/* 
---------------------------------------------------
  IO 0x20 --	R_BG2ROTDX  BG2 rotation and scaling distance X
                Consider using TOOL_ functions for this.
---------------------------------------------------	
*/
#define R_BG2ROTDX     *(volatile u16 *)(MEM_IO + 0x020)
/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG2ROTDX_SET(val)  R_BG2ROTDX = (val);



/* 
---------------------------------------------------
  IO 0x22 --	R_BG2ROTDMX  BG2 rotation and scaling distance X
                Consider using TOOL_ functions for this.
---------------------------------------------------	
*/
#define R_BG2ROTDMX     *(volatile u16 *)(MEM_IO + 0x022)
/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG2ROTDMX_SET(val)  R_BG2ROTDMX = (val);

/* 
---------------------------------------------------
  IO 0x24 --	R_BG2ROTDY  BG2 rotation and scaling distance Y
                Consider using TOOL_ functions for this.
---------------------------------------------------	
*/
#define R_BG2ROTDY     *(volatile u16 *)(MEM_IO + 0x024)
/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG2ROTDY_SET(val)  R_BG2ROTDY = (val);

/* 
---------------------------------------------------
  IO 0x26 --	R_BG2ROTDMY  BG2 rotation and scaling distance Y
                Consider using TOOL_ functions for this.
---------------------------------------------------	
*/
#define R_BG2ROTDMY     *(volatile u16 *)(MEM_IO + 0x026)
/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG2ROTDMY_SET(val)  R_BG2ROTDMY = (val);

/* 
---------------------------------------------------
  IO 0x30 --	R_BG3ROTDX  BG3 rotation and scaling distance X
                Consider using TOOL_ functions for this.
---------------------------------------------------	
*/
#define R_BG3ROTDX     *(volatile u16 *)(MEM_IO + 0x030)
/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG3ROTDX_SET(val)  R_BG3ROTDX = (val);

/* 
---------------------------------------------------
  IO 0x32 --	R_BG3ROTDMX  BG3 rotation and scaling distance X
                Consider using TOOL_ functions for this.
---------------------------------------------------	
*/
#define R_BG3ROTDMX     *(volatile u16 *)(MEM_IO + 0x032)
/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG3ROTDMX_SET(val)  R_BG3ROTDMX = (val);

/* 
---------------------------------------------------
  IO 0x34 --	R_BG3ROTDY  BG3 rotation and scaling distance Y
                Consider using TOOL_ functions for this.
---------------------------------------------------	
*/
#define R_BG3ROTDY     *(volatile u16 *)(MEM_IO + 0x034)
/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG3ROTDY_SET(val)  R_BG3ROTDY = (val);

/* 
---------------------------------------------------
  IO 0x36 --	R_BG3ROTDMY  BG3 rotation and scaling distance Y
                Consider using TOOL_ functions for this.
---------------------------------------------------	
*/
#define R_BG3ROTDMY     *(volatile u16 *)(MEM_IO + 0x036)
/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG3ROTDMY_SET(val)  R_BG3ROTDMY = (val);



/* 
---------------------------------------------------
  IO 0x28 --	R_BG2ROTX  BG2 rotation and scaling reference
                point X coordinate.
                This register actually spans 2 regs in HW, to
                allow for the following format: 
                1,                19,              8
                sign bit          integer portion  fractional

                remark: includes Reg 0x2A (HighWord of this value)
                Consider using TOOL_ functions for this.
---------------------------------------------------	
*/

#define R_BG2ROTX     *(volatile u32 *)(MEM_IO + 0x028)
/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG2ROTX_SET(val)  R_BG2ROTX = (val);

/* 
---------------------------------------------------
  IO 0x2C --	R_BG2ROTY  BG2 rotation and scaling reference
                point Y coordinate.
                This register actually spans 2 regs in HW, to
                allow for the following format: 
                1,                19,              8
                sign bit          integer portion  fractional

                remark: includes Reg 0x2E (HighWord of this value)
                Consider using TOOL_ functions for this.
---------------------------------------------------	
*/

#define R_BG2ROTY     *(volatile u32 *)(MEM_IO + 0x02C)
/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG2ROTY_SET(val)  R_BG2ROTY = (val);

/* 
---------------------------------------------------
  IO 0x38 --	R_BG3ROTX  BG3 rotation and scaling reference
                point X coordinate.
                This register actually spans 2 regs in HW, to
                allow for the following format: 
                1,                19,              8
                sign bit          integer portion  fractional

                remark: includes Reg 0x3A (HighWord of this value)
                Consider using TOOL_ functions for this.
---------------------------------------------------	
*/

#define R_BG3ROTX     *(volatile u32 *)(MEM_IO + 0x038)
/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG3ROTX_SET(val)  R_BG3ROTX = (val);

/* 
---------------------------------------------------
  IO 0x3C --	R_BG3ROTY  BG3 rotation and scaling reference
                point Y coordinate.
                This register actually spans 2 regs in HW, to
                allow for the following format: 
                1,                19,              8
                sign bit          integer portion  fractional

                remark: includes Reg 0x3E (HighWord of this value)
                Consider using TOOL_ functions for this.
---------------------------------------------------	
*/

#define R_BG3ROTY     *(volatile u32 *)(MEM_IO + 0x03C)
/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_BG3ROTY_SET(val)  R_BG3ROTY = (val);

/* 
---------------------------------------------------
  IO 0x4c -- R_MOSAIC - Mosaic effect control reg

  This register controls the amount of mosaic applied
  to all BGs and OBJs if the corresponding BGxCNT has
  its mosaic flag turned on

  A Value of 0 in all regs results in a normal 
  display, even with mosaic turned on.

  Since this reg is write only, you can only set ALL
  of the mosaic values at once.
---------------------------------------------------	
*/
#define R_MOSAIC		*(volatile u16 *)(MEM_IO + 0x4C)

/* Constants */
/* Masks */
#define V_MOSAIC_OBJ_VSIZE_MSK  0x0FFF
#define V_MOSAIC_OBJ_HSIZE_MSK  0xF0FF
#define V_MOSAIC_BG_VSIZE_MSK   0xFF0F
#define V_MOSAIC_BG_HSIZE_MSK   0xFFF0

/* Functions */
/* Macros */
#define M_MOSAIC_SET(bgh,bgv,objh,objv)     (R_MOSAIC = bgh | (bgv<<4) |(objh<<8) |(objv<<12) );


/* 
---------------------------------------------------
  IO 0x4x -- R_WINXX  Window X X Positions

  Holds both left and right xpos. These macros work
  for window 0 and 1.

---------------------------------------------------	
*/
#define R_WINXX(no)		*(volatile u16 *)(MEM_IO + 0x40 + (no<<1))

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_WINXX_SET(no,posx1,posx2)   (R_WINXX(no) =  (posx1<<8) | posx2);

/* 
---------------------------------------------------
  IO 0x4x -- R_WINXY  Window X Y Positions

  Holds both upper and lower ypos. These macros work
  for window 0 and 1.

---------------------------------------------------	
*/
#define R_WINXY(no)		*(volatile u16 *)(MEM_IO + 0x44 + (no<<1))

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_WINXY_SET(no,posx1,posx2)   (R_WINXY(no) =  (posx1<<8) | posx2);

/* 
---------------------------------------------------
  IO 0x40 -- R_WIN0X  Window 0 X Positions

  Holds both left and right xpos

---------------------------------------------------	
*/
#define R_WIN0X		*(volatile u16 *)(MEM_IO + 0x40)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_WIN0X_SET(posx1,posx2)   (R_WIN0X =  (posx1<<8) | posx2);

/* 
---------------------------------------------------
  IO 0x42 -- R_WIN1X  Window 1 X Positions

  Holds both left and right xpos

---------------------------------------------------	
*/
#define R_WIN1X		*(volatile u16 *)(MEM_IO + 0x42)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_WIN1X_SET(posx1,posx2)   (R_WIN1X =  (posx1<<8) | posx2);


/* 
---------------------------------------------------
  IO 0x44 -- R_WIN0Y  Window 0 Y Positions

  Holds both upper and lower ypos

---------------------------------------------------	
*/
#define R_WIN0Y		*(volatile u16 *)(MEM_IO + 0x44)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_WIN0Y_SET(posy1,posy2)   (R_WIN0Y =  (posy1<<8) | posy2);

/* 
---------------------------------------------------
  IO 0x46 -- R_WIN1Y  Window 1 Y Positions

  Holds both upper and lower ypos

---------------------------------------------------	
*/
#define R_WIN1Y		*(volatile u16 *)(MEM_IO + 0x44)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_WIN1Y_SET(posy1,posy2)   (R_WIN1Y =  (posy1<<8) | posy2);

/* 
---------------------------------------------------
  IO 0x48 -- R_WININ  Window 0 and 1 display attributes

  Controls which BGs/OBJs appear in the 
  inner area of the window, also switches
  Special Effects.

---------------------------------------------------	
*/
#define R_WININ		*(volatile u16 *)(MEM_IO + 0x48)

/* Constants */
/* Masks */
#define V_WININ_WIN0_DISPLAY_MSK    0xFFE0
#define V_WININ_WIN1_DISPLAY_MSK    0xE0FF

/* Functions */
/* Macros */
#define M_WININ_WIN0_DISPLAY_SET_BG0_ON  (R_WININ &= V_WININ_WIN0_DISPLAY_MSK);\
                                         (R_WININ |= 0x0001);
#define M_WININ_WIN0_DISPLAY_SET_BG1_ON  (R_WININ &= V_WININ_WIN0_DISPLAY_MSK);\
                                         (R_WININ |= 0x0002);
#define M_WININ_WIN0_DISPLAY_SET_BG2_ON  (R_WININ &= V_WININ_WIN0_DISPLAY_MSK);\
                                         (R_WININ |= 0x0004);
#define M_WININ_WIN0_DISPLAY_SET_BG3_ON  (R_WININ &= V_WININ_WIN0_DISPLAY_MSK);\
                                         (R_WININ |= 0x0008);
#define M_WININ_WIN0_DISPLAY_SET_OBJ_ON  (R_WININ &= V_WININ_WIN0_DISPLAY_MSK);\
                                         (R_WININ |= 0x0010);
#define M_WININ_WIN0_DISPLAY_SET_FX_ON   (R_WININ &= V_WININ_WIN0_DISPLAY_MSK);\
                                         (R_WININ |= 0x0020);
#define M_WININ_WIN0_DISPLAY_SET_BG0_OFF (R_WININ &= 0xFFFE);
#define M_WININ_WIN0_DISPLAY_SET_BG1_OFF (R_WININ &= 0xFFFD);
#define M_WININ_WIN0_DISPLAY_SET_BG2_OFF (R_WININ &= 0xFFFB);
#define M_WININ_WIN0_DISPLAY_SET_BG3_OFF (R_WININ &= 0xFFF7);
#define M_WININ_WIN0_DISPLAY_SET_OBJ_OFF (R_WININ &= 0xFFEF);
#define M_WININ_WIN0_DISPLAY_SET_FX_OFF  (R_WININ &= 0xFFDF);


#define M_WININ_WIN1_DISPLAY_SET_BG0_ON  (R_WININ &= V_WININ_WIN1_DISPLAY_MSK);\
                                         (R_WININ |= 0x0100);
#define M_WININ_WIN1_DISPLAY_SET_BG1_ON  (R_WININ &= V_WININ_WIN1_DISPLAY_MSK);\
                                         (R_WININ |= 0x0200);
#define M_WININ_WIN1_DISPLAY_SET_BG2_ON  (R_WININ &= V_WININ_WIN1_DISPLAY_MSK);\
                                         (R_WININ |= 0x0400);
#define M_WININ_WIN1_DISPLAY_SET_BG3_ON  (R_WININ &= V_WININ_WIN1_DISPLAY_MSK);\
                                         (R_WININ |= 0x0800);
#define M_WININ_WIN1_DISPLAY_SET_OBJ_ON  (R_WININ &= V_WININ_WIN1_DISPLAY_MSK);\
                                         (R_WININ |= 0x1000);
#define M_WININ_WIN1_DISPLAY_SET_FX_ON   (R_WININ &= V_WININ_WIN1_DISPLAY_MSK);\
                                         (R_WININ |= 0x2000);
#define M_WININ_WIN1_DISPLAY_SET_BG0_OFF (R_WININ &= 0xFEFF);
#define M_WININ_WIN1_DISPLAY_SET_BG1_OFF (R_WININ &= 0xFDFF);
#define M_WININ_WIN1_DISPLAY_SET_BG2_OFF (R_WININ &= 0xFBFF);
#define M_WININ_WIN1_DISPLAY_SET_BG3_OFF (R_WININ &= 0xF7FF);
#define M_WININ_WIN1_DISPLAY_SET_OBJ_OFF (R_WININ &= 0xEFFF);
#define M_WININ_WIN1_DISPLAY_SET_FX_OFF  (R_WININ &= 0xDFFF);


/* 
---------------------------------------------------
  IO 0x4A -- R_WINOUT  Window 0 and 1 display attributes

  Controls which BGs/OBJs appear outside of
  the area of the window, also switches
  Special Effects.

  In difference to WININ, Win0/1 share the same ON/OFF
  flags, and this reg also controls the OBJ window
---------------------------------------------------	
*/
#define R_WINOUT		*(volatile u16 *)(MEM_IO + 0x4A)

/* Constants */
/* Masks */
#define V_WINOUT_WIN0AND1_DISPLAY_MSK    0xFFE0
#define V_WINOUT_OBJ_DISPLAY_MSK    0xE0FF

/* Functions */
/* Macros */
#define M_WINOUT_WIN0AND1_DISPLAY_SET_BG0_ON    (R_WINOUT &= V_WINOUT_WIN0AND1_DISPLAY_MSK);\
                                                (R_WINOUT |= 0x0001);
#define M_WINOUT_WIN0AND1_DISPLAY_SET_BG1_ON    (R_WINOUT &= V_WINOUT_WIN0AND1_DISPLAY_MSK);\
                                                (R_WINOUT |= 0x0002);
#define M_WINOUT_WIN0AND1_DISPLAY_SET_BG2_ON    (R_WINOUT &= V_WINOUT_WIN0AND1_DISPLAY_MSK);\
                                                (R_WINOUT |= 0x0004);
#define M_WINOUT_WIN0AND1_DISPLAY_SET_BG3_ON    (R_WINOUT &= V_WINOUT_WIN0AND1_DISPLAY_MSK);\
                                                (R_WINOUT |= 0x0008);
#define M_WINOUT_WIN0AND1_DISPLAY_SET_OBJ_ON    (R_WINOUT &= V_WINOUT_WIN0AND1_DISPLAY_MSK);\
                                                (R_WINOUT |= 0x0010);
#define M_WINOUT_WIN0AND1_DISPLAY_SET_FX_ON     (R_WINOUT &= V_WINOUT_WIN0AND1_DISPLAY_MSK);\
                                                (R_WINOUT |= 0x0020);
#define M_WINOUT_WIN0AND1_DISPLAY_SET_BG0_OFF   (R_WINOUT &= 0xFFFE);
#define M_WINOUT_WIN0AND1_DISPLAY_SET_BG1_OFF   (R_WINOUT &= 0xFFFD);
#define M_WINOUT_WIN0AND1_DISPLAY_SET_BG2_OFF   (R_WINOUT &= 0xFFFB);
#define M_WINOUT_WIN0AND1_DISPLAY_SET_BG3_OFF   (R_WINOUT &= 0xFFF7);
#define M_WINOUT_WIN0AND1_DISPLAY_SET_OBJ_OFF   (R_WINOUT &= 0xFFEF);
#define M_WINOUT_WIN0AND1_DISPLAY_SET_FX_OFF    (R_WINOUT &= 0xFFDF);

#define M_WINOUT_OBJ_DISPLAY_SET_BG0_ON         (R_WINOUT &= V_WINOUT_OBJ_DISPLAY_MSK);\
                                                (R_WINOUT |= 0x0100);
#define M_WINOUT_OBJ_DISPLAY_SET_BG1_ON         (R_WINOUT &= V_WINOUT_OBJ_DISPLAY_MSK);\
                                                (R_WINOUT |= 0x0200);
#define M_WINOUT_OBJ_DISPLAY_SET_BG2_ON         (R_WINOUT &= V_WINOUT_OBJ_DISPLAY_MSK);\
                                                (R_WINOUT |= 0x0400);
#define M_WINOUT_OBJ_DISPLAY_SET_BG3_ON         (R_WINOUT &= V_WINOUT_OBJ_DISPLAY_MSK);\
                                                (R_WINOUT |= 0x0800);
#define M_WINOUT_OBJ_DISPLAY_SET_OBJ_ON         (R_WINOUT &= V_WINOUT_OBJ_DISPLAY_MSK);\
                                                (R_WINOUT |= 0x1000);
#define M_WINOUT_OBJ_DISPLAY_SET_FX_ON          (R_WINOUT &= V_WINOUT_OBJ_DISPLAY_MSK);\
                                                (R_WINOUT |= 0x2000);
#define M_WINOUT_OBJ_DISPLAY_SET_BG0_OFF        (R_WINOUT &= 0xFEFF);
#define M_WINOUT_OBJ_DISPLAY_SET_BG1_OFF        (R_WINOUT &= 0xFDFF);
#define M_WINOUT_OBJ_DISPLAY_SET_BG2_OFF        (R_WINOUT &= 0xFBFF);
#define M_WINOUT_OBJ_DISPLAY_SET_BG3_OFF        (R_WINOUT &= 0xF7FF);
#define M_WINOUT_OBJ_DISPLAY_SET_OBJ_OFF        (R_WINOUT &= 0xEFFF);
#define M_WINOUT_OBJ_DISPLAY_SET_FX_OFF         (R_WINOUT &= 0xDFFF);

/* 
---------------------------------------------------
  IO 0x050 -- R_BLDCNT
  Color Special FX control register. Just basic declaration
  for now. Improve me.
---------------------------------------------------
*/ 

#define R_BLDCNT		*(volatile u16 *)(MEM_IO + 0x50)

/* 
---------------------------------------------------
  IO 0x052 -- R_BLDALPHA
  Color Special FX register. Just basic declaration
  for now. Improve me.
---------------------------------------------------
*/ 

#define R_BLDALPHA		*(volatile u16 *)(MEM_IO + 0x52)

/* 
---------------------------------------------------
  IO 0x054 -- R_BLDY
  Color Special FX register. Just basic declaration
  for now. Improve me.
---------------------------------------------------
*/ 

#define R_BLDY		*(volatile u16 *)(MEM_IO + 0x54)





/*
---------------------------------------------------
Sound notes:

---------------------------------------------------
*/

/* 
---------------------------------------------------
  IO 0x060 -- R_SND1CNT1
  First control register for GBC-like sound channel
  1. This channel can generate rectangular 
  waveforms with sweep (frequency change) and envelope
  (volume change). Also needs setting of 
  R_SND1CNT2 and R_SND1CNT3 . The sound is output
  when you write the initialization bit of R_SND1CNT3.
  this is autoatically done when using M_SND1CNT3_SETUP
---------------------------------------------------	
*/

#define R_SND1CNT1		*(volatile u16 *)(MEM_IO + 0x60)

/* Constants */
#define V_SND1CNT1_SWEEP_FADEMODE_UP	0x0000
#define V_SND1CNT1_SWEEP_FADEMODE_DOWN	0x0080

/* Masks */
#define V_SND1CNT1_SWEEP_TIME_MASK	0xFF8F
#define V_SND1CNT1_NUM_SWEEPS_MASK	0xFFF8

/* Functions */
/* Macros */
#define M_SND1CNT1_SETUP(sweep_time,num_sweeps,fademode) \
												( R_SND1CNT1 =  (sweep_time << 4) | \
																(fademode <<3 ) | \
																 num_sweeps \
												);
#define M_SND1CNT1_SWEEP_TIME_SET(x)			(R_SND1CNT1 &= V_SND1CNT1_SWEEP_TIME_MASK);\
                                                (R_SND1CNT1 |= (x<<4));
#define M_SND1CNT1_NUM_SWEEPS_SET(x)			(R_SND1CNT1 &= V_SND1CNT1_NUM_SWEEPS_MASK);\
                                                (R_SND1CNT1 |= x);
#define M_SND1CNT1_SWEEP_DIR_SET_ASCENDING		(R_SND1CNT1 &= 0xFF7F);
#define M_SND1CNT1_SWEEP_DIR_SET_DESCENDING		(R_SND1CNT1 |= 0x0080);


/* 
---------------------------------------------------
  IO 0x062 -- R_SND1CNT2
  see R_SND1CNT1 for details
  envelope initial setting 0-15 (mute-full volume)
  length setting is NOT READABLE!
---------------------------------------------------	
*/

#define R_SND1CNT2		*(volatile u16 *)(MEM_IO + 0x62)

/* Constants */
#define V_SND1CNT2_VOL_FADEMODE_UP		0x0800
#define V_SND1CNT2_VOL_FADEMODE_DOWN	0x0000

/* Masks */
#define V_SND1CNT2_INITIAL_ENVELOPE_MASK		0x0FFF
#define V_SND1CNT2_ENVELOPE_NUMSTEPS_MASK		0xF8FF
#define V_SND1CNT2_WAVEFORM_DUTY_MASK			0xFF3F
#define V_SND1CNT2_SOUND_LEN_MASK				0xFFC0


/* Functions */
/* Macros */
#define M_SND1CNT2_SETUP(initial_vol,fademode,vol_numstep,snd_len,wave_duty) \
												(R_SND1CNT2 =	(initial_vol << 12) | \
																(fademode << 11) | \
																(vol_numstep << 8) | \
																(wave_duty << 6) | \
																(snd_len) \
												);

#define M_SND1CNT2_INITIAL_ENVELOPE_SET(x)		(R_SND1CNT2 &= V_SND1CNT2_INITIAL_ENVELOPE_MASK);\
                                                (R_SND1CNT2 |= (x<<12));
#define M_SND1CNT2_ENVELOPE_NUMSTEPS_SET(x)		(R_SND1CNT2 &= V_SND1CNT2_ENVELOPE_NUMSTEPS_MASK);\
                                                (R_SND1CNT2 |= (x<<8));
#define M_SND1CNT2_WAVEFORM_DUTY_SET(x)			(R_SND1CNT2 &= V_SND1CNT2_WAVEFORM_DUTY_MASK);\
                                                (R_SND1CNT2 |= (x<<6));
#define M_SND1CNT2_ENVELOPE_DIR_ATTENUATE		(R_SND1CNT2 &= 0xF7FF);
#define M_SND1CNT2_ENVELOPE_DIR_AMPLIFY			(R_SND1CNT2 |= 0x0800);

/* 
---------------------------------------------------
  IO 0x064 -- R_SND1CNT3
  see R_SND1CNT1 for details
---------------------------------------------------	
*/

#define R_SND1CNT3		*(volatile u16 *)(MEM_IO + 0x64)

/* Constants */
/* Masks */
#define V_SND1CNT3_FREQUENCY_MASK			0xF800


/* Functions */
/* Macros */
#define M_SND1CNT3_SETUP(frequency,loopmode)	(R_SND1CNT3 = 0x8000 | \
												  (loopmode << 14) | \
												  frequency \
												);

#define M_SND1CNT3_SOUND_LEN_COUNTDOWN			(R_SND1CNT3 |= 0x4000);
#define M_SND1CNT3_SOUND_LEN_CONTINOUS			(R_SND1CNT3 &= 0xBFFF);



/* 
---------------------------------------------------
  IO 0x068 -- R_SND2CNT1
  Sound2 envelope control.

  see R_SND1CNT1 for details
  envelope initial setting 0-15 (mute-full volume)
  length setting is NOT READABLE!
---------------------------------------------------	
*/

#define R_SND2CNT1		*(volatile u16 *)(MEM_IO + 0x68)

/* Constants */
#define V_SND2CNT1_VOL_FADEMODE_UP		0x0800
#define V_SND2CNT1_VOL_FADEMODE_DOWN	0x0000

/* Masks */
#define V_SND2CNT1_INITIAL_ENVELOPE_MASK		0x0FFF
#define V_SND2CNT1_ENVELOPE_NUMSTEPS_MASK		0xF8FF
#define V_SND2CNT1_WAVEFORM_DUTY_MASK			0xFF3F
#define V_SND2CNT1_SOUND_LEN_MASK				0xFFC0


/* Functions */
/* Macros */
#define M_SND2CNT1_SETUP(initial_vol,fademode,vol_numstep,snd_len,wave_duty) \
												(R_SND2CNT1 =	(initial_vol << 12) | \
																(fademode << 11) | \
																(vol_numstep << 8) | \
																(wave_duty << 6) | \
																(snd_len) \
												);

#define M_SND2CNT1_INITIAL_ENVELOPE_SET(x)		(R_SND2CNT1 &= V_SND2CNT1_INITIAL_ENVELOPE_MASK);\
                                                (R_SND2CNT1 |= (x<<12));
#define M_SND2CNT1_ENVELOPE_NUMSTEPS_SET(x)		(R_SND2CNT1 &= V_SND2CNT1_ENVELOPE_NUMSTEPS_MASK);\
                                                (R_SND2CNT1 |= (x<<8));
#define M_SND2CNT1_WAVEFORM_DUTY_SET(x)			(R_SND2CNT1 &= V_SND2CNT1_WAVEFORM_DUTY_MASK);\
                                                (R_SND2CNT1 |= (x<<6));
#define M_SND2CNT1_ENVELOPE_DIR_ATTENUATE		(R_SND2CNT1 &= 0xF7FF);
#define M_SND2CNT1_ENVELOPE_DIR_AMPLIFY			(R_SND2CNT1 |= 0x0800);

/* 
---------------------------------------------------
  IO 0x06C -- R_SND2CNT2
  Sound2 frequency control and initialization
  see R_SND1CNT1 for details
---------------------------------------------------	
*/

#define R_SND2CNT2		*(volatile u16 *)(MEM_IO + 0x6C)

/* Constants */
/* Masks */
#define V_SND2CNT2_FREQUENCY_MASK			0xF800


/* Functions */
/* Macros */
#define M_SND2CNT2_SETUP(frequency,loopmode)	(R_SND2CNT2 = 0x8000 | \
												(loopmode << 14) | \
												frequency \
												);

#define M_SND2CNT2_SOUND_LEN_COUNTDOWN			(R_SND2CNT2 |= 0x4000);
#define M_SND2CNT2_SOUND_LEN_CONTINOUS			(R_SND2CNT2 &= 0xBFFF);



/* 
---------------------------------------------------
  IO 0x070 -- R_SND3CNT1
  This AGB sound channel outputs wave data from waveform
  RAM, as CGB was already able to. The amount of waveform 
  data that can be selected is doubled to 16*16bits, divided
  into 2 banks of 8*16 bits. These can be written by the user
  by accessing MEM_IO+0x90. After writing the 8 first words,
  you can perform bank switching, then, you can write the second
  batch of data into the same memory space. 

  Be aware that the bank NOT specified in this register is the
  one that you are writing to.

  The waveform can either be played from the first/second
  4*16 bits (bank mode) or from the whole 8*16 bits. If you
  set it to double bank output mode, the specified sound bank
  is played first, then the other one.
---------------------------------------------------	
*/

#define R_SND3CNT1		*(volatile u16 *)(MEM_IO + 0x70)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_SND3CNT1_OUTPUT_ON			    (R_SND3CNT1 |= 0x0080);
#define M_SND3CNT1_OUTPUT_OFF			    (R_SND3CNT1 &= 0xFF7F);
#define M_SND3CNT1_BANK_SET_1			    (R_SND3CNT1 |= 0x0040);
#define M_SND3CNT1_BANK_SET_0			    (R_SND3CNT1 &= 0xFFBF);
#define M_SND3CNT1_WAVEFORM_SINGLEBANK	    (R_SND3CNT1 |= 0x0020);
#define M_SND3CNT1_WAVEFORM_DOUBLEBANK	    (R_SND3CNT1 &= 0xFFDF);

/* 
---------------------------------------------------
  IO 0x072 -- R_SND3CNT2
  Sound3 volume and length settings.
  See R_SND3CNT1 for more information
  FORCE_75PERC = Force a specific envelope. Usually, set this 0.
  OUTPUT_LEVEL = 0 Mute
                 1 Normal output
                 2 Shift contents right 1 bit (4 bit blocks in WAV data)
                 3 Shift contents right 2 bit (4 bit blocks in WAV data)
  LENGTH       = Set to 0 for 1 sec long sound... 255 for 1/256 sec sound 
---------------------------------------------------	
*/

#define R_SND3CNT2		*(volatile u16 *)(MEM_IO + 0x72)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_SND3CNT2_SETUP(force75perc, outputlevel, soundlen) \
                                            (R_SND3CNT2 =   (force75perc<<15) | \
												            (outputlevel << 13) | \
												            soundlen \
												            );

/* 
---------------------------------------------------
  IO 0x074 -- R_SND3CNT3
  Sound3 frequency and initialization settings.
---------------------------------------------------
*/

#define R_SND3CNT3		*(volatile u16 *)(MEM_IO + 0x74)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_SND3CNT3_SETUP(frequency,loopmode)	(R_SND3CNT3 = 0x8000 | \
												(loopmode << 14) | \
												frequency \
												);



/* 
---------------------------------------------------
  IO 0x090-0x09F -- R_SND3WAVX
  Sound3 Waveform RAM. Range is from 0x90 to 09F.
  Write to this is performed by setting the bank 
  you do NOT want to write to in SND3CNT1 first,
  and then perform the write. This is implemented
  like this so you can write one bank while playing the
  other one. So, set Bank 1 if you want to write Bank 0
  and vice versa. Not setting the bank will produce
  weird behaviour obviously.

  Allowed values for the x/regno parameter in this are
  0-7 (since there are 8*16 bits per bank to write to)

  Alsosee R_SND3CNT1 dormore details on this.
---------------------------------------------------
*/

#define R_SND3WAVX(x)		*(volatile u16 *)(MEM_IO + 0x90 + (x*2))

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_SND3WAVX_WRITE(regno,data)    (R_SND3WAVX(regno) = data);




/* 
---------------------------------------------------
  IO 0x078 -- R_SND4CNT1
  Sound4 envelope and lenght settings
  Sound4 is a circuit that generates white noise.
  env_ini      = initial volume (0-15)
  env_dir      = 0=attenuate 1=amplify
  env_numsteps = number of envelope steps (0-7)
  soundlen     = 0-63 (0=1sec ... 63=1/256 sec)
---------------------------------------------------
*/

#define R_SND4CNT1		*(volatile u16 *)(MEM_IO + 0x78)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_SND4CNT1_SETUP(env_ini, env_dir, env_numsteps, sound_len) \
                                                (R_SND4CNT1 =  \
												(env_ini << 12) | \
												(env_dir << 11) | \
												(env_numsteps << 8) | \
												sound_len \
												);


/* 
---------------------------------------------------
  IO 0x07C -- R_SND4CNT2
  Sound4 counter and initialization settings
  Sound4 is a circuit that generates white noise.
  counter shift = 0x00 - 0x0D
  counter step  = 0=15 steps 1=7 steps
  div_ratio     = n/a
---------------------------------------------------
*/

#define R_SND4CNT2		*(volatile u16 *)(MEM_IO + 0x7C)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_SND4CNT2_SETUP(counter_shift, counter_step, div_ratio, loopmode)	\
                                                (R_SND4CNT2 = 0x8000 | \
												(loopmode << 14) | \
												(counter_shift << 4) | \
												(counter_step << 3) | \
												div_ratio \
												);


/* 
---------------------------------------------------
  IO 0x080 -- R_SNDCNT1
  Do not confuse this with SNDxCNT1, this reg is
  the overall sound control.
  Sound 1-4 Output level and Stereo control
---------------------------------------------------	
*/

#define R_SNDCNT1		*(volatile u16 *)(MEM_IO + 0x80)

/* Constants */
/* Masks */
#define V_SNDCNT1_VOLUME_LEFT_MASK			0xFF8F
#define V_SNDCNT1_VOLUME_RIGHT_MASK			0xFFF8


/* Functions */
/* Macros */
#define M_SNDCNT1_VOLUME_LEFT_SET(x)		(R_SNDCNT1 &= V_SNDCNT1_VOLUME_LEFT_MASK);\
                                            (R_SNDCNT1 |= (x<<4));
#define M_SNDCNT1_VOLUME_RIGHT_SET(x)		(R_SNDCNT1 &= V_SNDCNT1_VOLUME_RIGHT_MASK);\
                                            (R_SNDCNT1 |= x);

#define M_SNDCNT1_OUTPUT_LEFT_ALL_ON		(R_SNDCNT1 |= 0xF000);
#define M_SNDCNT1_OUTPUT_LEFT_SOUND1_ON		(R_SNDCNT1 |= 0x1000);
#define M_SNDCNT1_OUTPUT_LEFT_SOUND2_ON		(R_SNDCNT1 |= 0x2000);
#define M_SNDCNT1_OUTPUT_LEFT_SOUND3_ON		(R_SNDCNT1 |= 0x4000);
#define M_SNDCNT1_OUTPUT_LEFT_SOUND4_ON		(R_SNDCNT1 |= 0x8000);
#define M_SNDCNT1_OUTPUT_LEFT_ALL_OFF		(R_SNDCNT1 &= 0x0FFF);
#define M_SNDCNT1_OUTPUT_LEFT_SOUND1_OFF	(R_SNDCNT1 &= 0xEFFF);
#define M_SNDCNT1_OUTPUT_LEFT_SOUND2_OFF	(R_SNDCNT1 &= 0xDFFF);
#define M_SNDCNT1_OUTPUT_LEFT_SOUND3_OFF	(R_SNDCNT1 &= 0xBFFF);
#define M_SNDCNT1_OUTPUT_LEFT_SOUND4_OFF	(R_SNDCNT1 &= 0x7FFF);
#define M_SNDCNT1_OUTPUT_RIGHT_ALL_ON		(R_SNDCNT1 |= 0x0F00);
#define M_SNDCNT1_OUTPUT_RIGHT_SOUND1_ON	(R_SNDCNT1 |= 0x0100);
#define M_SNDCNT1_OUTPUT_RIGHT_SOUND2_ON	(R_SNDCNT1 |= 0x0200);
#define M_SNDCNT1_OUTPUT_RIGHT_SOUND3_ON	(R_SNDCNT1 |= 0x0400);
#define M_SNDCNT1_OUTPUT_RIGHT_SOUND4_ON	(R_SNDCNT1 |= 0x0800);
#define M_SNDCNT1_OUTPUT_RIGHT_ALL_OFF		(R_SNDCNT1 &= 0xF0FF);
#define M_SNDCNT1_OUTPUT_RIGHT_SOUND1_OFF	(R_SNDCNT1 &= 0xFEFF);
#define M_SNDCNT1_OUTPUT_RIGHT_SOUND2_OFF	(R_SNDCNT1 &= 0xFDFF);
#define M_SNDCNT1_OUTPUT_RIGHT_SOUND3_OFF	(R_SNDCNT1 &= 0xFBFF);
#define M_SNDCNT1_OUTPUT_RIGHT_SOUND4_OFF	(R_SNDCNT1 &= 0xF7FF);

#define M_SNDCNT1_INIT_ALL_ON		M_SNDCNT1_OUTPUT_LEFT_ALL_ON \
									M_SNDCNT1_OUTPUT_RIGHT_ALL_ON \
									M_SNDCNT1_VOLUME_LEFT_SET(7) \
									M_SNDCNT1_VOLUME_RIGHT_SET(7)


/* 
---------------------------------------------------
  IO 0x082 -- R_SNDCNT2
  Direct Sound control and Sound 1-4 output ratio

  M_SNDCNT2_SOUND1TO4_OUTPUTRATIO_SET(x)
    This one is used to select how loud the output of the GBC 
	sound system will be. You can set 
	Output sound ratio for chan.1-4 (0=25%,1=50%,2=100%)

  M_SNDCNT2_DSOUNDA_OUTPUTRATIO_SET(x)
	Direct sound A output ratio (0=50%, 1=100%)

  
---------------------------------------------------	
*/

#define R_SNDCNT2		*(volatile u16 *)(MEM_IO + 0x82)

/* Constants */
/* Masks */
/* Functions */
/* Macros */

#define M_SNDCNT2_SOUND1TO4_OUTPUTRATIO_SET(x)		(R_SNDCNT2 &= 0xFFFC);\
													(R_SNDCNT2 |= (x));
#define M_SNDCNT2_DSOUNDA_OUTPUTRATIO_SET(x)		(R_SNDCNT2 &= NOT_BIT2 );\
													(R_SNDCNT2 |= (x)<<2);
#define M_SNDCNT2_DSOUNDB_OUTPUTRATIO_SET(x)		(R_SNDCNT2 &= NOT_BIT3 );\
													(R_SNDCNT2 |= (x)<<3);

#define M_SNDCNT2_DSOUNDA_OUTPUT_R_ENABLE(x)		(R_SNDCNT2 &= NOT_BIT8 );\
													(R_SNDCNT2 |= (x)<<8);
#define M_SNDCNT2_DSOUNDA_OUTPUT_L_ENABLE(x)		(R_SNDCNT2 &= NOT_BIT9 );\
													(R_SNDCNT2 |= (x)<<9);
#define M_SNDCNT2_DSOUNDA_TIMER_SELECT(x)			(R_SNDCNT2 &= NOT_BIT10 );\
													(R_SNDCNT2 |= (x)<<10);
#define M_SNDCNT2_DSOUNDA_FIFO_RESET				(R_SNDCNT2 |= BIT11 );

#define M_SNDCNT2_DSOUNDB_OUTPUT_R_ENABLE(x)		(R_SNDCNT2 &= NOT_BIT12 );\
													(R_SNDCNT2 |= (x)<<12);
#define M_SNDCNT2_DSOUNDB_OUTPUT_L_ENABLE(x)		(R_SNDCNT2 &= NOT_BIT13 );\
													(R_SNDCNT2 |= (x)<<13);
#define M_SNDCNT2_DSOUNDB_TIMER_SELECT(x)			(R_SNDCNT2 &= NOT_BIT14 );\
													(R_SNDCNT2 |= (x)<<14);
#define M_SNDCNT2_DSOUNDB_FIFO_RESET				(R_SNDCNT2 |= BIT15 );


/* 
---------------------------------------------------
  IO 0x084 -- R_SNDCNT3
  Do not confuse this with SNDxCNT3, this reg is
  the overall sound control.
  Master sound enable and Sound 1-4 play status
---------------------------------------------------	
*/

#define R_SNDCNT3		*(volatile u16 *)(MEM_IO + 0x84)

/* Constants */
/* Masks */
/* Functions */
#define F_SNDCNT3_SOUND1_ON        (R_SNDCNT3 & 0x0001)
#define F_SNDCNT3_SOUND2_ON        (R_SNDCNT3 & 0x0002)
#define F_SNDCNT3_SOUND3_ON        (R_SNDCNT3 & 0x0004)
#define F_SNDCNT3_SOUND4_ON        (R_SNDCNT3 & 0x0008)

/* Macros */
#define M_SNDCNT3_SOUND_MASTER_ON   (R_SNDCNT3 |= 0x0080);
#define M_SNDCNT3_SOUND_MASTER_OFF  (R_SNDCNT3 &= 0xFF7F);
#define M_SNDCNT3_SOUND1_ON         (R_SNDCNT3 |= 0x0001);
#define M_SNDCNT3_SOUND2_ON         (R_SNDCNT3 |= 0x0002);
#define M_SNDCNT3_SOUND3_ON         (R_SNDCNT3 |= 0x0004);
#define M_SNDCNT3_SOUND4_ON         (R_SNDCNT3 |= 0x0008);
#define M_SNDCNT3_SOUND1_OFF        (R_SNDCNT3 &= 0xFFFE);
#define M_SNDCNT3_SOUND2_OFF        (R_SNDCNT3 &= 0xFFFD);
#define M_SNDCNT3_SOUND3_OFF        (R_SNDCNT3 &= 0xFFFB);
#define M_SNDCNT3_SOUND4_OFF        (R_SNDCNT3 &= 0xFFF7);

		
/* 
---------------------------------------------------
  IO 0x0A0 -- R_SNDFIFO_A
  Sound FIFO buffer for DSound channel A (32bit)
  Stores a total of 4 bytes of sample data
---------------------------------------------------	
*/

#define R_SNDFIFO_A		*(volatile u32 *)(MEM_IO + 0xA0)

/* 
---------------------------------------------------
  IO 0x0A4 -- R_SNDFIFO_B
  Sound FIFO buffer for DSound channel B (32bit)
  Stores a total of 4 bytes of sample data
---------------------------------------------------	
*/

#define R_SNDFIFO_B		*(volatile u32 *)(MEM_IO + 0xA4)


/*
---------------------------------------------------
DMA0 notes:

  Notice that DMA0 is only usable within the
  internal memory range of the GBA. If you
  want to access the Gamepak memory starting
  at 0x08000000, use DMA1-3 instead.

  Also, DMA0 cannot be used or Direct Sound 
  DMA, use DMA1 and 2 for this.
---------------------------------------------------
*/
/* 
---------------------------------------------------
  IO 0x0B0 -- R_DMA0SRC (27 bit val)
  DMA0 source reg. 
  Highest prio DMA.
---------------------------------------------------	
*/
#define R_DMA0SRC		*(volatile u32 *)(MEM_IO + 0x0B0)

/* Masks */
#define V_DMA0SRC_MSK  0x07FFFFFF

/* Macros */
#define M_DMA0SRC_SET(adr)  (R_DMA0SRC = (adr & V_DMA0SRC_MSK));

/* 
---------------------------------------------------
  IO 0x0B4 -- R_DMA0DST (27 bit val)
  DMA0 destination reg. 
  Highest prio DMA.
---------------------------------------------------	
*/
#define R_DMA0DST		*(volatile u32 *)(MEM_IO + 0x0B4)

/* Masks */
#define V_DMA0DST_MSK  0x07FFFFFF

/* Macros */
#define M_DMA0DST_SET(adr)  (R_DMA0DST = (adr & V_DMA0DST_MSK));

/* 
---------------------------------------------------
  IO 0x0B8 -- R_DMA0COUNT
  DMA0 16bit / 32bit unit count to transfer
  Amount of 16bit or 32 bit units to be transferred
  Selection of 16 or 32bit is done in R_DMA0CNT
  Accepted values are:
  0x0000             -> transfer 0x4000 units
  0x0001 - 0x3FFF    -> transfer units given 
---------------------------------------------------	
*/
#define R_DMA0COUNT		*(volatile u16 *)(MEM_IO + 0x0B8)

/* Masks */
#define V_DMA0COUNT_MSK 0x3FFF

/* Macros */
#define M_DMA0COUNT_SET(val)     (R_DMA0COUNT = (val & V_DMA0COUNT_MSK));     

/* 
---------------------------------------------------
  IO 0x0BA -- R_DMA0CNT
  Controls parameters for DMA0
---------------------------------------------------	
*/
#define R_DMA0CNT		*(volatile u16 *)(MEM_IO + 0x0BA)

/* Masks */
#define V_DMA0CNT_SRCCNT_MSK 0xFE7F
#define V_DMA0CNT_DSTCNT_MSK 0xFF9F
#define V_DMA0CNT_STARTAT_MSK 0xCFFF

/* Macros */
#define M_DMA0CNT_DSTCNT_POST_INC       (R_DMA0CNT &= V_DMA0CNT_DSTCNT_MSK);
#define M_DMA0CNT_DSTCNT_POST_DEC       (R_DMA0CNT &= V_DMA0CNT_DSTCNT_MSK);\
                                        (R_DMA0CNT |= 0x0020);
#define M_DMA0CNT_DSTCNT_NOCHANGE       (R_DMA0CNT &= V_DMA0CNT_DSTCNT_MSK);\
                                        (R_DMA0CNT |= 0x0040);
#define M_DMA0CNT_DSTCNT_SET(x)         (R_DMA0CNT &= V_DMA0CNT_DSTCNT_MSK);\
                                        (R_DMA0CNT |= (x)<<5);
#define M_DMA0CNT_SRCCNT_POST_INC       (R_DMA0CNT &= V_DMA0CNT_SRCCNT_MSK);
#define M_DMA0CNT_SRCCNT_POST_DEC       (R_DMA0CNT &= V_DMA0CNT_SRCCNT_MSK);\
                                        (R_DMA0CNT |= 0x0080);
#define M_DMA0CNT_SRCCNT_NOCHANGE       (R_DMA0CNT &= V_DMA0CNT_SRCCNT_MSK);\
                                        (R_DMA0CNT |= 0x0100);
#define M_DMA0CNT_SRCCNT_SET(x)         (R_DMA0CNT &= V_DMA0CNT_SRCCNT_MSK);\
                                        (R_DMA0CNT |= (x)<<7);
#define M_DMA0CNT_REPEAT_ON             (R_DMA0CNT |= 0x0200);
#define M_DMA0CNT_REPEAT_OFF            (R_DMA0CNT &= 0xFDFF);
#define M_DMA0CNT_REPEAT_SET(x)         (R_DMA0CNT &= NOT_BIT9);\
										(R_DMA0CNT |= (x)<<9);
#define M_DMA0CNT_TRANSFER_SET(mode)    (R_DMA0CNT &= 0xFBFF);\
                                        (R_DMA0CNT |= (mode<<10));
#define M_DMA0CNT_TRANSFER_32BITS       (R_DMA0CNT |= 0x0400);
#define M_DMA0CNT_TRANSFER_16BITS       (R_DMA0CNT &= 0xFBFF);
#define M_DMA0CNT_STARTAT_SET(x)        (R_DMA0CNT &= V_DMA0CNT_STARTAT_MSK);\
                                        (R_DMA0CNT |= (x<<12));
#define M_DMA0CNT_STARTAT_NOW           (R_DMA0CNT &= V_DMA0CNT_STARTAT_MSK);
#define M_DMA0CNT_STARTAT_VBL           (R_DMA0CNT &= V_DMA0CNT_STARTAT_MSK);\
                                        (R_DMA0CNT |= 0x1000);
#define M_DMA0CNT_STARTAT_HBL           (R_DMA0CNT &= V_DMA0CNT_STARTAT_MSK);\
                                        (R_DMA0CNT |= 0x2000);
#define M_DMA0CNT_IRQ_SET(x)            (R_DMA0CNT &= 0xBFFF);\
										(R_DMA0CNT |= (x)<<14);
#define M_DMA0CNT_IRQ_ON                (R_DMA0CNT |= 0x4000);
#define M_DMA0CNT_IRQ_OFF               (R_DMA0CNT &= 0xBFFF);
#define M_DMA0CNT_DMA_ENABLED           (R_DMA0CNT |= 0x8000);
#define M_DMA0CNT_DMA_DISABLED          (R_DMA0CNT &= 0x7FFF);


/* 
---------------------------------------------------
  IO 0x0BC -- R_DMA1SRC (28 bit val)
---------------------------------------------------	
*/
#define R_DMA1SRC		*(volatile u32 *)(MEM_IO + 0x0BC)

/* Masks */
#define V_DMA1SRC_MSK  0x0FFFFFFF

/* Macros */
#define M_DMA1SRC_SET(adr)  (R_DMA1SRC = (adr & V_DMA1SRC_MSK));

/* 
---------------------------------------------------
  IO 0x0C0 -- R_DMA1DST (27 bit val)
---------------------------------------------------	
*/
#define R_DMA1DST		*(volatile u32 *)(MEM_IO + 0x0C0)

/* Masks */
#define V_DMA1DST_MSK  0x07FFFFFF

/* Macros */
#define M_DMA1DST_SET(adr)  (R_DMA1DST = (adr & V_DMA1DST_MSK));

/* 
---------------------------------------------------
  IO 0x0C4 -- R_DMA1COUNT
  Accepted values are:
  0x0000             -> transfer 0x4000 units
  0x0001 - 0x3FFF    -> transfer units given 
---------------------------------------------------	
*/
#define R_DMA1COUNT		*(volatile u16 *)(MEM_IO + 0x0C4)

/* Masks */
#define V_DMA1COUNT_MSK 0x3FFF

/* Macros */
#define M_DMA1COUNT_SET(val)     (R_DMA1COUNT = (val & V_DMA1COUNT_MSK));     

/* 
---------------------------------------------------
  IO 0x0C6 -- R_DMA1CNT
  Controls parameters for DMA1
---------------------------------------------------	
*/
#define R_DMA1CNT		*(volatile u16 *)(MEM_IO + 0x0C6)

/* Masks */
#define V_DMA1CNT_SRCCNT_MSK 0xFE7F
#define V_DMA1CNT_DSTCNT_MSK 0xFF9F
#define V_DMA1CNT_STARTAT_MSK 0xCFFF

/* Macros */
#define M_DMA1CNT_DSTCNT_POST_INC       (R_DMA1CNT &= V_DMA1CNT_DSTCNT_MSK);
#define M_DMA1CNT_DSTCNT_POST_DEC       (R_DMA1CNT &= V_DMA1CNT_DSTCNT_MSK);\
                                        (R_DMA1CNT |= 0x0020);
#define M_DMA1CNT_DSTCNT_NOCHANGE       (R_DMA1CNT &= V_DMA1CNT_DSTCNT_MSK);\
                                        (R_DMA1CNT |= 0x0040);
#define M_DMA1CNT_DSTCNT_SET(x)         (R_DMA1CNT &= V_DMA1CNT_DSTCNT_MSK);\
                                        (R_DMA1CNT |= (x)<<5);
#define M_DMA1CNT_SRCCNT_POST_INC       (R_DMA1CNT &= V_DMA1CNT_SRCCNT_MSK);
#define M_DMA1CNT_SRCCNT_POST_DEC       (R_DMA1CNT &= V_DMA1CNT_SRCCNT_MSK);\
                                        (R_DMA1CNT |= 0x0080);
#define M_DMA1CNT_SRCCNT_NOCHANGE       (R_DMA1CNT &= V_DMA1CNT_SRCCNT_MSK);\
                                        (R_DMA1CNT |= 0x0100);
#define M_DMA1CNT_SRCCNT_SET(x)         (R_DMA1CNT &= V_DMA1CNT_SRCCNT_MSK);\
                                        (R_DMA1CNT |= (x)<<7);
#define M_DMA1CNT_REPEAT_ON             (R_DMA1CNT |= 0x0200);
#define M_DMA1CNT_REPEAT_OFF            (R_DMA1CNT &= 0xFDFF);
#define M_DMA1CNT_REPEAT_SET(x)         (R_DMA1CNT &= NOT_BIT9);\
					(R_DMA1CNT |= (x)<<9);
#define M_DMA1CNT_TRANSFER_SET(mode)    (R_DMA1CNT &= 0xFBFF);\
                                        (R_DMA1CNT |= (mode<<10));
#define M_DMA1CNT_TRANSFER_32BITS       (R_DMA1CNT |= 0x0400);
#define M_DMA1CNT_TRANSFER_16BITS       (R_DMA1CNT &= 0xFBFF);
#define M_DMA1CNT_STARTAT_SET(x)        (R_DMA1CNT &= V_DMA0CNT_STARTAT_MSK);\
                                        (R_DMA1CNT |= (x<<12));
#define M_DMA1CNT_STARTAT_NOW           (R_DMA1CNT &= V_DMA1CNT_STARTAT_MSK);
#define M_DMA1CNT_STARTAT_VBL           (R_DMA1CNT &= V_DMA1CNT_STARTAT_MSK);\
                                        (R_DMA1CNT |= 0x1000);
#define M_DMA1CNT_STARTAT_HBL           (R_DMA1CNT &= V_DMA1CNT_STARTAT_MSK);\
                                        (R_DMA1CNT |= 0x2000);
#define M_DMA1CNT_STARTAT_SOUNDIRQ      (R_DMA1CNT &= V_DMA1CNT_STARTAT_MSK);\
                                        (R_DMA1CNT |= 0x3000);
#define M_DMA1CNT_IRQ_SET(x)            (R_DMA1CNT &= 0xBFFF);\
					(R_DMA1CNT |= (x)<<14);
#define M_DMA1CNT_IRQ_ON                (R_DMA1CNT |= 0x4000);
#define M_DMA1CNT_IRQ_OFF               (R_DMA1CNT &= 0xBFFF);
#define M_DMA1CNT_DMA_ENABLED           (R_DMA1CNT |= 0x8000);
#define M_DMA1CNT_DMA_DISABLED          (R_DMA1CNT &= 0x7FFF);



/* 
---------------------------------------------------
  IO 0x0C8 -- R_DMA2SRC (28 bit val)
---------------------------------------------------	
*/
#define R_DMA2SRC		*(volatile u32 *)(MEM_IO + 0x0C8)

/* Masks */
#define V_DMA2SRC_MSK  0x0FFFFFFF

/* Macros */
#define M_DMA2SRC_SET(adr)  (R_DMA2SRC = (adr & V_DMA2SRC_MSK));

/* 
---------------------------------------------------
  IO 0x0CC -- R_DMA2DST (27 bit val)
---------------------------------------------------	
*/
#define R_DMA2DST		*(volatile u32 *)(MEM_IO + 0x0CC)

/* Masks */
#define V_DMA2DST_MSK  0x07FFFFFF

/* Macros */
#define M_DMA2DST_SET(adr)  (R_DMA2DST = (adr & V_DMA2DST_MSK));

/* 
---------------------------------------------------
  IO 0x0D0 -- R_DMA2COUNT
  Accepted values are:
  0x0000             -> transfer 0x4000 units
  0x0001 - 0x3FFF    -> transfer units given 
---------------------------------------------------	
*/
#define R_DMA2COUNT		*(volatile u16 *)(MEM_IO + 0x0D0)

/* Masks */
#define V_DMA2COUNT_MSK 0x3FFF

/* Macros */
#define M_DMA2COUNT_SET(val)     (R_DMA2COUNT = (val & V_DMA2COUNT_MSK));     

/* 
---------------------------------------------------
  IO 0x0D2 -- R_DMA2CNT
  Controls parameters for DMA2
---------------------------------------------------	
*/
#define R_DMA2CNT		*(volatile u16 *)(MEM_IO + 0x0D2)

/* Masks */
#define V_DMA2CNT_SRCCNT_MSK 0xFE7F
#define V_DMA2CNT_DSTCNT_MSK 0xFF9F
#define V_DMA2CNT_STARTAT_MSK 0xCFFF

/* Macros */
#define M_DMA2CNT_DSTCNT_POST_INC       (R_DMA2CNT &= V_DMA2CNT_DSTCNT_MSK);
#define M_DMA2CNT_DSTCNT_POST_DEC       (R_DMA2CNT &= V_DMA2CNT_DSTCNT_MSK);\
                                        (R_DMA2CNT |= 0x0020);
#define M_DMA2CNT_DSTCNT_NOCHANGE       (R_DMA2CNT &= V_DMA2CNT_DSTCNT_MSK);\
                                        (R_DMA2CNT |= 0x0040);
#define M_DMA2CNT_DSTCNT_SET(x)         (R_DMA2CNT &= V_DMA2CNT_DSTCNT_MSK);\
                                        (R_DMA2CNT |= (x)<<5);
#define M_DMA2CNT_SRCCNT_POST_INC       (R_DMA2CNT &= V_DMA2CNT_SRCCNT_MSK);
#define M_DMA2CNT_SRCCNT_POST_DEC       (R_DMA2CNT &= V_DMA2CNT_SRCCNT_MSK);\
                                        (R_DMA2CNT |= 0x0080);
#define M_DMA2CNT_SRCCNT_NOCHANGE       (R_DMA2CNT &= V_DMA2CNT_SRCCNT_MSK);\
                                        (R_DMA2CNT |= 0x0100);
#define M_DMA2CNT_SRCCNT_SET(x)         (R_DMA2CNT &= V_DMA2CNT_SRCCNT_MSK);\
                                        (R_DMA2CNT |= (x)<<7);
#define M_DMA2CNT_REPEAT_ON             (R_DMA2CNT |= 0x0200);
#define M_DMA2CNT_REPEAT_OFF            (R_DMA2CNT &= 0xFDFF);
#define M_DMA2CNT_REPEAT_SET(x)         (R_DMA2CNT &= NOT_BIT9);\
					(R_DMA2CNT |= (x)<<9);
#define M_DMA2CNT_TRANSFER_SET(mode)    (R_DMA2CNT &= 0xFBFF);\
                                        (R_DMA2CNT |= (mode<<10));
#define M_DMA2CNT_TRANSFER_32BITS       (R_DMA2CNT |= 0x0400);
#define M_DMA2CNT_TRANSFER_16BITS       (R_DMA2CNT &= 0xFBFF);
#define M_DMA2CNT_STARTAT_SET(x)        (R_DMA2CNT &= V_DMA0CNT_STARTAT_MSK);\
                                        (R_DMA2CNT |= (x<<12));
#define M_DMA2CNT_STARTAT_NOW           (R_DMA2CNT &= V_DMA2CNT_STARTAT_MSK);
#define M_DMA2CNT_STARTAT_VBL           (R_DMA2CNT &= V_DMA2CNT_STARTAT_MSK);\
                                        (R_DMA2CNT |= 0x1000);
#define M_DMA2CNT_STARTAT_HBL           (R_DMA2CNT &= V_DMA2CNT_STARTAT_MSK);\
                                        (R_DMA2CNT |= 0x2000);
#define M_DMA2CNT_STARTAT_SOUNDIRQ      (R_DMA2CNT &= V_DMA2CNT_STARTAT_MSK);\
                                        (R_DMA2CNT |= 0x3000);
#define M_DMA2CNT_IRQ_SET(x)            (R_DMA2CNT &= 0xBFFF);\
					(R_DMA2CNT |= (x)<<14);
#define M_DMA2CNT_IRQ_ON                (R_DMA2CNT |= 0x4000);
#define M_DMA2CNT_IRQ_OFF               (R_DMA2CNT &= 0xBFFF);
#define M_DMA2CNT_DMA_ENABLED           (R_DMA2CNT |= 0x8000);
#define M_DMA2CNT_DMA_DISABLED          (R_DMA2CNT &= 0x7FFF);

/* 
---------------------------------------------------
  IO 0x0D4 -- R_DMA3SRC (28 bit val)
---------------------------------------------------	
*/
#define R_DMA3SRC		*(volatile u32 *)(MEM_IO + 0x0D4)

/* Masks */
#define V_DMA3SRC_MSK  0x0FFFFFFF

/* Macros */
#define M_DMA3SRC_SET(adr)  (R_DMA3SRC = (adr & V_DMA3SRC_MSK));

/* 
---------------------------------------------------
  IO 0x0D8 -- R_DMA3DST (28 bit val)
---------------------------------------------------	
*/
#define R_DMA3DST		*(volatile u32 *)(MEM_IO + 0x0D8)

/* Masks */
#define V_DMA3DST_MSK  0x0FFFFFFF

/* Macros */
#define M_DMA3DST_SET(adr)  (R_DMA3DST = (adr));

/* 
---------------------------------------------------
  IO 0x0DC -- R_DMA3COUNT
  Accepted values are:
  0x0000             -> transfer 0x10000 units 
                        (equals  0x20000 bytes in 16bit mode)
  0x0001 - 0xFFFF    -> transfer units given 
---------------------------------------------------	
*/
#define R_DMA3COUNT		*(volatile u16 *)(MEM_IO + 0x0DC)

/* Masks */
#define V_DMA3COUNT_MSK 0xFFFF

/* Macros */
#define M_DMA3COUNT_SET(val)     (R_DMA3COUNT = (val));     

/* 
---------------------------------------------------
  IO 0x0DE -- R_DMA3CNT
  Controls parameters for DMA3
---------------------------------------------------	
*/
#define R_DMA3CNT		*(volatile u16 *)(MEM_IO + 0x0DE)

/* Masks */
#define V_DMA3CNT_SRCCNT_MSK 0xFE7F
#define V_DMA3CNT_DSTCNT_MSK 0xFF9F
#define V_DMA3CNT_STARTAT_MSK 0xCFFF

/* Macros */
#define M_DMA3CNT_DSTCNT_POST_INC       (R_DMA3CNT &= V_DMA3CNT_DSTCNT_MSK);
#define M_DMA3CNT_DSTCNT_POST_DEC       (R_DMA3CNT &= V_DMA3CNT_DSTCNT_MSK);\
                                        (R_DMA3CNT |= 0x0020);
#define M_DMA3CNT_DSTCNT_NOCHANGE       (R_DMA3CNT &= V_DMA3CNT_DSTCNT_MSK);\
                                        (R_DMA3CNT |= 0x0040);
#define M_DMA3CNT_DSTCNT_SET(x)         (R_DMA3CNT &= V_DMA3CNT_DSTCNT_MSK);\
                                        (R_DMA3CNT |= (x)<<5);
#define M_DMA3CNT_SRCCNT_POST_INC       (R_DMA3CNT &= V_DMA3CNT_SRCCNT_MSK);
#define M_DMA3CNT_SRCCNT_POST_DEC       (R_DMA3CNT &= V_DMA3CNT_SRCCNT_MSK);\
                                        (R_DMA3CNT |= 0x0080);
#define M_DMA3CNT_SRCCNT_NOCHANGE       (R_DMA3CNT &= V_DMA3CNT_SRCCNT_MSK);\
                                        (R_DMA3CNT |= 0x0100);
#define M_DMA3CNT_SRCCNT_SET(x)         (R_DMA3CNT &= V_DMA3CNT_SRCCNT_MSK);\
                                        (R_DMA3CNT |= (x)<<7);
#define M_DMA3CNT_REPEAT_ON             (R_DMA3CNT |= 0x0200);
#define M_DMA3CNT_REPEAT_OFF            (R_DMA3CNT &= 0xFDFF);
#define M_DMA3CNT_REPEAT_SET(x)         (R_DMA3CNT &= NOT_BIT9);\
					(R_DMA3CNT |= (x)<<9);
#define M_DMA3CNT_TRANSFER_SET(mode)    (R_DMA3CNT &= 0xFBFF);\
                                        (R_DMA3CNT |= (mode<<10));
#define M_DMA3CNT_TRANSFER_32BITS       (R_DMA3CNT |= 0x0400);
#define M_DMA3CNT_TRANSFER_16BITS       (R_DMA3CNT &= 0xFBFF);
#define M_DMA3CNT_STARTAT_SET(x)        (R_DMA3CNT &= V_DMA0CNT_STARTAT_MSK);\
                                        (R_DMA3CNT |= (x<<12));
#define M_DMA3CNT_STARTAT_NOW           (R_DMA3CNT &= V_DMA3CNT_STARTAT_MSK);
#define M_DMA3CNT_STARTAT_VBL           (R_DMA3CNT &= V_DMA3CNT_STARTAT_MSK);\
                                        (R_DMA3CNT |= 0x1000);
#define M_DMA3CNT_STARTAT_HBL           (R_DMA3CNT &= V_DMA3CNT_STARTAT_MSK);\
                                        (R_DMA3CNT |= 0x2000);
#define M_DMA3CNT_STARTAT_SOUNDIRQ      (R_DMA3CNT &= V_DMA3CNT_STARTAT_MSK);\
                                        (R_DMA3CNT |= 0x3000);
#define M_DMA3CNT_IRQ_SET(x)            (R_DMA3CNT &= 0xBFFF);\
					(R_DMA3CNT |= (x)<<14);
#define M_DMA3CNT_IRQ_ON                (R_DMA3CNT |= 0x4000);
#define M_DMA3CNT_IRQ_OFF               (R_DMA3CNT &= 0xBFFF);
#define M_DMA3CNT_DMA_ENABLED           (R_DMA3CNT |= 0x8000);
#define M_DMA3CNT_DMA_DISABLED          (R_DMA3CNT &= 0x7FFF);

/* 
---------------------------------------------------
  IO 0x100 --	R_TIM0COUNT
  This holds the value TimerX is counting up to (from
  0) before firing an interrupt. Attributes in
  TIMXCNT affect the counting process.

  After you write to this register, this value is saved
  in the hardware and, if the timer is running, counts 
  up. 

  Once it reaches the value you wrote, if enabled, interrupts are fired,
  and the value is reset back to 0, and counting begins again.

  To overwrite the current value, write a new
  value into it. If you read from this register, you will get the
  value that the timer is currently at when counting up. 

  2002/01/01 Thx to costis for this info.
  2002/01/05 Updated the docs here, timer counts up, not down.
             thx again costis
---------------------------------------------------	
*/
#define R_TIM0COUNT		*(volatile u16 *)(MEM_IO + 0x100)

/* Functions */
#define F_TIM0COUNT_GET_CURRENT_VALUE  (R_TIM0COUNT)

/* Macros */
#define M_TIM0COUNT_SET(value)  (R_TIM0COUNT=value);

/* 
---------------------------------------------------
  IO 0x102 --	R_TIM0CNT
  Controls Attributes of the timer counting process

  Speed select:
  Setting -- Countup interval
  0         59,595 nanos (running 16,78MHz)
  1         3,841  micrs (running clock/64)
  2         15,256 micrs (running clock/256)
  3         61,025 micrs (running clock/1024)
---------------------------------------------------	
*/
#define R_TIM0CNT		*(volatile u16 *)(MEM_IO + 0x102)

/* Masks */
#define V_TIM0CNT_SPEED_SELECT_MSK  0xFFFC
/* Macros */
#define M_TIM0CNT_SPEED_SELECT_SET(value)   (R_TIM0CNT &= V_TIM0CNT_SPEED_SELECT_MSK);\
                                            (R_TIM0CNT |= value);
#define M_TIM0CNT_TIMER_START               (R_TIM0CNT |= 0x0080);
#define M_TIM0CNT_TIMER_STOP                (R_TIM0CNT &= 0xFF7F);
#define M_TIM0CNT_IRQ_ENABLE                (R_TIM0CNT |= 0x0040);
#define M_TIM0CNT_IRQ_DISABLE               (R_TIM0CNT &= 0xFFBF);

/* 
---------------------------------------------------
  IO 0x104 --	R_TIM1COUNT
  This holds the value TimerX is counting up to (from
  0) before firing an interrupt. Attributes in
  TIMXCNT affect the counting process
  After you write to this register, this value is saved
  in the hardware and, if the timer is running, counts 
  up. 

  Once it reaches the value you wrote, if enabled, interrupts are fired,
  and the value is reset back to 0, and counting begins again.

  To overwrite the current value, write a new
  value into it. If you read from this register, you will get the
  value that the timer is currently at when counting up. 

  2002/01/01 Thx to costis for this info.
  2002/01/05 Updated the docs here, timer counts up, not down.
             thx again costis
---------------------------------------------------	
*/
#define R_TIM1COUNT		*(volatile u16 *)(MEM_IO + 0x104)

/* Functions */
#define F_TIM1COUNT_GET_CURRENT_VALUE  (R_TIM1COUNT)

/* Macros */
#define M_TIM1COUNT_SET(value)  (R_TIM1COUNT=value);

/* 
---------------------------------------------------
  IO 0x106 --	R_TIM1CNT
  Controls Attributes of the timer counting process

  Speed select:
  Setting -- Countup interval
  0         59,595 nanos (running 16,78MHz)
  1         3,841  micrs (running clock/64)
  2         15,256 micrs (running clock/256)
  3         61,025 micrs (running clock/1024)
---------------------------------------------------	
*/
#define R_TIM1CNT		*(volatile u16 *)(MEM_IO + 0x106)

/* Masks */
#define V_TIM1CNT_SPEED_SELECT_MSK  0xFFFC
/* Macros */
#define M_TIM1CNT_SPEED_SELECT_SET(value)   (R_TIM1CNT &= V_TIM1CNT_SPEED_SELECT_MSK);\
                                            (R_TIM1CNT |= value);
#define M_TIM1CNT_TIMER_START               (R_TIM1CNT |= 0x0080);
#define M_TIM1CNT_TIMER_STOP                (R_TIM1CNT &= 0xFF7F);
#define M_TIM1CNT_IRQ_ENABLE                (R_TIM1CNT |= 0x0040);
#define M_TIM1CNT_IRQ_DISABLE               (R_TIM1CNT &= 0xFFBF);

/* 
---------------------------------------------------
  IO 0x108 --	R_TIM2COUNT
  This holds the value TimerX is counting up to (from
  0) before firing an interrupt. Attributes in
  TIMXCNT affect the counting process
  After you write to this register, this value is saved
  in the hardware and, if the timer is running, counts 
  up. 

  Once it reaches the value you wrote, if enabled, interrupts are fired,
  and the value is reset back to 0, and counting begins again.

  To overwrite the current value, write a new
  value into it. If you read from this register, you will get the
  value that the timer is currently at when counting up. 

  2002/01/01 Thx to costis for this info.
  2002/01/05 Updated the docs here, timer counts up, not down.
             thx again costis
---------------------------------------------------	
*/
#define R_TIM2COUNT		*(volatile u16 *)(MEM_IO + 0x108)

/* Functions */
#define F_TIM2COUNT_GET_CURRENT_VALUE  (R_TIM2COUNT)

/* Macros */
#define M_TIM2COUNT_SET(value)  (R_TIM2COUNT=value);

/* 
---------------------------------------------------
  IO 0x10A --	R_TIM2CNT
  Controls Attributes of the timer counting process

  Speed select:
  Setting -- Countup interval
  0         59,595 nanos (running 16,78MHz)
  1         3,841  micrs (running clock/64)
  2         15,256 micrs (running clock/256)
  3         61,025 micrs (running clock/1024)
---------------------------------------------------	
*/
#define R_TIM2CNT		*(volatile u16 *)(MEM_IO + 0x10A)

/* Masks */
#define V_TIM2CNT_SPEED_SELECT_MSK  0xFFFC
/* Macros */
#define M_TIM2CNT_SPEED_SELECT_SET(value)   (R_TIM2CNT &= V_TIM2CNT_SPEED_SELECT_MSK);\
                                            (R_TIM2CNT |= value);
#define M_TIM2CNT_TIMER_START               (R_TIM2CNT |= 0x0080);
#define M_TIM2CNT_TIMER_STOP                (R_TIM2CNT &= 0xFF7F);
#define M_TIM2CNT_IRQ_ENABLE                (R_TIM2CNT |= 0x0040);
#define M_TIM2CNT_IRQ_DISABLE               (R_TIM2CNT &= 0xFFBF);

/* 
---------------------------------------------------
  IO 0x10C --	R_TIM3COUNT
  This holds the value TimerX is counting up to (from
  0) before firing an interrupt. Attributes in
  TIMXCNT affect the counting process

  After you write to this register, this value is saved
  in the hardware and, if the timer is running, counts 
  up. 

  Once it reaches the value you wrote, if enabled, interrupts are fired,
  and the value is reset back to 0, and counting begins again.

  To overwrite the current value, write a new
  value into it. If you read from this register, you will get the
  value that the timer is currently at when counting up. 

  2002/01/01 Thx to costis for this info.
  2002/01/05 Updated the docs here, timer counts up, not down.
             thx again costis
---------------------------------------------------	
*/
#define R_TIM3COUNT		*(volatile u16 *)(MEM_IO + 0x10C)

/* Functions */
#define F_TIM3COUNT_GET_CURRENT_VALUE  (R_TIM3COUNT)

/* Macros */
#define M_TIM3COUNT_SET(value)  (R_TIM3COUNT=value);


/* 
---------------------------------------------------
  IO 0x10E --	R_TIM3CNT
  Controls Attributes of the timer counting process

  Speed select:
  Setting -- Countup interval
  0         59,595 nanos (running 16,78MHz)
  1         3,841  micrs (running clock/64)
  2         15,256 micrs (running clock/256)
  3         61,025 micrs (running clock/1024)
---------------------------------------------------	
*/
#define R_TIM3CNT		*(volatile u16 *)(MEM_IO + 0x10E)

/* Masks */
#define V_TIM3CNT_SPEED_SELECT_MSK  0xFFFC
/* Macros */
#define M_TIM3CNT_SPEED_SELECT_SET(value)   (R_TIM3CNT &= V_TIM3CNT_SPEED_SELECT_MSK);\
                                            (R_TIM3CNT |= value);
#define M_TIM3CNT_TIMER_START               (R_TIM3CNT |= 0x0080);
#define M_TIM3CNT_TIMER_STOP                (R_TIM3CNT &= 0xFF7F);
#define M_TIM3CNT_IRQ_ENABLE                (R_TIM3CNT |= 0x0040);
#define M_TIM3CNT_IRQ_DISABLE               (R_TIM3CNT &= 0xFFBF);

/* 
---------------------------------------------------
  IO 0x130 --	R_CTRLINPUT

  This register holds the values of the controller
  buttons. 1 means not pressed, 0 means pressed.
---------------------------------------------------	
*/
#define R_CTRLINPUT		*(volatile u16 *)(MEM_IO + 0x130)

/* Constants */
/* Masks */
#define V_CTRLINPUT_MSK  0xFC00
/* Functions */
#define F_CTRLINPUT_A_PRESSED        (~R_CTRLINPUT & 0x0001)
#define F_CTRLINPUT_B_PRESSED       ((~R_CTRLINPUT & 0x0002)>>1)
#define F_CTRLINPUT_SELECT_PRESSED  ((~R_CTRLINPUT & 0x0004)>>2)
#define F_CTRLINPUT_START_PRESSED   ((~R_CTRLINPUT & 0x0008)>>3)
#define F_CTRLINPUT_RIGHT_PRESSED   ((~R_CTRLINPUT & 0x0010)>>4)
#define F_CTRLINPUT_LEFT_PRESSED    ((~R_CTRLINPUT & 0x0020)>>5)
#define F_CTRLINPUT_UP_PRESSED      ((~R_CTRLINPUT & 0x0040)>>6)
#define F_CTRLINPUT_DOWN_PRESSED    ((~R_CTRLINPUT & 0x0080)>>7)
#define F_CTRLINPUT_R_PRESSED       ((~R_CTRLINPUT & 0x0100)>>8)
#define F_CTRLINPUT_L_PRESSED       ((~R_CTRLINPUT & 0x0200)>>9)
/* Macros */




/* 
---------------------------------------------------
  IO 0x132 -- R_CTRLINT  Controller Interrupt Request Register

  Will fire a controller interrupt if the flagged 
  conditions are met. You can flag any key, and give
  the interrupt condition of AND (all specified keys
  pressed) or OR (any specified key pressed) when 
  choosing multiple keys. 
---------------------------------------------------	
*/
#define R_CTRLINT		*(volatile u16 *)(MEM_IO + 0x132)

/* Constants */
/* Masks */
#define V_CTRLINT_MSK  0xFC00
/* Functions */
/* Macros */
#define M_CTRLINT_IRQ_DISABLE       (R_CTRLINT &= 0xBFFF);
#define M_CTRLINT_IRQ_ENABLE        (R_CTRLINT |= 0x4000);

#define M_CTRLINT_IRQ_COND_AND      (R_CTRLINT |= 0x8000);
#define M_CTRLINT_IRQ_COND_OR       (R_CTRLINT &= 0x7FFF);

#define M_CTRLINT_KEY_SET_A         (R_CTRLINT |= 0x0001);
#define M_CTRLINT_KEY_SET_B         (R_CTRLINT |= 0x0002);
#define M_CTRLINT_KEY_SET_SELECT    (R_CTRLINT |= 0x0004);
#define M_CTRLINT_KEY_SET_START     (R_CTRLINT |= 0x0008);
#define M_CTRLINT_KEY_SET_RIGHT     (R_CTRLINT |= 0x0010);
#define M_CTRLINT_KEY_SET_LEFT      (R_CTRLINT |= 0x0020);
#define M_CTRLINT_KEY_SET_UP        (R_CTRLINT |= 0x0040);
#define M_CTRLINT_KEY_SET_DOWN      (R_CTRLINT |= 0x0080);
#define M_CTRLINT_KEY_SET_R         (R_CTRLINT |= 0x0100);
#define M_CTRLINT_KEY_SET_L         (R_CTRLINT |= 0x0200);

#define M_CTRLINT_KEY_CLR_A         (R_CTRLINT &= 0xFFFE);
#define M_CTRLINT_KEY_CLR_B         (R_CTRLINT &= 0xFFFD);
#define M_CTRLINT_KEY_CLR_SELECT    (R_CTRLINT &= 0xFFFB);
#define M_CTRLINT_KEY_CLR_START     (R_CTRLINT &= 0xFFF7);
#define M_CTRLINT_KEY_CLR_RIGHT     (R_CTRLINT &= 0xFFEF);
#define M_CTRLINT_KEY_CLR_LEFT      (R_CTRLINT &= 0xFFDF);
#define M_CTRLINT_KEY_CLR_UP        (R_CTRLINT &= 0xFFBF);
#define M_CTRLINT_KEY_CLR_DOWN      (R_CTRLINT &= 0xFF7F);
#define M_CTRLINT_KEY_CLR_R         (R_CTRLINT &= 0xFEFF);
#define M_CTRLINT_KEY_CLR_L         (R_CTRLINT &= 0xFDFF);





/* 
---------------------------------------------------
  IO 0x208 -- R_INTMST  Interrupt Enable Master Register

  If this register is set to 0, NO IRQs are executed
  If set to 1, normal IRQ operation happens
---------------------------------------------------	
*/
#define R_INTMST		*(volatile u16 *)(MEM_IO + 0x208)

/* Constants */
/* Masks */
/* Functions */
#define F_INTMST_ENABLED     (R_INTMST & 1)
/* Macros */
#define M_INTMST_ENABLE      (R_INTMST |= 1);
#define M_INTMST_DISABLE     (R_INTMST &= 0);


/* 
---------------------------------------------------
  IO 0x200 -- R_INTENA  Interrupt Enable Register

  In this register, you can set up all interrupts
  manually, if they should occur or not.
---------------------------------------------------	
*/
#define R_INTENA		*(volatile u16 *)(MEM_IO + 0x200)

/* Constants */
/* Masks */
/* Functions */
/* Macros */
#define M_INTENA_VBL_ENABLE        (R_INTENA |= 0x0001);
#define M_INTENA_HBL_ENABLE        (R_INTENA |= 0x0002);
#define M_INTENA_VCNT_ENABLE       (R_INTENA |= 0x0004);
#define M_INTENA_TIM0_ENABLE       (R_INTENA |= 0x0008);
#define M_INTENA_TIM1_ENABLE       (R_INTENA |= 0x0010);
#define M_INTENA_TIM2_ENABLE       (R_INTENA |= 0x0020);
#define M_INTENA_TIM3_ENABLE       (R_INTENA |= 0x0040);
#define M_INTENA_SIO_ENABLE        (R_INTENA |= 0x0080);
#define M_INTENA_DMA0_ENABLE       (R_INTENA |= 0x0100);
#define M_INTENA_DMA1_ENABLE       (R_INTENA |= 0x0200);
#define M_INTENA_DMA2_ENABLE       (R_INTENA |= 0x0400);
#define M_INTENA_DMA3_ENABLE       (R_INTENA |= 0x0800);
#define M_INTENA_KEY_ENABLE        (R_INTENA |= 0x1000);
#define M_INTENA_CART_ENABLE       (R_INTENA |= 0x2000);
#define M_INTENA_INT_ENABLE(x)     (R_INTENA |= 1<<(x));

#define M_INTENA_VBL_DISABLE       (R_INTENA &= 0xFFFE);
#define M_INTENA_HBL_DISABLE       (R_INTENA &= 0xFFFD);
#define M_INTENA_VCNT_DISABLE      (R_INTENA &= 0xFFFB);
#define M_INTENA_TIM0_DISABLE      (R_INTENA &= 0xFFF7);
#define M_INTENA_TIM1_DISABLE      (R_INTENA &= 0xFFEF);
#define M_INTENA_TIM2_DISABLE      (R_INTENA &= 0xFFDF);
#define M_INTENA_TIM3_DISABLE      (R_INTENA &= 0xFFBF);
#define M_INTENA_SIO_DISABLE       (R_INTENA &= 0xFF7F);
#define M_INTENA_DMA0_DISABLE      (R_INTENA &= 0xFEFF);
#define M_INTENA_DMA1_DISABLE      (R_INTENA &= 0xFDFF);
#define M_INTENA_DMA2_DISABLE      (R_INTENA &= 0xFBFF);
#define M_INTENA_DMA3_DISABLE      (R_INTENA &= 0xF7FF);
#define M_INTENA_KEY_DISABLE       (R_INTENA &= 0xEFFF);
#define M_INTENA_CART_DISABLE      (R_INTENA &= 0xDFFF);
#define M_INTENA_INT_DISABLE(x)    (R_INTENA &= 0xFFFE<<(x));


/* 
---------------------------------------------------
  IO 0x202 -- R_INTREQ  Interrupt Request Register

  In this register, you can see which hardware
  devices in the GBA generated interrupts

  Also, you can reset currently scheduled interrupts
  by invoking M_INTREQ_*_RESET. I am not 100% sure if
  this really does it, but i heard it is the way to do
  it, although, to me, R_INTREQ seems read-only.
---------------------------------------------------	
*/
#define R_INTREQ		*(volatile u16 *)(MEM_IO + 0x202)

/* Constants */
/* Masks */
/* Functions */
#define F_INTREQ_VBL_STATUS  (R_INTREQ & 0x0001)
#define F_INTREQ_HBL_STATUS  ((R_INTREQ & 0x0002)>>1)
#define F_INTREQ_VCNT_STATUS ((R_INTREQ & 0x0004)>>2)
#define F_INTREQ_TIM0_STATUS ((R_INTREQ & 0x0008)>>3)
#define F_INTREQ_TIM1_STATUS ((R_INTREQ & 0x0010)>>4)
#define F_INTREQ_TIM2_STATUS ((R_INTREQ & 0x0020)>>5)
#define F_INTREQ_TIM3_STATUS ((R_INTREQ & 0x0040)>>6)
#define F_INTREQ_SIO_STATUS  ((R_INTREQ & 0x0080)>>7)
#define F_INTREQ_DMA0_STATUS ((R_INTREQ & 0x0100)>>8)
#define F_INTREQ_DMA1_STATUS ((R_INTREQ & 0x0200)>>9)
#define F_INTREQ_DMA2_STATUS ((R_INTREQ & 0x0400)>>10)
#define F_INTREQ_DMA3_STATUS ((R_INTREQ & 0x0800)>>11)
#define F_INTREQ_KEY_STATUS ((R_INTREQ & 0x1000)>>12)
#define F_INTREQ_CART_STATUS ((R_INTREQ & 0x2000)>>13)

/* Macros */

#define M_INTREQ_VBL_RESET       (R_INTREQ |= 0x0001);
#define M_INTREQ_HBL_RESET       (R_INTREQ |= 0x0002);
#define M_INTREQ_VCNT_RESET      (R_INTREQ |= 0x0004);
#define M_INTREQ_TIM0_RESET      (R_INTREQ |= 0x0008);
#define M_INTREQ_TIM1_RESET      (R_INTREQ |= 0x0010);
#define M_INTREQ_TIM2_RESET      (R_INTREQ |= 0x0020);
#define M_INTREQ_TIM3_RESET      (R_INTREQ |= 0x0040);
#define M_INTREQ_SIO_RESET       (R_INTREQ |= 0x0080);
#define M_INTREQ_DMA0_RESET      (R_INTREQ |= 0x0100);
#define M_INTREQ_DMA1_RESET      (R_INTREQ |= 0x0200);
#define M_INTREQ_DMA2_RESET      (R_INTREQ |= 0x0400);
#define M_INTREQ_DMA3_RESET      (R_INTREQ |= 0x0800);
#define M_INTREQ_KEY_RESET      (R_INTREQ |= 0x1000);
#define M_INTREQ_CART_RESET      (R_INTREQ |= 0x2000);

/*
---------------------------------------------------
  IO 0x204 -- R_WAITCNT  Memory Access Wait Control Register

---------------------------------------------------	
*/
#define R_WAITCNT		*(volatile u16 *)(MEM_IO + 0x204)

/* Constants */
/* Masks */
/* Functions */

/* 
---------------------------------------------------
---------------------------------------------------	

      Predefined Tools 

---------------------------------------------------
---------------------------------------------------	
*/
/*
--------------------------------------
 BG Tool defines
--------------------------------------
*/

#define TOOL_INIT_BG(bgno,prio,cbb,sbb,colmode,mapsize,mosaic,active) \
                        M_BGXCNT_PRIO_SET(bgno,prio) \
                        M_BGXCNT_CHRBB_SET(bgno,cbb) \
                        M_BGXCNT_SCRBB_SET(bgno,sbb) \
                        M_BGXCNT_COLMODE_SET(bgno,colmode) \
                        M_BGXCNT_SCRSIZE_SET(bgno,mapsize) \
                        M_BGXCNT_MOSAIC_SET(bgno,mosaic) \
                        M_DISCNT_BGX_SET(bgno,active)



/*
--------------------------------------
 OAM Tool defines
--------------------------------------
*/

/*

  Notes on TOOL_OAM_OBJ_CREATE

  This allows you to set up a new object (sprite) in 
  the OAM. Also consider using the structs and functions
  provided in mygba_sys.c instead of using this tool
  directly, if you wish to...

  Parameters are as follows:

  obj_number :  the OAM object number you want to
                create. values: 0-127
  char_number : the character start adress. If you are
                using 16 pals /16 color sprites, every
                basic char (8*8 pixels) fits into a 32
                bytes space, for 256 col mode, it needs
                64 bytes. Depending on this, you can write
                values from 0-1024 to this field.

  xpos:         x coordinate for the object (0-511)
  ypos:         y coordinate for the object (0-255)

  obj_mode   :  set the obj display mode:
                0 = normal object
                1 = use transparency features
                2 = OBJ window style

  obj_shape :   set the shape of your object:
                0 = square
                1 = horizontal rectangle
                2 = vertical rectangle

  obj_size :    set size of obj (depends on shape)

                    shape:0     shape:1     shape:2
                0 = 8*8         16*8        8*16
                1 = 16*16       32*8        8*32
                2 = 32*32       32*16       16*32
                3 = 64*64       64*32       32*64

  color_mode :  set the color mode:
                0 = 16 cols, 16 palettes
                1 = 256 cols, 1 palette

  pal_number :  only set this if you are using 16col:16pal 
                objects. Allowed vals: 0-15


  prio_to_bg :  specifies the priority relative to the bg
                values: 0 (highest) - 3 (lowest)

  vflip:        use for flipping obj vertically
                0 = no flip
                1 = flip
                
  hflip:        use for flipping obj horizontally
                0 = no flip
                1 = flip

  mosaic:       turn on/off mosaic for this OBJ
                0 = don't 
                1 = do

  rotation:     turn on/off rotation for this OBJ.
                Since Rotation/Scaling params are shared amongst
                OBJs, and as such are intrusive to more than the 
                OBJ set up in this code, they are not set up in this
                macro. Use TOOL_OAM_ROT_SETUP for setting rotation
                data.
                
                0 = off
                1 = on

  rotation_no:  set the rotation dataset (totally 32 independant
                sets are available for OBJ). Multiple OBJs can use
                the same dataset, so its your responsibility to
                coordinate OBJs writing to these datasets. See 
                rotation parameter above for more info.

                0-31 = set rotation data set

  dblsize:      Assigns the OBJ double the x and y size. This is 
                only useful for rotation, where the OBJ might break 
                out of its boundaries.
                0 = normal size
                1 = double size (both x and y)

*/


#define TOOL_OAM_OBJ_CREATE(obj_number,char_number,xpos,ypos,obj_mode,obj_shape,obj_size,color_mode,pal_number,prio_to_bg,vflip,hflip,mosaic,rotation,rotation_no,dblsize) \
                        \
                        ACCESS_16(MEM_OAM+8*obj_number)= \
                        (obj_shape<<14)  | \
                        (color_mode<<13) | \
                        (mosaic<<12)     | \
                        (obj_mode<<10)   | \
                        (rotation<<8)    | \
                        (dblsize<<9)     | \
                        (ypos); \
                        \
                        ACCESS_16(MEM_OAM+8*obj_number+2)= \
                        (obj_size<<14)   |\
                        (vflip<<13)      |\
                        (hflip<<12)      |\
                        (rotation_no<<9) |\
                        (xpos);\
                        \
                        ACCESS_16(MEM_OAM+8*obj_number+4)= \
                        (pal_number<<12) |\
                        (prio_to_bg<<10) |\
                        (char_number);

#define TOOL_OAM_OBJ_KILL(obj_number)\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)) = 0;\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+2) = 0;\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+4) = 0;

#define TOOL_OAM_OBJ_SET_XPOS(obj_number,xpos)\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+2) &= 0xFE00;\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+2) |= xpos;

#define TOOL_OAM_OBJ_SET_YPOS(obj_number,ypos)\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)) &= 0xFF00;\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)) |= ypos;

#define TOOL_OAM_OBJ_SET_HFLIP_ON(obj_number)\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+2) |= 0x1000;

#define TOOL_OAM_OBJ_SET_HFLIP_OFF(obj_number)\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+2) &= 0xEFFF;

#define TOOL_OAM_OBJ_SET_VFLIP_ON(obj_number)\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+2) |= 0x2000;

#define TOOL_OAM_OBJ_SET_VFLIP_OFF(obj_number)\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+2) &= 0xDFFF;

#define TOOL_OAM_OBJ_SET_MOSAIC_ON(obj_number)\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)) |= 0x1000;

#define TOOL_OAM_OBJ_SET_MOSAIC_OFF(obj_number)\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)) &= 0xEFFF;

#define TOOL_OAM_OBJ_SET_CHR(obj_number,char_number)\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+4) &= 0xFC00;\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+4) |= char_number;

#define TOOL_OAM_OBJ_SET_PRIO(obj_number,prio)\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+4) &= 0xF3FF;\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+4) |= prio<<10;

#define TOOL_OAM_OBJ_SET_PAL(obj_number,pal_number)\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+4) &= 0xFFF;\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+4) |= pal_number<<12;
#define TOOL_OAM_OBJ_SET_ROT(obj_number,rot)\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)) &= 0xFEFF;\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)) |= rot<<8;
#define TOOL_OAM_OBJ_SET_ROTSET(obj_number,rotset)\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+2) &= 0xC1FF;\
                        *(volatile u16 *)(MEM_OAM+(8*obj_number)+2) |= rotset<<9;


/*
    Notes on TOOL_OAM_ROT_SETUP

    This tool accepts fixed point numbers for writing the
    Rotation registers. Writing Integers to this tool will
    cause the display to misbehave. You can convert integers
    into the required 1:7:8 fixed point format (16bit signed)
    using the CONV_S*_TO_SFP16 defines. For more information
    you can have a look at them pretty much at the top of this file.

    Be aware that you have to use signed integers or floats for 
    the conversion.

    Example of a correct call:

    float my_dx  =1;
    float my_dy  =0;
    float my_dmx =0;
    float my_dmy =1;
    TOOL_OAM_ROT_SETUP( 3,
                        CONV_FLOAT_TO_SFP16(my_dx),
                        CONV_FLOAT_TO_SFP16(my_dy),
                        CONV_FLOAT_TO_SFP16(my_dmx),
                        CONV_FLOAT_TO_SFP16(my_dmy))

    This will insert the rotation values into the rotation
    dataset 3 (out of 32 total, specifiy 0-31). The values
    specified above are the middle values (no scaling, no rot.).
    To see rotation, set dy to 1.2 ...
    You can assign this dataset to an OBJ using the rotation_no 
    parameter of TOOL_OAM_OBJ_CREATE.

    
*/
#define TOOL_OAM_ROT_SETUP(rotation_no,dx,dy,dmx,dmy)\
                        ACCESS_16(MEM_OAM+(32*rotation_no)+6) = dx;\
                        ACCESS_16(MEM_OAM+(32*rotation_no)+14) = dmx;\
                        ACCESS_16(MEM_OAM+(32*rotation_no)+22) = dy;\
                        ACCESS_16(MEM_OAM+(32*rotation_no)+30) = dmy;


/*
--------------------------------------
 BG Tool defines
--------------------------------------
*/
#define TOOL_BG_EMPTY_SCR(sbb,val,size)  memset(MEM_SCR_BB_PTR(sbb),val,size);

/*
--------------------------------------
 PAL Tool defines
--------------------------------------
*/

#define TOOL_SET_BG_PAL_16(pal_number,color_number,red,green,blue) \
                        *(volatile u16 *)(MEM_PAL_BG+((pal_number)<<5)+((color_number)<<1))=RGB(red,green,blue);
#define TOOL_SET_BG_PAL_256(color_number,red,green,blue) \
                        *(volatile u16 *)(MEM_PAL_BG+((color_number)<<1))=RGB(red,green,blue);
#define TOOL_SET_OBJ_PAL_16(pal_number,color_number,red,green,blue) \
                        *(volatile u16 *)(MEM_PAL_OBJ+(((pal_number)<<5))+((color_number)<<1))=RGB(red,green,blue);
#define TOOL_SET_OBJ_PAL_256(color_number,red,green,blue) \
                        *(volatile u16 *)(MEM_PAL_OBJ+((color_number)<<1))=RGB(red,green,blue);

#define TOOL_GET_BG_PALCOL(color_number) (*MEM_PAL_COL_PTR(color_number))
#define TOOL_GET_BG_PALCOL_R(color_number) (*MEM_PAL_COL_PTR(color_number) & 0x1f)
#define TOOL_GET_BG_PALCOL_G(color_number) (((*MEM_PAL_COL_PTR(color_number))>>5) & 0x1f)
#define TOOL_GET_BG_PALCOL_B(color_number) (((*MEM_PAL_COL_PTR(color_number))>>10) & 0x1f)

#define TOOL_EMPTY_OAM  memset(MEM_OAM_PTR,0x00,MEM_OAM_SIZE);

/*
--------------------------------------
 WIN Tool defines
--------------------------------------
*/

#define TOOL_SET_WIN0XY(x1,y1,x2,y2) \
                        M_WIN0X_LEFT_SET(x1)\
                        M_WIN0X_RIGHT_SET(x2)\
                        M_WIN0Y_UPPER_SET(y1)\
                        M_WIN0Y_LOWER_SET(y2)

#define TOOL_SET_WIN1XY(x1,y1,x2,y2) \
                        M_WIN1X_LEFT_SET(x1)\
                        M_WIN1X_RIGHT_SET(x2)\
                        M_WIN1Y_UPPER_SET(y1)\
                        M_WIN1Y_LOWER_SET(y2)



/*
--------------------------------------
 DMA Tool defines

 Please be careful with DMA, different channels 
 do different things. General rules:

  DMA0   - fastest
           can only be used in system area, NOT 
		   able to read from ROM

  DMA1/2 - sound FIFO DMA
           slightly slower, but suitable for all
		   areas. only these can be used to feed the 
		   sound FIFOs. Be aware that, when using the AFM
		   modplayer by hitmen, these DMA channels are taken
		   for as long as the music is playing!

  DMA3   - general purpose DMA, use this if in doubt


 Use these to schedule your DMA transfers easily.
 srcadr         - start address to read data from
 dstadr         - address to write the data to
 size_in_words  - how many elements to copy. Note that
                  the size of an element depends on the
				  32bit_mode flag
 32bit_mode     - if this is 1, one element is 32bits
                  if this is 0, one element is 16bits
 timing         - specifies when the DMA will start:
                  set to one of these values:
                   - DMA_STARTAT_NOW
                   - DMA_STARTAT_VBL
                   - DMA_STARTAT_HBL
				  
--------------------------------------
*/

#define TOOL_DMA0_SET(srcadr,dstadr,size,bitmode,timing) \
	                  M_DMA0SRC_SET((u32)(srcadr)) \
					  M_DMA0DST_SET((u32)(dstadr)) \
                      M_DMA0COUNT_SET(size) \
                      M_DMA0CNT_TRANSFER_SET(bitmode) \
                      M_DMA0CNT_STARTAT_SET(timing) \
                      M_DMA0CNT_DMA_ENABLED


#define TOOL_DMA1_SET(srcadr,dstadr,size,bitmode,timing) \
	                  M_DMA1SRC_SET((u32)(srcadr)) \
					  M_DMA1DST_SET((u32)(dstadr)) \
                      M_DMA1COUNT_SET(size) \
                      M_DMA1CNT_TRANSFER_SET(bitmode) \
                      M_DMA1CNT_STARTAT_SET(timing) \
                      M_DMA1CNT_DMA_ENABLED


#define TOOL_DMA2_SET(srcadr,dstadr,size,bitmode,timing) \
	                  M_DMA2SRC_SET((u32)(srcadr)) \
					  M_DMA2DST_SET((u32)(dstadr)) \
                      M_DMA2COUNT_SET(size) \
                      M_DMA2CNT_TRANSFER_SET(bitmode) \
                      M_DMA2CNT_STARTAT_SET(timing) \
                      M_DMA2CNT_DMA_ENABLED

#define TOOL_DMA3_SET(srcadr,dstadr,size,bitmode,timing) \
	                  M_DMA3SRC_SET((u32)(srcadr)) \
					  M_DMA3DST_SET((u32)(dstadr)) \
                      M_DMA3COUNT_SET(size) \
                      M_DMA3CNT_TRANSFER_SET(bitmode) \
                      M_DMA3CNT_STARTAT_SET(timing) \
                      M_DMA3CNT_DMA_ENABLED

#define TOOL_DMA0_SET_EX(srcadr,dstadr,size,bitmode,timing,repeat,srcadrcnt,dstadrcnt,irq) \
	                    M_DMA0SRC_SET((u32)(srcadr)) \
					    M_DMA0DST_SET((u32)(dstadr)) \
                        M_DMA0COUNT_SET(size) \
                        M_DMA0CNT_TRANSFER_SET(bitmode) \
                        M_DMA0CNT_STARTAT_SET(timing) \
						M_DMA0CNT_DSTCNT_SET(dstadrcnt)\
						M_DMA0CNT_SRCCNT_SET(srcadrcnt)\
						M_DMA0CNT_REPEAT_SET(repeat)\
						M_DMA0CNT_IRQ_SET(irq)\
                        M_DMA0CNT_DMA_ENABLED

#define TOOL_DMA1_SET_EX(srcadr,dstadr,size,bitmode,timing,repeat,srcadrcnt,dstadrcnt,irq) \
	                    M_DMA1SRC_SET((u32)(srcadr)) \
					    M_DMA1DST_SET((u32)(dstadr)) \
                        M_DMA1COUNT_SET(size) \
                        M_DMA1CNT_TRANSFER_SET(bitmode) \
                        M_DMA1CNT_STARTAT_SET(timing) \
						M_DMA1CNT_DSTCNT_SET(dstadrcnt)\
						M_DMA1CNT_SRCCNT_SET(srcadrcnt)\
						M_DMA1CNT_REPEAT_SET(repeat)\
						M_DMA1CNT_IRQ_SET(irq)\
                        M_DMA1CNT_DMA_ENABLED

#define TOOL_DMA2_SET_EX(srcadr,dstadr,size,bitmode,timing,repeat,srcadrcnt,dstadrcnt,irq) \
	                    M_DMA2SRC_SET((u32)(srcadr)) \
					    M_DMA2DST_SET((u32)(dstadr)) \
						M_DMA2COUNT_SET(size) \
                        M_DMA2CNT_TRANSFER_SET(bitmode) \
                        M_DMA2CNT_STARTAT_SET(timing) \
						M_DMA2CNT_DSTCNT_SET(dstadrcnt)\
						M_DMA2CNT_SRCCNT_SET(srcadrcnt)\
						M_DMA2CNT_REPEAT_SET(repeat)\
						M_DMA2CNT_IRQ_SET(irq)\
                        M_DMA2CNT_DMA_ENABLED

#define TOOL_DMA3_SET_EX(srcadr,dstadr,size,bitmode,timing,repeat,srcadrcnt,dstadrcnt,irq) \
	                M_DMA3SRC_SET((u32)(srcadr)) \
			M_DMA3DST_SET((u32)(dstadr)) \
                        M_DMA3COUNT_SET(size) \
                        M_DMA3CNT_TRANSFER_SET(bitmode) \
                        M_DMA3CNT_STARTAT_SET(timing) \
			M_DMA3CNT_DSTCNT_SET(dstadrcnt)\
			M_DMA3CNT_SRCCNT_SET(srcadrcnt)\
			M_DMA3CNT_REPEAT_SET(repeat)\
			M_DMA3CNT_IRQ_SET(irq)\
                        M_DMA3CNT_DMA_ENABLED
  


/*
--------------------------------------
 INT Tool defines
 Use these to set your Interrupt handling
 function.

 Example:
 TOOL_SET_INTFUNC_VBL(&myVBLFunction)
--------------------------------------
*/

#define TOOL_SET_INTFUNC(intno,adr)     IntrTable[intno]=(u32) adr ;

#define TOOL_SET_INTFUNC_VBL(adr)       IntrTable[0]=(u32) adr ;
#define TOOL_SET_INTFUNC_HBL(adr)       IntrTable[1]=(u32) adr ;
#define TOOL_SET_INTFUNC_VCNT(adr)      IntrTable[2]=(u32) adr ;
#define TOOL_SET_INTFUNC_TIM0(adr)      IntrTable[3]=(u32) adr ;
#define TOOL_SET_INTFUNC_TIM1(adr)      IntrTable[4]=(u32) adr ;
#define TOOL_SET_INTFUNC_TIM2(adr)      IntrTable[5]=(u32) adr ;
#define TOOL_SET_INTFUNC_TIM3(adr)      IntrTable[6]=(u32) adr ;
#define TOOL_SET_INTFUNC_SIO(adr)       IntrTable[7]=(u32) adr ;
#define TOOL_SET_INTFUNC_DMA0(adr)      IntrTable[8]=(u32) adr ;
#define TOOL_SET_INTFUNC_DMA1(adr)      IntrTable[9]=(u32) adr ;
#define TOOL_SET_INTFUNC_DMA2(adr)      IntrTable[10]=(u32) adr ;
#define TOOL_SET_INTFUNC_DMA3(adr)      IntrTable[11]=(u32) adr ;
#define TOOL_SET_INTFUNC_KEY(adr)       IntrTable[12]=(u32) adr ;
#define TOOL_SET_INTFUNC_CART(adr)      IntrTable[13]=(u32) adr ;

extern volatile u8 _hamlib_intset;

#define TOOL_INT_DISABLE_START  { _hamlib_intset=0; if(F_INTMST_ENABLED)  { _hamlib_intset=1; M_INTMST_DISABLE }
#define TOOL_INT_DISABLE_END    if(_hamlib_intset) { M_INTMST_ENABLE } } 



/*
--------------------------------------
 Initialization Tool defines
--------------------------------------
*/

#define TOOL_INIT_DISPLAY(bgmode) \
						M_DISCNT_FRCBLK_ENA \
						M_DISCNT_BG2_ON \
						M_DISCNT_BGMODE_SET(bgmode) \
						M_DISCNT_FRCBLK_DIS

#define TOOL_INIT_SOUND \
						M_SNDCNT3_SOUND_MASTER_ON \
						M_SNDCNT2_DSOUNDA_OUTPUT_L_ENABLE(1) \
						M_SNDCNT2_DSOUNDA_OUTPUT_R_ENABLE(1) \
						M_SNDCNT2_DSOUNDB_OUTPUT_L_ENABLE(1) \
						M_SNDCNT2_DSOUNDB_OUTPUT_R_ENABLE(1) \
						M_SNDCNT2_DSOUNDA_OUTPUTRATIO_SET(1) \
						M_SNDCNT2_SOUND1TO4_OUTPUTRATIO_SET(1) \
						M_SNDCNT1_INIT_ALL_ON						

#define TOOL_INIT_INT   ham_InitIntrTable();


#define TOOL_INIT_ALL(bgmode) \
                        TOOL_INIT_DISPLAY(bgmode) \
                        TOOL_INIT_SOUND \
                        TOOL_INIT_INT



/*
--------------------------------------
 Profiling Tool defines

 WARNING: If you want to use this, make sure that your code does NOT use Timer3. This
 is needed by the profiler.

 Also, naturally, using the profiler online in a GBA program will cause a slight 
 delay because of the profilers counting overhead.

 The following defines can help you to find out how fast a certain piece of your
 program executes. Just add the following define to the top of the source file you
 want to have profiler support in:

 TOOL_PROFILER_INIT

 then, put the start- and endpoints like this:

 TOOL_PROFILER_START
 your_code_here
 TOOL_PROFILER_END

 When the end is encountered, HAM will VBAText out the time elapsed between the start
 and end of the code in question.
--------------------------------------
*/

#ifdef HAM_MBV2_USED
#define PROF_PRINTF dprintf
#else
#define PROF_PRINTF ham_VBAText
#endif //HAM_MBV2_USED

#define TOOL_PROFILER_INIT static u32 ham_prof_subticks=0; \
                          static u32 ham_prof_scanline_start,ham_prof_scanline_end; \
                          static u32 ham_prof_frames_elapsed; \
                          static u32 ham_prof_first_run=1; \
                          void ham_prof_timer3_int() \
                          { ham_prof_frames_elapsed++;  }

#define TOOL_PROFILER_START if(ham_prof_first_run)\
                            {\
                            PROF_PRINTF("HAM Profiler started.     Profiler measures time in ticks.\n"); \
                            PROF_PRINTF("                                     1 Frame = 4399 Ticks.\n"); \
                            PROF_PRINTF("---------------+---------------+--------------+-----------\n"); \
                            PROF_PRINTF("Frames elapsed | Ticks elapsed | Scanl. Start | Scanl. End\n"); \
                            ham_prof_first_run=0;\
                            }\
                            ham_prof_frames_elapsed=0;ham_prof_scanline_start=F_VCNT_CURRENT_SCANLINE; \
                            ham_StartIntHandler(INT_TYPE_TIM3,(void*)&ham_prof_timer3_int); \
                            M_TIM3COUNT_SET(65536-4399) \
                            M_TIM3CNT_SPEED_SELECT_SET(1) \
                            M_TIM3CNT_IRQ_ENABLE \
                            M_TIM3CNT_TIMER_START

#define TOOL_PROFILER_END   ham_prof_scanline_end=F_VCNT_CURRENT_SCANLINE; \
                            M_TIM3CNT_TIMER_STOP \
                            ham_prof_subticks= F_TIM3COUNT_GET_CURRENT_VALUE - 61137; \
                            M_TIM3CNT_IRQ_DISABLE \
                            PROF_PRINTF("      %3d      |     %4d      |      %3d     |     %3d\n",\
                            ham_prof_frames_elapsed, \
                            ham_prof_subticks, \
                            ham_prof_scanline_start, \
                            ham_prof_scanline_end);
                            


#ifdef __cplusplus
extern "C" {
#endif




/* 
---------------------------------------------------
      Debuglib support (For MBV2 style Fileserver/
      printf support. Only needed with an MBV2 and 
      if you are using MBV2LIB.)
---------------------------------------------------	
*/

//
// disregard this, it is for internal use by MBV2lib.
//

extern  int dsprintf(char *,const char*, ...);
extern  void __PrintStr(char *);

extern  unsigned char __outstr[];
extern  unsigned char __finstr[];
extern  unsigned char __kinstr[];
extern  int finptr;
extern  int foutptr;
extern  int kinptr;
extern  int koutptr;

// These two defines actually enable the printf support,
// you want to use dprintf() whenever you print anything.
// I enhanced this function (Originally by Jeff Frohwein) 
// to cope with Interrupts.
// Also, to prevent lockups for people without debugging 
// capabilities (no MBV2) this define resolves to nothing.

#ifdef HAM_DEBUGGER

#define dprintf(x...) ( { if(F_INTMST_ENABLED) {  M_INTMST_DISABLE  dsprintf(__outstr, x); __PrintStr(__outstr); M_INTMST_ENABLE }  else { dsprintf(__outstr, x); __PrintStr(__outstr); } } )
#define dfprintf(y,x...) ({ dsprintf(__outstr, x); __PrintStrToFile(y,__outstr); })

#else //HAM_DEBUGGER

#define dprintf(x...)  
#define dfprintf(y,x...)  

#endif //HAM_DEBUGGER

// This is referncing mbv2lib's get char support
extern  int dgetch (void);

// Set this to 1 in your program to switch to dbg Show when assertion fails
extern u8 _hamlib_assert_to_dbgshow;



/* 
---------------------------------------------------
    MYGBA debugger support
    Enable this only if you have CONTROL_WITH_DEBUGLIB
    set. This enables you to access the functions 
    provided by debug.c throughout the program.
    Example uses are looking at memory, registers,
    and other stuff.
---------------------------------------------------	
*/

extern  void dbg_Show(char*);            //show main debugger interface



/* 
---------------------------------------------------

    libHAM System Structs

    this is the official relacement for the rather
	awkward system structs system. libHAM is a library
    system that will do all the old sys structs did and 
	much much more. Refer to the libHAM documentation
	for more information.

    The following code exposes all the functions currently
	in libham to your program
---------------------------------------------------	
*/

#ifdef HAM_WITH_LIBHAM


// -------------------
// from tile.c
// -------------------

//
// structs
//
typedef struct tile_info_typ
{
    u16 col_mode;			//color mode of these tiles, 0=16 pals 1=1 pal
	u8  first_block;		//the VRAM block the tiles start at, access with MEM_HAMBG_SLOT_PTR(x)			
    u8  offset_from_cbb;	//how many tiles away from CBB
    u8  cbb;				//the CBB
} tile_info,*tile_info_ptr;

typedef struct map_info_typ
{
    u8   first_block;	// bb of this map, access with MEM_HAMBG_SLOT_PTR(x)
    u8   map_rot;		// designates this map to be rotation type map
    u16  map_size;		// size of map data as specified in HW
	u16  map_x;			// actual map size in tiles / x
	u16  map_y;			// actual map size in tiles / y
} map_info,*map_info_ptr;

typedef struct map_fragment_info_typ
{
    u16* src;	        // location of this map (usually ROM/EXWRAM)
    u16* src_ofs;	    // location of the data start IN the map currently set
    u8   map_rot;		// designates this map to be rotation type map
    u8   map_tume;		// shows if this map is based on tumeric data
	u16  map_total_x;	// actual map size in tiles / x
	u16  map_total_y;	// actual map size in tiles / y
	u16  map_ofs_x;		// offset into maps location start in tiles / x
	u16  map_ofs_y;		// offset into maps location start in tiles / y
	u16  map_tiles_x;	// actual map size (from offset on) in tiles / x
	u16  map_tiles_y;	// actual map size (from offset on) in tiles / y
	u16  map_line_ofs;	// offset that needs to be added after reading 1 line
                        // of the map to reach next lines map_x
} map_fragment_info,*map_fragment_info_ptr;

//
// functions
//

extern tile_info_ptr ham_InitTileSet(void* src,u16 size_u16,u8 col_mode,u8 cbb_only_mode); 
extern tile_info_ptr ham_InitTileEmptySet(u16 num_tiles, u8 col_mode, u8 cbb_only_mode);
extern map_info_ptr  ham_InitMapEmptySet(u8 map_size,u8 map_rot); 
extern map_info_ptr  ham_InitMapSet(void* src,u16 size_u16,u8 map_size,u8 map_rot); 
extern void          ham_SetMapTile(u8 bgno,u32 x,u32 y,u32 tileno);
extern void          ham_SetMapTileEx(u8 bgno,u32 x,u32 y,u32 tileno,u32 hflip, u32 vflip, u32 palno);
extern u16			 ham_GetMapTile(u8 bgno,u8 x,u8 y);
extern void ham_ReloadTileGfx(tile_info_ptr ti, u16* src,u16 target_tileno,u16 num_tiles);
extern map_fragment_info_ptr ham_InitMapFragment(void* src,
										  u16 map_total_x,u16 map_total_y,
                                          u16 map_ofs_x, u16 map_ofs_y,
                                          u8 map_tiles_x,u8 map_tiles_y,
                                          u8 map_rot);
extern void ham_ChangeMapFragment(map_fragment_info_ptr mfi,
                                          u16 map_ofs_x, u16 map_ofs_y,
                                          u8 map_tiles_x,u8 map_tiles_y);
extern void ham_InsertMapFragment(map_fragment_info_ptr mfi,u8 bgno,u8 x,u8 y);
extern void ham_DeInitTileSet(tile_info_ptr ti);
extern void ham_DeInitMapSet(map_info_ptr mi);
extern void ham_DeInitMapFragment(map_fragment_info_ptr mfi);

// -------------------
// from fx.c
// -------------------

extern void ham_SetFxMode(u16 source_layer, u16 target_layer, u16 fx_mode);
extern void ham_SetFxAlphaLevel(u16 source_intensity,u16 target_intensity);
extern void ham_SetFxBrightnessLevel(u16 intensity);


// -------------------
// from obj.c
// -------------------

//
// structs
//

typedef struct obj_info_typ
{
	u8   taken;
    u8   active;
	u16  objatr0;
	u16  objatr1;
	u16  objatr2;
	u8	 prev_obj;
	u8	 next_obj;
} obj_info,*obj_info_ptr;

//
// variables
//
extern obj_info ham_obj[];
extern u8 ham_obj_first;

//
// functions
//
extern void ham_InitObj(void);
extern void ham_ResetObj(void);
extern void ham_DeleteObj(u8 entryno);
extern u8   ham_CreateObj(void* src,u16 obj_shape,u16 obj_size,u16 obj_mode,u16 col_mode,u16 pal_no ,u16 mosaic,u16 hflip,u16 vflip,u16 prio, u16 dbl_size, u16 x, u16 y);
extern u8   ham_CreateObjFromGfxSlot(u16 slot,u16 obj_shape,u16 obj_size,u16 obj_mode,u16 col_mode,u16 pal_no ,u16 mosaic,u16 hflip,u16 vflip,u16 prio, u16 dbl_size, u16 x, u16 y);
extern u8   ham_CreateObjFromBg(u8 bgno, u32 bg_tileno_x, u32 bg_tileno_y, u16 obj_shape,u16 obj_size, u32 erase_bg, u32 tileno_filler);
extern u16  ham_CreateObjGfx(void* src,u16 obj_shape,u16 obj_size,u16 col_mode);
extern void ham_DeleteObjGfx(u16 slotno);
extern u8   ham_CloneObj(u8 objno, u16 x, u16 y);
extern void ham_SetObjMode(u8 objno,u16 obj_mode);
extern void ham_SetObjX(u8 objno,u16 x);
extern void ham_SetObjY(u8 objno,u16 y);
extern void ham_SetObjXY(u8 objno,u16 x,u16 y);
extern void ham_SetObjMosaic(u8 objno,u8 val);
extern void ham_SetObjDblSize(u8 objno,u8 val);
extern void ham_SetObjHFlip(u8 objno,u8 val);
extern void ham_SetObjVFlip(u8 objno,u8 val);
extern void ham_SetObjPrio(u8 objno,u8 val);
extern void ham_SetObjVisible(u8 objno,u8 val);
extern void ham_SetObjRotEnable(u8 objno,u8 val);
extern void ham_SetObjRotSetSelect(u8 objno,u8 val);
extern void ham_UpdateObjGfx(u8 objno,void* src);
extern u16  ham_GetObjGfxSlot(u8 objno);
extern u8   ham_GetObjPal16(u8 objno);
extern void ham_SetObjGfxSlot(u8 objno,u16 slotno,u8 palno);
extern void ham_UpdateObjPal16(u8 objno,u8 pal_no);
extern void ham_SetObjBefore(u8 objno,u8 objno_target);
extern void ham_CopyObjToOAM(void);
extern u32  ham_GetObjAvailable(void);

// -------------------
// from bg.c
// -------------------

//
// structs
//
typedef struct bg_info_typ
{
	map_info_ptr  mi; // active map info for this BG
	tile_info_ptr ti; // active tile info for this BG
    u16  mosaic;      // mosaic stat (0=off)
    u16  active;      // tells if BG should be active (0=off)
    u16  prio;        // priority of BG screen (0=highest,3=lowest)
	u8	 is_rot_bg;	  // tells the system if this is a rot/scale bg
	s16	 x_scroll;	  // in this place HAMlib stores scrolling info
	s16	 y_scroll;    // same here
} bg_info,*bg_info_ptr;

//
// variables
//
extern bg_info ham_bg[];

//
// functions
//
extern void          ham_InitBg( u16 bgno, u16 active, u16 prio, u16 mosaic);
extern void          ham_ResetBg(void);
extern u8            ham_GetBgType(u8 bgno);
extern void          ham_SetBgMode(u8 bgmode);
extern void          ham_SetBgVisible(u32 bgno,u32 active);
extern void			 ham_DeInitBg( u16 bgno); 
extern void          ham_SetBgXY(u16 bgno,u16 x, u16 y);


// -------------------
// from bitmap.c
// -------------------

//extern void ham_PutPixel(u8 x,u8 y,u16 col_value);
extern void ham_PutPixel(u32 x,u32 y,u32 col_value);
extern void ham_PutLine(s16 x1, s16 y1, s16 x2, s16 y2, u16 col_value);
extern void ham_LoadBitmap(void* src);
extern void ham_ClearBackBuffer(u32 col_value);
extern void ham_FlipBGBuffer(void);
extern u8   ham_GetBGBuffer(void);


// -------------------
// from win.c
// -------------------

//
// structs
//
typedef struct win_info_typ
{
    u32 x1;              // window left upper x coordinate
    u32 y1;              // window left upper y coordinate
    u32 x2;              // window right lower x coordinate
    u32 y2;              // window right lower y coordinate
    u32 inside_on_for;   // this can be filled with alist of | red values
                        // possible values are defined above with WIN_* defines
    u32 outside_on_for;  // this can be filled with alist of | red values
                        // possible values are defined above with WIN_* defines
    u32 fx_on;           // enables/disables specialfx for this window
} win_info,*win_info_ptr;

//
// variables
//
extern win_info ham_win[];

//
// functions
//
extern void ham_InitWin(void);
extern void ham_CopyWinToHW(void);
extern void ham_CreateWin(u32 winno,u32 x1,u32 y1,u32 x2,u32 y2,u32 inside_on_for,u32 outside_on_for,u32 fx_on);
extern void ham_DeleteWin(u32 winno);


// -------------------
// from rot.c
// -------------------

extern const FIXED SIN_SFP32[];
extern const FIXED COS_SFP32[];
//
// functions
//
extern void	ham_RotBg(u8 bgno,u16 angle,s32 center_x,s32 center_y, FIXED zoom);
extern void	ham_RotBgEx(u8 bgno,u16 angle,s32 center_x,s32 center_y, u16 x_scroll, u16 y_scroll, FIXED zoom_x, FIXED zoom_y);
extern void ham_RotObjDefineSet(u8 rotset,u16 angle, FIXED zoom_x, FIXED zoom_y);
// -------------------
// from tools.c
// -------------------

//
// functions
//
extern char* HexToChar(u32 hexval);
extern int CharToHex(char* thestr);


// -------------------
// from text.c
// -------------------


//
// structs
//
typedef struct text_info_typ
{
    bg_info_ptr     bg;
	map_info_ptr	mi;
	tile_info_ptr	ti;
	u8				front_col;
	u8				back_col;
	u8				bgno;
} text_info,*text_info_ptr;

//
// variables
//
extern text_info_ptr ham_text;


//
// functions
//

//
// NAME:    ham_initText
// USAGE:   ham_initText();
// NOTES:   use this call once before you use any other 
//          libHAM text output functions. It loads the 
//          standard HAM font and sets up the text system.

extern void          ham_InitText(u8 bgno);
extern void          ham_DeInitText(void);
extern void          ham_VBAText(char*, ...);
extern void          ham_DrawText(u8 x, u8 y,char*, ...);
extern void	     ham_SetTextCol(u8 front_col,u8 back_col);

// -------------------
// from vsprintf.c
// -------------------

//
// functions
//
extern int           vsprintf(char *buf, const char *fmt, va_list args);


// -------------------
// from ham.c
// -------------------

//
// functions
//
extern void         ham_Init(void);
extern u32          ham_GetHAMlibVersion(void);
extern void         ham_ResetAll(void);
extern int atexit(void (*)(void)); 



// -------------------
// from memory.c
// -------------------

//
// variables
//
extern u8 ham_memvram_alloctable[];
extern u8 ham_memobj_alloctable[];
extern u8 ham_memobj_refcnt[];

//
// functions
//

extern void              ham_InitMemVram(void);
extern void				 ham_InitMemObj(void);
extern void              ham_InitMemSRAM(void);
extern u32               ham_AllocMemSRAM(u32 bytes_needed);
extern void              ham_DeAllocMemSRAM(u32 blockno);
extern u8                ham_AllocMemVram(u16 size_u16,u8 cbb_only_mode);
extern u16				 ham_AllocMemObj(u8 obj_shape,u8 obj_size,u8 col_mode);
extern void              ham_DeAllocMemVram(u8 first_block);
extern void				 ham_DeAllocMemObj(u16 first_block);
extern void				 ham_ReAllocMemObj(u16 first_block);
extern void*             ham_Malloc(u32 num_bytes);
extern u32               ham_GetAvailableHeap(void);


// -------------------
// from pal.c
// -------------------

//
// functions
//

extern void ham_LoadBGPal(void* src,u16 size_u16s);
extern void ham_LoadBGPal16(void* src,u8 pal_no);
extern void ham_LoadBGPal256(void* src);
extern void ham_LoadObjPal(void* src,u16 size_u16s);
extern void ham_LoadObjPal16(void* src,u8 pal_no);
extern void ham_LoadObjPal256(void* src);
extern void ham_FadePal(u8 palno,s8 delta_per_call);
extern u8   ham_FadePalCol(u16 colno,s8 delta_per_call);
extern void ham_SetBgPalCol(u8 color_number, u16 rgb_value);
extern void ham_SetBgPalColRGB(u8 color_number, u8 r, u8 g, u8 b);
extern void ham_SetObjPalCol(u8 color_number, u16 rgb_value);
extern void ham_SetObjPalColRGB(u8 color_number, u8 r, u8 g, u8 b);


// -------------------
// from int.c
// -------------------


//
// functions
//

extern void ham_EmptyInt();
extern void ham_InitIntrTable();
extern void ham_StartIntHandler(u8 intno,void* fp);
extern void ham_StopIntHandler(u8 intno);



// -------------------
// from sound.c
// -------------------
typedef struct sample_info_typ sample_info ;

struct sample_info_typ
{
    u32 length;				//length of the sample in bytes
	u8* data;				//pointer to the samples data
	u16 bitrate;			//the bit rate of the sample in bytes/sec
	u8  pre_divide_ratio;   //the pre divide ratio from this sample/mixer (do not edit)
	u16 divide_ratio;		//the ratio from this sample/mixer (do not edit, automatically set by ham_PlaySample)
	u16 mix_buf_size;		//number of bytes needed to fill one mixer frame (also sample bytes/frame)
	u32 current_pos;		//current position played in the sample
	u8  playing;			//indicates the sample is currently playing
	struct sample_info_typ *next;	//next sample to be mixed
	struct sample_info_typ *prev;	//prev sample to be mixed
};

//Waveform Data Structure
typedef struct {
    u16 type;   //Type
    u16 stat;   //Status
    u32 freq;   //Frequency Calculation Value
    u32 loop;   //Loop Start Point
    u32 size;   //Sampling Number
    s8 data[1]; //Sampling Data Order
} WaveData;


typedef struct mixer_info_typ
{
	u16 bitrate;					//bitrate for the mixer, dictates DS output speed
									//should go to 22050 Hz
	u8* mix_buf[2];					//pointer to mixer buffer. Size of the buffer is set
									//in ham_CreateMixer and is 2*bitrate/60
	u16 mix_buf_size;				//size of a single mixer buffer
	u16 mix_buf_cycles;				//number of cycles needed to process one buffer
	s8 buf_no;						//this determines which of the two buffers is currently
									//used for input (other one is output)
	struct sample_info_typ *first_sample;	//pointer to first sample to mix
	struct sample_info_typ *last_sample;	//pointer to first sample to mix

}mixer_info,*mixer_info_ptr;


extern mixer_info ham_mixer;

//
// functions
//
extern void			ham_InitSound(void);
extern void			ham_InitDSound(void);
extern void			ham_DeInitDSound(void);
extern void			ham_InitMixer(u16 bitrate);
extern void			ham_DeInitMixer(void);
extern void			ham_SyncMixer(void) MEM_FUNC_IN_IWRAM;
extern void			ham_UpdateMixer(void) MEM_FUNC_IN_IWRAM;
extern void			ham_PlaySample(sample_info *sample); 
extern void			ham_ChangeSampleBitrate(sample_info* sample,u16 bitrate);
extern sample_info *ham_InitSample(u8* data,u32 length,u16 bitrate); 
extern void			ham_DeInitSample(sample_info *sample); 
extern void			ham_KillSample(sample_info *sample);


// -------------------
// from save.c
// -------------------

//
// structs
//
typedef struct ramdata_info_typ
{
    u16 length_blocks;			  // length of data in 64byte blocks.
    u32 length_bytes;             // length of the data in bytes
    u16 info_blockno;	          // offset of info from RAM start in 64byte blocks.
    u16 data_blockno;	          // offset of data from RAM start in 64byte blocks.
    u32 next_ramdata;	          // block number of next ramdata_info. end of chain if 0xffffffff
    u32 small_data_store;         // if only an int/other atom is to be stored, data will be stored
                                  // right here instead of another block.

    //the above constitutes 14 bytes of data
    //the total size of the struct should always be 64byte
    //so make room for a string of 50 bytes

    char ident[46];			    // Identity String of this block.

} ramdata_info,*ramdata_info_ptr;


typedef struct ram_info_typ
{
    u32              ram_mode;			// mode of RAM system.
                                        //  0xFF = uninitialized
                                        //  0x00 = SRAM 256
                                        //  0x01 = EEPROM 0.5k
                                        //  0x02 = EEPROM 4k
    u32              ham_config_found;  // state of Save RAM
                                        //  0x00 = HAM initialization block NOT found
                                        //  0x01 = HAM initialization block was found
    void*            saveRAM_start;     // start adress of selected Save RAM

    u32              rootblk;           // the root block number

} ram_info,*ram_info_ptr;

//
// functions
//
extern void                 ham_ResetRAM(void);
extern u32                  ham_InitRAM(u32 ram_type);
extern void                 ham_LoadRAMInfo(u32 blockno,ramdata_info_ptr pramdat);
extern void                 ham_SaveRAMInfo(ramdata_info_ptr pramdat);
extern void                 ham_SaveIntToRAM(char* ident_string,int the_number);
extern u32                  ham_LoadIntFromRAM(char* ident_string, int* target_int);
extern void                 ham_SaveRawToRAM(char* ident_string,void* data, u32 length_in_bytes);
extern u32                  ham_LoadRawFromRAM(char* ident_string, void* target_data);



// -------------------
// from tume.c
// -------------------


typedef struct tume_info_typ
{
    TUMERIC_ROOT *root;
    u16 tiletomap_idx[256];
    tile_info_ptr room_ti[4];
    u16 bg_roomid[4];
    u16 bg_layerid[4];
    s32 worldx;
    s32 worldy;
    s32 world_tilex;
    s32 world_tiley;
    s32 last_worldx[4];
    s32 last_worldy[4];
    s32 last_world_tilex[4];
    s32 last_world_tiley[4];
    u32 realx;
    u32 realy;
    u32 real_tilex;
    u32 real_tiley;
    u32 size_x;
    u32 size_y;
    u32 size_tilex;
    u32 size_tiley;


} tume_info,*tume_info_ptr;

//
// functions
//
extern u32 ham_InitTume(TUMERIC_ROOT* tum_root);
extern u32 ham_InitTumeRoom(u32 room_id);
extern void ham_InitTumeLayer(u32 room_id,u32 layer_id, u8 bgno, u32 prio, u32 worldx, u32 worldy);
extern void ham_MoveTumeLayer(u8 bgno,s32 worldx, s32 worldy);

// -------------------
// from pad.c
// -------------------

typedef unsigned char PADBUTTON;

typedef struct PADBUTTONS
{
   PADBUTTON Left:1;
   PADBUTTON Right:1;
   PADBUTTON Down:1;
   PADBUTTON Up:1;
   PADBUTTON Start:1;
   PADBUTTON Select:1;
   PADBUTTON A:1;
   PADBUTTON B:1;
   PADBUTTON L:1;
   PADBUTTON R:1;
   PADBUTTON None:1;
}PADBUTTONS;

typedef struct PAD
{
   PADBUTTONS Pressed;
   PADBUTTONS Held;
} PAD;

extern PAD Pad;

extern void ham_InitPad(void);
extern void ham_ResetPad(void);
extern void ham_UpdatePad(void);



// -------------------
// from assert.c
// -------------------

//
// defines
//
#ifdef HAM_DEBUGGER

// the assert itself use it like this:
// ASSERT( numberOfSprites < MAX_NUM_SPRITES, "You are using too many sprites." )

//
// functions
//
void AssertImplementation( bool a_Assertion, char* a_Message, char* a_Expression, 
					   char* a_FileName, u32 a_LineNumber );


#define	ASSERT( a_Assertion, a_Message )    if(F_INTMST_ENABLED)  { M_INTMST_DISABLE \
			  (AssertImplementation(  ( a_Assertion ),\
                                      ( a_Message ),  \
                                        #a_Assertion, \
                                        __FILE__,     \
                                        __LINE__ )); M_INTMST_ENABLE } \
										else { \
			  (AssertImplementation(  ( a_Assertion ),\
                                      ( a_Message ),  \
                                        #a_Assertion, \
                                        __FILE__,     \
										__LINE__ )); }



#else //HAM_DEBUGGER

#define ASSERT( a_Assertion, a_Message ) 

#endif //HAM_DEBUGGER

#endif //HAM_WITH_LIBHAM


/*
-----------------------------------------------------
-----------------------------------------------------

        End of the file.

-----------------------------------------------------
-----------------------------------------------------
*/  

#ifdef __cplusplus
}
#endif

#include "gba.h"

#endif //MYGBA_H

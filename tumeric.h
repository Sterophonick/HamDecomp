// Created by Tumeric v1.25
// http://www.taswegian.com/TwoHeaded/tumeric.html
//
// modified by Emanuel Schleussinger
// for compliance with HAM conventions

#ifndef __TUMERIC_HEADER
#define __TUMERIC_HEADER

typedef u16 TUMERIC_PALETTE;
typedef u16 TUMERIC_TILE;
typedef u8 TUMERIC_CHARSET;

typedef struct TUMERIC_TILETABLE
{
    const TUMERIC_TILE *pTileSet;
} TUMERIC_TILETABLE;

typedef struct TUMERIC_CREATURELIST
{
    const u16 nType;
    const u8 nFlags;
    const u16 nX;
    const u16 nY;
} TUMERIC_CREATURELIST;

typedef struct TUMERIC_CREATURE
{
    const u16 nLayerID;
    const u16 nCreature;
    const struct TUMERIC_CREATURELIST *pCreatureList;
} TUMERIC_CREATURE;

typedef struct TUMERIC_LAYER
{
    u8 nFlag;
    u16 nTile;
} TUMERIC_LAYER;

typedef struct TUMERIC_LAYERTABLE
{
    s32 nLayerID;
    const struct TUMERIC_LAYER *pLayer;
} TUMERIC_LAYERTABLE;

typedef struct TUMERIC_ROOM
{
    s32 nWidth;             // width in tiles
    s32 nHeight;            // height in tiles
    s32 nLayerCount;
    const struct TUMERIC_LAYERTABLE *pLayerTable;
    const struct TUMERIC_TILETABLE *pTileTable;
    const struct TUMERIC_CREATURE *pCreatureTable;
    s32 nCharacterCount;
    const TUMERIC_CHARSET *pCharSet;
    const TUMERIC_PALETTE *pPalette;
} TUMERIC_ROOM;

typedef struct TUMERIC_ROOMTABLE
{
    const struct TUMERIC_ROOM *pRoom;
} TUMERIC_ROOMTABLE;

typedef struct TUMERIC_ROOT
{
    s32 nTileWidth;
    s32 nTileHeight;
    s32 nRooms;
    const struct TUMERIC_ROOMTABLE *pRoomTable;
} TUMERIC_ROOT;

#endif //__TUMERIC_HEADER


/*
** Copyright (c) 1997, 3Dfx Interactive, Inc.
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of 3Dfx Interactive, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of 3Dfx Interactive, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef __linux__
#include <conio.h>
#endif
#include <assert.h>
#include <string.h>

#include <glide.h>
#include "tlib.h"


GrHwConfiguration hwconfig;
static char version[80];

static const char name[]    = "test19";
static const char purpose[] = "texture filter modes test";
static const char usage[]   = "-n <frames> -r <res> -d <filename>";

typedef FxU32 Palette[256];
typedef struct {
  FxU8  yRGB[16];
  FxI16 iRGB[4][3];
  FxI16 qRGB[4][3];
  FxU32 packed_data[12];
} NCCTable;

typedef union {
    Palette  palette;
    NCCTable nccTable;
} TextureTable;

typedef struct {
    GrTexInfo    info;
    GrTexTable_t tableType;
    TextureTable tableData;
} Texture;

#define NO_TABLE ((GrTexTable_t)(~0))

static int loadTexture( const char *filename, 
                        GrTexInfo *info, 
                        GrTexTable_t *tableType,
                        void *table );
static GrTexTable_t texTableType( GrTextureFormat_t format );

void main( int argc, char **argv) {
    char match; 
    char **remArgs;
    int  rv;

    GrScreenResolution_t resolution = GR_RESOLUTION_640x480;
    float                scrWidth   = 640.0f;
    float                scrHeight  = 480.0f;
    int frames                      = -1;
    FxBool               scrgrab = FXFALSE;
    char                 filename[256];
    FxU8                 subframe = 0;

    Texture texture;

    /* Process Command Line Arguments */
    while( rv = tlGetOpt( argc, argv, "nrd", &match, &remArgs ) ) {
        if ( rv == -1 ) {
            printf( "Unrecognized command line argument\n" );
            printf( "%s %s\n", name, usage );
            printf( "Available resolutions:\n%s\n",
                    tlGetResolutionList() );
            return;
        }
        switch( match ) {
        case 'n':
            frames = atoi( remArgs[0] );
            break;
        case 'r':
            resolution = tlGetResolutionConstant( remArgs[0], 
                                                  &scrWidth, 
                                                  &scrHeight );
            break;
        case 'd':
            scrgrab = FXTRUE;
            frames = 4;
            strcpy(filename, remArgs[0]);
            break;
        }
    }

    tlSetScreen( scrWidth, scrHeight );

    grGlideGetVersion( version );

    printf( "%s:\n%s\n", name, purpose );
    printf( "%s\n", version );
    printf( "Resolution: %s\n", tlGetResolutionString( resolution ) );
    if ( frames == -1 ) {
        printf( "Press A Key To Begin Test.\n" );
        tlGetCH();
    }
    
    /* Initialize Glide */
    grGlideInit();
    assert( grSstQueryHardware( &hwconfig ) );
    grSstSelect( 0 );
    assert( grSstWinOpen( 0,
                      resolution,
                      GR_REFRESH_60Hz,
                      GR_COLORFORMAT_ABGR,
                      GR_ORIGIN_UPPER_LEFT,
                      2, 1 ) );
    
    tlConSet( 0.0f, 0.0f, 1.0f, 0.5f, 
              60, 15, 0xffffff );
    
    /* Set up Render State - Decal Texture - DISABLE mipmapping */
    grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER,
                    GR_COMBINE_FACTOR_ONE,
                    GR_COMBINE_LOCAL_NONE,
                    GR_COMBINE_OTHER_TEXTURE,
                    FXFALSE );
    grTexCombine( GR_TMU0,
                  GR_COMBINE_FUNCTION_LOCAL,
                  GR_COMBINE_FACTOR_NONE,
                  GR_COMBINE_FUNCTION_LOCAL,
                  GR_COMBINE_FACTOR_NONE,
                  FXFALSE, FXFALSE );
    grTexMipMapMode( GR_TMU0,
                     GR_MIPMAP_DISABLE,
                     FXFALSE );

    /* Load texture data into system ram */
    assert( loadTexture( "miro.3df", 
                         &texture.info, 
                         &texture.tableType, 
                         &texture.tableData ) );
    /* Download texture data to TMU */
    grTexDownloadMipMap( GR_TMU0,
                         grTexMinAddress( GR_TMU0 ),
                         GR_MIPMAPLEVELMASK_BOTH,
                         &texture.info );
    if ( texture.tableType != NO_TABLE ) {
        grTexDownloadTable( GR_TMU0,
                            texture.tableType,
                            &texture.tableData );
    }

    /* Select Texture As Source of all texturing operations */
    grTexSource( GR_TMU0,
                 grTexMinAddress( GR_TMU0 ),
                 GR_MIPMAPLEVELMASK_BOTH,
                 &texture.info );



#define RED  0x000000ff
#define BLUE 0x00ff0000


    tlConOutput( "m - toggle magnify/minify texture\n" );
    tlConOutput( "f - toggle pointSample/Bilinear\n" );
    tlConOutput( "Press any other key to quit\n\n" );
    while( frames-- && tlOkToRender()) {
        GrVertex vtxA, vtxB, vtxC, vtxD;
        
        static int minify;
        static int bilerp;

        if (hwconfig.SSTs[0].type == GR_SSTTYPE_SST96) {
          tlGetDimsByConst(resolution,
                           &scrWidth, 
                           &scrHeight );
        
          grClipWindow(0, 0, (FxU32) scrWidth, (FxU32) scrHeight);
        }

        grBufferClear( 0, 0, GR_ZDEPTHVALUE_FARTHEST );
        
        /*---- 
          A-B
          |\|
          C-D
          -----*/
        vtxA.oow = 1.0f;
        vtxB = vtxC = vtxD = vtxA;

        if ( minify ) {
            vtxA.x = vtxC.x = tlScaleX( 0.0f );
            vtxB.x = vtxD.x = tlScaleX( 1.0f );
            vtxA.y = vtxB.y = tlScaleY( 0.0f );
            vtxC.y = vtxD.y = tlScaleY( 1.0f ); 
        } else { /* magnify */
            vtxA.x = vtxC.x = tlScaleX( 0.45f );
            vtxB.x = vtxD.x = tlScaleX( 0.55f );
            vtxA.y = vtxB.y = tlScaleY( 0.45f );
            vtxC.y = vtxD.y = tlScaleY( 0.55f ); 
        }
            
        vtxA.tmuvtx[0].sow = vtxC.tmuvtx[0].sow = 0.0f;
        vtxB.tmuvtx[0].sow = vtxD.tmuvtx[0].sow = 255.0f;
        vtxA.tmuvtx[0].tow = vtxB.tmuvtx[0].tow = 0.0f;
        vtxC.tmuvtx[0].tow = vtxD.tmuvtx[0].tow = 255.0f;

        if ( bilerp ) {
            grTexFilterMode( GR_TMU0,
                             GR_TEXTUREFILTER_BILINEAR,
                             GR_TEXTUREFILTER_BILINEAR );
            tlConOutput( "GR_TEXTUREFILTER_BILINEAR     \r" );
        } else {
            grTexFilterMode( GR_TMU0,
                             GR_TEXTUREFILTER_POINT_SAMPLED,
                             GR_TEXTUREFILTER_POINT_SAMPLED );
            tlConOutput( "GR_TEXTUREFILTER_POINT_SAMPLED\r" );
        }

        grDrawTriangle( &vtxA, &vtxD, &vtxC );
        grDrawTriangle( &vtxA, &vtxB, &vtxD );

        tlConRender();
        grBufferSwap( 1 );
        grSstIdle();

        /* grab the frame buffer */
        if (scrgrab) {
          char fname[256], tmp[32];
          FxU16 cnt;

          cnt = strcspn(filename, ".");
          strncpy(fname, filename, cnt);
          fname[cnt] = 0;
          sprintf(tmp,"_%d\0", subframe);
          strcat(fname, tmp);
          strcat(fname, filename+cnt);
          if (!tlScreenDump(fname, (FxU16)scrWidth, (FxU16)scrHeight))
            printf( "Cannot open %s\n", filename);

          subframe++;

          /* cycle through all mode */
          switch (subframe)
            {
            case 0:
              minify = 0;
              bilerp = 0;
              break;
            case 1:
              minify = 0;
              bilerp = 1;
              break;
            case 2:
              minify = 1;
              bilerp = 0;
              break;
            case 3:
              minify = 1;
              bilerp = 1;
              break;
            }
        }
        
        while( tlKbHit() ) {
            switch( tlGetCH() ) {
            case 'm':
            case 'M':
                minify = !minify;
                break;
            case 'f':
            case 'F':
                bilerp = !bilerp;
                break;
            default:
                frames = 0;
                break;
            }
        }
    }
    
    grGlideShutdown();
    return;
}


/*------------------------------------------------------
  Load Texture
  
  This example loads textures from a .3df file.  3DF files
  containe pre-computed mipmaps data along with any
  necessary supplementary information, for example 
  palettes, ncc-tables, level-of-detail description,
  aspect ratio or format 
  
  The gu3dfGetInfo and gu3dfLoad APIs load A 3DF file
  into Gu3DfInfo structure from a file on disk.  Data
  can then be extracted from the gu3DfInfo structure 
  to initialize a GrTexInfo structure used in the 
  glide texturing routines.  Also note that texture table
  ( either NCC or Palette ) management is left up to the
  application programmer.  
  ------------------------------------------------------*/
static int loadTexture( const char *filename, 
                        GrTexInfo *info, 
                        GrTexTable_t *tableType,
                        void *table ) {
    Gu3dfInfo tdfInfo;
    int rv = 0;

    assert( filename );
    assert( info );
    assert( tableType );
    assert( table );
    if ( gu3dfGetInfo( filename, &tdfInfo ) ) {
        tdfInfo.data = malloc( tdfInfo.mem_required );
        assert( tdfInfo.data );
        if ( gu3dfLoad( filename, &tdfInfo ) ) {
            info->smallLod    = tdfInfo.header.small_lod;
            info->largeLod    = tdfInfo.header.large_lod;
            info->aspectRatio = tdfInfo.header.aspect_ratio;
            info->format      = tdfInfo.header.format;
            info->data        = tdfInfo.data;
            *tableType = texTableType( info->format );
            switch( *tableType ) {
            case GR_TEXTABLE_NCC0:
            case GR_TEXTABLE_NCC1:
            case GR_TEXTABLE_PALETTE:
                memcpy( table, &(tdfInfo.table), sizeof( TextureTable ) );
                break;
            default:
                break;
            }
            rv = 1;
        }
    }
    return rv;
}

static GrTexTable_t texTableType( GrTextureFormat_t format ) {
    GrTexTable_t rv = (GrTexTable_t)NO_TABLE;
    switch( format ) {
    case GR_TEXFMT_YIQ_422:
    case GR_TEXFMT_AYIQ_8422:
        rv = GR_TEXTABLE_NCC0;
        break;
    case GR_TEXFMT_P_8:
    case GR_TEXFMT_AP_88:
        rv = GR_TEXTABLE_PALETTE;
        break;
    }
    return rv;
}

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
#ifndef __linux__
#include <conio.h>
#endif
#include <assert.h>
#include <string.h>

#include <glide.h>
#include "tlib.h"


GrHwConfiguration hwconfig;
static char version[80];

static const char name[]    = "test09";
static const char purpose[] = "chromakey - render a red and blue"
                              " triangle but chromakey one out";
static const char usage[]   = "-n <frames> -r <res> -d <filename>";

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

    unsigned long chromaColor;

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
            frames = 2;
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
    
    tlConSet( 0.0f, 0.0f, 1.0f, 1.0f,
              60, 30, 0xffffff );

    /* Set up Render State - gouraud shading + enable chromakey on blue */
    grColorCombine( GR_COMBINE_FUNCTION_LOCAL,
                    GR_COMBINE_FACTOR_NONE,
                    GR_COMBINE_LOCAL_CONSTANT,
                    GR_COMBINE_OTHER_NONE,
                    FXFALSE );
    grChromakeyMode( GR_CHROMAKEY_ENABLE );

#define RED  0x000000ff
#define BLUE 0x00ff0000

    chromaColor = BLUE;
    grChromakeyValue( chromaColor );

    tlConOutput( "Press <SPACE> to toggle chomakey color\n" );
    tlConOutput( "Press any other key to quit\n" );
    while( frames-- && tlOkToRender()) {
        GrVertex vtxA, vtxB, vtxC;

        if (hwconfig.SSTs[0].type == GR_SSTTYPE_SST96) {
          tlGetDimsByConst(resolution,
                           &scrWidth, 
                           &scrHeight );
        
          grClipWindow(0, 0, (FxU32) scrWidth, (FxU32) scrHeight);
        }

        switch( chromaColor ) {
        case RED:
            tlConOutput( "Chromakey RED \r" );
            break;
        case BLUE:
            tlConOutput( "Chromakey BLUE\r" );
            break;
        }

        grBufferClear( 0x00, 0, 0 );

        vtxA.x = tlScaleX( 0.5f ), vtxA.y = tlScaleY( 0.1f );
        vtxB.x = tlScaleX( 0.8f ), vtxB.y = tlScaleY( 0.9f );
        vtxC.x = tlScaleX( 0.2f ), vtxC.y = tlScaleY( 0.9f );
        
        grConstantColorValue( RED );
        grDrawTriangle( &vtxA, &vtxB, &vtxC );

        grSstOrigin( GR_ORIGIN_LOWER_LEFT );

        grConstantColorValue( BLUE );
        grDrawTriangle( &vtxA, &vtxB, &vtxC );

        grSstOrigin( GR_ORIGIN_UPPER_LEFT );

        tlConRender();
        grBufferSwap( 1 );

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

          /* cycle through all mode */
          if ( chromaColor == RED )
            chromaColor = BLUE;
          else
            chromaColor = RED;
          grChromakeyValue( chromaColor );

          subframe++;
          /* scrgrab = FXFALSE; */
        }

        while( tlKbHit() ) {
            switch( tlGetCH() ) {
            case ' ':
                if ( chromaColor == RED )
                    chromaColor = BLUE;
                else
                    chromaColor = RED;
                grChromakeyValue( chromaColor );
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






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

#include <glide.h>
#include "tlib.h"


GrHwConfiguration hwconfig;
static char version[80];

static const char name[]    = "test15";
static const char purpose[] = "clip rectangle testing - clip rectangle travels around screen";
static const char usage[]   = "-n <frames> -r <res>";

void main( int argc, char **argv) {
    char match; 
    char **remArgs;
    int  rv;

    GrScreenResolution_t resolution = GR_RESOLUTION_640x480;
    float                scrWidth   = 640.0f;
    float                scrHeight  = 480.0f;
    int frames                      = -1;

    float clipX, clipY, clipSize, clipSizeDelta, clipPosDelta;

    /* Process Command Line Arguments */
    while( rv = tlGetOpt( argc, argv, "nr", &match, &remArgs ) ) {
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

    /* Set up Render State - iterated alpha + z buffering */
    grColorCombine( GR_COMBINE_FUNCTION_LOCAL,
                    GR_COMBINE_FACTOR_NONE,
                    GR_COMBINE_LOCAL_CONSTANT,
                    GR_COMBINE_OTHER_NONE,
                    FXFALSE );
    
    clipX    = 0.2f;
    clipY    = 0.5f;
    clipSize = 0.3f;
    
    clipSizeDelta = 0.005f;
    clipPosDelta = 0.01f;

#define CLIPSIZE_MIN 0.05f
#define CLIPSIZE_MAX 0.6f

#define GRAY  0x00808080
#define BLACK 0x00000000

    tlConOutput( "+/- - grow/shrink clip rectangle\n" );
    tlConOutput( "a/d - clip window left/right \n" );
    tlConOutput( "w/s - clip window up/down \n" );
    tlConOutput( "Press any other key to quit\n" );
    while( frames-- && tlOkToRender()) {
        int x, y;
        GrVertex vtxA, vtxB, vtxC;
        int minx, miny, maxx, maxy;

        if (hwconfig.SSTs[0].type == GR_SSTTYPE_SST96) {
          tlGetDimsByConst(resolution,
                           &scrWidth, 
                           &scrHeight );
        
          grClipWindow(0, 0, (FxU32) scrWidth, (FxU32) scrHeight);
        }

        grClipWindow( (int)tlScaleX(0.0f),(int)tlScaleY(0.0f),
                      (int)tlScaleX(1.0f),(int)tlScaleY(1.0f) );
        grBufferClear( BLACK, 0, GR_ZDEPTHVALUE_FARTHEST );

        /* Set Clipping Rectangle */
        minx = (int)tlScaleX(clipX);
        miny = (int)tlScaleY(clipY);
        if ( (clipX+clipSize)>1.0f ) 
            maxx = (int)tlScaleX( 1.0f );
        else 
            maxx = (int)tlScaleX( clipX + clipSize );
        if ( (clipY+clipSize)>1.0f ) 
            maxy = (int)tlScaleY( 1.0f );
        else 
            maxy = (int)tlScaleY( clipY + clipSize );
        grClipWindow( minx, miny, maxx, maxy );

        /* Draw 10x10 grid of triangles */
        for( y = 0; y < 10; y++ ) {
            for( x = 0; x < 10; x++ ) {
                /* 
                   A
                   |\
                   B-C
                 */
                vtxA.x = vtxB.x = tlScaleX( ((float)x)/10.0f );
                vtxA.y = tlScaleY( ((float)y)/10.0f );
                vtxB.y = vtxC.y = tlScaleY( (((float)y)/10.0f) + 0.1f );
                vtxC.x = tlScaleX( (((float)x)/10.0f) + 0.1f );
                
                grConstantColorValue( GRAY );
                grDrawTriangle( &vtxA, &vtxB, &vtxC );
            }
        }

        tlConRender();
        grBufferSwap( 1 );
        grSstIdle();
        
        while( tlKbHit() ) {
            switch( tlGetCH() ) {
            case '+':
                if ( clipSize < CLIPSIZE_MAX )
                    clipSize += clipSizeDelta;
                break;
            case '-':
                if ( clipSize > CLIPSIZE_MIN )
                    clipSize -= clipSizeDelta;
                break;
            case 'a':
            case 'A':
                if ( clipX > 0.0f )
                    clipX -= clipPosDelta;
                if ( clipX < 0.0f ) clipX = 0.0f;
                break;
            case 'd':
            case 'D':
                if ( clipX < 1.0f )
                    clipX += clipPosDelta;
                break;
            case 'w':
            case 'W':
                if ( clipY > 0.0f ) 
                    clipY -= clipPosDelta;
                if ( clipY < 0.0f ) clipY = 0.0f;
                break;
            case 's':
            case 'S':
                if ( clipY < 1.0f ) 
                    clipY += clipPosDelta;
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

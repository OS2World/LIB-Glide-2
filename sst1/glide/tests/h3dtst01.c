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

#if !defined (__linux__) && !defined(__OS2__)
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef __linux__
#include <conio.h>
#endif
#include <assert.h>

#include <time.h>

#include <glide.h>
#include "tlib.h"

#define H3D_RED 0x000000ff
#define H3D_GREEN 0x0000ff00
#define H3D_YELLOW 0x0000ffff
#define H3D_BLACK 0x00000000

GrHwConfiguration hwconfig;
static char version[80];
static int scrWidth, scrHeight, boxwidth, boxheight, origin;


void init(void) {
  GrState state;
  
  grGlideGetState(&state);
  
  grColorCombine( GR_COMBINE_FUNCTION_LOCAL, GR_COMBINE_FACTOR_NONE,
    GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_NONE, FXFALSE );
  
  grColorMask(FXTRUE,0); // make sure color buf is on
  grDepthBufferFunction( GR_CMP_ALWAYS );
  
  grBufferClear( 0x00, 0, GR_ZDEPTHVALUE_FARTHEST );
  
  grGlideSetState(&state);
}

void drawBoxes(int w, int h, unsigned long c1, unsigned long c2) {
  GrVertex p1,p2,p3,p4;

  p1.x = 1.f;
  p1.y = 1.f;
  p2.x = (float)w;
  p2.y = 1.f;
  p3.x = 1.f;
  p3.y = (float)h;
  p4.x = (float)w;
  p4.y = (float)h;
  grConstantColorValue( c1 );
  grDrawLine(&p1,&p2);
  grDrawLine(&p1,&p3);
  grDrawLine(&p3,&p4);
  grDrawLine(&p2,&p4);
  grDrawLine(&p1,&p4);
  grDrawLine(&p2,&p3);
  
  p1.y += origin;
  p2.y += origin;
  p3.y += origin;
  p4.y += origin;
  grConstantColorValue( c2 );
  grDrawLine(&p1,&p2);
  grDrawLine(&p1,&p3);
  grDrawLine(&p3,&p4);
  grDrawLine(&p2,&p4);
  grDrawLine(&p1,&p4);
  grDrawLine(&p2,&p3);  
}

void main( int argc, char **argv) {
  int     keepworking = 1;
  GrScreenResolution_t resolution;
  int			frames		= -1;
  int     redraw = 1;
  int     redrawPattern = 0;
  int     patternon = 1;

#ifdef H3D
  resolution = GR_RESOLUTION_640x240_AUTOFLIPPED;
#else
  resolution = GR_RESOLUTION_640x480;
#endif
  
  /* Initialize Glide */
  grGlideInit();
  grGlideGetVersion( version );
  grSstQueryHardware( &hwconfig );
  grSstSelect( 0 );
  if ( grSstWinOpen( 0, resolution, GR_REFRESH_60Hz, GR_COLORFORMAT_ABGR, GR_ORIGIN_UPPER_LEFT, 2,1 ) ) {
    grColorCombine( GR_COMBINE_FUNCTION_LOCAL,
      GR_COMBINE_FACTOR_NONE,
      GR_COMBINE_LOCAL_CONSTANT,
      GR_COMBINE_OTHER_NONE,
      FXFALSE );
    
  } else {
    keepworking = 0;
  }
  grRenderBuffer( GR_BUFFER_BACKBUFFER );

  scrWidth	= grSstScreenWidth();
  scrHeight	= grSstScreenHeight();  
  origin    = (int)scrHeight-240; // top of right eye buffer
  boxheight = (int)240;
  boxwidth  = (int)scrWidth;

  while( keepworking) {
    
    while( tlKbHit() ) {
      switch( tlGetCH() ) {
      case '=' :
      case '+' :
        //  move origin of 2nd page causes both box heights to resize
        if (origin < (scrHeight-1.f)) 
        {
          origin++;
          boxheight--;
          redraw = 1;
        }
        break;
      case '_':
      case '-' :
        if (origin > ((scrHeight/2)-1.f)) 
        {
          origin--;
          boxheight++;
          redraw = 1;
        }
        break;
      case '.' :
        //  move origin of 2nd page causes both box heights to resize
        if (origin < (scrHeight-1.f)) 
        {
          origin++;
          boxheight--;
        }
        break;
      case ',' :
        if (origin > ((scrHeight/2)-1.f)) 
        {
          origin--;
          boxheight++;
        }
        break;
      case 'p':
        patternon ^= 1;
        redrawPattern = 1;
        break;
      case 'q':
        keepworking = 0;
        break;
      case ' ':
        redraw = 1;
      default:
        break;
      }
    }
    
    if (redraw) {
      grColorMask(FXTRUE,0);
      grBufferClear( 0, 0, 0 );
      drawBoxes(boxwidth,boxheight,H3D_RED,H3D_YELLOW);
      redraw = 0;
      grBufferSwap( 1 );
    } else if (redrawPattern) {
#ifdef H3D
      grHints(GR_HINT_H3DENABLE,patternon);   // set the pattern on or off
#endif
      redrawPattern = 0;
    }
  }  
  grGlideShutdown();
  return;
}


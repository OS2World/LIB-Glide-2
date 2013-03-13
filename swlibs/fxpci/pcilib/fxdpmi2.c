/*
** THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
** PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
** TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
** INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE 
** DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com). 
** THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
** EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
** FULL TEXT OF THE NON-WARRANTY PROVISIONS.  
** 
** USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
** RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
** TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
** AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
** SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
** THE UNITED STATES.  
** 
** COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED
*/

#include <stdlib.h>
#include <stdio.h>
#include <3dfx.h>
#define FX_DLL_DEFINITION
#include <fxdll.h>
#include "fxpci.h"
#include "pcilib.h"
#include <fxdpmi.h>
#include <fxmemmap.h>

static char pciIdent[] = "@#% fxPCI for DOS";

FxBool pciInitializeDDio(void)
{
    pciIdent[0] = '@';		/* to prevent compiler warnings ONLY! */
    return(FXTRUE);
}
    
FX_EXPORT FxBool FX_CSTYLE
pciMapPhysicalToLinear( FxU32 *linear_addr, FxU32 physical_addr,
                       FxU32 *length ) 
{ 
    FxBool onWindows;

    if ( !pciLibraryInitialized ) {
	pciErrorCode = PCI_ERR_NOTOPEN;
	return FXFALSE;
    }
    
    /*  
    **  First, check to see if we're a DOS app under Windows, and if
    **  so, then check to see if there's already an app connected to
    **  the VXD.
    */
    DpmiCheckVxd((FxBool *) &onWindows, &pciVxdVer);

    if (onWindows) {
      if (BYTE1(pciVxdVer) != FX_MAJOR_VER || BYTE0(pciVxdVer) < FX_MINOR_VER) {
        pciErrorCode = PCI_ERR_WRONGVXD;
        return FXFALSE;
      }
      
      if (VXDREFCOUNT(pciVxdVer) > 0) {
        pciErrorCode = PCI_ERR_VXDINUSE;
        return FXFALSE;
      }
    }

    /* If we got here, it's OK to map the memory */
    *linear_addr = DpmiMapPhysicalToLinear( physical_addr, *length );
    return FXTRUE;
}

void pciUnmapPhysicalDD( FxU32 linear_addr, FxU32 length ) 
{
    DpmiUnmapMemory();
    return;
}

FX_EXPORT FxBool FX_CSTYLE
pciClose( void )
{
    if ( !pciLibraryInitialized ) {
        pciErrorCode = PCI_ERR_NOTOPEN2;
        return FXFALSE;
    }
    DpmiUnmapMemory();
    pciLibraryInitialized = FXFALSE;
    return FXTRUE;
}

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

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <3dfx.h>
#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <fxmemmap.h>
#include <fxdpmi.h>
#include "fxpci.h"
#include "pcilib.h"


/* -------------------------------------------------- */
/* Memmap through Conrad's memmap vxd                 */

static char pciIdent[] = "@#% fxPCI for Windows '95";

HANDLE hMemmapFile;

FxBool pciInitializeDDio(void)
{
    if (pciIdent[0]);
    hMemmapFile = CreateFile("\\\\.\\FXMEMMAP.VXD", 0, 0, NULL, 0,
                           FILE_FLAG_DELETE_ON_CLOSE, NULL);
    if ( hMemmapFile == INVALID_HANDLE_VALUE ) {
	pciErrorCode = PCI_ERR_MEMMAPVXD;
	return FXFALSE;
    }
    return FXTRUE;
}

FX_EXPORT FxBool FX_CSTYLE
pciMapPhysicalToLinear( FxU32 *linear_addr, FxU32 physical_addr,
                       FxU32 *length ) 
{ 
    FxU32 nret;
    FxU32 Physical [2];         /* Physical address[0] & size[1] */
    FxU32 Linear [2];           /* Linear address[0] & size[1] */
    LPDWORD pPhysical = Physical;
    LPDWORD pLinear = Linear;

    Physical[0] = physical_addr;
    Physical[1] = *length;

    if ( !pciLibraryInitialized ) {
	pciErrorCode = PCI_ERR_NOTOPEN;
	return FXFALSE;
    }
    
    /*
     * Check version:
     * The policy is that major and minor versions must match, and
     * further that the reference count is less than or equal to one. 
     */
    DeviceIoControl(hMemmapFile, GETAPPVERSIONDWORD, NULL, 0, &pciVxdVer, 
                    sizeof(pciVxdVer), &nret, NULL);
    if (HIBYTE(pciVxdVer) != FX_MAJOR_VER || LOBYTE(pciVxdVer) < FX_MINOR_VER) {
      pciErrorCode = PCI_ERR_WRONGVXD;
      return FXFALSE;
    }

    if (VXDREFCOUNT(pciVxdVer) > 1) {
      pciErrorCode = PCI_ERR_VXDINUSE;
      return FXFALSE;
    }

    /* Map physical to linear */
    /* xxx - returns 0 in Linear if fails, but really should
       check return value, 0 is suceess, -1 is failure. */

#ifdef DIRECTX
    DeviceIoControl(hMemmapFile, GETLINEARADDR, 
                    &pPhysical, sizeof(pPhysical), 
                    &pLinear, sizeof(pLinear), 
                    &nret, NULL);
#else
    /* Stuff added to auto-switch passthru using fxmemmap */
    if (getenv("SST_DUALHEAD") == NULL) {
      DeviceIoControl(hMemmapFile, GETLINEARADDR_AUTO, 
                      &pPhysical, sizeof(pPhysical), 
                      &pLinear, sizeof(pLinear), 
                      &nret, NULL);
    } else {
      DeviceIoControl(hMemmapFile, GETLINEARADDR, 
                      &pPhysical, sizeof(pPhysical), 
                      &pLinear, sizeof(pLinear), 
                      &nret, NULL);
    }
#endif /* DIRECTX */


    *linear_addr = Linear[0];

    if ( nret == 0 ) {
      pciErrorCode = PCI_ERR_MEMMAP;
      return FXFALSE;
    }
    return FXTRUE;
}

void pciUnmapPhysicalDD( FxU32 linear_addr, FxU32 length ) 
{
    FxU32 nret;
    DeviceIoControl(hMemmapFile, DECREMENTMUTEX,
                    NULL, 0, NULL, 0,
                    &nret, NULL);
    return;
}

FX_EXPORT FxBool FX_CSTYLE
pciClose( void )
{
    if ( !pciLibraryInitialized ) {
        pciErrorCode = PCI_ERR_NOTOPEN2;
        return FXFALSE;
    }
    CloseHandle( hMemmapFile );
    pciLibraryInitialized = FXFALSE;
    return FXTRUE;
}

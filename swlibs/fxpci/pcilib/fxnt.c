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

/* ------------------------------------------------------------- */
/* Memmap and portio through Microsoft's sample mapmem miniport  */

#include <windows.h>
#include <stddef.h>
#include <winioctl.h>
#include <gpioctl.h>

static char pciIdent[] = "@#% fxPCI for Windows NT";

/* xxx The following two typedefs are here to avoid including
   miniport.h and ntddk.h from the NT DDK */

typedef enum _INTERFACE_TYPE
{
  Internal,
  Isa,
  Eisa,
  MicroChannel,
  TurboChannel,
  PCIBus,
  MaximumInterfaceType
} INTERFACE_TYPE, *PINTERFACE_TYPE;

typedef LARGE_INTEGER PHYSICAL_ADDRESS;

#include <mapmem.h>

static HANDLE hGpdFile;
static HANDLE hMapmemFile;

FxBool pciInitializeDDio(void)
{
    if (pciIdent[0]);

    hGpdFile = CreateFile( "\\\\.\\GpdDev", GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                          NULL, OPEN_EXISTING, 0, NULL );
    if ( hGpdFile == INVALID_HANDLE_VALUE ) {
      pciErrorCode = PCI_ERR_GENPORT;
      return FXFALSE;
    }

    hMapmemFile = CreateFile("\\\\.\\MAPMEM", GENERIC_READ | GENERIC_WRITE,
                           0, NULL, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    if ( hMapmemFile == INVALID_HANDLE_VALUE ) {
	pciErrorCode = PCI_ERR_MAPMEMDRV;
	return FXFALSE;
    }    
    return FXTRUE;
}

FxU8
pioInByte ( unsigned short port )
{
  BOOL   IoctlResult;
  LONG   IoctlCode;
  ULONG  PortNumber;
  UCHAR  DataBuffer;
  ULONG  DataLength;
  ULONG  ReturnedLength;

  IoctlCode = IOCTL_GPD_READ_PORT_UCHAR;
  PortNumber = port;
  DataLength = sizeof(DataBuffer);

  IoctlResult = DeviceIoControl( hGpdFile, IoctlCode, &PortNumber,
                                 sizeof(PortNumber), 
                                 &DataBuffer, DataLength,
                                 &ReturnedLength, NULL ); 

  if ( IoctlResult && ReturnedLength == DataLength )
    return DataBuffer;

  else
    return FXFALSE;
} /* pioInByte */

FxU16
pioInWord ( unsigned short port )
{
  BOOL   IoctlResult;
  LONG   IoctlCode;
  ULONG  PortNumber;
  USHORT DataBuffer;
  ULONG  DataLength;
  ULONG  ReturnedLength;

  IoctlCode = IOCTL_GPD_READ_PORT_USHORT;
  PortNumber = port;
  DataLength = sizeof(DataBuffer);

  IoctlResult = DeviceIoControl( hGpdFile, IoctlCode, &PortNumber,
                                sizeof(PortNumber), 
                                &DataBuffer, DataLength,
                                &ReturnedLength, NULL ); 
  
  if ( IoctlResult && ReturnedLength == DataLength )
    return DataBuffer;

  else
    return FXFALSE;
} /* pioInWord */

FxU32
pioInLong ( unsigned short port )
{
  BOOL   IoctlResult;
  LONG   IoctlCode;
  ULONG  PortNumber;
  ULONG  DataBuffer;
  ULONG  DataLength;
  ULONG  ReturnedLength;

  IoctlCode = IOCTL_GPD_READ_PORT_ULONG;
  PortNumber = port;
  DataLength = sizeof(DataBuffer);

  IoctlResult = DeviceIoControl( hGpdFile, IoctlCode, &PortNumber, sizeof(PortNumber),
                                 &DataBuffer, DataLength, &ReturnedLength, NULL );

  if ( IoctlResult && ReturnedLength == DataLength )
    return DataBuffer;

  else
    return FXFALSE;
} /* pioInLong */

FxBool
pioOutByte ( unsigned short port, FxU8 data )
{
  BOOL                IoctlResult;
  LONG                IoctlCode;
  GENPORT_WRITE_INPUT InputBuffer;
  ULONG               DataLength;
  ULONG               ReturnedLength;

  IoctlCode = IOCTL_GPD_WRITE_PORT_UCHAR;
  InputBuffer.PortNumber = port;
  InputBuffer.CharData = (UCHAR) data;
  DataLength = offsetof(GENPORT_WRITE_INPUT, CharData) +
               sizeof(InputBuffer.CharData);

  IoctlResult = DeviceIoControl( hGpdFile, IoctlCode, &InputBuffer, DataLength,
                                 NULL, 0, &ReturnedLength, NULL );

  if ( IoctlResult )
    return FXTRUE;
  else
    return FXFALSE;
} /* pioOutByte */

FxBool
pioOutWord ( unsigned short port, FxU16 data )
{
  BOOL                IoctlResult;
  LONG                IoctlCode;
  GENPORT_WRITE_INPUT InputBuffer;
  ULONG               DataLength;
  ULONG               ReturnedLength;

  IoctlCode = IOCTL_GPD_WRITE_PORT_USHORT;
  InputBuffer.PortNumber = port;
  InputBuffer.ShortData = (USHORT) data;
  DataLength = offsetof(GENPORT_WRITE_INPUT, ShortData) +
               sizeof(InputBuffer.ShortData);

  IoctlResult = DeviceIoControl( hGpdFile, IoctlCode, &InputBuffer, DataLength,
                                 NULL, 0, &ReturnedLength, NULL );

  if ( IoctlResult )
    return FXTRUE;
  else
    return FXFALSE;
} /* pioOutWord */

FxBool
pioOutLong ( unsigned short port, FxU32 data )
{
  BOOL                IoctlResult;
  LONG                IoctlCode;
  GENPORT_WRITE_INPUT InputBuffer;
  ULONG               DataLength;
  ULONG               ReturnedLength;

  IoctlCode = IOCTL_GPD_WRITE_PORT_ULONG;
  InputBuffer.PortNumber = port;
  InputBuffer.LongData = (ULONG) data;
  DataLength = offsetof(GENPORT_WRITE_INPUT, LongData) +
               sizeof(InputBuffer.LongData);

  IoctlResult = DeviceIoControl( hGpdFile, IoctlCode, &InputBuffer, DataLength,
                                 NULL, 0, &ReturnedLength, NULL );

  if ( IoctlResult )
    return FXTRUE;
  else
    return FXFALSE;
} /* pioOutLong */


FX_EXPORT FxBool FX_CSTYLE
pciMapPhysicalToLinear( FxU32 *linear_addr, FxU32 physical_addr, FxU32 *length ) 
{ 
    FxU32 cbReturned;
    PHYSICAL_MEMORY_INFO pmi;

    if ( !pciLibraryInitialized ) {
	pciErrorCode = PCI_ERR_NOTOPEN;
	return FXFALSE;
    }
    
    pmi.InterfaceType       = PCIBus;
    pmi.BusNumber           = 0;
    pmi.BusAddress.HighPart = 0x00000000;
    pmi.BusAddress.LowPart  = physical_addr;
    pmi.AddressSpace        = 0;
    pmi.Length              = *length;

    if ( !DeviceIoControl( hMapmemFile,
                          (FxU32) IOCTL_MAPMEM_MAP_USER_PHYSICAL_MEMORY,
                          &pmi, sizeof(PHYSICAL_MEMORY_INFO),
                          linear_addr, sizeof(PVOID),
                          &cbReturned, NULL ) ) {
      pciErrorCode = PCI_ERR_MAPMEM;
      return FXFALSE;
    }
    return FXTRUE;
}

void pciUnmapPhysicalDD( FxU32 linear_addr, FxU32 length ) 
{
    FxU32 cbReturned;
  
    DeviceIoControl( hMapmemFile,
                  (FxU32) IOCTL_MAPMEM_UNMAP_USER_PHYSICAL_MEMORY,
                  &linear_addr, sizeof(PVOID),
                  NULL, 0,
                  &cbReturned, NULL );
    return;
}

FX_EXPORT FxBool FX_CSTYLE
pciClose( void )
{
    if ( !pciLibraryInitialized ) {
        pciErrorCode = PCI_ERR_NOTOPEN2;
        return FXFALSE;
    }
    CloseHandle( hGpdFile );
    CloseHandle( hMapmemFile );
    pciLibraryInitialized = FXFALSE;
    return FXTRUE;
}

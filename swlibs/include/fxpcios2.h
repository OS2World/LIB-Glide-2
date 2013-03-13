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
*
*  OS/2 Port written by (C) 1999 Przemyslaw Dobrowolski <dobrawka@asua.org.pl>
*
*/

#ifndef _FXPCI_H_
#define _FXPCI_H_

#include <os2.h>

#define _3DFX_PCI_ID 0x121A

#define IOCTL_MAP   1
#define IOCTL_UMAP  2

struct map_ioctl {
    union {
        ULONG   phys;
        ULONG*  user;
    } a;
    ULONG   size;
};

typedef int PciMemType;

#define MAX_PCI_DEVICES 512

#define PCI_ERR_NOERR           0
#define PCI_ERR_NO_PMAP         1
#define PCI_ERR_NO_BUS          2
#define PCI_ERR_NOTOPEN         3
#define PCI_ERR_NOTOPEN2        4
#define PCI_ERR_NOTOPEN3        5
#define PCI_ERR_OUTOFRANGE      6
#define PCI_ERR_NODEV           7
#define PCI_ERR_NODEV2          8 
#define PCI_ERR_WRITEONLY       9  
#define PCI_ERR_READONLY        1 
#define PCI_ERR_MAPMEM          11


typedef enum {
    READ_ONLY,
    WRITE_ONLY,
    READ_WRITE
} PciIOFlag;

typedef struct {
    FxU32              regAddress;
    FxU32              sizeInBytes;
    PciIOFlag          rwFlag;
} PciRegister;

extern const PciRegister PCI_VENDOR_ID;     
extern const PciRegister PCI_DEVICE_ID;     
extern const PciRegister PCI_COMMAND;       
extern const PciRegister PCI_STATUS;
extern const PciRegister PCI_REVISION_ID;
extern const PciRegister PCI_CLASS_CODE;
extern const PciRegister PCI_CACHE_LINE_SIZE;
extern const PciRegister PCI_LATENCY_TIMER;
extern const PciRegister PCI_HEADER_TYPE;
extern const PciRegister PCI_BIST;
extern const PciRegister PCI_BASE_ADDRESS_0;
extern const PciRegister PCI_BASE_ADDRESS_1;
extern const PciRegister PCI_BASE_ADDRESS_2;
extern const PciRegister PCI_BASE_ADDRESS_3;
extern const PciRegister PCI_BASE_ADDRESS_4;
extern const PciRegister PCI_BASE_ADDRESS_5;
extern const PciRegister PCI_INTERRUPT_LINE;
extern const PciRegister PCI_INTERRUPT_PIN;
extern const PciRegister PCI_MIN_GNT;
extern const PciRegister PCI_MAX_LAT;

extern const PciRegister PCI_SST1_INIT_ENABLE; // 0x40
extern const PciRegister PCI_SST1_BUS_SNOOP_0; // 0x44
extern const PciRegister PCI_SST1_BUS_SNOOP_1; // 0x48
extern const PciRegister PCI_SST1_CFG_STATUS;  // 0x4C


#define CONFIG_ADDRESS_ENABLE_BIT 0x80000000UL
#define CONFIG_ADDRESS_PORT 0xCF8
#define CONFIG_DATA_PORT    0xCFC

#define CONFIG_MAPPING_ENABLE_BYTE 0x80
#define CONFIG_MAPPING_DISABLE_BYTE 0x00

#define CONFIG_SPACE_ENABLE_PORT 0xCF8
#define FORWARD_REGISTER_PORT    0xCFA
#define CONFIG_MAPPING_OFFSET    0xC000

FX_ENTRY const char* FX_CALL
pciGetErrorString( void );

FX_ENTRY FxU32 FX_CALL
pciGetErrorCode( void );

FX_ENTRY FxBool FX_CALL 
pciOpen( void );

FX_ENTRY FxBool FX_CALL 
pciClose( void );

FX_ENTRY FxBool FX_CALL 
pciDeviceExists( FxU32 device_number );

FX_ENTRY FxBool FX_CALL 
pciGetConfigData( PciRegister reg, FxU32 device_number, FxU32 *data );

FX_ENTRY FxBool FX_CALL 
pciSetConfigData( PciRegister reg, FxU32 device_number, FxU32 *data );

FX_ENTRY FxBool FX_CALL 
pciMapPhysicalToLinear(FxU32 *linear_addr, FxU32 physical_addr,FxU32 *length);

FX_ENTRY void   FX_CALL 
pciUnmapPhysical( FxU32 linear_addr, FxU32 length );

const char *
pciGetVendorName( FxU16 vendor_id );
const char *
pciGetClassName( FxU32 class_code , FxU32 deviceID);

FX_ENTRY FxBool FX_CALL 
pciFindCard(FxU32 vendorID, FxU32 deviceID, FxU32 *devNum);

FX_ENTRY FxBool FX_CALL 
pciFindCardMulti(FxU32 vID, FxU32 dID, FxU32 *devNum, FxU32 cardNum);

FX_ENTRY FxU32 * FX_CALL 
pciMapCard(FxU32 vID, FxU32 dID, FxI32 len, FxU32 *devNo, FxU32 addrNo);

FX_ENTRY FxU32 * FX_CALL
pciMapCardMulti(FxU32 vID,FxU32 dID,FxI32 l,FxU32 *dNo,FxU32 cNo,FxU32 aNo);

FX_ENTRY FxBool FX_CALL
pciFindMTRRMatch(FxU32 pBaseAddrs, FxU32 psz, PciMemType type, FxU32 *mtrrNum);

FX_ENTRY FxBool FX_CALL
pciFindFreeMTRR(FxU32 *mtrrNum);

FX_ENTRY FxBool FX_CALL
pciSetMTRR(FxU32 mtrrNo, FxU32 pBaseAddr, FxU32 psz, PciMemType type);

FX_ENTRY FxBool FX_CALL
pciSetPassThroughBase(FxU32* pBaseAddr, FxU32 baseAddrLen);

FX_ENTRY FxBool FX_CALL
pciOutputDebugString(const char* debugMsg);


#endif // _FXPCI_H_
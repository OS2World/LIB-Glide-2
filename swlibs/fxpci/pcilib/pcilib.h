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
**
**
** $Revision: 5 $ 
** $Date: 9/29/97 6:57p $ 
**
*/

#ifndef _PCILIB_H_
#define _PCILIB_H_

#define CONFIG_ADDRESS_PORT 0xCF8
#define CONFIG_DATA_PORT    0xCFC
#define CONFIG_ADDRESS_ENABLE_BIT 0x80000000UL

#define CONFIG_MAPPING_ENABLE_BYTE 0x80
#define CONFIG_MAPPING_DISABLE_BYTE 0x00

#define CONFIG_SPACE_ENABLE_PORT 0xCF8
#define FORWARD_REGISTER_PORT    0xCFA
#define CONFIG_MAPPING_OFFSET    0xC000

#define PCI_INTERRUPT  0x1A
#define PCI_FUNC_CODE  0xB1
#define PCI_PHYS_ADDR  0x000FFE6E

#define BYTE0(a) ((a) & 0xff)
#define BYTE1(a) (((a) >> 8) & 0xff)
#define VXDREFCOUNT(a) (((a) >> 16) & 0xff)

/* PRIVATE DATA */
FxU32   pciVxdVer;
FxU32   pciErrorCode;
FxBool  pciLibraryInitialized;

FxBool	pciInitializeDDio(void);
void	pciUnmapPhysicalDD( FxU32 linear_addr, FxU32 length ) ;

FxU8    pioInByte ( unsigned short port );              /* inp */
FxU16   pioInWord ( unsigned short port );              /* inpw */
FxU32   pioInLong ( unsigned short port );              /* inpd */

FxBool  pioOutByte ( unsigned short port, FxU8 data );  /* outp */
FxBool  pioOutWord ( unsigned short port, FxU16 data ); /* outpw */
FxBool  pioOutLong ( unsigned short port, FxU32 data ); /* outpd */

#endif /* _PCILIB_H_ */
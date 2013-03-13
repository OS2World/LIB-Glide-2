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
#include <i86.h>
#include <dos.h>
#include <pharlap.h>
#include "fxphrlap.h"

#define PHARLAP_INTERRUPT 0x21
#define PHARLAP_UNSPECIFIED_ERROR 1

/*
** pciPhrlapGetSegmentLinearBase (_dx_tolinear)
*/
int pciPhrlapGetSegmentLinearBase( USHORT selector, ULONG* lin_addrp )
{
  union REGS r;

  /*
  ** Phar Lap 2508h - Get Segment Linear Base Address
  */
  r.w.ax = 0x2508;

  /*
  ** BX segment selector
  */
  r.w.bx = (unsigned short) selector;
  int386( PHARLAP_INTERRUPT, &r, &r );

  if ( r.w.cflag )
  {
    return PHARLAP_UNSPECIFIED_ERROR;
  }

  else
  {
    /*
    ** ECX linear base address of segment
    */
    *lin_addrp = (ULONG) r.x.ecx;
    return 0;
  }
}

/*
** pciPhrlapMapPhysMemEndSegment (_dx_map_phys)
*/
int pciPhrlapMapPhysMemEndSegment( USHORT selector, ULONG phys_addr, ULONG page_cnt, ULONG* offsetp )
{
  union REGS r;
  struct SREGS sr;

  /*
  ** Phar Lap 250ah - Map Physical Memory at End of Segment
  */
  r.w.ax = 0x250a;

  /*
  ** ES  segment selector in the LDT of segment to modify
  ** EBX physical base address of memory to map, must be a multiple of four kilobytes
  ** ECX number of physical 4-KB memory pages to map
  */
  r.x.ebx = (unsigned int) phys_addr;
  r.x.ecx = (unsigned int) page_cnt;
  segread( &sr );
  sr.es = selector;
  int386x( PHARLAP_INTERRUPT, &r, &r, &sr );

  if ( r.w.cflag )
  {
    /*
    ** EAX error code
    */
    return r.x.eax;
  }

  else
  {
    /*
    ** EAX offset in segment of mapped memory
    */
    *offsetp = (ULONG) r.x.eax;
    return 0;
  }
}

/*
** pciPhrlapGetConfigInfo (_dx_config_inf)
*/
void pciPhrlapGetConfigInfo( CONFIG_INF* configp, UCHAR* swf_namep )
{
  union REGS r;

  /*
  ** Phar Lap 2526h - Get Configuration Information
  */
  r.w.ax = 0x2526;

  /*
  ** EBX 512 byte configuration buffer
  ** ECX 256 byte buffer for name and path of 386 | VMM swap file
  */
  r.x.ebx = (unsigned int) configp;
  r.x.ecx = (unsigned int) swf_namep;
  int386( PHARLAP_INTERRUPT, &r, &r );
}

/*
** pciPhrlapReadPageTableEntry (_dx_rd_ptinfn)
*/
int pciPhrlapReadPageTableEntry( ULONG linadr, ULONG* ptep, ULONG* ptinfp )
{
  union REGS r;

  /*
  ** Phar Lap 252bh/9 - Read Page Table Entry and Associated Information
  */
  r.w.ax = 0x252b;
  r.h.bh = 0x9;
  r.h.bl = 0x0;     /* select linear */

  /*
  ** ECX linear address to read PTE for
  */
  r.x.ecx = linadr;
  int386( PHARLAP_INTERRUPT, &r, &r );

  if ( r.w.cflag )
  {
    /*
    ** EAX error code
    */
    return r.x.eax;
  }

  else
  {
    /*
    ** EAX page table entry
    ** EBX additional page table information
    */
    *ptep = (ULONG) r.x.eax;
    *ptinfp = (ULONG) r.x.ebx;
    return 0;
  }
}

/*
** pciPhrlapWritePageTableEntry (_dx_wr_ptinfn)
*/
int pciPhrlapWritePageTableEntry( ULONG linadr, ULONG pte, ULONG ptinf )
{
  union REGS r;

  /*
  ** Phar Lap 252bh/10 - Write Page Table Entry and Associated Information
  */
  r.w.ax = 0x252b;
  r.h.bh = 0xa;
  r.h.bl = 0x0;     /* select linear */

  /*
  ** ECX linear address to read PTE for
  ** ESI page table entry
  ** EDI additional page table information
  */
  r.x.ecx = linadr;
  r.x.esi = pte;
  r.x.edi = ptinf;

  int386( PHARLAP_INTERRUPT, &r, &r );

  if ( r.w.cflag )
  {
    /*
    ** EAX error code
    */
    return r.x.eax;
  }

  else
  {
    return 0;
  }
}

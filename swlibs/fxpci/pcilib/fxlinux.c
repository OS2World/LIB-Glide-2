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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/io.h>
#include <fcntl.h>
#include "fxpci.h"
#include "pcilib.h"

const FxU32 PCI_VENDOR_ID_LINUX = 0x0;
const FxU32 PCI_DEVICE_ID_LINUX = 0x2;
const FxU32 PCI_COMMAND_LINUX = 0x4;
const FxU32 PCI_BASE_ADDRESS_0_LINUX = 0x10;
const FxU32 SST1_PCI_INIT_ENABLE_LINUX = 0x40;
const FxU32 SST1_PCI_BUS_SNOOP0_LINUX = 0x44;
const FxU32 SST1_PCI_BUS_SNOOP1_LINUX = 0x48;

static int linuxDevFd=-1;

struct pioData {
  short port;
  short size;
  int device;
  void *value;
};

FxBool pciInitializeDDio(void)
{
  return FXTRUE;
}

FxBool pciOutputDebugStringDD(const char *msg) {
  printf(msg);
  return FXTRUE;
}

FxBool pciInitializeLinux(void)
{
  if (!getenv("SST_NO_DEV3DFX")) linuxDevFd=open("/dev/3dfx", O_RDWR, 0);
  if (linuxDevFd==-1) {
    if (iopl(3)<0) {
      pciErrorCode = PCI_ERR_NO_IO_PERM;
      return FXFALSE;
    }
  }
  return FXTRUE;
}

FxBool 
pciMapPhysicalToLinearLinux( FxU32 *linear_addr, FxU32 physical_addr,
			FxU32 *length ) {
  int fd;
  if (linuxDevFd!=-1) {
    fd=linuxDevFd;
  } else {
    if ((fd=open("/dev/mem", O_RDWR))<0) {
      pciErrorCode=PCI_ERR_NO_MEM_PERM;
      return FXFALSE;
    }
  }
  if (((*linear_addr)=(FxU32)mmap(0, *length, PROT_READ|PROT_WRITE,
				  MAP_SHARED, fd, physical_addr))<0) {
    if (fd!=linuxDevFd) close(fd);
    return FXFALSE;
  }
  if (fd!=linuxDevFd) close(fd);
  return FXTRUE;
}

void pciUnmapPhysicalLinux( FxU32 linear_addr, FxU32 length ) {
  munmap((void*)linear_addr, length);
}

FxBool
pciCloseLinux( void )
{
  if ( !pciLibraryInitialized ) {
    pciErrorCode = PCI_ERR_NOTOPEN2;
    return FXFALSE;
  }
  if (linuxDevFd!=-1) close(linuxDevFd);
  pciLibraryInitialized = FXFALSE;
  return FXTRUE;
}

FxU8 pioInByte(unsigned short port) {
  char tmp;
  struct pioData desc;

  if (linuxDevFd==-1) {
    tmp=inb(port);
    /* fprintf(stderr, "Read byte at %x got %d\n", port, tmp); */
    return tmp;
  }
  desc.port=port;
  desc.size=sizeof(tmp);
  desc.value=&tmp;
  /* fprintf(stderr, "Read byte desc at %x tmp at %x\n", &desc, &tmp); */
  ioctl(linuxDevFd, _IOR(0, 0, sizeof(struct pioData)), &desc);
  /* fprintf(stderr, "Got byte %d versus %d\n", tmp, inb(port)); */
  return tmp;
}

FxU16 pioInWord(unsigned short port) {
  short tmp;
  struct pioData desc;

  if (linuxDevFd==-1) {
    tmp=inw(port);
    /* fprintf(stderr, "Read word at %x got %x\n", port, tmp); */
    return tmp;
  }
  desc.port=port;
  desc.size=sizeof(tmp);
  desc.value=&tmp;
  /* fprintf(stderr, "Read word desc at %x tmp at %x\n", &desc, &tmp); */
  ioctl(linuxDevFd, _IOR(0, 0, sizeof(struct pioData)), &desc);
  /* fprintf(stderr, "Got word %d versus %d\n", tmp, inw(port)); */
  return tmp;
}

FxU32 pioInLong(unsigned short port) {
  int tmp;
  struct pioData desc;

  if (linuxDevFd==-1) {
    tmp=inl(port);
    /* fprintf(stderr, "Read long at %x got %x\n", port, tmp); */
    return tmp;
  }
  desc.port=port;
  desc.size=sizeof(tmp);
  desc.value=&tmp;
  /* fprintf(stderr, "Read long desc at %x tmp at %x\n", &desc, &tmp); */
  ioctl(linuxDevFd, _IOR(0, 0, sizeof(struct pioData)), &desc);
  /* fprintf(stderr, "Got long %x versus %x\n", tmp, inl(port)); */
  return tmp;
}

FxBool pioOutByte(unsigned short port, FxU8 data) {
  struct pioData desc;
  /* fprintf(stderr, "write byte=%d desc at %x data at %x\n", data,
      &desc, &data); */
  if (linuxDevFd==-1) {
    outb(data, port);
    return FXTRUE;
  }
  desc.port=port;
  desc.size=sizeof(data);
  desc.value=&data;
  return ioctl(linuxDevFd, _IOW(0, 1, sizeof(struct pioData)), &desc)!=-1;
}

FxBool pioOutWord(unsigned short port, FxU16 data) {
  struct pioData desc;
  /* fprintf(stderr, "write word=%x to port=%x desc at %x data at %x\n", 
	  data, port, &desc, &data); */
  if (linuxDevFd==-1) {
    outw(data, port);
    return FXTRUE;
  }
  desc.port=port;
  desc.size=sizeof(data);
  desc.value=&data;
  return ioctl(linuxDevFd, _IOW(0, 1, sizeof(struct pioData)), &desc)!=-1;
}

FxBool pioOutLong(unsigned short port, FxU32 data) {
  struct pioData desc;
  /* fprintf(stderr, "write long=%x to port=%x desc at %x data at %x\n", 
	  data, port, &desc, &data); */
  if (linuxDevFd==-1) {
    outl(data, port);
    return FXTRUE;
  }
  desc.port=port;
  desc.size=sizeof(data);
  desc.value=&data;
  return ioctl(linuxDevFd, _IOW(0, 1, sizeof(struct pioData)), &desc)!=-1;
}

FxBool hasDev3DfxLinux(void) {
  if (linuxDevFd==-1) return FXFALSE;
  return FXTRUE;
}

int getNumDevicesLinux(void) {
  if (linuxDevFd==-1) return -1;
  return ioctl(linuxDevFd, _IO('3', 2));
}

FxU32
pciFetchRegisterLinux( FxU32 cmd, FxU32 size, FxU32 device) {
  struct pioData desc;
  char cval;
  short sval;
  int ival;

  if (linuxDevFd==-1) return -1;
  desc.port=cmd;
  desc.size=size;
  desc.device=device;
  switch (size) {
  case 1:
    desc.value=&cval;
    break;
  case 2:
    desc.value=&sval;
    break;
  case 4:
    desc.value=&ival;
    break;
  default:
    return 0;
  }
  if (ioctl(linuxDevFd, _IOR('3', 3, sizeof(struct pioData)), &desc)==-1)
    return 0;
  switch (size) {
  case 1:
    return cval;
  case 2:
    return sval;
  case 4:
    return ival;
  default:
    return 0;
  }
}

FxBool
pciUpdateRegisterLinux(FxU32 cmd, FxU32 data, FxU32 size, FxU32 device) {
  struct pioData desc;

  if (linuxDevFd==-1) return -1;
  desc.port=cmd;
  desc.size=size;
  desc.device=device;
  desc.value=&data;
  if (ioctl(linuxDevFd, _IOW('3', 4, sizeof(struct pioData)), &desc)==-1)
    return FXFALSE;
  return FXTRUE;
}


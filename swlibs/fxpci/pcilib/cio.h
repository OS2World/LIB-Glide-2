/* Copyright (C) 1995 by Holger Veit (Holger.Veit@gmd.de) */
/* Use at your own risk! No Warranty! The author is not responsible for
 * any damage or loss of data caused by proper or improper use of this
 * device driver.
 */

#ifndef _CIO_H_
#define _CIO_H_

#ifdef __OS2__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __EMX__
#include <sys/hw.h>

  #define c_inb(x) _inp8(x)
  #define c_inw(x) _inp16(x)
  #define c_inl(x) _inp32(x)

  #define c_outb(a,b) _outp8(a,b)
  #define c_outw(a,b) _outp16(a,b)
  #define c_outl(a,b) _outp32(a,b)

#else

int __cdecl io_init(void);
int __cdecl io_exit(void);

int __cdecl io_init1(void);
int __cdecl io_exit1(void);

char __cdecl  c_inb(short);
short __cdecl c_inw(short);
long __cdecl  c_inl(short);
void __cdecl  c_outb(short,char);
void __cdecl  c_outw(short,short);
void __cdecl  c_outl(short,long);
void __cdecl  c_readmsr(long, long *);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __EMX_
#else
   #error "This file must be used only with OS/2 compiler"
#endif // __OS2__

#endif

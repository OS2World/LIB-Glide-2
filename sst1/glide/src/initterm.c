#include <os2emx.h>
#include <os2thunk.h>
#include <glide.h>

int _CRT_init(void);
void _CRT_term(void);

unsigned long _DLL_InitTerm(unsigned long modhandle, unsigned long flag)
{
  if (!flag) 
  {
    _CRT_init();
    return(TRUE);
  }

  // when user pressed ctr-break... Or error suprise.
  //
  // We must return to video card display...
  grGlideShutdown();
  _CRT_term();
  return(TRUE);
}
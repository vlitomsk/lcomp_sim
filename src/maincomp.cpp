
#include "../include/stubs.h"
#include "../include/salayer.h"

#include "../include/ioctl.h"
#include "../include/ifc_ldev.h"
#include "../include/ldevbase.h"

#include <stdio.h>

// add class for new board here.
// main function of dll
extern "C" LUnknown* CreateInstance(ULONG Slot)
{
   LUnknown* pI;
//   char buf[128];

   SLOT_PAR sl;
   LDaqBoard *pL = new LDaqBoard(Slot);
   if(pL==NULL) return NULL;

   void * hVxd = pL->OpenLDevice();
   if(hVxd==INVALID_HANDLE_VALUE) { delete pL; return NULL; }

   pL->GetSlotParam(&sl);

     /* {
         pI = static_cast<IDaqLDevice*>(new DaqL780(Slot));
         pI->AddRef();
      } break;*/


   pL->CloseLDevice();
   delete pL;

   return pI;
}


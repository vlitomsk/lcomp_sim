
#include "../include/stubs.h"
#include "../include/salayer.h"

#include "../include/ioctl.h"
#include "../include/ifc_ldev.h"
#include "../include/ldevbase.h"

#include "../include/simulator.h"

#include <stdio.h>
#include <new>

// add class for new board here.
// main function of dll
extern "C" LUnknown* CreateInstance(ULONG Slot)
{
   LUnknown* pI;
//   char buf[128];

   SLOT_PAR sl;
   sl.BoardType = PCIC;
   
   LDaqBoardSimulator *pL = new (std::nothrow) LDaqBoardSimulator(Slot);
   if(pL==NULL) return NULL;

   void * hVxd = pL->OpenLDevice();
   if(hVxd==INVALID_HANDLE_VALUE) { delete pL; return NULL; }

   pL->GetSlotParam(&sl);

   pI = static_cast<IDaqLDevice*>(new DaqL780Simulator(Slot));
   pI->AddRef();

   pL->CloseLDevice();
   delete pL;

   return pI;
}


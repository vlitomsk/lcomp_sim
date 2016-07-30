#include <stdint.h>

typedef struct { volatile int counter; } atomic_t;

class SAL // system abstraction layer
{
public:
	SAL();
	~SAL();
   void LSetLastError(uint32_t Err);
   uint32_t LGetLastError(void);


   void LUnmapMemory(void * lpAddr, int32_t Size );
   
   void * LMapMemory(void * hDevice, void * lpAddr, int32_t Size, int32_t off);
   
   void * LVirtualAlloc(size_t size);
   int LVirtualFree(void * addr);
   int LIsWin32();

   void atomic_inc(atomic_t *v);
   void atomic_dec(atomic_t *v);

private:
	class sySAL;
	sySAL *d;	
};

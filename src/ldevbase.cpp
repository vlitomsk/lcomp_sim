#define INITGUID 

#include "../include/stubs.h"
#include "../include/ioctl.h"
#include "../include/salayer.h"
#include "../include/ifc_ldev.h"
#include "../include/ldevbase.h"
#include <string.h>

HRESULT __stdcall LDaqBoardSimulator::QueryInterface(const IID& iid, void** ppv)
{
   if(iid==IID_IUnknown)
   {
      *ppv = static_cast<IDaqLDevice*>(this);
   }
   else
   {
      if(iid==IID_ILDEV)
      {
         *ppv = static_cast<IDaqLDevice*>(this);
      }
      else 
      {
         /*if(iid==IID_ILDEV2)
         {
            *ppv = static_cast<IDaqLDevice2*>(this);
         }
         else*/
         {
            *ppv=NULL;
            return ((HRESULT)0x80004002L); // E_NOINTERFACE;
         }
      }
   }
   reinterpret_cast<LUnknown*>(*ppv)->AddRef();
   return ((HRESULT)0x00000000L); // S_OK;
}

ULONG __stdcall LDaqBoardSimulator::AddRef()
{
   sal->atomic_inc(&m_cRef);
   return m_cRef.counter;
}

ULONG __stdcall LDaqBoardSimulator::Release()
{
   sal->atomic_dec(&m_cRef);
   if(m_cRef.counter==0) {
      delete this;
      return 0;
   }
   return m_cRef.counter;
}

// Working with I/O ports
// data - data len - in bytes(sizeof)
FDF(ULONG) LDaqBoardSimulator::inbyte(ULONG offset, PUCHAR data, ULONG len, ULONG key )
{
	return L_SUCCESS;
}

FDF(ULONG) LDaqBoardSimulator::inword(ULONG offset, PUSHORT data, ULONG len, ULONG key )
{
	return L_SUCCESS;
}

FDF(ULONG) LDaqBoardSimulator::indword(ULONG offset, PULONG data, ULONG len, ULONG key )
{
	return L_SUCCESS;
}

FDF(ULONG) LDaqBoardSimulator::outbyte(ULONG offset, PUCHAR data, ULONG len, ULONG key )
{
	return L_SUCCESS;
}

FDF(ULONG) LDaqBoardSimulator::outword(ULONG offset, PUSHORT data, ULONG len, ULONG key )
{
	return L_SUCCESS;
}

FDF(ULONG) LDaqBoardSimulator::outdword(ULONG offset, PULONG data, ULONG len, ULONG key )
{
	return L_SUCCESS;
}

// Working with mem I/O ports
FDF(ULONG) LDaqBoardSimulator::inmbyte(ULONG Offset, PUCHAR Data, ULONG Length, ULONG Key)
{
	return L_SUCCESS;
}

FDF(ULONG) LDaqBoardSimulator::inmword(ULONG Offset, PUSHORT Data, ULONG Length, ULONG Key)
{
	return L_SUCCESS;
}

FDF(ULONG) LDaqBoardSimulator::inmdword(ULONG Offset, PULONG Data, ULONG Length, ULONG Key)
{
	return L_SUCCESS;
}

FDF(ULONG) LDaqBoardSimulator::outmbyte(ULONG Offset, PUCHAR Data, ULONG Length, ULONG Key)
{
	return L_SUCCESS;
}

FDF(ULONG) LDaqBoardSimulator::outmword(ULONG Offset, PUSHORT Data, ULONG Length, ULONG Key)
{
	return L_SUCCESS;
}

FDF(ULONG) LDaqBoardSimulator::outmdword(ULONG Offset, PULONG Data, ULONG Length, ULONG Key)
{
	return L_SUCCESS;
}

// COMMON FUNCTIONS //////////////////////////////////////
FDF(ULONG) LDaqBoardSimulator::GetSlotParam(PSLOT_PAR slPar)
{
   memcpy(slPar,&sl,sizeof(SLOT_PAR));
   return 0;
}


FDF(HANDLE) LDaqBoardSimulator::OpenLDevice()
{
   sal->LSetLastError(L_SUCCESS);
   return (HANDLE)1234;
}

FDF(ULONG) LDaqBoardSimulator::CloseLDevice()
{
	ULONG status = L_SUCCESS;

	if (map_inBuffer) {
		ULONG sts = sal->LVirtualFree(map_inBuffer);
		map_inBuffer = NULL;
		if (!sts) status = L_ERROR;
	}

	return status;
}

// uni stream interface
FDF(ULONG) LDaqBoardSimulator::RequestBufferStream(ULONG *Size, ULONG StreamId) //in 16 bit words
{
	if (StreamId != L_STREAM_ADC)
		return L_NOTSUPPORTED;

	ULONG status = L_SUCCESS;
	ULONG pb = 0;
	ULONG RealPages;
	void *buffer;

	if (StreamId == L_STREAM_ADC) {
		if (map_inBuffer != NULL)
		{
			sal->LVirtualFree(map_inBuffer);
			map_inBuffer = NULL;
		}

		RealPages = (*Size) / 2048 + ((*Size) % 2048 ? 1 : 0);
		pb = (RealPages + 1) * 2048;

		buffer = sal->LVirtualAlloc(pb*sizeof(USHORT));
		if (buffer == NULL) return L_ERROR;

		map_inSize = *Size * sizeof(short);
		map_inBuffer = buffer;
	}
	else
		status = L_NOTSUPPORTED;

   return status;
}


LPVOID LDaqBoardSimulator::GetDAQ_PtrSz(ULONG StreamId, ULONG *sz)
{
   if(sal->LIsWin32()) // windows c++ style
   {
      switch(StreamId)
      {
      case L_STREAM_ADC: { *sz = sizeof(ADC_PAR); return (LPVOID)&adc_par; }
      case L_STREAM_DAC: { *sz = sizeof(DAC_PAR); return (LPVOID)&dac_par; }
      case L_STREAM_DDS: { *sz = sizeof(DAC_PAR); return (LPVOID)&dac_par; }
      }
   }
   else // linux, с style
   {
      switch(StreamId)
      {
      case L_STREAM_ADC: { *sz = sizeof(WDAQ_PAR); return (LPVOID)&wadc_par; }
      case L_STREAM_DAC: { *sz = sizeof(WDAQ_PAR); return (LPVOID)&wdac_par; }
      case L_STREAM_DDS: { *sz = sizeof(WDAQ_PAR); return (LPVOID)&wdac_par; }
      }
   }
   return NULL;
}

FDF(ULONG) LDaqBoardSimulator::SetParametersStream(PDAQ_PAR sp, PULONG UsedSize, void** Data, void** Sync, ULONG StreamId)
{
   ULONG status = L_ERROR;
   ULONG tPages, tFIFO, tIrqStep;

//   tPages   = (USHORT)OutBuf[0];
//   tFIFO    = (USHORT)OutBuf[1];
//   tIrqStep = (USHORT)OutBuf[2];
   if (sp != NULL) {
	   tPages = sp->Pages;
	   tFIFO = sp->FIFO;
	   tIrqStep = sp->IrqStep;
   }
   else {
	   tPages = tFIFO = tIrqStep = 666;
   }

   *UsedSize = tPages*tIrqStep;

   *Sync = new ULONG;
   *Data = new ULONG[*UsedSize];//&d1[2048];

   if(sp!=NULL) // update properties to new real values;
   {
      sp->Pages = tPages;   
      sp->FIFO = tFIFO;
      sp->IrqStep = tIrqStep;
   }

   switch(StreamId) // for GetParameter
   {
      case L_STREAM_ADC: { sync_addr =(PULONG)*Sync;  data_addr =  (PULONG)*Data;} break;
   }

   return status;
}


FDF(ULONG) LDaqBoardSimulator::FillDAQparameters(PDAQ_PAR sp)
{
   if(sp == NULL) return L_ERROR;

   switch(sp->s_Type)
   {
      case L_ADC_PARAM:    return FillADCparameters(sp);
      case L_DAC_PARAM:    return FillDACparameters(sp);
      case L_DDS_FM_PARAM: return FillDDSparameters(sp);
      default : return L_ERROR;
   }
}


FDF(ULONG) LDaqBoardSimulator::IoAsync(PDAQ_PAR sp)
{
   if(sp == NULL) return L_ERROR;

   switch(sp->s_Type)
   {
      /*case L_ASYNC_ADC_CFG: return ConfigADC(sp);
      case L_ASYNC_TTL_CFG: return ConfigTTL(sp);
      case L_ASYNC_DAC_CFG: return ConfigDAC(sp);

      case L_ASYNC_ADC_INP: return InputADC(sp);
      case L_ASYNC_TTL_INP: return InputTTL(sp);

      case L_ASYNC_TTL_OUT: return OutputTTL(sp);*/
      case L_ASYNC_DAC_OUT: return OutputDAC(sp);

      default : return L_ERROR;
   }
}

// end of uni stream interface

// нужно проверять LastError
FDF(ULONG) LDaqBoardSimulator::InitStartLDevice()
{
   ULONG Err=0;
   if(InitStartLDeviceEx(L_STREAM_ADC)) SETBIT(Err,L_STREAM_ADC_ERROR);
   if(InitStartLDeviceEx(L_STREAM_DAC)) SETBIT(Err,L_STREAM_DAC_ERROR);
   //sal->LSetLastError(0x20000000+Err);
   //if(Err==0x3) return L_ERROR;
   return L_SUCCESS;
}

// нужно проверять LastError
FDF(ULONG) LDaqBoardSimulator::StartLDevice()
{
   ULONG Err=0;
   if(StartLDeviceEx(L_STREAM_ADC)) SETBIT(Err,L_STREAM_ADC_ERROR);
   if(StartLDeviceEx(L_STREAM_DAC)) SETBIT(Err,L_STREAM_DAC_ERROR);
   sal->LSetLastError(0x20000000+Err);
   if(Err==0x3) return L_ERROR;
   return L_SUCCESS;

}

// нужно проверять LastError
FDF(ULONG) LDaqBoardSimulator::StopLDevice()
{
   ULONG Err=0;
   if(StopLDeviceEx(L_STREAM_ADC)) SETBIT(Err,L_STREAM_ADC_ERROR);
   if(StopLDeviceEx(L_STREAM_DAC)) SETBIT(Err,L_STREAM_DAC_ERROR);
   sal->LSetLastError(0x20000000+Err);
   if(Err==0x3) return L_ERROR;
   return L_SUCCESS;

};


FDF(ULONG) LDaqBoardSimulator::GetParameter(ULONG name, PULONG param)
{
	return L_SUCCESS;
}

FDF(ULONG) LDaqBoardSimulator::SetParameter(ULONG name, PULONG param)
{
	return L_SUCCESS;
}

// work with event
// DIOC_SETEVENT - adc stop event; DIOC_SETEVENT_DAC - dac stop event
FDF(ULONG) LDaqBoardSimulator::SetLDeviceEvent(HANDLE hEvnt,ULONG EventId)
{
	return L_NOTSUPPORTED;
}

void LDaqBoardSimulator::CopyDAQtoWDAQ(PDAQ_PAR dp, LPVOID ss, int sp_type)
{
//DAC_PAR_0 d0_sp;  //0
//DAC_PAR_1 d1_sp;  //1
//ADC_PAR_0 a0_sp;  //2
//ADC_PAR_1 a1_sp;  //3
PDAC_PAR dac = (PDAC_PAR)dp;
PADC_PAR adc = (PADC_PAR)dp;
PWDAQ_PAR sp = (PWDAQ_PAR)ss;
      // декодируем тип структуры
   switch (sp_type) {
      case 0:
         {
            sp->t1.s_Type    = dac->t1.s_Type;
            sp->t1.FIFO      = dac->t1.FIFO;
            sp->t1.IrqStep   = dac->t1.IrqStep;
            sp->t1.Pages     = dac->t1.Pages;
            sp->t1.AutoInit  = dac->t1.AutoInit;
            sp->t1.dRate     = dac->t1.dRate;
            sp->t1.Rate      = dac->t1.Rate;
            sp->t1.DacNumber = dac->t1.DacNumber;
            sp->t1.DacEna    = dac->t1.DacEna;
            sp->t1.IrqEna    = dac->t1.IrqEna;
         } break;
      case 1:
         {
            sp->t2.s_Type   = dac->t2.s_Type;
            sp->t2.FIFO     = dac->t2.FIFO;
            sp->t2.IrqStep  = dac->t2.IrqStep;
            sp->t2.Pages    = dac->t2.Pages;
            sp->t2.AutoInit = dac->t2.AutoInit;
            sp->t2.dRate    = dac->t2.dRate;
            sp->t2.Rate     = dac->t2.Rate;
            sp->t2.DacEna   = dac->t2.DacEna;
            sp->t2.IrqEna   = dac->t2.IrqEna;
         } break;
      case 2:
         {
            sp->t3.s_Type   = adc->t1.s_Type;
            sp->t3.FIFO     = adc->t1.FIFO;
            sp->t3.IrqStep  = adc->t1.IrqStep;
            sp->t3.Pages    = adc->t1.Pages;
            sp->t3.AutoInit = adc->t1.AutoInit;
            sp->t3.dRate    = adc->t1.dRate;
            sp->t3.dKadr    = adc->t1.dKadr;
            sp->t3.dScale   = adc->t1.dScale;
            sp->t3.Rate     = adc->t1.Rate;
            sp->t3.Kadr     = adc->t1.Kadr;
            sp->t3.Scale    = adc->t1.Scale;
            sp->t3.FPDelay  = adc->t1.FPDelay;

            sp->t3.SynchroType        = adc->t1.SynchroType;
            sp->t3.SynchroSensitivity = adc->t1.SynchroSensitivity;
            sp->t3.SynchroMode        = adc->t1.SynchroMode;
            sp->t3.AdChannel          = adc->t1.AdChannel;
            sp->t3.AdPorog            = adc->t1.AdPorog;
            sp->t3.NCh                = adc->t1.NCh;
            for(int i=0;i<128;i++) sp->t3.Chn[i] = adc->t1.Chn[i];
            sp->t3.AdcEna = adc->t1.AdcEna;
            sp->t3.IrqEna = adc->t1.IrqEna;
         } break;
      case 3:
         {
            sp->t4.s_Type    = adc->t2.s_Type;
            sp->t4.FIFO      = adc->t2.FIFO;
            sp->t4.IrqStep   = adc->t2.IrqStep;
            sp->t4.Pages     = adc->t2.Pages;
            sp->t4.AutoInit  = adc->t2.AutoInit;
            sp->t4.dRate     = adc->t2.dRate;
            sp->t4.dKadr     = adc->t2.dKadr;
            sp->t4.Reserved1 = adc->t2.Reserved1;
            sp->t4.DM_Ena    = adc->t2.DM_Ena;
            sp->t4.Rate      = adc->t2.Rate;
            sp->t4.Kadr      = adc->t2.Kadr;
            sp->t4.StartCnt  = adc->t2.StartCnt;
            sp->t4.StopCnt   = adc->t2.StopCnt;


            sp->t4.SynchroType = adc->t2.SynchroType;
            sp->t4.SynchroMode = adc->t2.SynchroMode;
            sp->t4.AdPorog     = adc->t2.AdPorog;
            sp->t4.SynchroSrc  = adc->t2.SynchroSrc;
            sp->t4.AdcIMask    = adc->t2.AdcIMask;
            sp->t4.NCh         = adc->t2.NCh;
            for(int i=0;i<128;i++) sp->t4.Chn[i] = adc->t2.Chn[i];
            sp->t4.AdcEna = adc->t2.AdcEna;
            sp->t4.IrqEna = adc->t2.IrqEna;
         } break;
      }
}

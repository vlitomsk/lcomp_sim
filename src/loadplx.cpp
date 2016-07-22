#include <stdio.h>
#include <string.h>

#include "../include/stubs.h"
#include "../include/ioctl.h"
#include "../include/salayer.h"
#include "../include/pcicmd.h"

#include "../include/ifc_ldev.h"
#include "../include/ldevbase.h"
#include "../include/plx.h"


FDF(ULONG) DaqL780::LoadBios(char *FileName)
{
USHORT  *LCBios;
FILE *BiosFile;
LONG   NBytes;
PUCHAR  BiosCode =0;
USHORT  *Tmp;
USHORT  Count;
CHAR  FName[255];
ULONG cbRet;
//ULONG data;

   int status = L_ERROR;
   do
   {
      strcpy(FName,FileName);
      strcat(FName,".bio");
      BiosFile=fopen(FName, "rb");
      if(!BiosFile) { break; }
      fseek(BiosFile,0,SEEK_END);
      NBytes = ftell(BiosFile);
      rewind(BiosFile);

      BiosCode=new UCHAR[NBytes+2];
      if (BiosCode == NULL) break;
      if(fread(BiosCode,1,NBytes,BiosFile) != NBytes)  break;
      LCBios=(PUSHORT)BiosCode;

      // RESET ��� ADSP-218x // ���������� ��� ioctl
      if(sal->LDeviceIoControl(hVxd,sal->CTL(DIOC_RESET_PLX),NULL,0,NULL,0,&cbRet,L_STREAM_NULL)) break;

      // Load DSP DM word
      Tmp=LCBios+LCBios[0]+1; // calculate DM address &LCBios[0]+LCBios[0]
      Count=*Tmp++;           // counter

      if(PutArray_DM(0x2000,Count,Tmp)) break;
      if(PutWord_DM(L_BOARD_REVISION_PLX,(USHORT)(sl.BoardType==PCIC ? 'C':'B'))) break; // revision
      // Load DSP PM word
      Tmp=&LCBios[3];//LCBios+3;
      Count=(USHORT)(LCBios[0]-2);
      if(PutArray_PM(0x0001,Count/2,(PULONG)Tmp)) break;
      // Load last DSP PM word
      ULONG d2=*((PULONG)&LCBios[1]);
      if(PutWord_PM(0x0000,d2)) break;
      // rewrite ������� �������� �������� ���������� ()
      if(PlataTest()!=L_SUCCESS) break;
      if(sal->LDeviceIoControl(hVxd,sal->CTL(DIOC_SET_DSP_TYPE),NULL,0,NULL,0,&cbRet,L_STREAM_NULL)) break; //(L_SUCCESS)
      if(PutWord_DM(L_ADC_ENABLE_PLX,0)) break; // stop adc...
      status = L_SUCCESS;
   } while (status==L_ERROR);
   // ��������� ������ � ������ �� �������
   if(BiosCode) delete[] BiosCode;
   if(BiosFile) fclose(BiosFile);
   return status;
}


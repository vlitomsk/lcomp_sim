class DaqL780Simulator: public LDaqBoardSimulator
{
public:
   // Base functions
   IFC(ULONG) GetWord_DM(USHORT Addr, PUSHORT Data);
   IFC(ULONG) PutWord_DM(USHORT Addr, USHORT Data);
   IFC(ULONG) PutWord_PM(USHORT Addr, ULONG Data);
   IFC(ULONG) GetWord_PM(USHORT Addr, PULONG Data);

   IFC(ULONG) GetArray_DM(USHORT Addr, ULONG Count, PUSHORT Data);
   IFC(ULONG) PutArray_DM(USHORT Addr, ULONG Count, PUSHORT Data);
   IFC(ULONG) PutArray_PM(USHORT Addr, ULONG Count, PULONG Data);
   IFC(ULONG) GetArray_PM(USHORT Addr, ULONG Count, PULONG Data);

   IFC(ULONG) SendCommand(USHORT cmd);
      
      // Service functions
   IFC(ULONG) PlataTest();


   IFC(ULONG) EnableCorrection(USHORT Ena);
      
   inline IFC(ULONG)  LoadBios(char *FileName) { return L_SUCCESS; }
   
   IFC(ULONG)  ReadFlashWord(USHORT Addr, PUSHORT Data);
   IFC(ULONG)  WriteFlashWord(USHORT FlashAddress, USHORT Data);
   IFC(ULONG)  ReadPlataDescr(LPVOID pd);
   IFC(ULONG)  WritePlataDescr(LPVOID pd, USHORT Ena);
   IFC(ULONG)  EnableFlashWrite(USHORT Flag);

public:
	volatile USHORT dac_values[2];
   DaqL780Simulator(ULONG Slot) :LDaqBoardSimulator(Slot) {
	   dac_values[0] = dac_values[1] = 0;
   }
   ULONG  FillADCparameters(PDAQ_PAR sp);
   ULONG  FillDACparameters(PDAQ_PAR sp);

   ULONG InputTTL(PDAQ_PAR sp);  //2 in 1 all
   ULONG OutputTTL(PDAQ_PAR sp);  // in each set channel
   ULONG ConfigTTL(PDAQ_PAR sp); // 1221 and 1450 780C

   ULONG OutputDAC(PDAQ_PAR sp); //2 in 1

   ULONG InputADC(PDAQ_PAR sp);

   ULONG csGetParameter(ULONG name, PULONG param, ULONG status);
   ULONG csSetParameter(ULONG name, PULONG param, ULONG status);



protected:
};

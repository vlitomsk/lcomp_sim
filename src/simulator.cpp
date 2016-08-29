#include <string.h>
#include <stdio.h>

#include "../include/stubs.h"
#include "../include/ioctl.h"
#include "../include/salayer.h"
#include "../include/pcicmd.h"

#include "../include/ifc_ldev.h"
#include "../include/ldevbase.h"
#include "../include/simulator.h"

FDF(ULONG) DaqL780Simulator::GetWord_DM(USHORT Addr, PUSHORT Data)
{
	return L_SUCCESS;
}

FDF(ULONG) DaqL780Simulator::PutWord_DM(USHORT Addr, USHORT Data)
{
	return L_SUCCESS;
}

FDF(ULONG) DaqL780Simulator::SendCommand(USHORT Cmd)
{
	return L_SUCCESS;
}

FDF(ULONG) DaqL780Simulator::PutWord_PM(USHORT Addr, ULONG Data)
{
	return L_SUCCESS;
}

FDF(ULONG) DaqL780Simulator::GetWord_PM(USHORT Addr, PULONG Data)
{
	return L_SUCCESS;
}

FDF(ULONG) DaqL780Simulator::PutArray_DM(USHORT Addr, ULONG Count, PUSHORT Data)
{
	return L_SUCCESS;
}

FDF(ULONG) DaqL780Simulator::GetArray_DM(USHORT Addr, ULONG Count, PUSHORT Data)
{
	return L_SUCCESS;
}

FDF(ULONG) DaqL780Simulator::PutArray_PM(USHORT Addr, ULONG Count, PULONG Data)
{
	return L_SUCCESS;
}

FDF(ULONG) DaqL780Simulator::GetArray_PM(USHORT Addr, ULONG Count, PULONG Data)
{
	return L_SUCCESS;
}

FDF(ULONG) DaqL780Simulator::PlataTest()
{
	return L_SUCCESS;
}


//FDF(ULONG)
ULONG DaqL780Simulator::FillADCparameters(PDAQ_PAR sp)
{
	const double max_rate = 400.0;
	ULONG i;
	double QF;
	double DSP_CLOCK_OUT_PLX;
	int BN;
	double SCLOCK_DIV;
	double kadrdelay;
	double delta_rate;
	double rate;

	PADC_PAR ap = (PADC_PAR)sp;

	if (ap->t1.dRate < 0) return L_ERROR;
	if (ap->t1.dKadr < 0) return L_ERROR;

	if (ap->t1.FIFO == 0) return L_ERROR;
	if (ap->t1.Pages == 0) return L_ERROR;
	if (ap->t1.IrqStep == 0) return L_ERROR;

	QF = pdu.t1.Quartz / 1000.0;
	DSP_CLOCK_OUT_PLX = 2.0*QF;
	if (DSP_CLOCK_OUT_PLX == 0) return L_ERROR;

	if (ap->t1.dRate < 0.1) ap->t1.dRate = 0.1;
	if (ap->t1.dRate > max_rate) ap->t1.dRate = max_rate;

	SCLOCK_DIV = DSP_CLOCK_OUT_PLX / (2.0*(ap->t1.dRate)) - 0.5;
	adc_par.t1.Rate = (USHORT)SCLOCK_DIV;
	ap->t1.dRate = DSP_CLOCK_OUT_PLX / (2.0*(adc_par.t1.Rate + 1));
	adc_par.t1.FPDelay = (USHORT)(DSP_CLOCK_OUT_PLX / (ap->t1.dRate) + 5.5);

	// величина задержки в единицах SCLOCK SPORT DSP
	if (ap->t1.dRate > 1000.0) ap->t1.dKadr = 0; //  no interkadr on freq up 1000 kHz

	if ((1.0 / (ap->t1.dRate)) > (ap->t1.dKadr)) ap->t1.dKadr = 1.0 / (ap->t1.dRate);
	//
	kadrdelay = (ap->t1.dKadr)*(ap->t1.dRate) - 0.5;
	if (kadrdelay > 65500.0) kadrdelay = 65500.0;

	adc_par.t1.Kadr = (USHORT)kadrdelay;

	ap->t1.dKadr = (adc_par.t1.Kadr + 1) / (ap->t1.dRate);
	adc_par.t1.Scale = 0;

	// More
	adc_par.t1.s_Type = ap->t1.s_Type;
	adc_par.t1.SynchroType = ap->t1.SynchroType;
	adc_par.t1.SynchroSensitivity = ap->t1.SynchroSensitivity;
	adc_par.t1.SynchroMode = ap->t1.SynchroMode;
	adc_par.t1.AdChannel = ap->t1.AdChannel;
	adc_par.t1.AdPorog = ap->t1.AdPorog;
	adc_par.t1.dKadr = ap->t1.dKadr;
	adc_par.t1.dRate = ap->t1.dRate;
	adc_par.t1.dScale = ap->t1.dScale;

	adc_par.t1.FIFO = ap->t1.FIFO;
	adc_par.t1.IrqStep = ap->t1.IrqStep;
	adc_par.t1.Pages = ap->t1.Pages;
	if (ap->t1.NCh > 128) ap->t1.NCh = 128;
	adc_par.t1.NCh = ap->t1.NCh;
	for (i = 0; i < ap->t1.NCh; i++) adc_par.t1.Chn[i] = ap->t1.Chn[i];

	adc_par.t1.AutoInit = ap->t1.AutoInit;
	adc_par.t1.IrqEna = ap->t1.IrqEna;
	adc_par.t1.AdcEna = ap->t1.AdcEna;

	// make a copy  of adc_par in wadc_par for C-style interface to driver ////////
	CopyDAQtoWDAQ(&adc_par.t1, &wadc_par.t3, 2);
	return L_SUCCESS;
}
#include <cstring>
FDF(ULONG) DaqL780Simulator::ReadPlataDescr(LPVOID pd)
{
	PPLATA_DESCR_U pdt = (PPLATA_DESCR_U)pd;
	// pdt->t1. -- соответствует L780
	pdt->t1.IsDacPresent = 1;
	strcpy(pdt->t1.BrdName, "L780");
	// тут, может быть, стоит еще какие-то поля заполнить..
	return L_SUCCESS;
}

FDF(ULONG) DaqL780Simulator::WritePlataDescr(LPVOID pd, USHORT Ena)
{
   return L_SUCCESS;
}

//FDF(ULONG) DaqL780Simulator::InputADC(USHORT Chan, PUSHORT Data)
ULONG DaqL780Simulator::InputADC(PDAQ_PAR sp)
{
   return L_SUCCESS;
}

ULONG DaqL780Simulator::ConfigTTL(PDAQ_PAR sp)
{
   return L_SUCCESS;
}


//FDF(ULONG) DaqL780Simulator::InputTTL(PULONG Data, ULONG Mode)
ULONG DaqL780Simulator::InputTTL(PDAQ_PAR sp)
{
   return L_SUCCESS;
}


//FDF(ULONG) DaqL780Simulator::OutputTTL(ULONG Data, ULONG Mode)
ULONG DaqL780Simulator::OutputTTL(PDAQ_PAR sp)
{
   return L_SUCCESS;
}


FDF(ULONG) DaqL780Simulator::EnableCorrection(USHORT Ena)
{
   return L_SUCCESS;
}

//FDF(ULONG) DaqL780Simulator::OutputDAC(short Data, ULONG Mode)
ULONG DaqL780Simulator::OutputDAC(PDAQ_PAR sp)
{
	PASYNC_PAR ap = (PASYNC_PAR)sp;
	if (ap->Mode > 1)
		return L_ERROR;
	dac_values[ap->Mode] = ap->Data[0] & 0xFFF;
	return L_SUCCESS;
}


//FDF(ULONG)
ULONG DaqL780Simulator::FillDACparameters(PDAQ_PAR sp)
{
   return L_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// Процедура чтения слова из пользовательского ППЗУ
////////////////////////////////////////////////////////////////////////////////
FDF(ULONG) DaqL780Simulator::ReadFlashWord(USHORT FlashAddress, PUSHORT Data)
{
	return L_SUCCESS;
}

FDF(ULONG) DaqL780Simulator::WriteFlashWord(USHORT FlashAddress, USHORT Data)
{
	return L_SUCCESS;
}

FDF(ULONG) DaqL780Simulator::EnableFlashWrite(USHORT Flag)
{
	return L_SUCCESS;
}

ULONG DaqL780Simulator::csGetParameter(ULONG name, PULONG param, ULONG status)
{
	return L_SUCCESS;
}

ULONG DaqL780Simulator::csSetParameter(ULONG name, PULONG param, ULONG status)
{
	return L_SUCCESS;
}

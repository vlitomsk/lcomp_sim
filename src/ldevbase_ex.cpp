#include "../include/stubs.h"
#include "../include/ioctl.h"
#include "../include/salayer.h"
#include "../include/ifc_ldev.h"
#include "../include/ldevbase.h"
#include <cstdio>
#include <Windows.h>
#include <cstdint>
// extended interface for ADC/DAC start-stop

FDF(ULONG) LDaqBoardSimulator::InitStartLDeviceEx(ULONG StreamId)
{
	return L_SUCCESS;
}

double getDacVolt(USHORT code) {
	int val = code;
	
	if (val > 2048)
		val = val - 4096;
	return val * 5.0 / 2048.0;
}
#include <cmath>
USHORT getAdcCode(double volt) {
	USHORT rounded = round(abs(volt) * 8192.0 / 5.0);
	if (volt < 0) {
		return rounded;//8192 - rounded;
	} else {
		return rounded;
	}
}

#include "../include/simulator.h"
USHORT calcAdcCode(double tm, LDaqBoardSimulator * p_brd, int logicChnum) {
	DaqL780Simulator * sim = (DaqL780Simulator*)p_brd;

	switch (logicChnum) {
	case 0:
		return getAdcCode(getDacVolt(sim->dac_values[0]) * 0.1);
	case 1:
		return getAdcCode(getDacVolt(sim->dac_values[0]));
	case 4:
		return getAdcCode(getDacVolt(sim->dac_values[1]));
	};
}

DWORD __stdcall LDaqBoardSimulator::sim_thread_routine(LPVOID param) {
	LDaqBoardSimulator *p_brd = (LDaqBoardSimulator*)param;
	int steps_passed = 0;
	const ADC_PAR_0 *adc_par = &p_brd->adc_par.t1;
	ULONG ramFifos = 0;
	const double tmPerPage = adc_par->IrqStep * adc_par->dKadr; // ms
	const double tmPerRamBuf = adc_par->Pages * tmPerPage; // ms
	const double tmRate = 1.0 / adc_par->Rate; // ms
	const int deltaSync = adc_par->IrqStep * adc_par->NCh;
	const int ramFifoSize = adc_par->NCh * adc_par->IrqStep * adc_par->Pages;
    const int64_t millisecond = 10000;
    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = -(LONGLONG)(adc_par->dKadr * millisecond); // '-' means relatime time
	HANDLE hKadrTimer;
	if (!(hKadrTimer = CreateWaitableTimer(NULL, TRUE, NULL))) {
	    return GetLastError();
    }

    do {
		USHORT * pramFifo = (USHORT*)p_brd->data_addr;
        for (ULONG page = 0; p_brd->running && page < adc_par->Pages; ++page) {
            if (!SetWaitableTimer(hKadrTimer, &liDueTime, 0, NULL, NULL, TRUE)) {
                CloseHandle(hKadrTimer);
                return GetLastError();
            }

            for (ULONG kadr = 0; kadr < adc_par->IrqStep; ++kadr) {
                for (ULONG chnum = 0; chnum < adc_par->NCh; ++chnum) {
                    const double tm = tmPerRamBuf * ramFifos 
                                    + tmPerPage * page 
                                    + adc_par->dKadr * kadr 
                                    + tmRate * chnum; // ms from start

                    *(pramFifo++) = calcAdcCode(tm, p_brd, adc_par->Chn[chnum]);
                }
            }
           
            if (WaitForSingleObject(hKadrTimer, INFINITE) != WAIT_OBJECT_0) {
                CloseHandle(hKadrTimer);
                return GetLastError();
            }
            //*(p_brd->sync_addr) += deltaSync; // TODO инкремент указателя на 1 -- это сколько отсчетов?
			*(p_brd->sync_addr) = (*(p_brd->sync_addr) + deltaSync) % ramFifoSize;
        }
        ++ramFifos;
    } while (p_brd->running && adc_par->AutoInit);

    return 0;
}

FDF(ULONG) LDaqBoardSimulator::StartLDeviceEx(ULONG StreamId)
{
	if (running)
		return L_SUCCESS;
	running = true;
	sim_thread = CreateThread(NULL, 0, &LDaqBoardSimulator::sim_thread_routine, this, 0, NULL);
	if (sim_thread == NULL)
		return L_ERROR;

    // start thread here
	ULONG status = L_SUCCESS;
	return status;
}

FDF(ULONG) LDaqBoardSimulator::StopLDeviceEx(ULONG StreamId)
{
	running = false;
	if (WaitForSingleObject(sim_thread, INFINITE) != WAIT_OBJECT_0)
		return L_ERROR;
	sim_thread = NULL;

	ULONG status = L_SUCCESS;
	return status;
};

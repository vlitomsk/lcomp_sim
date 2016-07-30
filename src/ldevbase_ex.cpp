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

DWORD __stdcall LDaqBoardSimulator::sim_thread_routine(LPVOID param) {
	LDaqBoardSimulator *p_brd = (LDaqBoardSimulator*)param;
	int steps_passed = 0;
	const double irqstep_tm = 
	while (p_brd->running) {
		const double passed_time = steps_passed * irqstep_tm;
		for (int tick = 0; tick < p_brd->adc_par.t1.IrqStep; ++tick) {
			const double tm = passed_time + tick * p_brd->adc_par.t1.dKadr;
		}
		++steps_passed;
	}
}

FDF(ULONG) LDaqBoardSimulator::StartLDeviceEx(ULONG StreamId)
{
	if (running)
		return L_SUCCESS;
	HANDLE sim_thread = CreateThread(NULL, 0, LDaqBoardSimulator::sim_thread_routine, this, 0, NULL);
	if (sim_thread == NULL)
		return L_ERROR;
	running = true;

    // start thread here
	ULONG status = L_SUCCESS;
	return status;
}

FDF(ULONG) LDaqBoardSimulator::StopLDeviceEx(ULONG StreamId)
{
    // stop thread here
	ULONG status = L_SUCCESS;
	return status;
};

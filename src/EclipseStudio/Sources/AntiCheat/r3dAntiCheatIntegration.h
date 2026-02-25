#pragma once

#include "AntiCheat/r3dAntiCheat.h"
#include "AntiCheat/r3dAntiCheatProtocol.h"

inline void r3dAntiCheat_GameInit()
{
	__try
	{
		r3dAntiCheat::Init();
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		r3dOutToLog("r3dAntiCheat_GameInit: exception caught, anti-cheat disabled\n");
	}
}

inline void r3dAntiCheat_GameShutdown()
{
	__try
	{
		r3dAntiCheat::Shutdown();
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// ignore
	}
}

inline void r3dAntiCheat_UpdateSecurityReport(
	DWORD curTicks,
	DWORD& nextScanTime,
	BYTE& cachedFlags,
	BYTE& outFlags,
	DWORD& outCodeCRC)
{
	__try
	{
		if(curTicks >= nextScanTime)
		{
			cachedFlags = r3dAntiCheat::PerformScan();
			nextScanTime = curTicks + 25000 + (rand() % 10000);
		}
		outFlags = cachedFlags;
		outCodeCRC = r3dAntiCheat::GetCodeCRC();
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		outFlags = 0;
		outCodeCRC = 0;
	}
}

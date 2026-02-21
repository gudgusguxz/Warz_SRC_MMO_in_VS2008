#pragma once

#include "AntiCheat/r3dAntiCheat.h"
#include "AntiCheat/r3dAntiCheatProtocol.h"

inline void r3dAntiCheat_GameInit()
{
	r3dAntiCheat::Init();
}

inline void r3dAntiCheat_GameShutdown()
{
	r3dAntiCheat::Shutdown();
}

inline void r3dAntiCheat_UpdateSecurityReport(
	DWORD curTicks,
	DWORD& nextScanTime,
	BYTE& cachedFlags,
	BYTE& outFlags,
	DWORD& outCodeCRC)
{
	if(curTicks >= nextScanTime)
	{
		cachedFlags = r3dAntiCheat::PerformScan();
		nextScanTime = curTicks + 25000 + (rand() % 10000);
	}
	outFlags = cachedFlags;
	outCodeCRC = r3dAntiCheat::GetCodeCRC();
}

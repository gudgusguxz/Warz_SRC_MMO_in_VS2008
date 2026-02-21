#pragma once

#include "r3dProtect.h"

enum r3dAntiCheatFlags
{
	AC_CLEAN              = 0x00,
	AC_DEBUGGER_PRESENT   = 0x01,
	AC_CHEAT_TOOL_FOUND   = 0x02,
	AC_SUSPICIOUS_MODULE  = 0x04,
	AC_HW_BREAKPOINT      = 0x08,
	AC_TIMING_ANOMALY     = 0x10,
	AC_CODE_MODIFIED      = 0x20,
};

class r3dAntiCheat
{
public:
	static void  Init();
	static void  Shutdown();
	static BYTE  PerformScan();
	static DWORD GetCodeCRC();
	static void  DecryptString(char* out, const BYTE* enc, int len, BYTE key);

private:
	static bool  CheckDebugger();
	static bool  CheckRemoteDebugger();
	static bool  CheckHardwareBreakpoints();
	static bool  CheckTimingAnomaly();
	static bool  ScanProcesses();
	static bool  ScanWindowTitles();
	static bool  CheckLoadedModules();
	static DWORD ComputeCodeCRC();

	static DWORD s_initialCodeCRC;
	static int   s_initialModuleCount;
	static bool  s_initialized;
};

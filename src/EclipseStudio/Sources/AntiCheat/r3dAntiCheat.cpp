#include "r3dPCH.h"
#include "r3d.h"
#include "r3dAntiCheat.h"

#include <TlHelp32.h>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")

extern DWORD r3dCRC32(const BYTE* data, DWORD size);

// static members
DWORD r3dAntiCheat::s_initialCodeCRC = 0;
int   r3dAntiCheat::s_initialModuleCount = 0;
bool  r3dAntiCheat::s_initialized = false;

//
// XOR-encrypted process name blacklist (key=0x5A)
// "cheatengine","ollydbg","x64dbg","x32dbg","ida64","ida32",
// "processhacker","httpdebugger","wireshark","fiddler","dnspy","hxd"
//
static const BYTE s_encProc[][16] = {
	{0x39,0x32,0x3f,0x3b,0x2e,0x3f,0x34,0x3d,0x33,0x34,0x3f,0},  // cheatengine
	{0x35,0x36,0x36,0x23,0x3e,0x38,0x3d,0},                        // ollydbg
	{0x22,0x6c,0x6e,0x3e,0x38,0x3d,0},                              // x64dbg
	{0x22,0x69,0x68,0x3e,0x38,0x3d,0},                              // x32dbg
	{0x33,0x3e,0x3b,0x6c,0x6e,0},                                    // ida64
	{0x33,0x3e,0x3b,0x69,0x68,0},                                    // ida32
	{0x2a,0x28,0x35,0x39,0x3f,0x29,0x29,0x32,0x3b,0x39,0x31,0x3f,0x28,0}, // processhacker
	{0x32,0x2e,0x2e,0x2a,0x3e,0x3f,0x38,0x2f,0x3d,0x3d,0x3f,0x28,0},      // httpdebugger
	{0x2d,0x33,0x28,0x3f,0x29,0x32,0x3b,0x28,0x31,0},              // wireshark
	{0x3c,0x33,0x3e,0x3e,0x36,0x3f,0x28,0},                        // fiddler
	{0x3e,0x34,0x29,0x2a,0x23,0},                                    // dnspy
	{0x32,0x22,0x3e,0},                                              // hxd
};
static const int s_numEncProc = sizeof(s_encProc) / sizeof(s_encProc[0]);

//
// XOR-encrypted window title fragments (key=0x5A)
// "Cheat Engine","OllyDbg","[CPU","x64dbg","x32dbg","IDA -","Process Hacker"
//
static const BYTE s_encWnd[][20] = {
	{0x19,0x32,0x3f,0x3b,0x2e,0x7a,0x1f,0x34,0x3d,0x33,0x34,0x3f,0}, // Cheat Engine
	{0x15,0x36,0x36,0x23,0x1e,0x38,0x3d,0},                            // OllyDbg
	{0x01,0x19,0x0a,0x0f,0},                                            // [CPU
	{0x22,0x6c,0x6e,0x3e,0x38,0x3d,0},                                  // x64dbg
	{0x22,0x69,0x68,0x3e,0x38,0x3d,0},                                  // x32dbg
	{0x13,0x1e,0x1b,0x7a,0x77,0},                                       // IDA -
	{0x0a,0x28,0x35,0x39,0x3f,0x29,0x29,0x7a,0x12,0x3b,0x39,0x31,0x3f,0x28,0}, // Process Hacker
};
static const int s_numEncWnd = sizeof(s_encWnd) / sizeof(s_encWnd[0]);

// decrypt XOR-encrypted string
void r3dAntiCheat::DecryptString(char* out, const BYTE* enc, int len, BYTE key)
{
	for(int i = 0; i < len; i++)
		out[i] = (char)(enc[i] ^ key);
	out[len] = 0;
}

// case-insensitive substring search
static bool StrContainsI(const char* haystack, const char* needle)
{
	size_t hLen = strlen(haystack);
	size_t nLen = strlen(needle);
	if(nLen > hLen) return false;
	for(size_t i = 0; i <= hLen - nLen; i++)
	{
		bool match = true;
		for(size_t j = 0; j < nLen; j++)
		{
			char h = haystack[i+j];
			char n = needle[j];
			if(h >= 'A' && h <= 'Z') h += 32;
			if(n >= 'A' && n <= 'Z') n += 32;
			if(h != n) { match = false; break; }
		}
		if(match) return true;
	}
	return false;
}

void r3dAntiCheat::Init()
{
	r3dOutToLog("r3dAntiCheat::Init started\n");

	// count loaded modules as baseline (SEH protected)
	__try
	{
		HMODULE hMods[256];
		DWORD cbNeeded = 0;
		if(EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded))
			s_initialModuleCount = (int)(cbNeeded / sizeof(HMODULE));
		else
			s_initialModuleCount = 0;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		r3dOutToLog("r3dAntiCheat: EnumProcessModules failed with exception\n");
		s_initialModuleCount = 0;
	}

	// defer CRC computation to first scan to avoid blocking game start
	// (CRC of .text section can be very large and slow)
	s_initialCodeCRC = 0;

	s_initialized = true;
	r3dOutToLog("r3dAntiCheat::Init done (modules=%d)\n", s_initialModuleCount);
}

void r3dAntiCheat::Shutdown()
{
	s_initialized = false;
}

bool r3dAntiCheat::CheckDebugger()
{
	__try
	{
		return IsDebuggerPresent() != 0;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

bool r3dAntiCheat::CheckRemoteDebugger()
{
	__try
	{
		BOOL remoteDbg = FALSE;
		CheckRemoteDebuggerPresent(GetCurrentProcess(), &remoteDbg);
		return remoteDbg != FALSE;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

bool r3dAntiCheat::CheckHardwareBreakpoints()
{
	// GetThreadContext on current thread is undefined behavior and
	// can deadlock on some Windows versions. Wrap in SEH and use
	// a short timeout via a helper thread for safety.
	__try
	{
		CONTEXT ctx;
		memset(&ctx, 0, sizeof(ctx));
		ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
		// NOTE: GetThreadContext on current thread only works reliably
		// for CONTEXT_DEBUG_REGISTERS on most Windows versions.
		// Full CONTEXT_FULL would be unsafe here.
		if(GetThreadContext(GetCurrentThread(), &ctx))
		{
			if(ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3)
				return true;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// if we can't read debug registers, skip this check
	}
	return false;
}

bool r3dAntiCheat::CheckTimingAnomaly()
{
	__try
	{
		LARGE_INTEGER freq, start, end;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&start);

		// do some busy work
		volatile int dummy = 0;
		for(int i = 0; i < 100; i++)
			dummy += i;

		QueryPerformanceCounter(&end);

		double elapsed = (double)(end.QuadPart - start.QuadPart) / (double)freq.QuadPart;
		// if a simple loop takes >500ms, likely being single-stepped
		// (use generous threshold to avoid false positives during map loading)
		return (elapsed > 0.500);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

bool r3dAntiCheat::ScanProcesses()
{
	__try
	{
		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if(snap == INVALID_HANDLE_VALUE)
			return false;

		char decName[64];
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(pe);

		bool found = false;
		if(Process32First(snap, &pe))
		{
			do {
				for(int i = 0; i < s_numEncProc; i++)
				{
					int len = 0;
					while(s_encProc[i][len] != 0) len++;
					DecryptString(decName, s_encProc[i], len, 0x5A);
					if(StrContainsI(pe.szExeFile, decName))
					{
						found = true;
						break;
					}
				}
				if(found) break;
			} while(Process32Next(snap, &pe));
		}

		CloseHandle(snap);
		return found;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

struct EnumWndCtx {
	bool found;
};

static BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lParam)
{
	EnumWndCtx* ctx = (EnumWndCtx*)lParam;
	char title[256];
	if(GetWindowTextA(hwnd, title, sizeof(title)) > 0)
	{
		char decName[64];
		for(int i = 0; i < s_numEncWnd; i++)
		{
			int len = 0;
			while(s_encWnd[i][len] != 0) len++;
			r3dAntiCheat::DecryptString(decName, s_encWnd[i], len, 0x5A);
			if(StrContainsI(title, decName))
			{
				ctx->found = true;
				return FALSE; // stop enumerating
			}
		}
	}
	return TRUE;
}

bool r3dAntiCheat::ScanWindowTitles()
{
	__try
	{
		EnumWndCtx ctx;
		ctx.found = false;
		EnumWindows(EnumWndProc, (LPARAM)&ctx);
		return ctx.found;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

bool r3dAntiCheat::CheckLoadedModules()
{
	if(s_initialModuleCount == 0)
		return false;

	__try
	{
		HMODULE hMods[256];
		DWORD cbNeeded = 0;
		if(!EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded))
			return false;

		int curCount = (int)(cbNeeded / sizeof(HMODULE));
		// allow generous tolerance for legitimate late-loaded DLLs
		// (map loading pulls in DirectX, audio, physics, etc.)
		return (curCount > s_initialModuleCount + 20);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

DWORD r3dAntiCheat::ComputeCodeCRC()
{
	__try
	{
		// walk PE headers to find .text section
		HMODULE hMod = GetModuleHandle(NULL);
		if(!hMod) return 0;

		const IMAGE_DOS_HEADER* dosH = (const IMAGE_DOS_HEADER*)hMod;
		if(dosH->e_magic != IMAGE_DOS_SIGNATURE) return 0;

		const IMAGE_NT_HEADERS* ntH = (const IMAGE_NT_HEADERS*)((const BYTE*)hMod + dosH->e_lfanew);
		if(ntH->Signature != IMAGE_NT_SIGNATURE) return 0;

		const IMAGE_SECTION_HEADER* sec = IMAGE_FIRST_SECTION(ntH);
		for(WORD i = 0; i < ntH->FileHeader.NumberOfSections; i++, sec++)
		{
			if(memcmp(sec->Name, ".text", 5) == 0)
			{
				const BYTE* textBase = (const BYTE*)hMod + sec->VirtualAddress;
				DWORD textSize = sec->Misc.VirtualSize;

				// sanity check: don't CRC more than 64MB
				if(textSize > 64 * 1024 * 1024)
				{
					r3dOutToLog("r3dAntiCheat: .text section too large (%u bytes), skipping CRC\n", textSize);
					return 0;
				}

				// verify memory is readable before CRCing
				MEMORY_BASIC_INFORMATION mbi;
				if(!VirtualQuery(textBase, &mbi, sizeof(mbi)))
					return 0;
				if(mbi.State != MEM_COMMIT)
					return 0;

				return r3dCRC32(textBase, textSize);
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		r3dOutToLog("r3dAntiCheat: ComputeCodeCRC exception\n");
	}
	return 0;
}

DWORD r3dAntiCheat::GetCodeCRC()
{
	return s_initialCodeCRC;
}

BYTE r3dAntiCheat::PerformScan()
{
	if(!s_initialized)
		return AC_CLEAN;

	__try
	{
		BYTE flags = AC_CLEAN;

		if(CheckDebugger() || CheckRemoteDebugger())
			flags |= AC_DEBUGGER_PRESENT;

		if(CheckHardwareBreakpoints())
			flags |= AC_HW_BREAKPOINT;

		if(CheckTimingAnomaly())
			flags |= AC_TIMING_ANOMALY;

		if(ScanProcesses() || ScanWindowTitles())
			flags |= AC_CHEAT_TOOL_FOUND;

		if(CheckLoadedModules())
			flags |= AC_SUSPICIOUS_MODULE;

		// lazy-init code CRC on first scan (deferred from Init to avoid blocking game start)
		if(s_initialCodeCRC == 0)
		{
			s_initialCodeCRC = ComputeCodeCRC();
			if(s_initialCodeCRC != 0)
				r3dOutToLog("r3dAntiCheat: baseline CRC computed: 0x%08X\n", s_initialCodeCRC);
		}

		// check code integrity
		if(s_initialCodeCRC != 0)
		{
			DWORD curCRC = ComputeCodeCRC();
			if(curCRC != 0 && curCRC != s_initialCodeCRC)
				flags |= AC_CODE_MODIFIED;
		}

		return flags;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		r3dOutToLog("r3dAntiCheat: PerformScan exception\n");
		return AC_CLEAN;
	}
}

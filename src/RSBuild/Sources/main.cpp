#include "r3dPCH.h"
#include "r3d.h"

#include "GameLevel.h"
#include "main.h"
#include "r3dFSBuilder.h"
#include "BuilderConfig.h"

extern	HANDLE		r3d_CurrentProcess;
extern	r3dFSBuilder	builder("");
	char*		g_configFile = "build\\RSBuild.xml";

class r3dSun * Sun;
r3dCamera gCam;
r3dScreenBuffer * DepthBuffer;
r3dLightSystem WorldLightSystem;
bool g_bEditMode = false;

void r3dScaleformBeginFrame() {}
void r3dScaleformEndFrame() {}
unsigned long DriverUpdater(struct HWND__ *,unsigned long,unsigned long,unsigned long,unsigned long,unsigned long,unsigned long) {return 0; }
void SetNewSimpleFogParams() {};
void SetVolumeFogParams() {};

DWORD WINAPI ScanDirectoryInThread(LPVOID param) 
{
    r3dFSBuilder* builder = static_cast<r3dFSBuilder*>(param);
	builder->flist_.reserve(64000);
    builder->ScanDirectory(NULL);
    return 0;
}

DWORD WINAPI CheckTexturesInThread(LPVOID param) 
{
    r3dFSBuilder* builder = static_cast<r3dFSBuilder*>(param);
    if (!builder->CheckTextures()) 
	{
        r3dOutToLog("There is bad .dds files, exiting\n");
        return 1;
    }
    return 0;
}

DWORD WINAPI ReconvertAllSCOInThread(LPVOID param) 
{
    r3dFSBuilder* builder = static_cast<r3dFSBuilder*>(param); 
    builder->ReconvertAllSCO();
    return 0;
}

void CreateArchive() 
{
    BuilderConfig config(builder);
    config.ParseConfig(g_configFile);

    builder.Initialize();
	
	HANDLE thread0 = CreateThread(
        NULL,                          
        0,                             
        ScanDirectoryInThread,         
        static_cast<LPVOID>(&builder), 
        0,                             
        NULL                           
    );

	if (thread0 == NULL) 
	{
        r3dError("Error creating threads\n");
        return;
    }

	DWORD waitResult = WaitForSingleObject(thread0, INFINITE);
    if (waitResult == WAIT_FAILED) 
	{
        r3dError("Error waiting for thread\n");
        return;
    }

	CloseHandle(thread0);

    HANDLE thread1 = CreateThread(
        NULL,                          
        0,                             
        CheckTexturesInThread,         
        static_cast<LPVOID>(&builder), 
        0,                             
        NULL                           
    );

	HANDLE thread2 = CreateThread(
        NULL,                          
        0,                             
        ReconvertAllSCOInThread,       
        static_cast<LPVOID>(&builder), 
        0,                             
        NULL                           
    );

    if (thread1 == NULL || thread2 == NULL) 
	{
        r3dError("Error creating threads\n");
        return;
    }

    waitResult = WaitForSingleObject(thread1, INFINITE);
    if (waitResult == WAIT_FAILED) 
	{
        r3dError("Error waiting for thread1\n");
        return;
    }

    waitResult = WaitForSingleObject(thread2, INFINITE);
    if (waitResult == WAIT_FAILED) 
	{
        r3dError("Error waiting for thread2\n");
        return;
    }

    CloseHandle(thread1);
    CloseHandle(thread2);

    builder.BuildFileList();
    builder.CreateArchive();

    builder.Cleanup();
}

void TestArchive()
{
  r3dFileSystem fs;
  fs.OpenArchive("build\\lzo\\wo");
  r3dOutToLog("bobobo\n");
  
  float t1 = r3dGetTime();
  for(size_t i=0; i<fs.fl_.files_.size(); i++)
  {
    fs.ExtractFile(fs.fl_.files_[i], "build\\1\\");
  }
  r3dOutToLog("Extracted: %f\n", r3dGetTime() - t1);
  
  return;
}

void DoSomeWork()
{
  CreateArchive();
  //TestArchive();
}


void game::Shutdown(void) { r3dError("not a gfx app"); }
void game::MainLoop(void) { r3dError("not a gfx app"); }
void game::Init(void)     { r3dError("not a gfx app"); }
void game::PreInit(void)  { r3dError("not a gfx app"); }

static BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
{
  switch(dwCtrlType) 
  {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
      r3dOutToLog("Control-c ...\n");
      builder.DeleteVolumes();
      //HRESULT res = TerminateProcess(r3d_CurrentProcess, 0);
      break;
  }

  return FALSE;
}

static void ParseArgs(int argc, char* argv[])
{
  r3dOutToLog("cmd: %d\n", argc);
  for(int i=1; i<argc; i++) {
    g_configFile = argv[i];
    r3dOutToLog("%d: %s\n", i, argv[i]);
  }

  return;
}

//
// server entry
//
int main(int argc, char* argv[])
{
  extern int _r3d_bLogToConsole;
  _r3d_bLogToConsole = 1;
  
  // do not close on debug terminate key
  extern int _r3d_bTerminateOnZ;
  _r3d_bTerminateOnZ = 0;
  
  extern int _r3d_MatLib_SkipAllMaterials;
  _r3d_MatLib_SkipAllMaterials = 1;
  r3dMaterialLibrary::Reset();
  
  /*
  // skip filling mesh buffers
  extern int r3dMeshObject_SkipFillBuffers;
  r3dMeshObject_SkipFillBuffers = 1;
  
  // do not use textures
  extern int r3dTexture_UseEmpty;
  r3dTexture_UseEmpty = 1;
  */
  
  r3d_CurrentProcess = GetCurrentProcess();
  SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);
  
  win::hInstance = GetModuleHandle(NULL);

  RegisterAllVars();
  
  try
  {
    ParseArgs(argc, argv);

/*
    serverCreateTempD3DWindow();
    r3dRenderer = new r3dRenderLayer;
    r3dRenderer->Init(win::hWnd, NULL);
    r3dRenderer->SetMode(32, 32, 32, R3DSetMode_Windowed, 0);
*/    

    DoSomeWork();
    
    ClipCursor(NULL);
  } 
  catch(const char* what)
  {
    r3dOutToLog("!!! Exception: %s\n", what);
    // fall thru
  }
  UnregisterAllVars();
  
  DestroyWindow(win::hWnd);
  HRESULT res = TerminateProcess(r3d_CurrentProcess, 0);

  return 0;
}

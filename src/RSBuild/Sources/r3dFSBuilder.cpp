#include "r3dPCH.h"
#include "r3d.h"

// for PatchMatchSpec
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

#include "r3dFSBuilder.h"
#include "FileSystem/r3dFSStructs.h"
#include "FileSystem/r3dFSCompress.h"
#include "r3dMeshConvert.h"

bool pattern_match(const char *str_, const char *pattern) 
{
#if 1
    return PathMatchSpec(str_, pattern) == TRUE;
#else
    enum State {
        Exact,      	// exact match
        Any,        	// ?
        AnyRepeat    	// *
    };
    
    char str[MAX_PATH];
    strcpy_s(str, sizeof(str), str_);
    strlwr(str);

    const char *s = str;
    const char *p = pattern;
    const char *q = 0;
    int state = 0;

    bool match = true;
    while (match && *p) {
        if (*p == '*') {
            state = AnyRepeat;
            q = p+1;
        } else if (*p == '?') state = Any;
        else state = Exact;

        if (*s == 0) break;

        switch (state) {
            case Exact:
                match = *s == *p;
                s++;
                p++;
                break;

            case Any:
                match = true;
                s++;
                p++;
                break;

            case AnyRepeat:
                match = true;
                s++;

                if (*s == *q) p++;
                break;
        }
    }

    if (state == AnyRepeat) return (*s == *q);
    else if (state == Any) return (*s == *p);
    else return match && (*s == *p);
#endif    
} 

r3dFSBuilder::r3dFSBuilder(const char* path)
{
  basefs_ = NULL;
  fs_     = NULL;
  
  basePath_[0]  = 0;
  outputDir_[0] = 0;
  webCDNDir_[0] = 0;
  
  r3dscpy(outputBaseName_, "wz");

  if(path == NULL || *path == 0) {
    basePath_[0] = '\0';
  } else {
    sprintf_s(basePath_, sizeof(basePath_), "%s\\", path);
  }

  SYSTEMTIME t;
  GetSystemTime(&t);
  DWORD year  = (t.wYear - 2010) << 28;
  DWORD month = t.wMonth << 24;
  DWORD day   = t.wDay   << 16;
  DWORD hour  = t.wHour  << 8;
  DWORD min   = t.wMinute;
  buildVersion_ = year | month | day | hour | min;
  
  return;
}

r3dFSBuilder::~r3dFSBuilder()
{
}

void r3dFSBuilder::AddExclude(const char* name, EMatchType type)
{
  match_s match;
  match.type = type;
  strcpy_s(match.name, sizeof(match.name), name);
  strlwr(match.name);

  excludes_.push_back(match);
}

void r3dFSBuilder::AddInclude(const char* name, EMatchType type)
{
  match_s match;
  match.type = type;
  strcpy_s(match.name, sizeof(match.name), name);
  strlwr(match.name);

  includes_.push_back(match);
}

bool r3dFSBuilder::IsDefaultSkippedDir(const char* name)
{
  if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
    return true;

  // skip .svn dir
  if(stricmp(name, ".svn") == 0)
    return true;
    
  return false;
}

bool r3dFSBuilder::ShouldInclude(const char* full, const char* name, int isDir)
{
  if(isDir && IsDefaultSkippedDir(name))
    return false;

  for(size_t i=0; i<includes_.size(); i++)
  {
    const match_s& match = includes_[i];
    if(match.type == MATCH_FULL) 
    {
      if(pattern_match(full, match.name)) {
        //printf("!!! included %s because of %s\n", full, match.name);
        return true;
      }
    } 
    else if(match.type == MATCH_PART) 
    {
      if(pattern_match(name, match.name)) {
        //printf("!!! partially included %s because of %s\n", full, match.name);
        return true;
      }
    } 
    else 
    {
      assert(0);
    }
  }

  return false;    
}

bool r3dFSBuilder::ShouldExclude(const char* full, const char* name, int isDir)
{
  if(isDir && IsDefaultSkippedDir(name))
    return true;
  
  for(size_t i=0; i<excludes_.size(); i++)
  {
    const match_s& match = excludes_[i];
    if(match.type == MATCH_FULL) 
    {
      if(pattern_match(full, match.name)) {
        //printf("!!! ignored %s because of %s\n", full, match.name);
        return true;
      }
    } 
    else if(match.type == MATCH_PART) 
    {
      if(pattern_match(name, match.name)) {
        //printf("!!! partially ignored %s because of %s\n", full, match.name);
        return true;
      }
    } 
    else 
    {
      assert(0);
    }
  }

  return false;    
}

bool r3dFSBuilder::BuildFileList()
{
  r3dOutToLog("Building file list...\n");

  flist_.clear();
  r3d_assert(flist_.size() == 0);
  flist_.reserve(64000);
  ScanDirectory(NULL);
  r3dOutToLog(" %d files\n", flist_.size());

  return true;
}

CRITICAL_SECTION cs;

struct ThreadData 
{
    std::string dir;
    r3dFSBuilder* builder;
	size_t startIdx;
    size_t endIdx;
    int* numBads;
	int* stat1;
    int* stat2;
};

unsigned int __stdcall ScanDirectoryThread(void* param) 
{
    ThreadData* data = reinterpret_cast<ThreadData*>(param);
    data->builder->ScanDirectory(data->dir.c_str());
    delete data;
    return 0;
}

void r3dFSBuilder::ScanDirectory(const char* dir) 
{
    WIN32_FIND_DATA ffblk;
    HANDLE hFind;
    char buf[MAX_PATH];
    std::vector<HANDLE> threads;

    if (dir == NULL)
        sprintf_s(buf, sizeof(buf), "%s%s", basePath_, "*.*");
    else
        sprintf_s(buf, sizeof(buf), "%s%s\\%s", basePath_, dir, "*.*");

    hFind = FindFirstFile(buf, &ffblk);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (strcmp(ffblk.cFileName, ".") == 0 || strcmp(ffblk.cFileName, "..") == 0)
            continue;

        if (dir == NULL)
            sprintf_s(buf, sizeof(buf), "%s%s", basePath_, ffblk.cFileName);
        else
            sprintf_s(buf, sizeof(buf), "%s%s\\%s", basePath_, dir, ffblk.cFileName);

        int isDir = (ffblk.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        if (ShouldExclude(buf, ffblk.cFileName, isDir)) {
            if (!ShouldInclude(buf, ffblk.cFileName, isDir))
                continue;
        }

        if (isDir) 
		{
            ThreadData* data = new ThreadData();
			data->dir = buf;
			data->builder = this;

            HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, ScanDirectoryThread, data, 0, NULL);
            if (thread) 
			{
                threads.push_back(thread);
            } 
			else 
			{
                delete data;
            }
        } 
		else 
		{
            if (ffblk.nFileSizeHigh != 0) 
			{
                r3dError("file %s is bigger than 4gb\n", buf);
                continue;
            }

            file_s f;
            strcpy_s(f.name, sizeof(f.name), buf);
            f.size = ffblk.nFileSizeLow;

            EnterCriticalSection(&cs);
            flist_.push_back(f);
            LeaveCriticalSection(&cs);
        }
    } while (FindNextFile(hFind, &ffblk) != 0);

    FindClose(hFind);

    for (std::vector<HANDLE>::iterator it = threads.begin(); it != threads.end(); ++it) 
	{
		WaitForSingleObject(*it, INFINITE);
		CloseHandle(*it);
	}

    return;
}

void r3dFSBuilder::Initialize() 
{
    InitializeCriticalSection(&cs);
}

void r3dFSBuilder::Cleanup() 
{
    DeleteCriticalSection(&cs);
}

DWORD r3dFSBuilder::DetectFileFlags(const char* fname)
{
	// return r3dFS_FileEntry::FLAG_EXTRACT;
	if (pattern_match(fname, "*.exe") ||
		pattern_match(fname, "*.dll") ||
		//pattern_match(fname, "Crosshair\*.*") ||
		//pattern_match(fname, "HackShield\*.*") ||
		pattern_match(fname, "HShield\*.*") ||
		pattern_match(fname, "ahnrpt.ini") ||
		//pattern_match(fname, "HSUpdate.env") ||
		pattern_match(fname, "ahn.ui") ||
		pattern_match(fname, "*.ui") ||
		pattern_match(fname, "*.aht") ||
		pattern_match(fname, "*.ahc") ||
		pattern_match(fname, "*.hsb") ||
		pattern_match(fname, "*.id") ||
		pattern_match(fname, "*.env") ||
		pattern_match(fname, "*.mhe") ||
		pattern_match(fname, "*.msd") ||
		pattern_match(fname, "*.key") ||
		pattern_match(fname, "*.scd") ||
		pattern_match(fname, "*.uic") ||
		pattern_match(fname, "3n.mhe"))
	{
		r3dOutToLog("FLAG EXTRACT fname %s\n", fname);
		return r3dFS_FileEntry::FLAG_EXTRACT;
	}

	return 0;
}

inline size_t Min(size_t a, size_t b) 
{
    return (a < b) ? a : b;
}

DWORD WINAPI ProcessFilesInThread(LPVOID param) 
{
    ThreadData* data = static_cast<ThreadData*>(param);
    for (size_t i = data->startIdx; i < data->endIdx; ++i) 
	{
        if (data->builder->ExistInBaseArchive(data->builder->flist_[i], i, data->builder->flist_.size()))
            continue;

		EnterCriticalSection(&cs);
        r3dFS_FileEntry* fe = data->builder->fs_->fl_.AddNew(data->builder->flist_[i].name);
        fe->size = data->builder->flist_[i].size;
        fe->flags |= data->builder->DetectFileFlags(fe->name);
		LeaveCriticalSection(&cs);
    }
    delete data;
    return 0;
}

void r3dFSBuilder::CreateFileSystem() 
{
    r3d_assert(fs_ == NULL);
    r3dOutToLog("Checking for new files...\n");
    CLOG_INDENT;

    size_t numFiles = flist_.size();
    r3d_assert(numFiles);

    fs_ = new r3dFileSystem();
    fs_->fl_.files_.reserve(numFiles);
    fs_->fl_.buildVersion_ = buildVersion_;
    fs_->VOLUME_SIZE = 1024 * 1024 * 1024;

    const int numThreads = 16;
    size_t chunkSize = (numFiles + numThreads - 1) / numThreads;

    std::vector<HANDLE> threads;

    for (int i = 0; i < numThreads; ++i) {
        size_t startIdx = i * chunkSize;
        size_t endIdx = Min(startIdx + chunkSize, numFiles);

        if (startIdx >= numFiles)
            break;

        ThreadData* data = new ThreadData();
		data->builder = this;
		data->startIdx = startIdx;
		data->endIdx = endIdx;
        HANDLE thread = CreateThread(
            NULL,             
            0,                
            ProcessFilesInThread, 
            data,             
            0,                
            NULL              
        );

        if (thread) 
		{
            threads.push_back(thread);
        } 
		else 
		{
            delete data;
        }
    }

    for (size_t i = 0; i < threads.size(); ++i) 
	{
        WaitForSingleObject(threads[i], INFINITE);
        CloseHandle(threads[i]);
    }

	return;
}

bool r3dFSBuilder::OpenBaseArchive(const char* base)
{
  r3d_assert(flist_.size() == 0 && "must be called before creating file list");

  r3dOutToLog("Using %s as base archive\n", base);

  r3d_assert(basefs_ == NULL);
  basefs_ = new r3dFileSystem();
  if(!basefs_->OpenArchive(base)) {
    r3dError("can't open base archive %s\n", base);
    return false;
  }
  
  return true;
}

bool r3dFSBuilder::ExistInBaseArchive(const file_s& file, int curFile, int totFiles)
{
  if(basefs_ == NULL)
    return false;
    
  const r3dFS_FileEntry* fe = basefs_->GetFileEntry(file.name);
  if(fe == NULL)
    return false;
    
  if(fe->size != file.size) {
    r3dOutToLog("changed file %s\n", file.name);
    // size was changed, file updated
    return false;
  }
  
  // need to compare CRC
  r3dOutToLog("comparing [%04d/%04d]\r", curFile+1, totFiles); //file.name

  DWORD crc32;
  DWORD size;
  if(!r3dGetFileCrc32(file.name, &crc32, &size))
    r3dError("Can't get crc for file %s\n", file.name);
  if(crc32 != fe->crc32) {
    r3dOutToLog("changed file %s\n", file.name);
    return false;
  }
   
  return true;
}

DWORD WINAPI CheckTexturesInThread2(LPVOID param) {
    ThreadData* data = static_cast<ThreadData*>(param);

    for (size_t i = data->startIdx; i < data->endIdx; ++i) {
        const char* dds = data->builder->flist_[i].name;
        if (stricmp(dds + strlen(dds) - 4, ".dds") != 0)
            continue;

        D3DXIMAGE_INFO im = {0};
        if (FAILED(D3DXGetImageInfoFromFile(dds, &im)))
            continue;

        switch (im.Format) {
            case D3DFMT_DXT1:
            case D3DFMT_DXT2:
            case D3DFMT_DXT3:
            case D3DFMT_DXT4:
            case D3DFMT_DXT5:
                if ((im.Height % 4) != 0 || (im.Width % 4) != 0) {
                    EnterCriticalSection(&cs);
                    r3dOutToLog("%s: bad dimension %dx%d\n", dds, im.Width, im.Height);
                    (*data->numBads)++;
                    LeaveCriticalSection(&cs);
                }
                break;
        }
    }

    delete data;
    return 0;
}

bool r3dFSBuilder::CheckTextures() {
    r3dOutToLog("Scanning for .DDS files\n");
    CLOG_INDENT;

    size_t numFiles = flist_.size();
    if (numFiles == 0) {
        r3dOutToLog("No files to process.\n");
        return true;
    }

    int numBads = 0;
    const int numThreads = 16;
    size_t chunkSize = (numFiles + numThreads - 1) / numThreads;

    std::vector<HANDLE> threads;

    for (int i = 0; i < numThreads; ++i) {
        size_t startIdx = i * chunkSize;
        size_t endIdx = Min(startIdx + chunkSize, numFiles);

        if (startIdx >= numFiles)
            break;

        ThreadData* data = new ThreadData();
        data->builder = this;
        data->startIdx = startIdx;
        data->endIdx = endIdx;
        data->numBads = &numBads;

        HANDLE thread = CreateThread(
            NULL,
            0,
            CheckTexturesInThread2,
            data,
            0,
            NULL
        );

        if (thread) 
		{
            threads.push_back(thread);
        } else 
		{
            delete data;
        }
    }

    for (size_t i = 0; i < threads.size(); ++i) 
	{
		WaitForSingleObject(threads[i], INFINITE);
		CloseHandle(threads[i]);
	}

    if (numBads > 0) 
	{
        r3dOutToLog("%d bad dds found\n", numBads);
        return false;
    }

    return true;
}

DWORD WINAPI ProcessFilesSCO2InThread(LPVOID param)
{
    ThreadData* data = static_cast<ThreadData*>(param);
    int stat1 = 0;
    int stat2 = 0;
    
    for (size_t i = data->startIdx; i < data->endIdx; ++i) 
    {
        const char* sco = data->builder->flist_[i].name;
        if(stricmp(sco + strlen(sco) - 4, ".sco") != 0)
            continue;

        char scb[MAX_PATH];
        strcpy_s(scb, sizeof(scb), sco);
        scb[strlen(scb)-1] = 'b';
        
        FILETIME t1, t2;
        if(!getFileTimestamp(sco, t1)) {
            int err = GetLastError();
            r3dError("failed to get timestamp for %s: %d\n", sco, err);
        }

        if(!getFileTimestamp(scb, t2)) {
            r3dOutToLog("+ %s\n", sco);
			EnterCriticalSection(&cs);
            if(int cnvres = convertScoToBinary(sco, scb) != 0) {
				LeaveCriticalSection(&cs);
                r3dError("failed to convert %s, err=%d\n", sco, cnvres);
            }
			LeaveCriticalSection(&cs);
            stat1++;
            continue;
        }

		{
			FILE* f = fopen(scb, "rb");
			r3d_assert(f);
			uint32_t version;
			fread(&version, sizeof(uint32_t), 1, f);
			fclose(f);

			extern uint32_t R3DMESH_BINARY_VERSION_get();
			if(version != R3DMESH_BINARY_VERSION_get()) {
				r3dOutToLog("v %s\n", sco);
				_unlink(scb);
				EnterCriticalSection(&cs);
				convertScoToBinary(sco, scb);
				LeaveCriticalSection(&cs);
			 stat2++;
				continue;
			}
		}

        if(t1.dwLowDateTime != t2.dwLowDateTime || t1.dwHighDateTime != t2.dwHighDateTime) {
            r3dOutToLog("* %s\n", sco);
			EnterCriticalSection(&cs);
            convertScoToBinary(sco, scb);
			LeaveCriticalSection(&cs);
            stat2++;
            continue;
        }
    }

    *data->stat1 += stat1;
    *data->stat2 += stat2;

    delete data;
    return 0;
}

bool r3dFSBuilder::ReconvertAllSCO()
{
    #if !RECONVERT_SCO_MESHES
    r3dOutToLog("ReconvertAllSCO disabled\n");
    return true;
    #endif

    r3dOutToLog("Scanning for .SCO files\n");
    CLOG_INDENT;

    int stat1 = 0;
    int stat2 = 0;

    size_t numFiles = flist_.size();
    const int numThreads = 16;
    size_t chunkSize = (numFiles + numThreads - 1) / numThreads;

    std::vector<HANDLE> threads;

    for (int i = 0; i < numThreads; ++i) {
        size_t startIdx = i * chunkSize;
        size_t endIdx = Min(startIdx + chunkSize, numFiles);

        if (startIdx >= numFiles)
            break;

        ThreadData* data = new ThreadData();
        data->builder = this;
        data->startIdx = startIdx;
        data->endIdx = endIdx;
        data->stat1 = &stat1;
        data->stat2 = &stat2;

        HANDLE thread = CreateThread(
            NULL,             
            0,                
            ProcessFilesSCO2InThread, 
            data,             
            0,                
            NULL              
        );

        if (thread) 
		{
            threads.push_back(thread);
        } 
		else 
		{
            delete data;
        }
    }

    for (size_t i = 0; i < threads.size(); ++i) {
        WaitForSingleObject(threads[i], INFINITE);
        CloseHandle(threads[i]);
    }

    r3dOutToLog("%d .sco converted, %d .sco reconverted\n", stat1, stat2);

    return true;
}

struct SCOProcessData 
{
    r3dFS_FileList* fl;
    std::vector<const char*>* todelete;
    int* stat1;
    int* stat2;
    r3dFS_FileList* basefs_fl;
    size_t startIdx;
    size_t endIdx;
};

DWORD WINAPI ProcessFilesSCOInThread(LPVOID param)
{
    SCOProcessData* data = static_cast<SCOProcessData*>(param);
    r3dFS_FileList& fl = *data->fl;
    std::vector<const char*>& todelete = *data->todelete;
    int* stat1 = data->stat1;
    int* stat2 = data->stat2;
    r3dFS_FileList* basefs_fl = data->basefs_fl;

    for (size_t i = data->startIdx; i < data->endIdx; ++i) {
        const r3dFS_FileEntry* fe = fl.files_[i];

        if (stricmp(fe->name + strlen(fe->name) - 4, ".sco") != 0)
            continue;
        
        char scb[MAX_PATH];
        strcpy_s(scb, sizeof(scb), fe->name);
        scb[strlen(scb)-1] = 'b';
        
        const r3dFS_FileEntry* fe2 = fl.Find(scb);
        if (fe2 == NULL) {
            if (basefs_fl == NULL || basefs_fl->Find(scb) == NULL) {
                (*stat1)++;
                continue;
            }
        }

        FILETIME t1, t2;
        if (!getFileTimestamp(fe->name, t1) || !getFileTimestamp(scb, t2)) {
            (*stat1)++;
            continue;
        }
        
        if (t1.dwLowDateTime != t2.dwLowDateTime || t1.dwHighDateTime != t2.dwHighDateTime) {
            (*stat2)++;
            continue;
        }

        EnterCriticalSection(&cs);
        todelete.push_back(fe->name);
        LeaveCriticalSection(&cs);
    }

    delete data;
    return 0;
}

void r3dFSBuilder::FilterOutSCO()
{
    r3dOutToLog("Filtering out unneeded .sco\n");
    CLOG_INDENT;

    int stat1 = 0;
    int stat2 = 0;

    std::vector<const char*> todelete;

    r3dFS_FileList& fl = fs_->fl_;
    size_t numFiles = fl.files_.size();
    const int numThreads = 16;
    size_t chunkSize = (numFiles + numThreads - 1) / numThreads;

    std::vector<HANDLE> threads;
    for (int i = 0; i < numThreads; ++i) {
        size_t startIdx = i * chunkSize;
        size_t endIdx = Min(startIdx + chunkSize, numFiles);

        if (startIdx >= numFiles)
            break;

        SCOProcessData* data = new SCOProcessData();
        data->fl = &fl;
        data->todelete = &todelete;
        data->stat1 = &stat1;
        data->stat2 = &stat2;
        data->basefs_fl = basefs_ ? &basefs_->fl_ : NULL;
        data->startIdx = startIdx;
        data->endIdx = endIdx;

        HANDLE thread = CreateThread(
            NULL,
            0,
            ProcessFilesSCOInThread,
            data,
            0,
            NULL
        );

        if (thread) 
		{
            threads.push_back(thread);
        } 
		else 
		{
            delete data;
        }
    }

    for (size_t i = 0; i < threads.size(); ++i) {
        WaitForSingleObject(threads[i], INFINITE);
        CloseHandle(threads[i]);
    }

    for (size_t i = 0; i < todelete.size(); i++) {
        const char* fname = todelete[i];
        fl.Remove(fname);
    }

    r3dOutToLog("%d .sco missed binary, %d .sco have outdated binary\n", stat1, stat2);
    r3dOutToLog("%d .sco removed from archive\n", todelete.size());
}

void r3dFSBuilder::FilterAndDeleteLostSCB()
{
  r3dOutToLog("Filtering out lost .scb\n");
  CLOG_INDENT;

  int stat1 = 0;
  
  // store array of filenames from r3dFS_FileEntry to delete
  std::vector<const char*> todelete;

  r3dFS_FileList& fl = fs_->fl_;
  for(size_t i=0; i<fl.files_.size(); i++) 
  {
    const char* scb = fl.files_[i]->name;
    if(stricmp(scb + strlen(scb) - 4, ".scb") != 0)
      continue;
      
    // check if we have .scb with same name
    char sco[MAX_PATH];
    strcpy_s(sco, sizeof(sco), scb);
    sco[strlen(sco)-1] = 'o';
    
    const r3dFS_FileEntry* fe2 = fl.Find(sco);
    if(fe2 != NULL) {
      continue;
    }
    
    // we have lost .SCB (without .SCO counterpart)
    stat1++;
    //r3dOutToLog("%s is lost\n", scb);
    _unlink(scb);

    // remove it from archive
    todelete.push_back(scb);
  }
  
  // delete them from filesystem
  for(size_t i=0; i<todelete.size(); i++) {
    const char* fname = todelete[i];
    fl.Remove(fname);
    // please note that fname will be invalid at the moment. fe->name was freed
  }
  
  r3dOutToLog("%d lost .scb deleted\n", stat1);
  
  return;
}

bool r3dFSBuilder::CreateArchive()
{
  if(*outputDir_ == 0) r3dError("no output directory");
  if(*webCDNDir_ == 0) r3dError("no web cdn url");
  
  if(flist_.size() == 0) {
    r3dError("no files\n");
    return false;
  }

  CreateFileSystem();

  FilterAndDeleteLostSCB(); // need to do it before filtering SCO
  FilterOutSCO();

  // check if no new files was added
  if(fs_->GetNumFiles() == 0) {
    r3dOutToLog("No files was changed or added\n");
    return true;
  }

  char vname[MAX_PATH];
  sprintf_s(vname, sizeof(vname), "%s\\%s%08X", outputDir_, outputBaseName_, buildVersion_);

  r3dOutToLog("Creating%s archive %s\n", basefs_ ? " incremental" : "", vname);
  CLOG_INDENT;
  fs_->BuildNewArchive(vname);

  char xmlname[MAX_PATH];
  sprintf_s(xmlname, sizeof(xmlname), "%s\\%s.xml", outputDir_, outputBaseName_);
  FILE* f = fopen_for_write(xmlname, "wt");
  
  if(basefs_ != NULL)
  {
    // incremental update
    r3d_assert(basefs_->fl_.IsValid());
    fprintf(f, "<p>\n");
    fprintf(f, " <d ver=\"%u\" base=\"%s%s%08X\" inc=\"%s%s%08X\"", 
      buildVersion_, 
      webCDNDir_, outputBaseName_, basefs_->fl_.buildVersion_,
      webCDNDir_, outputBaseName_, buildVersion_);
    fprintf(f, "/>\n");
    fprintf(f, "</p>\n");
  }
  else
  {
    // full update
    fprintf(f, "<p>\n");
    fprintf(f, " <d ver=\"%u\" base=\"%s%s%08X\"", 
      buildVersion_, 
      webCDNDir_, outputBaseName_, buildVersion_);
    fprintf(f, "/>\n");
    fprintf(f, "</p>\n");
  }
  
  fclose(f);
  return true;
}

void r3dFSBuilder::DeleteVolumes()
{
  if(fs_ == NULL)
    return;
    
  fs_->CloseVolumes();
  fs_->RemoveVolumeFiles();
}
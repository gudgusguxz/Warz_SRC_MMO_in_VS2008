#include "r3dPCH.h"
#include "r3d.h"
#include "CkGzip.h"

#include "GameCode/UserProfile.h"

#include "WOBackendAPI.h"

	const char*	gDomainBaseUrl= "/SV-SIAM/api/";
	int		gDomainPort   = 80; // PAX_BUILD - change to 80 and no SSL
	bool		gDomainUseSSL = false;
	
CWOBackendReq::CWOBackendReq(const char* url)
{
	Init(url);
}

CWOBackendReq::CWOBackendReq(const CUserProfile* prof, const char* url)
{
	Init(url);

	AddSessionInfo(prof->CustomerID, prof->SessionID);
}

void CWOBackendReq::Init(const char* url)
{
#ifndef FINAL_BUILD
	// automatically switch to local site version 
	if(stricmp(g_api_ip->GetString(), "localhost") == 0)
	{
		gDomainBaseUrl= "/Site/";
		gDomainPort   = 56016;
		gDomainUseSSL = false;
	}
#endif

	resp_       = NULL;
	initedOk_   = false;
	
	savedUrl_   = url;
	resultCode_ = 0;
	bodyStr_    = "";
	bodyLen_    = 0;
	
	static bool s_httpUnlocked = false;
	if(!s_httpUnlocked)
	{
		int success = http.UnlockComponent("ARKTOSHttp_decCLPWFQXmU");
		if(success != 1)
		{
			r3dOutToLog("WO_API: failed to unlock CkHttp\n");
			resultCode_ = 8;
			return;
		}
		s_httpUnlocked = true;
	}
#ifdef WO_SERVER	
	http.put_ConnectTimeout(10);
#else
	// NOTE: This API is synchronous (blocks UI). Keep timeouts reasonable.
	// Buying/exchange endpoints tend to be perceived as "spinning"; prefer faster fail.
	if(strstr(url, "api_Buy") || strstr(url, "api_Exchange") || strstr(url, "api_SteamBuy"))
		http.put_ConnectTimeout(10);
	else
		http.put_ConnectTimeout(30);
#endif	
	if(strstr(url, "api_Buy") || strstr(url, "api_Exchange") || strstr(url, "api_SteamBuy"))
		http.put_ReadTimeout(25);
	else
		http.put_ReadTimeout(60);

	// create request
	char fullUrl[512];
	if(url[0] != '/')
	  sprintf_s(fullUrl, sizeof(fullUrl), "%s%s", gDomainBaseUrl, url);
	else
	  strcpy_s(fullUrl, sizeof(fullUrl), url);

	req.UsePost();
	req.put_Path(fullUrl);
	req.put_Utf8(true);
	req.put_SendCharset(true);
	req.put_Charset("utf-8");

	initedOk_ = true;
}

CWOBackendReq::~CWOBackendReq()
{
	SAFE_DELETE(resp_);
}

void CWOBackendReq::AddSessionInfo(DWORD id, DWORD sid)
{
	r3d_assert(id);
	
	AddParam("s_id",  (int)id);
	AddParam("s_key", (int)sid);
}

void CWOBackendReq::AddParam(const char* name, const char* val)
{
	req.AddParam(name, val);
}

void CWOBackendReq::AddParam(const char* name, int val)
{
	char	buf[1024];
	sprintf_s(buf, sizeof(buf), "%d", val);
	AddParam(name, buf);
}

void CWOBackendReq::AddParamF(const char* name, float val)
{
	char	buf[1024];
	sprintf_s(buf, sizeof(buf), "%.3f", val);
	AddParam(name, buf);
}

int CWOBackendReq::ParseResult(CkHttpResponse* resp)
{
	if(resp == NULL)
	{
		r3dOutToLog("WO_API: http timeout\n");
		return 8;
	}

	if(resp->get_StatusCode() != 200)
	{
		r3dOutToLog("WO_API: returned http%d\n", resp->get_StatusCode());

		//resp->get_Body(data_);
		//data_.appendChar(0);
		//r3dOutToLog("WO_API: %s\n", data_.getData());

		return 8;
	}
	
	// NOTE: we can't use getBodyStr() because it skip zeroes inside answer body
	resp->get_Body(data_);
	data_.appendChar(0);
	
	// if context is gzipped, uncompress it
	if(data_.getSize() >= 2 && data_.getByte(0) == '$' && data_.getByte(1) == '1')
	{
		CkGzip gzip;
		if(gzip.UnlockComponent("ARKTOSZIP_cRvE6e7mpSqD") == false)
		{
			r3dOutToLog("WO_API: failed to unlock gzip\n");
			return 9;
		}
	
		// remove header and decompress
		CkByteData udata;
		data_.removeChunk(0, 2);
		if(!gzip.UncompressMemory(data_, udata)) {
			r3dOutToLog("WO_API: decompress failed\n");
			return 9;
		}
		//r3dOutToLog("WO_API: decompressed %d->%d\n", data_.getSize(), udata.getSize());

		// swap data with uncompressed one
		data_ = udata;
		data_.appendChar(0);
	}
	bodyStr_ = (const char*)data_.getData();
	bodyLen_ = data_.getSize() - 1;
	
	// validate http answer
	if(bodyLen_ < 4)
	{
		r3dOutToLog("WO_API: too small answer\n");
		return 9;
	}
	
	// check if we got XML header "<?xml"
	if(bodyStr_[0] == '<' && bodyStr_[1] == '?' && bodyStr_[2] == 'x')
	{
		return 0;
	}
	
	// check for 'WO_x' header
	if(bodyStr_[0] != 'W' || bodyStr_[1] != 'O' || bodyStr_[2] != '_')
	{
		r3dOutToLog("WO_API: wrong header: %s\n", bodyStr_);
		return 9;
	}

	int resultCode = bodyStr_[3] - '0';
	
	// offset body content past header
	bodyStr_ += 4;
	bodyLen_ -= 4;

	// parse result code
	switch(resultCode)
	{
		case 0:	// operation ok
			break;
			
		case 1:	// session not valid
			r3dOutToLog("WO_API: session disconnected\n");
			break;

		default:
			r3dOutToLog("WO_API: failed with error code %d %s\n", resultCode, bodyStr_);
			break;
	}

	return resultCode;
}

bool CWOBackendReq::Issue()
{
	SAFE_DELETE(resp_);
	if(!initedOk_)
		return false;

	float t1 = r3dGetTime();
	resp_ = http.SynchronousRequest(g_api_ip->GetString(), gDomainPort, gDomainUseSSL, req);
	float dt = r3dGetTime() - t1;
	#ifndef FINAL_BUILD
	//r3dOutToLog("WOApi: %s NETWORK time: %.4f\n", savedUrl_, r3dGetTime()-t1);
	#endif

	resultCode_ = ParseResult(resp_);
	if(dt > 2.0f || resultCode_ != 0)
	{
		r3dOutToLog("WO_API: %s time=%.3f rc=%d http=%d\n", savedUrl_, dt, resultCode_, resp_ ? resp_->get_StatusCode() : 0);
	}
	return resultCode_ == 0;
}


void CWOBackendReq::ParseXML(pugi::xml_document& xmlFile)
{
	pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace((void*)bodyStr_, bodyLen_);
	if(!parseResult)
	{
		// Avoid crashing client on truncated/HTML/error responses. Callers can validate expected nodes.
		r3dOutToLog("WO_API: Failed to parse server XML (%s)\n", parseResult.description());
		return;
	}
}
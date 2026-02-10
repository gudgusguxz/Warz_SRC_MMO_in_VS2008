#include "stdafx.h"
#include "CaptureConfig.h"
#include <shlobj.h>

#define ADD_STRING(i, s)		ComboBox_AddString(GetDlgItem(i), s)

#define SET_TEXT(i, s)			::SetWindowText(GetDlgItem(i), s)
#define GET_TEXT(i, s)			::GetWindowText(GetDlgItem(i), s, MAX_PATH)

#define SET_TEXT_INT(i, s)		::SetWindowText(GetDlgItem(i), _itot(s, buf, 10))
#define SET_TEXT_INT_16(i, s)	::SetWindowText(GetDlgItem(i), _itot(s, buf, 16))
#define GET_TEXT_INT(i, s)		::GetWindowText(GetDlgItem(i), buf, MAX_PATH); s = _tstoi(buf)
#define GET_TEXT_INT_16(i, s)	::GetWindowText(GetDlgItem(i), buf, MAX_PATH); s = _tcstol(buf, NULL, 16)

#define SET_TEXT_FLOAT(i, s)	::swprintf(buf, MAX_PATH,  _T("%.3f"), s);	::SetWindowText(GetDlgItem(i), buf)
#define GET_TEXT_FLOAT(i, s)	::GetWindowText(GetDlgItem(i), buf, MAX_PATH); s = (float)_tstof(buf)

#define SET_CHECK(i, s)			::SendMessage(GetDlgItem(i), BM_SETCHECK, s?BST_CHECKED:BST_UNCHECKED, NULL)
#define GET_CHECK(i, s)			s = (::SendMessage(GetDlgItem(i), BM_GETCHECK, NULL , NULL) == BST_CHECKED)

CCaptureConfig::CCaptureConfig(BCAP_CONFIG *cfg /*=NULL*/)
{
	if(cfg) m_cfg = *cfg;

	m_cap_mode = BCAP_MODE_D3D9_SCALE;
	m_min_fps = 0;
	m_max_fps = 0;
	m_priority = THREAD_PRIORITY_NORMAL;

	// get mydocument folder
	m_target_directory[0]=0;

	::GetModuleFileName(NULL, m_target_directory, MAX_PATH);
	*(_tcsrchr( m_target_directory, '\\' )) = NULL;
	
	wcscpy(m_cfg.LogoPath, m_target_directory);
	wcscat(m_cfg.LogoPath, L"\\logo.png");
}



LRESULT CCaptureConfig::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow();

	// video width
	ADD_STRING(IDC_VIDEO_SIZEW,			_T("-1"));
	ADD_STRING(IDC_VIDEO_SIZEW,			_T("0"));
	ADD_STRING(IDC_VIDEO_SIZEW,			_T("320"));
	ADD_STRING(IDC_VIDEO_SIZEW,			_T("640"));
	ADD_STRING(IDC_VIDEO_SIZEW,			_T("800"));
	ADD_STRING(IDC_VIDEO_SIZEW,			_T("1024"));
	ADD_STRING(IDC_VIDEO_SIZEW,			_T("1280"));
	ADD_STRING(IDC_VIDEO_SIZEW,			_T("1600"));
	ADD_STRING(IDC_VIDEO_SIZEW,			_T("1920"));
	ADD_STRING(IDC_VIDEO_SIZEW,			_T("2560"));

	// video height
	ADD_STRING(IDC_VIDEO_SIZEH,			_T("-1"));
	ADD_STRING(IDC_VIDEO_SIZEH,			_T("0"));
	ADD_STRING(IDC_VIDEO_SIZEH,			_T("240"));
	ADD_STRING(IDC_VIDEO_SIZEH,			_T("480"));
	ADD_STRING(IDC_VIDEO_SIZEH,			_T("600"));
	ADD_STRING(IDC_VIDEO_SIZEH,			_T("720"));
	ADD_STRING(IDC_VIDEO_SIZEH,			_T("900"));
	ADD_STRING(IDC_VIDEO_SIZEH,			_T("1024"));
	ADD_STRING(IDC_VIDEO_SIZEH,			_T("1080"));
	ADD_STRING(IDC_VIDEO_SIZEH,			_T("1200"));
	ADD_STRING(IDC_VIDEO_SIZEH,			_T("1600"));

	// Video fps
	ADD_STRING(IDC_VIDEO_FPS,			_T("15.000"));
	ADD_STRING(IDC_VIDEO_FPS,			_T("20.000"));
	ADD_STRING(IDC_VIDEO_FPS,			_T("24.000"));
	ADD_STRING(IDC_VIDEO_FPS,			_T("25.000"));
	ADD_STRING(IDC_VIDEO_FPS,			_T("29.976"));
	ADD_STRING(IDC_VIDEO_FPS,			_T("30.000"));
	ADD_STRING(IDC_VIDEO_FPS,			_T("60.000"));
	ADD_STRING(IDC_VIDEO_FPS,			_T("120.000"));

	// video fourcc
	ADD_STRING(IDC_VIDEO_CODEC,			_T("AUTO"));
	ADD_STRING(IDC_VIDEO_CODEC,			_T("MPEG"));
	ADD_STRING(IDC_VIDEO_CODEC,			_T("MJPG"));
	ADD_STRING(IDC_VIDEO_CODEC,			_T("MP4V"));
	ADD_STRING(IDC_VIDEO_CODEC,			_T("H264"));
	ADD_STRING(IDC_VIDEO_CODEC,			_T("ARGB"));
	ADD_STRING(IDC_VIDEO_CODEC,			_T("RGB "));
	ADD_STRING(IDC_VIDEO_CODEC,			_T("YV12"));
	ADD_STRING(IDC_VIDEO_CODEC,			_T("CUDA"));
	ADD_STRING(IDC_VIDEO_CODEC,			_T("SYNC"));
	ADD_STRING(IDC_VIDEO_CODEC,			_T("APP "));
	ADD_STRING(IDC_VIDEO_CODEC,			_T("NVEN"));

	// video VBR/CBR
	ADD_STRING(IDC_VIDEO_RATECONTROL,	_T("0"));
	ADD_STRING(IDC_VIDEO_RATECONTROL,	_T("1"));

	// video bitrate
	ADD_STRING(IDC_VIDEO_KBITRATE,		_T("300"));
	ADD_STRING(IDC_VIDEO_KBITRATE,		_T("500"));
	ADD_STRING(IDC_VIDEO_KBITRATE,		_T("700"));
	ADD_STRING(IDC_VIDEO_KBITRATE,		_T("1000"));
	ADD_STRING(IDC_VIDEO_KBITRATE,		_T("1500"));
	ADD_STRING(IDC_VIDEO_KBITRATE,		_T("2000"));
	ADD_STRING(IDC_VIDEO_KBITRATE,		_T("3000"));
	ADD_STRING(IDC_VIDEO_KBITRATE,		_T("5000"));
	ADD_STRING(IDC_VIDEO_KBITRATE,		_T("10000"));
	ADD_STRING(IDC_VIDEO_KBITRATE,		_T("30000"));
	ADD_STRING(IDC_VIDEO_KBITRATE,		_T("50000"));

	// video quality
	ADD_STRING(IDC_VIDEO_QUALITY,		_T("40"));
	ADD_STRING(IDC_VIDEO_QUALITY,		_T("50"));
	ADD_STRING(IDC_VIDEO_QUALITY,		_T("60"));
	ADD_STRING(IDC_VIDEO_QUALITY,		_T("70"));
	ADD_STRING(IDC_VIDEO_QUALITY,		_T("80"));
	ADD_STRING(IDC_VIDEO_QUALITY,		_T("90"));
	ADD_STRING(IDC_VIDEO_QUALITY,		_T("100"));

	// keyframe interval
	ADD_STRING(IDC_VIDEO_INTERVAL,		_T("0"));
	ADD_STRING(IDC_VIDEO_INTERVAL,		_T("10"));
	ADD_STRING(IDC_VIDEO_INTERVAL,		_T("30"));
	ADD_STRING(IDC_VIDEO_INTERVAL,		_T("60"));
	ADD_STRING(IDC_VIDEO_INTERVAL,		_T("120"));
	ADD_STRING(IDC_VIDEO_INTERVAL,		_T("300"));

	// audio channels
	ADD_STRING(IDC_AUDIO_CHANNEL,		_T("1"));
	ADD_STRING(IDC_AUDIO_CHANNEL,		_T("2"));
	
	// audio samplerate
	ADD_STRING(IDC_AUDIO_SAMPLERATE,	_T("22050"));
	ADD_STRING(IDC_AUDIO_SAMPLERATE,	_T("24000"));
	ADD_STRING(IDC_AUDIO_SAMPLERATE,	_T("44100"));
	ADD_STRING(IDC_AUDIO_SAMPLERATE,	_T("48000"));

	// audio codec
	ADD_STRING(IDC_AUDIO_CODEC,			_T("ff")); 
	ADD_STRING(IDC_AUDIO_CODEC,			_T("55"));
	ADD_STRING(IDC_AUDIO_CODEC,			_T("50"));
	ADD_STRING(IDC_AUDIO_CODEC,			_T("1"));

	// audio VBR/CBR
	ADD_STRING(IDC_AUDIO_RATECONTROL,	_T("0"));
	ADD_STRING(IDC_AUDIO_RATECONTROL,	_T("1"));
	
	// audio bitrate
	ADD_STRING(IDC_AUDIO_KBITRATE,		_T("32"));
	ADD_STRING(IDC_AUDIO_KBITRATE,		_T("48"));
	ADD_STRING(IDC_AUDIO_KBITRATE,		_T("56"));
	ADD_STRING(IDC_AUDIO_KBITRATE,		_T("64"));
	ADD_STRING(IDC_AUDIO_KBITRATE,		_T("80"));
	ADD_STRING(IDC_AUDIO_KBITRATE,		_T("96"));
	ADD_STRING(IDC_AUDIO_KBITRATE,		_T("112"));
	ADD_STRING(IDC_AUDIO_KBITRATE,		_T("128"));
	ADD_STRING(IDC_AUDIO_KBITRATE,		_T("160"));
	ADD_STRING(IDC_AUDIO_KBITRATE,		_T("192"));
	ADD_STRING(IDC_AUDIO_KBITRATE,		_T("224"));
	ADD_STRING(IDC_AUDIO_KBITRATE,		_T("256"));
	ADD_STRING(IDC_AUDIO_KBITRATE,		_T("320"));
	ADD_STRING(IDC_AUDIO_KBITRATE,		_T("384"));

	// audio quality
	ADD_STRING(IDC_AUDIO_QUALITY,		_T("40"));
	ADD_STRING(IDC_AUDIO_QUALITY,		_T("50"));
	ADD_STRING(IDC_AUDIO_QUALITY,		_T("60"));
	ADD_STRING(IDC_AUDIO_QUALITY,		_T("70"));
	ADD_STRING(IDC_AUDIO_QUALITY,		_T("80"));
	ADD_STRING(IDC_AUDIO_QUALITY,		_T("90"));
	ADD_STRING(IDC_AUDIO_QUALITY,		_T("100"));

	// min fps
	ADD_STRING(IDC_FPS_MIN,				_T("10"));
	ADD_STRING(IDC_FPS_MIN,				_T("15"));
	ADD_STRING(IDC_FPS_MIN,				_T("20"));
	ADD_STRING(IDC_FPS_MIN,				_T("30"));
	ADD_STRING(IDC_FPS_MIN,				_T("60"));

	// max fps
	ADD_STRING(IDC_FPS_MAX,				_T("10"));
	ADD_STRING(IDC_FPS_MAX,				_T("15"));
	ADD_STRING(IDC_FPS_MAX,				_T("20"));
	ADD_STRING(IDC_FPS_MAX,				_T("30"));
	ADD_STRING(IDC_FPS_MAX,				_T("60"));
	
	// thread priorty
	ADD_STRING(IDC_THREAD_PRIORITY,		_T("-1"));
	ADD_STRING(IDC_THREAD_PRIORITY,		_T("0"));
	ADD_STRING(IDC_THREAD_PRIORITY,		_T("1"));
	ADD_STRING(IDC_THREAD_PRIORITY,		_T("2"));

	ADD_STRING(IDC_LOGO_ALPHA,		_T("0"));
	ADD_STRING(IDC_LOGO_ALPHA,		_T("20"));
	ADD_STRING(IDC_LOGO_ALPHA,		_T("40"));
	ADD_STRING(IDC_LOGO_ALPHA,		_T("60"));
	ADD_STRING(IDC_LOGO_ALPHA,		_T("80"));

	ADD_STRING(IDC_LOGO_POSX,		_T("0"));
	ADD_STRING(IDC_LOGO_POSX,		_T("50"));
	ADD_STRING(IDC_LOGO_POSX,		_T("100"));

	ADD_STRING(IDC_LOGO_POSY,		_T("0"));
	ADD_STRING(IDC_LOGO_POSY,		_T("50"));
	ADD_STRING(IDC_LOGO_POSY,		_T("100"));

	ADD_STRING(IDC_LOGO_SIZEW,		_T("0"));
	ADD_STRING(IDC_LOGO_SIZEW,		_T("50"));
	ADD_STRING(IDC_LOGO_SIZEW,		_T("100"));
	ADD_STRING(IDC_LOGO_SIZEW,		_T("200"));
	ADD_STRING(IDC_LOGO_SIZEW,		_T("300"));

	ADD_STRING(IDC_LOGO_SIZEH,		_T("0"));
	ADD_STRING(IDC_LOGO_SIZEH,		_T("50"));
	ADD_STRING(IDC_LOGO_SIZEH,		_T("100"));
	ADD_STRING(IDC_LOGO_SIZEH,		_T("200"));
	ADD_STRING(IDC_LOGO_SIZEH,		_T("300"));

	UpdateData(TRUE);
	return 1;
}


void CCaptureConfig::UpdateData(BOOL bUpdate)
{
	if(bUpdate)
	{
		int i;
		TCHAR buf[MAX_PATH];
		TCHAR fourcc[5] = { (TCHAR)(m_cfg.VideoCodec)&0xff, (TCHAR)(m_cfg.VideoCodec>>8)&0xff, 
							(TCHAR)(m_cfg.VideoCodec>>16)&0xff, (TCHAR)(m_cfg.VideoCodec>>24)&0xff, '\0' };

		for(i = 0 ; i < 3 ; i++)
			Button_SetCheck(GetDlgItem(IDC_MODE_GDI+i), m_cap_mode == i);

		for(i = 0 ; i < 2 ; i++)
			Button_SetCheck(GetDlgItem(IDC_F_AVI+i), m_cfg.FileType == i);

		SET_TEXT_INT	(IDC_VIDEO_SIZEW,		m_cfg.VideoSizeW);
		SET_TEXT_INT	(IDC_VIDEO_SIZEH,		m_cfg.VideoSizeH);
		SET_TEXT_FLOAT	(IDC_VIDEO_FPS,			m_cfg.VideoFPS);

		SET_TEXT		(IDC_VIDEO_CODEC,		fourcc);
		SET_TEXT_INT	(IDC_VIDEO_RATECONTROL,	m_cfg.VideoRateControl);
		SET_TEXT_INT	(IDC_VIDEO_KBITRATE,	m_cfg.VideoKBitrate);
		SET_TEXT_INT	(IDC_VIDEO_QUALITY,		m_cfg.VideoQuality);
		SET_TEXT_INT	(IDC_VIDEO_INTERVAL,	m_cfg.VideoKeyframeInterval);
		SET_TEXT_INT	(IDC_AUDIO_CHANNEL,		m_cfg.AudioChannels);
		SET_TEXT_INT	(IDC_AUDIO_SAMPLERATE,	m_cfg.AudioSampleRate);
		SET_TEXT_INT_16	(IDC_AUDIO_CODEC,		m_cfg.AudioCodec);
		SET_TEXT_INT	(IDC_AUDIO_RATECONTROL,	m_cfg.AudioRateControl);
		SET_TEXT_INT	(IDC_AUDIO_KBITRATE,	m_cfg.AudioKBitrate);
		SET_TEXT_INT	(IDC_AUDIO_QUALITY,		m_cfg.AudioQuality);
		SET_TEXT_INT	(IDC_FPS_MIN,			m_min_fps);
		SET_TEXT_INT	(IDC_FPS_MAX,			m_max_fps);
		SET_TEXT_INT	(IDC_THREAD_PRIORITY,	m_priority);

		SET_TEXT		(IDC_LOGO_PATH,			m_cfg.LogoPath);
		SET_TEXT_INT	(IDC_LOGO_ALPHA,		m_cfg.LogoAlpha);
		SET_TEXT_INT	(IDC_LOGO_POSX,			m_cfg.LogoPosX);
		SET_TEXT_INT	(IDC_LOGO_POSY,			m_cfg.LogoPosY);
		SET_TEXT_INT	(IDC_LOGO_SIZEW,		m_cfg.LogoSizeW);
		SET_TEXT_INT	(IDC_LOGO_SIZEH,		m_cfg.LogoSizeH);

		SET_TEXT_INT	(IDC_CAPTURE_RECT_L,	m_cfg.CaptureRect.left);
		SET_TEXT_INT	(IDC_CAPTURE_RECT_T,	m_cfg.CaptureRect.top);
		SET_TEXT_INT	(IDC_CAPTURE_RECT_R,	m_cfg.CaptureRect.right);
		SET_TEXT_INT	(IDC_CAPTURE_RECT_B,	m_cfg.CaptureRect.bottom);

		SET_CHECK		(IDC_INCLUDE_CURSOR,	m_cfg.IncludeCursor);
		SET_CHECK		(IDC_ADJUST_AUDIOMIXER,	m_cfg.AdjustAudioMixer);

		SET_TEXT		(IDC_TARGET_DIRECTORY,	m_target_directory);
	}
	else
	{
		int i;
		TCHAR buf[MAX_PATH];
		TCHAR fourcc[MAX_PATH];

		for(i = 0 ; i < 3 ; i++)
		{
			if(Button_GetCheck(GetDlgItem(IDC_MODE_GDI+i)))
			{
				m_cap_mode = i;
				break;
			}
		}

		for(i = 0 ; i < 2 ; i++)
		{
			if(Button_GetCheck(GetDlgItem(IDC_F_AVI+i)))
			{
				m_cfg.FileType = i;
				break;
			}
		}

		GET_TEXT_INT	(IDC_VIDEO_SIZEW,		m_cfg.VideoSizeW);
		GET_TEXT_INT	(IDC_VIDEO_SIZEH,		m_cfg.VideoSizeH);
		GET_TEXT_FLOAT	(IDC_VIDEO_FPS,			m_cfg.VideoFPS);
		
		GET_TEXT		(IDC_VIDEO_CODEC,		fourcc); 
		m_cfg.VideoCodec = MAKEFOURCC(fourcc[0], fourcc[1], fourcc[2], fourcc[3]);
		GET_TEXT_INT	(IDC_VIDEO_RATECONTROL,	m_cfg.VideoRateControl);
		GET_TEXT_INT	(IDC_VIDEO_KBITRATE,	m_cfg.VideoKBitrate);
		GET_TEXT_INT	(IDC_VIDEO_QUALITY,		m_cfg.VideoQuality);
		GET_TEXT_INT	(IDC_VIDEO_INTERVAL,	m_cfg.VideoKeyframeInterval);
		GET_TEXT_INT	(IDC_AUDIO_CHANNEL,		m_cfg.AudioChannels);
		GET_TEXT_INT	(IDC_AUDIO_SAMPLERATE,	m_cfg.AudioSampleRate);
		GET_TEXT_INT_16	(IDC_AUDIO_CODEC,		m_cfg.AudioCodec);
		GET_TEXT_INT	(IDC_AUDIO_RATECONTROL,	m_cfg.AudioRateControl);
		GET_TEXT_INT	(IDC_AUDIO_KBITRATE,	m_cfg.AudioKBitrate);
		GET_TEXT_INT	(IDC_AUDIO_QUALITY,		m_cfg.AudioQuality);
		GET_TEXT_INT	(IDC_FPS_MIN,			m_min_fps);
		GET_TEXT_INT	(IDC_FPS_MAX,			m_max_fps);
		GET_TEXT_INT	(IDC_THREAD_PRIORITY,	m_priority);

		GET_TEXT		(IDC_LOGO_PATH,			m_cfg.LogoPath);
		GET_TEXT_INT	(IDC_LOGO_ALPHA,		m_cfg.LogoAlpha);
		GET_TEXT_INT	(IDC_LOGO_POSX,			m_cfg.LogoPosX);
		GET_TEXT_INT	(IDC_LOGO_POSY,			m_cfg.LogoPosY);
		GET_TEXT_INT	(IDC_LOGO_SIZEW,		m_cfg.LogoSizeW);
		GET_TEXT_INT	(IDC_LOGO_SIZEH,		m_cfg.LogoSizeH);

		GET_TEXT_INT	(IDC_CAPTURE_RECT_L,	m_cfg.CaptureRect.left);
		GET_TEXT_INT	(IDC_CAPTURE_RECT_T,	m_cfg.CaptureRect.top);
		GET_TEXT_INT	(IDC_CAPTURE_RECT_R,	m_cfg.CaptureRect.right);
		GET_TEXT_INT	(IDC_CAPTURE_RECT_B,	m_cfg.CaptureRect.bottom);

		GET_CHECK		(IDC_INCLUDE_CURSOR,	m_cfg.IncludeCursor);
		GET_CHECK		(IDC_ADJUST_AUDIOMIXER,	m_cfg.AdjustAudioMixer);

		GET_TEXT		(IDC_TARGET_DIRECTORY,	m_target_directory);
	}
}


LRESULT CCaptureConfig::OnBrowse(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(BROWSEINFO));
    bi.hwndOwner=m_hWnd;		
    bi.pidlRoot=0;					
    bi.pszDisplayName=0;			
    bi.lpszTitle=_T("");	
    bi.lParam=0;					
    bi.ulFlags=
		BIF_RETURNONLYFSDIRS |		
		BIF_STATUSTEXT |			
		BIF_NEWDIALOGSTYLE |		
		0; 

	LPITEMIDLIST lpItemId=::SHBrowseForFolder(&bi); 
	if (lpItemId)
	{
		TCHAR szBuf[MAX_PATH];
		::SHGetPathFromIDList(lpItemId, szBuf);
		::GlobalFree(lpItemId);

		_tcscpy(m_target_directory, szBuf);
		UpdateData(TRUE);
	}

	return 1;
}


// capture rect test
LRESULT CCaptureConfig::OnCaptureRectTest(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	TCHAR buf[MAX_PATH];

	SET_TEXT_INT	(IDC_CAPTURE_RECT_L,	16);
	SET_TEXT_INT	(IDC_CAPTURE_RECT_T,	16);
	SET_TEXT_INT	(IDC_CAPTURE_RECT_R,	480);
	SET_TEXT_INT	(IDC_CAPTURE_RECT_B,	360);

	return 0;
}


LRESULT CCaptureConfig::OnOk(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	UpdateData(FALSE);

	EndDialog(IDOK);
	return 1;
}


LRESULT CCaptureConfig::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return 1;
}


// preset - youtube
LRESULT CCaptureConfig::OnBnClickedBtnPresetYoutube(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_YOUTUBE);
	UpdateData(TRUE);
	return 0;
}


LRESULT CCaptureConfig::OnBnClickedBtnPresetYoutubeHq(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_YOUTUBE_HIGH_QUALITY);
	UpdateData(TRUE);
	return 0;
}

// preset - naver blog
LRESULT CCaptureConfig::OnBnClickedBtnPresetNaverBlog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_NAVER_BLOG);
	UpdateData(TRUE);
	return 0;
}

// preset - video edting
LRESULT CCaptureConfig::OnBnClickedBtnPresetVideoEditing(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_VIDEO_EDITING);
	UpdateData(TRUE);
	return 0;
}

// preset - default
LRESULT CCaptureConfig::OnBnClickedBtnPresetDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_DEFAULT);
	UpdateData(TRUE);
	return 0;
}

// preset - half
LRESULT CCaptureConfig::OnBnClickedBtnPresetHalf(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_HALFSIZE);
	UpdateData(TRUE);
	return 0;
}

// preset - 640x480
LRESULT CCaptureConfig::OnBnClickedBtnPreset640x480(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_640x480);
	UpdateData(TRUE);
	return 0;
}

// preset - 800x600
LRESULT CCaptureConfig::OnBnClickedBtnPreset800x600(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_800x600);
	UpdateData(TRUE);
	return 0;
}

// preset - 320x240
LRESULT CCaptureConfig::OnBnClickedBtnPreset320x240(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_320x240);
	UpdateData(TRUE);
	return 0;
}

// preset - 400x300
LRESULT CCaptureConfig::OnBnClickedBtnPreset400x300(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_400x300);
	UpdateData(TRUE);
	return 0;
}

// preset - 512x384
LRESULT CCaptureConfig::OnBnClickedBtnPreset512x384(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_512x384);
	UpdateData(TRUE);
	return 0;
}

// preset - 576x432
LRESULT CCaptureConfig::OnBnClickedBtnPreset576x432(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_576x432);
	UpdateData(TRUE);
	return 0;
}

// preset - mpeg1
LRESULT CCaptureConfig::OnBnClickedBtnPresetMpeg1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_MPEG1);
	UpdateData(TRUE);
	return 0;
}


// preset - mjpeg
LRESULT CCaptureConfig::OnBnClickedBtnPresetMjpeg(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_MJPEG);
	UpdateData(TRUE);
	return 0;
}

// preset - mjpeg-hq
LRESULT CCaptureConfig::OnBnClickedBtnPresetMjpegHigh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_MJPEG_HIGH_QUALITY);
	UpdateData(TRUE);
	return 0;
}

// preset - mpeg4
LRESULT CCaptureConfig::OnBnClickedBtnPresetMpeg4(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BCapConfigPreset(&m_cfg, BCAP_PRESET_MPEG4);
	UpdateData(TRUE);
	return 0;
}

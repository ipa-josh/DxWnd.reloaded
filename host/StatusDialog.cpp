// StatusDialog.cpp : implementation file
//

#include "stdafx.h"
#include "dxwndhost.h"
#include "StatusDialog.h"

// CStatusDialog dialog

IMPLEMENT_DYNAMIC(CStatusDialog, CDialog)

CStatusDialog::CStatusDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CStatusDialog::IDD, pParent)
{
}

CStatusDialog::~CStatusDialog()
{
}

void CStatusDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStatusDialog, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

#define IDStatusTIMER 1

// CStatusDialog message handlers

void CStatusDialog::OnTimer(UINT_PTR nIDEvent)
{
	int DxStatus;
	int IconId;
	LPCSTR Status;
	char sMsg[1024];
	char sMsgBuf[80+1];
	char DllVersion[21];
	DXWNDSTATUS DxWndStatus;
	extern PRIVATEMAP *pTitles; 
	extern TARGETMAP *pTargets;
	TARGETMAP *Target;
	extern char *GetTSCaption(int);

	CDialog::OnTimer(nIDEvent);
	DxStatus=GetHookStatus(&DxWndStatus);
	switch (DxStatus){
		case DXW_IDLE: IconId=IDI_DXIDLE; Status="DISABLED"; break;
		case DXW_ACTIVE: IconId=IDI_DXWAIT; Status="READY"; break;
		case DXW_RUNNING: IconId=IDI_DXRUN; Status="RUNNING"; break;
		default: IconId=IDI_DXIDLE; Status="???"; break;
	}

	GetDllVersion(DllVersion);
	if(DxStatus==DXW_RUNNING){

		Target=&pTargets[DxWndStatus.TaskIdx];

		sprintf_s(sMsg, 1024, 
			"DxWnd %s\nHook status: %s\n"
			"Running \"%s\"\nScreen = (%dx%d) %dBPP\n"
			"FullScreen = %s\nDX version = %d\n"
			"Logging = %s",	
			DllVersion, Status,
			pTitles[DxWndStatus.TaskIdx].title,
			DxWndStatus.Width, DxWndStatus.Height, DxWndStatus.ColorDepth, 
			DxWndStatus.IsFullScreen ? "Yes":"No", DxWndStatus.DXVersion,
			DxWndStatus.isLogging?"ON":"OFF");
		if(Target->flags2 & (SHOWFPS|SHOWFPSOVERLAY)){
			sprintf(sMsgBuf, "\nFPS = %d", DxWndStatus.FPSCount);   
			strcat(sMsg, sMsgBuf);
		}
		if(Target->flags2 & TIMESTRETCH){
			if(DxWndStatus.TimeShift>=-8 && DxWndStatus.TimeShift<=8){
				sprintf(sMsgBuf, "\nTime speed %s", GetTSCaption(DxWndStatus.TimeShift));
				strcat(sMsg, sMsgBuf);
			}
		}
	}
	else
		sprintf_s(sMsg, 1024, "DxWnd %s\nHook status: %s", DllVersion, Status);

	this->SetDlgItemTextA(IDC_STATUSINFO, sMsg);
}

BOOL CStatusDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetTimer(IDStatusTIMER, 1000, NULL);

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CStatusDialog::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	KillTimer(IDStatusTIMER);
	// stop timer

	CDialog::OnOK();
}

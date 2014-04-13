/*
DXWnd/dxwnd.cpp
DirectX Hook Module
Copyright(C) 2004-2011 SFB7/GHO

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// MainFrm.cpp : CMainFrame defines the class behavior.
//

#include "stdafx.h"
#include "dxwndhost.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame class constructor / destructor
HANDLE GlobalLocker;

CMainFrame::CMainFrame()
{
	// form constructor starts the service thread responsible to mantain a fixed screen settings
	// see above ....
	// duplicate activation check
	GlobalLocker=CreateSemaphore(NULL, 0, 1, "DxWnd LOCKER");
	if(GlobalLocker==NULL){
		MessageBoxEx(0, "CreateSemaphore FAILED.\nExiting.", "Warning", MB_OK | MB_ICONEXCLAMATION, NULL);
		return;
	}
	if(GetLastError()==ERROR_ALREADY_EXISTS){
		MessageBoxEx(0, "DxWnd is already running.\nExiting.", "Warning", MB_OK | MB_ICONEXCLAMATION, NULL);
		exit(1);
	}
}

CMainFrame::~CMainFrame()
{
	char InitPath[MAX_PATH];
	char val[32];
	GetCurrentDirectory(MAX_PATH, InitPath);
	strcat_s(InitPath, sizeof(InitPath), "\\dxwnd.ini");

	// when in icon tray, skip...
	if (!cx || !cy) return;

	// adjust client win coordinates
	RECT rect;
	rect.top = y;
	rect.bottom = y + cy;
	rect.left = x;
	rect.right = x + cx;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, 1);
	x = rect.left;
	y = rect.top;
	cx = rect.right - rect.left;
	cy = rect.bottom - rect.top;

	// save window rect
	sprintf_s(val, sizeof(val), "%i", x);
	WritePrivateProfileString("window", "posx", val, InitPath);
	sprintf_s(val, sizeof(val), "%i", y);
	WritePrivateProfileString("window", "posy", val, InitPath);
	sprintf_s(val, sizeof(val), "%i", cx);
	WritePrivateProfileString("window", "sizx", val, InitPath);
	sprintf_s(val, sizeof(val), "%i", cy);
	WritePrivateProfileString("window", "sizy", val, InitPath);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	char InitPath[MAX_PATH];
	RECT DesktopRect;
	GetCurrentDirectory(MAX_PATH, InitPath);
	strcat_s(InitPath, sizeof(InitPath), "\\dxwnd.ini");
	cs.x = GetPrivateProfileInt("window", "posx", 50, InitPath);
	cs.y = GetPrivateProfileInt("window", "posy", 50, InitPath);
	cs.cx = GetPrivateProfileInt("window", "sizx", 320, InitPath);
	cs.cy = GetPrivateProfileInt("window", "sizy", 200, InitPath);

	// keep window inside desktop boundaries
	::GetWindowRect(::GetDesktopWindow(), &DesktopRect);
	if(cs.cx < 320) cs.cx = 320;
	if(cs.cy < 200) cs.cy = 200;
	if(cs.x < DesktopRect.left) cs.x = DesktopRect.left;
	if(cs.y < DesktopRect.top) cs.y = DesktopRect.top;
	if(cs.x+cs.cx > DesktopRect.right) cs.x = DesktopRect.right - cs.cx;
	if(cs.y+cs.cy > DesktopRect.bottom) cs.y = DesktopRect.bottom - cs.cy;

	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}

void CMainFrame::OnMove(int x, int y)
{
	CFrameWnd::OnMove(x, y);
	this->x=x;
	this->y=y;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	this->cx=cx;
	this->cy=cy;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Diagnostic Class

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Message Handler


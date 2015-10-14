// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "sMIShop.h"
#include "MainFrm.h"
//#include "ChildFrm.h"
//#include "sMIShopDoc.h"
#include "sMIShopView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	ON_COMMAND(ID_FILE_PRINT, &CChildFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CChildFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CChildFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CChildFrame::OnUpdateFilePrintPreview)
	ON_WM_SETFOCUS()
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()

// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs

	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame message handlers

void CChildFrame::OnFilePrint()
{
	if (m_dockManager.IsPrintPreviewValid())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CChildFrame::OnFilePrintPreview()
{
	if (m_dockManager.IsPrintPreviewValid())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // force Print Preview mode closed
	}
}

void CChildFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_dockManager.IsPrintPreviewValid());
}


void CChildFrame::ActivateFrame(int nCmdShow)
{
	// TODO: Add your specialized code here and/or call the base class

	//MessageBox(_T("on"));
	//nCmdShow = SW_SHOWNORMAL;
	CMDIChildWndEx::ActivateFrame(nCmdShow);
}


void CChildFrame::OnSetFocus(CWnd* pOldWnd)
{
	CMDIChildWndEx::OnSetFocus(pOldWnd);
	//m_bInFocusChildFrm = false;
		//pOldWnd->
	//CMainFrame* pFrmae = (CMainFrame*)AfxGetMainWnd();
	//CsMIShopDoc* pDoc = (CsMIShopDoc*)pFrmae->GetActiveDocument();
	//CsMIShopView* pView = (CsMIShopView*)pFrmae->GetActiveView();
	//pDoc->m
	//pView->bUpdateSlice = true;
	//pView->OnDraw(pView->GetDC());
	//MessageBox(_T("test"));
	//pFrmae->m_wndNaviView.OnPaint();
	//pFrmae->m_wndNaviView.Invalidate(false);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	CsMIShopView* pView = (CsMIShopView*)GetActiveView();
	pView->bUpdateSlice = true;

}

void CChildFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	
	CsMIShopView* pView = (CsMIShopView*)GetActiveView();
	pView->bUpdateSlice = true;
	pView->OnDraw(pView->GetDC());

	CMDIChildWndEx::OnNcLButtonDown(nHitTest, point);
}

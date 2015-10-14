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

#include "stdafx.h"
#include "ViewTree.h"
#include "sMIShopDoc.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "sMIShop.h"
#include "afxwinappex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CViewTree::OnNMDblclk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree message handlers

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}


void CViewTree::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CsMIShopDoc* pDoc = (CsMIShopDoc*)pFrame->GetActiveDocument();
	CsMIShopApp* pApp = (CsMIShopApp*)AfxGetApp();
	
	

	HTREEITEM temp = this->GetSelectedItem();
	CString str_filename =_T("\\") + this->GetItemText(temp);
	CString path = pFrame->m_wndFileView.m_str_path + str_filename;
	LPCTSTR Iplstr = path;

	pApp->m_pDocManager->OpenDocumentFile(Iplstr);


	//pFrame->CreateDocumentWindow

//	pApp->OpenDocumentFile(Iplstr);
	//pDoc->OnNewDocument();
	//pDoc->m_pVoxel;
	*pResult = 0;
}

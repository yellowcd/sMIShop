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

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "FileView.h"
//#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "CalendarBar.h"
#include "Resource.h"
#include "Navigator.h"
#include "MyShellTreeCtrl.h"
#include "ToolBox.h"
#include "ChildFrm.h"

class COutlookBar : public CMFCOutlookBar
{
	virtual BOOL AllowShowOnPaneMenu() const { return TRUE; }
	virtual void GetPaneName(CString& strName) const { BOOL bNameValid = strName.LoadString(IDS_OUTLOOKBAR); ASSERT(bNameValid); if (!bNameValid) strName.Empty(); }
};

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	
	//CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;
	
	
	//CCalendarBar      m_wndCalendar;
	CMFCCaptionBar    m_wndCaptionBar;
	CScrollBar m_FileScrollBar;
	

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CMyShellTreeCtrl& tree, /*CCalendarBar& calendar, */int nInitialWidth);
	BOOL CreateCaptionBar();
	void ViewPropertyWnd();

	int FindFocusedOutlookWnd(CMFCOutlookBarTabCtrl** ppOutlookWnd);

	CMFCOutlookBarTabCtrl* FindOutlookParent(CWnd* pWnd);
	CMFCOutlookBarTabCtrl* m_pCurrOutlookWnd;
	CMFCOutlookBarPane*    m_pCurrOutlookPage;

public:
	CNavigator m_wndNaviView;//네비게이터 인스턴스
	afx_msg void OnUpdateViewNaviview(CCmdUI *pCmdUI);
	afx_msg void OnViewNaviview();
	afx_msg void OnFolderView();
	afx_msg void OnFilewView();
	afx_msg void OnUpdateFolderView(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFilewView(CCmdUI *pCmdUI);
	HTREEITEM m_root;
	CMyShellTreeCtrl m_wndTree;//폴더 트리
	COutlookBar       m_wndNavigationBar;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CFileView         m_wndFileView;//파일뷰 인스턴스
	CToolBox m_wndToolBox;//툴박스 인스턴스
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	CChildFrame m_wndChildFrm;
	afx_msg void OnToolboxView();
	afx_msg void OnUpdateToolboxView(CCmdUI *pCmdUI);
};
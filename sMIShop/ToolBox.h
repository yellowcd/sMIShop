#pragma once

#include "MyStaticCtrl.h"
#include "ShellListFilewView.h"
#include "stdafx.h"
#include "sMIShopView.h"

class CMyButton : public CMFCButton
{

public:
	//DECLARE_MESSAGE_MAP()
	//afx_msg void OnBnClicked();
};
class CToolBoxWnd : public CWnd
{

public:
	DECLARE_MESSAGE_MAP()
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	bool m_bSelectedDraw;
	CButton m_Button_Group;
	CButton m_Button_Draw;
	void adjustlayout();
	void FillToolBox();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_Button_Eraze;
	bool m_bSelectedEraze;
	

public:
	CEdit m_Edit_BrSize;
	afx_msg void OnButtonEraze();
	afx_msg void OnButtonDraw();


	//int m_iSelectedWork;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnUpdateEditBrsize(CCmdUI *pCmdUI);
	afx_msg void OnEditBrsize();
};
// CToolBox

class CToolBox : public CDockablePane
{
	DECLARE_DYNAMIC(CToolBox)

public:
	CToolBox();
	virtual ~CToolBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//CMyStaticCtrl m_wndStatic;
	CToolBoxWnd m_wndToolBox;
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//CListCtrl m_wndList;
	//CWnd m_wndBkView;
	//CMyButton m_CLineButton;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLineButton();
	afx_msg void OnUpdateLineButton(CCmdUI *pCmdUI);
	//void FillToolBox();
	void adjustlayout();
	bool m_bSelectedDraw;
	bool m_bSelectedEraze;
	int m_iSelectedWork;
	float m_fBrSize;
	CsMIShopView* pView;
	void SetView(CsMIShopView* pv)	{ pView = pv; }

public:

};



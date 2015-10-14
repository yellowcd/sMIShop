#pragma once

#include "ViewTree.h"
#include "FileView.h"
//#include "splVoxLib/Voxel.h"
#include "sMIShopView.h"
#include "opencv2/opencv.hpp"
#include "stdafx.h"

//class CNavigatorBkLayer : public CWnd
//{
//public:
//
//	DECLARE_MESSAGE_MAP()
//	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
//	afx_msg void OnPaint();
//	afx_msg void OnUpdateExtendedEdit(CCmdUI *pCmdUI);
//};

// CNavigator class
class CNavigator : public CDockablePane
{
	DECLARE_DYNAMIC(CNavigator)

public:
	CNavigator();
	virtual ~CNavigator();

	void SetView(CsMIShopView *pv) {
		m_pView = pv;
	}
protected:
	//CViewTree m_wndNaviView;
	//CImageList m_NaviViewImages;
	//CFileViewToolBar m_wndToolBar;
	CScrollBar m_NaviScrollBar;
	
	

	


	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	void FillNaviView();
	void AdjustLayout();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUpdateExtendedEdit(CCmdUI *pCmdUI);
	afx_msg void OnPaint();

public:
	int m_int_NaviExtendedEdit;
	//CVoxel<short int> *m_pVox;
	
	BITMAPINFO	m_bmi;

	cv::Mat m_Mat_ImageView;
	bool m_bOpenFile;
	CSliderCtrl m_NaviExtendedSlider;
	CEdit m_NaviExtendedEdit;
	CStatic m_NaviExtendedView;
	CSliderCtrl m_ViewSlider;
	CButton m_SelectXYZ;

	afx_msg void OnViewNaviview();
	afx_msg void OnUpdateViewNaviview(CCmdUI *pCmdUI);
	afx_msg void OnExtendedEdit();

	int m_iOldZoomValue;
	bool m_bZoomSlider;
	bool m_bUpdateSlicePos;
	CStatic m_SlicePos;
	int m_iCurPos;
	afx_msg void OnUpdateIdsGetdata(CCmdUI *pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//CNavigatorBkLayer m_wndBkLayer;
	CWnd m_wndBkLayer;
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	CsMIShopView *m_pView;
	CButton m_Button_XYZGroupBox;
	CButton m_Button_Z;
	CButton m_Button_Y;
	CButton m_Button_X;
	afx_msg void OnUpdateRadioZ(CCmdUI *pCmdUI);
	afx_msg void OnRadioZ();
	afx_msg void OnRadioY();
	afx_msg void OnRadioX();
	int m_iXYZSelected;
	int m_iCurSliceZ;
	int m_iCurSliceY;
	int m_iCurSliceX;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

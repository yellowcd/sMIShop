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

// sMIShopView.h : interface of the CsMIShopView class
//

#pragma once

#include "opencv2/opencv.hpp"
#include "sMIShopDoc.h"
//#include "Navigator.h"


class CsMIShopView : public CView
{
protected: // create from serialization only
	CsMIShopView();
	DECLARE_DYNCREATE(CsMIShopView)

// Attributes
public:
	CsMIShopDoc* GetDocument() const;

	// width and height of mr image slice
	int m_slcWidth;
	int m_slcHeight;
	// view window options
	int m_viewRatioX;
	int m_viewRatioY;

	BITMAPINFO	m_bmi; // for erasing the displays
	int m_wndWidth;
	int m_wndHeight;

	uchar *m_viewBuffer;
	cv::Mat m_viewBufHeader;
	int m_nViewBufStep;
	int m_nViewBufSz;

	// view mode - one of Z, Y, X
	VoxViewMode m_mode;
	// indices of view slices for each axis dimension
	int m_iVSliceX;
	int m_iVSliceY;
	int m_iVSliceZ;

	// current ROI for viewing
	cv::Rect m_viewroi;
	// main slice for viewing
	cv::Mat m_slice;
	cv::Mat m_slice_8uc;

	// colors to draw keypoints
	std::vector<cv::Scalar> m_kpt_cols;

	// flag to update buffer
	bool bUpdateBuffer;
	// flag to update slice
	bool bUpdateSlice;



// Operations
public:
	void SetViewBufferStep();
	void SetViewBuffer(bool bnorm=true);
	template <typename TM, typename OP>
	bool HighlightMaskZ(CVoxel<TM> *pMask, OP op, cv::Scalar cm, cv::Scalar cnm);
	template <typename TM, typename OP>
	bool HighlightMaskY(CVoxel<TM> *pMask, OP op, cv::Scalar cm, cv::Scalar cnm);
	template <typename TM, typename OP>
	bool HighlightMaskX(CVoxel<TM> *pMask, OP op, cv::Scalar cm, cv::Scalar cnm);
	template <typename T>
	bool GetSliceX(CVoxel<T> *pVoxel);
	template <typename T>
	bool GetSliceY(CVoxel<T> *pVoxel);
	template <typename T>
	bool GetSliceZ(CVoxel<T> *pVoxel);
	template <typename T>
	bool GetSlice(CVoxel<T> *pVoxel);
	void IncSlice(int intv);			
	int GetCurrSlice();
	void MakeKptColors(int ncol);
	inline void UpdateBuffer() {bUpdateBuffer = true;}

	//bool SetWindowSize
// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CsMIShopView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public:
	//�׺������ ������Ʈ
	bool bUpdateBufferNavigator;
	//zool ����
	float m_fRatio;
	//���۵���
	bool m_bOnWheel;
	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	bool m_bLButtonDown;
	bool m_bRButtonDown;
	//ȭ�� �̵� ����
	CPoint m_curPos;
	CPoint m_curTrans;

	
	bool m_bROIOverflow;
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

	//ȭ�� ũ��
	int m_nZ;
	int m_nY;
	int m_nX;

	//���õ� tool
	//bool m_bSelectedDraw;
	//bool m_bSelectedEraze;
	int m_iSelectedWork;

	//���� Ŀ����� �������
	HCURSOR m_OldCursor;
	
	//Select Tool
	bool m_bClickedLButtonDraw;
	bool m_bClickedLButtonEraze;

	//Draw point
	CPoint m_point_CurDraw;

	//Draw�Ǵ� ���̾�
	cv::Mat m_CurDrawLayer;
	
	//�귯�� ������
	float m_fBrSize;

	//���� �Ǵ� Ŀ����ġ
	CPoint m_point_WaitCursor;

	//�귯�� ������ ����
	bool m_bChangedBrSize;

	//�귯�� ������ ���� �� ����
	CPoint m_point_ChangedBrSizeValue;
	
	//ȭ���� �Ѿ�� ���
	bool m_bOverside;
	
	//Windowing
	int m_iImageLevel;
	int m_iImageWidth;

	//Intensity �ʱ�ȭ
	bool m_bInitializeIntensity;
	//��� ����
	bool m_bSetIntensity;

	//�⺻ ��⼳��
	int m_iStandardIntensityValue;
	//��� ����
	int m_iChangedIntensityValue;
	
	HACCEL m_hAccel;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#ifndef _DEBUG  // debug version in sMIShopView.cpp
inline CsMIShopDoc* CsMIShopView::GetDocument() const
   { return reinterpret_cast<CsMIShopDoc*>(m_pDocument); }
#endif


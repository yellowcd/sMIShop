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

// sMIShopView.cpp : implementation of the CsMIShopView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "sMIShop.h"
#endif

#include "sMIShopDoc.h"
#include "sMIShopView.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "splVoxLib\Voxel.h"
#include "splVoxLib\Util.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CsMIShopView

IMPLEMENT_DYNCREATE(CsMIShopView, CView)

BEGIN_MESSAGE_MAP(CsMIShopView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_ACTIVATE()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// CsMIShopView construction/destruction

CsMIShopView::CsMIShopView() :
m_nViewBufSz(0), m_mode(ZV),
m_iVSliceX(0), m_iVSliceY(0), m_iVSliceZ(0),
m_viewroi(cv::Rect(0, 0, 0, 0)), m_viewBuffer(NULL),
m_viewRatioX(2), m_viewRatioY(2),
bUpdateBufferNavigator(false),
m_fRatio(1), m_bOnWheel(false),
m_bLButtonDown(false), m_curPos(NULL), m_bROIOverflow(false), bUpdateSlice(true),
/*m_bSelectedDraw(false),*/ m_bClickedLButtonDraw(false), /*m_bSelectedEraze(false),*/ m_fBrSize(10), m_bClickedLButtonEraze(false),
m_bOverside(false),
m_bSetIntensity(true), m_bInitializeIntensity(true), m_iChangedIntensityValue(0), m_iSelectedWork(0), m_bRButtonDown(false)
{
	// TODO: add construction code here
	

}

CsMIShopView::~CsMIShopView()
{
	SAFE_DELETE_ARRAY(m_viewBuffer);
}

BOOL CsMIShopView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CsMIShopView drawing

void CsMIShopView::OnDraw(CDC* pDC)
{
	CsMIShopDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc || !pDoc->m_pVoxel)
		return;

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();

	// TODO: add draw code for native data here
	// construct view buffer
	if ( bUpdateSlice ) {
		GetSlice(pDoc->m_pVoxel);
		bUpdateSlice = false;
		bUpdateBuffer = true;

		m_bSetIntensity = true;
		
	}

	CRect rect;
	this->GetClientRect(&rect);
	
	if ( rect.Width() != m_wndWidth || rect.Height() != m_wndHeight ) {
		m_wndWidth = rect.Width();
		m_wndHeight = rect.Height();

		m_bmi.bmiHeader.biWidth = rect.Width();
		m_bmi.bmiHeader.biHeight = -rect.Height();
		m_bmi.bmiHeader.biSizeImage = m_nViewBufSz;

		bUpdateBuffer = true;
	}
	if ( bUpdateBuffer ) {
		SetViewBuffer();
		//cv::imwrite("temp_slice.png", m_viewBufHeader);
		bUpdateBuffer = false;
	}
	
	//zoom이 실행될때
	if (m_bOnWheel || pFrame->m_wndNaviView.m_bZoomSlider)
	{
		m_bOnWheel = false;
		pFrame->m_wndNaviView.m_bZoomSlider = false;
		CString Zoom_Edit;
		Zoom_Edit.Format(_T("%d"), (int)(m_fRatio* 100 + 0.5f));
		pFrame->m_wndNaviView.m_NaviExtendedEdit.SetWindowTextW(Zoom_Edit);
		SetViewBuffer();
	}
		

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bitMap, *pOldBitmap;
	bitMap.CreateCompatibleBitmap(pDC, m_wndWidth, m_wndHeight);
	pOldBitmap = (CBitmap*)memDC.SelectObject(&bitMap);
	memDC.SelectObject(&bitMap);

	SetDIBitsToDevice(memDC.GetSafeHdc(),
		0, 0,
		m_wndWidth, m_wndHeight,
		0, 0,
		0, m_wndHeight,
		m_viewBufHeader.data, &m_bmi,
		DIB_RGB_COLORS); // send DIB to DC


	pDC->SetStretchBltMode(COLORONCOLOR);
	pDC->BitBlt(0, 0, m_wndWidth, m_wndHeight, &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBitmap);
}
template <typename T>
bool CsMIShopView::GetSliceX(CVoxel<T> *pVoxel)
{
	// check current slice
	if( m_iVSliceX < 0 || m_iVSliceX >= pVoxel->m_nX )
		return false;
	m_slice = pVoxel->VoxelXSlice2cvMat<T>(m_iVSliceX);
	CsMIShopDoc* pDoc = GetDocument();
	m_CurDrawLayer = pDoc->m_pDrawLayerX[m_iVSliceX];
	return true;
}
template <typename T>
bool CsMIShopView::GetSliceY(CVoxel<T> *pVoxel)
{
	// check current slice
	if( m_iVSliceY < 0 || m_iVSliceY >= pVoxel->m_nY )
		return false;
	m_slice = pVoxel->VoxelYSlice2cvMat<T>(m_iVSliceY);
	CsMIShopDoc* pDoc = GetDocument();
	m_CurDrawLayer = pDoc->m_pDrawLayerY[m_iVSliceY];
	return true;
}
template <typename T>
bool CsMIShopView::GetSliceZ(CVoxel<T> *pVoxel)
{
	// check current slice
	if( m_iVSliceZ < 0 || m_iVSliceZ >= pVoxel->m_nZ )
		return false;
	m_slice = pVoxel->VoxelZSlice2cvMat<T>(m_iVSliceZ);
	CsMIShopDoc* pDoc = GetDocument();
	m_CurDrawLayer = pDoc->m_pDrawLayerZ[m_iVSliceZ];
	return true;
}
template <typename T>
bool CsMIShopView::GetSlice(CVoxel<T> *pVoxel)
{
	if ( pVoxel ) {
		switch( m_mode )
		{
		case ZV:
			GetSliceZ(pVoxel);
			break;
		case YV:
			GetSliceY(pVoxel);
			break;
		case XV:
			GetSliceX(pVoxel);
			break;
		default :
			return false;
		}
		m_viewroi = cv::Rect(0, 0, m_slice.cols, m_slice.rows);
		// make drawable slice
		m_slice.convertTo(m_slice_8uc, CV_8U);
		cv::cvtColor(m_slice_8uc, m_slice_8uc, CV_GRAY2BGR);
		return true;
	}
	return false;
}

void CsMIShopView::SetViewBuffer(bool bnorm)
{
	bUpdateBufferNavigator = true;
	SetViewBufferStep();
	int nViewBufSz = m_nViewBufStep*m_wndHeight;
	if (nViewBufSz > m_nViewBufSz) {
		SAFE_DELETE_ARRAY(m_viewBuffer);
		m_nViewBufSz *= 2;
		m_viewBuffer = new uchar[m_nViewBufSz];
	}
	m_viewBufHeader = cv::Mat(m_wndHeight, m_wndWidth, CV_8UC3, m_viewBuffer, m_nViewBufStep);

	// draw keypoints
	CsMIShopDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc || !pDoc->m_pVoxel)
		return;
	if (m_mode == ZV && pDoc->m_kptTracks.size() == pDoc->m_pVoxel->m_nZ) {
		//DrawPoints2cvMat(m_slice_8uc, pDoc->m_kptTracks[m_iVSliceZ], 1, cv::Scalar(0,255,0), 1);
		DrawPoints2cvMat(m_slice_8uc, pDoc->m_kptTracks[m_iVSliceZ], 1, m_kpt_cols, 1);
	}

	m_nZ = pDoc->m_pVoxel->m_nZ;
	m_nY = pDoc->m_pVoxel->m_nY;
	m_nX = pDoc->m_pVoxel->m_nX;

	
	// view가 넘어가지 않도록 제한
	if ((m_slice.cols - m_slice.cols / m_fRatio) / 2 + m_curPos.x >= 0 && (m_slice.rows - m_slice.rows / m_fRatio) / 2 + m_curPos.y >= 0
		&& (m_slice.cols - m_slice.cols / m_fRatio) / 2 + m_curPos.x + m_slice.cols / m_fRatio <= m_slice.cols
		&& (m_slice.rows - m_slice.rows / m_fRatio) / 2 + m_curPos.y + m_slice.rows / m_fRatio <= m_slice.rows)
	{
		m_viewroi = cv::Rect((m_slice.cols - m_slice.cols / m_fRatio) / 2 + m_curPos.x,
			(m_slice.rows - m_slice.rows / m_fRatio) / 2 + m_curPos.y,
			m_slice.cols / m_fRatio,
			m_slice.rows / m_fRatio);
	}
	else
	{
		if ((m_slice.rows - m_slice.rows / m_fRatio) / 2 + m_curPos.y < 0)
			m_curPos.y = -(m_slice.rows - m_slice.rows / m_fRatio) / 2;
		else if ((m_slice.cols - m_slice.cols / m_fRatio) / 2 + m_curPos.x < 0)
			m_curPos.x = -(m_slice.cols - m_slice.cols / m_fRatio) / 2;
		else if ((m_slice.cols - m_slice.cols / m_fRatio) / 2 + m_curPos.x + m_slice.cols / m_fRatio > m_slice.cols)
			m_curPos.x = (m_slice.cols - m_slice.cols / m_fRatio) / 2;
		else if ((m_slice.rows - m_slice.rows / m_fRatio) / 2 + m_curPos.y + m_slice.rows / m_fRatio > m_slice.rows)
			m_curPos.y = (m_slice.rows - m_slice.rows / m_fRatio) / 2;
	//	m_bOverside = true;
	}

	CRect rc;

	GetClientRect(&rc);

	float ratio_w = rc.Width()/(float)m_nX;
	float ratio_h = rc.Height() / (float)m_nY;
	
	
	 // Drawing
	if (m_bClickedLButtonDraw)
	{
		cv::circle(m_CurDrawLayer, cv::Point((int)(m_viewroi.x + (m_point_CurDraw.x * (m_CurDrawLayer.cols / (float)m_wndWidth) / m_fRatio))
			, (int)(m_viewroi.y + (m_point_CurDraw.y * (m_CurDrawLayer.rows / (float)m_wndHeight) / m_fRatio)))
			, m_fBrSize * (m_CurDrawLayer.rows / (float)m_wndHeight) / m_fRatio, cv::Scalar(255), -1, CV_AA);


		//cv::imshow("m_CurDrawLayer", m_CurDrawLayer);
		//m_bChangedDraw = true;

	}
	/// 지울때
	else if (m_bClickedLButtonEraze)
	{
		cv::circle(m_CurDrawLayer, cv::Point((int)(m_viewroi.x + (m_point_CurDraw.x * (m_CurDrawLayer.cols / (float)m_wndWidth) / m_fRatio))
			, (int)(m_viewroi.y + (m_point_CurDraw.y * (m_CurDrawLayer.rows / (float)m_wndHeight) / m_fRatio)))
			, m_fBrSize * (m_CurDrawLayer.rows / (float)m_wndHeight) / m_fRatio, cv::Scalar(NULL), -1, CV_AA);

		//m_bChangedDraw = true;

	}


	//Initialize image Windowing 
	if (m_bInitializeIntensity)
	{
		//m_iChangedIntensityValue
		m_bInitializeIntensity = false;
		
		m_iImageLevel = (pDoc->m_iMaxValue - pDoc->m_iMinValue)/2;
		m_iImageWidth = pDoc->m_iMaxValue - pDoc->m_iMinValue;
		m_iStandardIntensityValue = m_iImageLevel / (float)m_iImageWidth * 256.0f;
		m_iChangedIntensityValue = m_iImageLevel / (float)m_iImageWidth * 256.0f;
	}

	int iMin = (m_iImageLevel - m_iImageWidth/2.0f);
	int iMax = (m_iImageLevel + m_iImageWidth/2.0f);
	float fIntensityRange = 255.0f / (iMax - iMin);

	//set windowing
	for (int y = 0; y < m_CurDrawLayer.rows; y++)
	for (int x = 0; x < m_CurDrawLayer.cols; x++)
	{
		if (m_bSetIntensity){
			
			if ( m_slice_8uc.at<uchar>(y, x * 3 + 0)  > iMax)
			{
				m_slice_8uc.at<uchar>(y, x * 3 + 0) = 255;
				m_slice_8uc.at<uchar>(y, x * 3 + 1) = 255;
				m_slice_8uc.at<uchar>(y, x * 3 + 2) = 255;
			}
			else if (m_slice_8uc.at<uchar>(y, x * 3 + 0)  < iMin)
			{
				m_slice_8uc.at<uchar>(y, x * 3 + 0) = 0;
				m_slice_8uc.at<uchar>(y, x * 3 + 1) = 0;
				m_slice_8uc.at<uchar>(y, x * 3 + 2) = 0;
			}
			else
			{
				m_slice_8uc.at<uchar>(y, x * 3 + 0) = Round((m_slice_8uc.at<uchar>(y, x * 3 + 0) - iMin)* fIntensityRange);
				m_slice_8uc.at<uchar>(y, x * 3 + 1) = Round((m_slice_8uc.at<uchar>(y, x * 3 + 1) - iMin)* fIntensityRange);
				m_slice_8uc.at<uchar>(y, x * 3 + 2) = Round((m_slice_8uc.at<uchar>(y, x * 3 + 2) - iMin)* fIntensityRange);
			}
		}
		
		if (m_CurDrawLayer.at<uchar>(y, x) == 255)
		{
			m_slice_8uc.at<uchar>(y, x * 3 + 0) = 0;
			m_slice_8uc.at<uchar>(y, x * 3 + 1) = 0;
			m_slice_8uc.at<uchar>(y, x * 3 + 2) = 255;
		}

		

	}

	m_bSetIntensity = false;

	// crop view ROI
	cv::Mat viewroi(m_slice_8uc, m_viewroi);

	// resize view ROI to fit current window
	cv::resize(viewroi, m_viewBufHeader, cv::Size(m_wndWidth, m_wndHeight),0,0, CV_INTER_NN);

	
	//View인스턴스 셋팅
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	pFrame->m_wndNaviView.SetView(this);
	pFrame->m_wndNaviView.OnPaint();
	pFrame->m_wndToolBox.SetView(this);

}

template <typename TM, typename OP>
bool CsMIShopView::HighlightMaskZ(CVoxel<TM> *pMask, OP op, cv::Scalar cm, cv::Scalar cnm)
{

}
template <typename TM, typename OP>
bool CsMIShopView::HighlightMaskY(CVoxel<TM> *pMask, OP op, cv::Scalar cm, cv::Scalar cnm)
{

}
template <typename TM, typename OP>
bool CsMIShopView::HighlightMaskX(CVoxel<TM> *pMask, OP op, cv::Scalar cm, cv::Scalar cnm)
{

}


// CsMIShopView printing


void CsMIShopView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CsMIShopView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CsMIShopView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CsMIShopView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CsMIShopView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CsMIShopView diagnostics

#ifdef _DEBUG
void CsMIShopView::AssertValid() const
{
	CView::AssertValid();
}

void CsMIShopView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CsMIShopDoc* CsMIShopView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CsMIShopDoc)));
	return (CsMIShopDoc*)m_pDocument;
}
#endif //_DEBUG


// CsMIShopView message handlers
void CsMIShopView::SetViewBufferStep()
{
	m_nViewBufStep = (m_wndWidth*3+3) & -4;
	//m_nViewBufStep = m_wndWidth*3;
}

void CsMIShopView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CsMIShopDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if ( pDoc->m_pVoxel ) {
		CChildFrame* pChFrm = (CChildFrame*)GetParentFrame();
		pChFrm->SetWindowPos(&wndTop, 
			1, 1, pDoc->m_pVoxel->m_nX*m_viewRatioX, pDoc->m_pVoxel->m_nY*m_viewRatioX,
			0);
		if ( pDoc->m_pVoxel->m_nZ > 1 ) {
			pChFrm->EnableScrollBar(SB_VERT, ESB_ENABLE_BOTH);
			pChFrm->ShowScrollBar(SB_VERT, TRUE);
		}
	}

	CRect rect;

	this->GetClientRect(rect);
	m_wndWidth = rect.Width();
	m_wndHeight = rect.Height();
	SetViewBufferStep();
	m_nViewBufSz = m_nViewBufStep*m_wndHeight;
	m_viewBuffer = new uchar[m_nViewBufSz];

	m_bmi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	m_bmi.bmiHeader.biWidth = m_wndWidth;
	m_bmi.bmiHeader.biHeight = -m_wndHeight;
	m_bmi.bmiHeader.biPlanes = 1;
	m_bmi.bmiHeader.biBitCount = 24;
	m_bmi.bmiHeader.biCompression = BI_RGB;
	m_bmi.bmiHeader.biSizeImage = m_nViewBufSz;
	m_bmi.bmiHeader.biXPelsPerMeter=0;
	m_bmi.bmiHeader.biYPelsPerMeter=0;
	m_bmi.bmiHeader.biClrUsed =0;
	m_bmi.bmiHeader.biClrImportant =0;

	// initialize random processes
	srand(NULL);

	// 네비게이터 view 인스턴스 초기화
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	pFrame->m_wndNaviView.SetView(this);
	pFrame->m_wndNaviView.m_bOpenFile = true;

	m_hAccel = LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
	
}


BOOL CsMIShopView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	// 휠을 통한 zoom
	if ( (nFlags &MK_CONTROL) && (nFlags &WM_MOUSEWHEEL) )
	{
		if ((zDelta < 0) & (m_fRatio > 1.0f))
		{
			m_fRatio -= 1 / 10.0f;
		}
		else if ((zDelta > 0) & (m_fRatio < 8.0f))
		{
			m_fRatio += 1/10.0f;
		}

		m_bOnWheel = true;

		//OnInitialUpdate();
		OnDraw(this->GetDC());
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CsMIShopView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CsMIShopView::IncSlice(int intv)
{
	CsMIShopDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc || !pDoc->m_pVoxel)
		return;

	switch( m_mode ) {
	case ZV:
		m_iVSliceZ = max(min(m_iVSliceZ+intv, pDoc->m_pVoxel->m_nZ-1), 0);
		break;
	case YV:
		m_iVSliceY = max(min(m_iVSliceY+intv, pDoc->m_pVoxel->m_nY-1), 0);
		break;
	case XV:
		m_iVSliceX = max(min(m_iVSliceX+intv, pDoc->m_pVoxel->m_nX-1), 0);
		break;
	}
	bUpdateSlice = true;
	Invalidate(FALSE);
	
}

int CsMIShopView::GetCurrSlice()
{
	switch( m_mode ) {
	case ZV:
		return m_iVSliceZ;
	case YV:
		return m_iVSliceY;
	case XV:
		return m_iVSliceX;
	default:
		return 0;
	}
}

void CsMIShopView::MakeKptColors(int ncol)
{
	m_kpt_cols.clear();
	for (int i = 0; i < ncol; i++ ) {
		double r = (double)rand()/(double)RAND_MAX*255.0;
		double g = (double)rand()/(double)RAND_MAX*255.0;
		double b = (double)rand()/(double)RAND_MAX*255.0;
		m_kpt_cols.push_back(cv::Scalar(b, g, r));
	}
}

void CsMIShopView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CsMIShopDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc || !pDoc->m_pVoxel)
		return;

	switch ( nChar ) {
		case VK_UP:	{
			IncSlice(-1);
			//MessageBox(_T("OnKeyDown"));
		}
		break;
		case VK_DOWN:{
			IncSlice(1);
		}
		break;
		case 'c': case 'C':{
			// construct keypoint tracks
			int nkpts = pDoc->InitKPtTrackCandidates(m_iVSliceZ);
			// construct random colors to draw each keypoint
			MakeKptColors(nkpts);
			UpdateBuffer();
			Invalidate();
		}
		break;
		/*
		case 'c': case 'C':{
			int a= 0;
			break;
		}
		case 'c': case 'C':{
			int a= 0;
		}
		break;
		*/
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

//void CsMIShopView::SetupDisplayBuffer2()
//{
//	CsMIShopDoc *pDoc = GetDocument();
//
//	CVoxel<short> *pVoxel = pDoc->m_pVoxel;
//	
//	int nX = pVoxel->m_nX;
//	int nY = pVoxel->m_nY;
//	int nZ = pVoxel->m_nZ;
//	int w = m_wndWidth;
//	int h = m_wndHeight;
//	int i, j, x, y, z, idx;
//	BYTE br, bg, bb, iV;
//	//double iRange = 255.0 / (double)(m_nMaxI - m_nMinI);
//	//double iRange = 1;
//
//		// check current slice
//	if (m_iVSliceZ < 0 || m_iVSliceZ >= nZ)
//		return;
//	//z = m_iVSliceZ;
//	// m_iRatio: 1.0 if 1 pixel for 1 voxel
//	//				  2.0 if 2 pixels for 1 voxel
//
//	cv::Mat cvtVoxel;
//	cvtVoxel.create(m_wndHeight, m_wndWidth, CV_8UC3);
//
//	for (j = 0; j < h; j++) {
//		
//		/*if (m_b_zoom)
//		y = (int)Round((double)(j + m_curPos.y + m_wndWidth / 2) / m_iRatio);
//		else*/
//		y = (int)Round((double)(j ) / m_fRatio);
//		for (i = 0; i < w; i++) {
//			idx = 3 * (j*w + i);
//			/*if (m_b_zoom)
//			x = (int)Round((double)(i + m_curPos.x + m_wndHeight / 2) / m_iRatio);
//			else*/
//			x = (int)Round((double)(i ) / m_fRatio);
//			if (0 <= y && y < nY && 0 <= x && x < nX) {
//
//				if (pVoxel->GetAt(x, y, z) < 0) {
//					iV = 0;
//				}
//				else if (pVoxel->GetAt(x, y, z) > 255) {
//					iV = 255;
//				}
//				else {
//					iV = (BYTE)Round((double)(pVoxel->GetAt(x, y, z) ));
//				}
//
//				br = bg = bb = iV;
//			}
//
//			cvtVoxel.at<uchar>(y,x*3+2) = br;
//			cvtVoxel.at<uchar>(y, x * 3 + 1) = bg;
//			cvtVoxel.at<uchar>(y, x * 3 + 0) = bb;
//		}
//	}
//	
//	cv::imshow("cvtVoxel",cvtVoxel);
//}

void CsMIShopView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	/*CRect rc;
	this->GetClientRect(rc);*/
	

	if (m_iSelectedWork == 1)
	{
		if (nFlags & MK_CONTROL)
		{
			m_point_ChangedBrSizeValue.y = point.y;
			m_bChangedBrSize = true;
		}
		else if (nFlags & MK_SHIFT)
		{
			bUpdateBuffer = true;

			m_point_CurDraw = point;

			m_bClickedLButtonEraze = true;

			OnDraw(this->GetDC());



			m_OldCursor = GetCursor();
			SetCursor(NULL);
			CDC *pDC = this->GetDC();
			CPen Pen, *pOldPen;
			//pDC->SetROP2(R2_NOP);
			Pen.CreatePen(PS_NONE, 2, RGB(255, 255, 255));
			pOldPen = pDC->SelectObject(&Pen);
			pDC->SelectStockObject(NULL_BRUSH);

			pDC->Ellipse(point.x - m_fBrSize, point.y - m_fBrSize, point.x + m_fBrSize, point.y + m_fBrSize);


			pDC->SelectObject(pOldPen);
			Pen.DeleteObject();


			SetCapture();
			ReleaseDC(pDC);
		}
		else
		{

			bUpdateBuffer = true;

			m_point_CurDraw = point;

			m_bClickedLButtonDraw = true;

			OnDraw(this->GetDC());



			m_OldCursor = GetCursor();
			SetCursor(NULL);
			CDC *pDC = this->GetDC();
			CPen Pen, *pOldPen;
			//pDC->SetROP2(R2_NOP);
			Pen.CreatePen(PS_NONE, 2, RGB(255, 0, 0));
			pOldPen = pDC->SelectObject(&Pen);
			pDC->SelectStockObject(NULL_BRUSH);

			pDC->Ellipse(point.x - m_fBrSize, point.y - m_fBrSize, point.x + m_fBrSize, point.y + m_fBrSize);


			pDC->SelectObject(pOldPen);
			Pen.DeleteObject();

			SetCapture();
			ReleaseDC(pDC);
		}

		
		
	}
	else if (m_iSelectedWork == 2)
	{
		if (nFlags & MK_CONTROL)
		{
			m_point_ChangedBrSizeValue.y = point.y;
			m_bChangedBrSize = true;

		}
		else
		{
			bUpdateBuffer = true;

			m_point_CurDraw = point;

			m_bClickedLButtonEraze = true;

			OnDraw(this->GetDC());



			m_OldCursor = GetCursor();
			SetCursor(NULL);
			CDC *pDC = this->GetDC();
			CPen Pen, *pOldPen;
			//pDC->SetROP2(R2_NOP);
			Pen.CreatePen(PS_NONE, 2, RGB(255, 255, 255));
			pOldPen = pDC->SelectObject(&Pen);
			pDC->SelectStockObject(NULL_BRUSH);

			pDC->Ellipse(point.x - m_fBrSize, point.y - m_fBrSize, point.x + m_fBrSize, point.y + m_fBrSize);


			pDC->SelectObject(pOldPen);
			Pen.DeleteObject();


			SetCapture();
			ReleaseDC(pDC);
		}
		
	}
	else if (m_iSelectedWork == 0)
	{
		m_bLButtonDown = true;
		m_curTrans = point;

		//bUpdateBuffer = true;
		//OnDraw(this->GetDC());
	}


	CView::OnLButtonDown(nFlags, point);
}

void CsMIShopView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	//m_bSelectedDraw = pFrame->m_wndToolBox.m_bSelectedDraw;
	//m_bSelectedEraze = pFrame->m_wndToolBox.m_bSelectedEraze;

	CRect rc;
	GetClientRect(&rc);
	
	this->SetFocus();


	
	if (m_iSelectedWork != 0 && !((point.x > rc.left) & (point.y > rc.top) & (point.x < rc.right) & (point.y < rc.bottom)))
	{
		//마우스가 이미지 밖이면 원래대로
		SetCursor(m_OldCursor);
		ReleaseCapture();
		bUpdateBuffer = true;
		OnDraw(this->GetDC());
	}
	else if ((nFlags & MK_CONTROL) && (m_bChangedBrSize))
	{
		//브러쉬 크기 변경
		if (m_point_ChangedBrSizeValue.y != point.y)
		{
			
			if (m_point_ChangedBrSizeValue.y - point.y > 0)
				m_fBrSize += 1.0f;
			else if (m_point_ChangedBrSizeValue.y - point.y < 0)
				m_fBrSize -= 1.0f;
			m_point_ChangedBrSizeValue.y = point.y;

			
		}
			

		if (m_fBrSize < 1)
			m_fBrSize = 1;
		else if (m_fBrSize > 50)
			m_fBrSize = 50;

		m_OldCursor = GetCursor();
		SetCursor(NULL);
		CDC *pDC = this->GetDC();
		CPen Pen, *pOldPen;
		bUpdateBuffer = true;
		OnDraw(this->GetDC());
		if (m_iSelectedWork == 1)
			Pen.CreatePen(PS_NONE, 2, RGB(217, 65, 197));
		else if (m_iSelectedWork == 2)
			Pen.CreatePen(PS_NONE, 2, RGB(255,255,255));
		pOldPen = pDC->SelectObject(&Pen);
		pDC->SelectStockObject(NULL_BRUSH);


		pDC->Ellipse(point.x - m_fBrSize, point.y - m_fBrSize, point.x + m_fBrSize, point.y + m_fBrSize);
		pDC->SelectObject(pOldPen);
		Pen.DeleteObject();

		SetCapture();
		ReleaseDC(pDC);

		CString str;
		str.Format(_T("%d"), (int)m_fBrSize);
		pFrame->m_wndToolBox.m_wndToolBox.m_Edit_BrSize.SetWindowTextW(str);
	}
	else if (m_iSelectedWork == 1 /*&& (point.x > rc.left & point.y > rc.top & point.x < rc.right & point.y < rc.bottom)*/)
	{

		if (m_bClickedLButtonEraze && (nFlags & MK_SHIFT))
		{
			CDC *pDC = this->GetDC();
			CPen Pen, *pOldPen;

			bUpdateSlice = true;
			OnDraw(this->GetDC());
			Pen.CreatePen(PS_NONE, 2, RGB(255, 255, 255));
			pOldPen = pDC->SelectObject(&Pen);
			pDC->SelectStockObject(NULL_BRUSH);


			pDC->Ellipse(point.x - m_fBrSize, point.y - m_fBrSize, point.x + m_fBrSize, point.y + m_fBrSize);
			pDC->SelectObject(pOldPen);
			Pen.DeleteObject();



			ReleaseDC(pDC);

			m_point_CurDraw = point;
		}
		else if (!m_bClickedLButtonEraze && (nFlags & MK_SHIFT))
		{
			m_OldCursor = GetCursor();
			SetCursor(NULL);
			CDC *pDC = this->GetDC();
			CPen Pen, *pOldPen;

			bUpdateBuffer = true;
			OnDraw(this->GetDC());

			Pen.CreatePen(PS_NONE, 2, RGB(255, 255, 255));
			pOldPen = pDC->SelectObject(&Pen);

			pDC->SelectStockObject(NULL_BRUSH);

			pDC->Ellipse(point.x - m_fBrSize, point.y - m_fBrSize, point.x + m_fBrSize, point.y + m_fBrSize);

			pDC->SelectObject(pOldPen);
			Pen.DeleteObject();


			SetCapture();
			ReleaseDC(pDC);
		}
		//Draw 일때 커서변경
		else if (m_bClickedLButtonDraw)
		{
			CDC *pDC = this->GetDC();
			CPen Pen, *pOldPen;

			bUpdateBuffer = true;
			OnDraw(this->GetDC());

			Pen.CreatePen(PS_NONE, 2, RGB(217, 65, 197));
			pOldPen = pDC->SelectObject(&Pen);

			pDC->SelectStockObject(NULL_BRUSH);


			pDC->Ellipse(point.x - m_fBrSize, point.y - m_fBrSize, point.x + m_fBrSize, point.y + m_fBrSize);

			pDC->SelectObject(pOldPen);
			Pen.DeleteObject();

			ReleaseDC(pDC);

			bUpdateBuffer = true;
			m_point_CurDraw = point;
		}
		else
		{
			m_OldCursor = GetCursor();
			SetCursor(NULL);
			CDC *pDC = this->GetDC();
			CPen Pen, *pOldPen;

			bUpdateBuffer = true;
			OnDraw(this->GetDC());

			Pen.CreatePen(PS_NONE, 2, RGB(217, 65, 197));
			pOldPen = pDC->SelectObject(&Pen);

			pDC->SelectStockObject(NULL_BRUSH);

			pDC->Ellipse(point.x - m_fBrSize, point.y - m_fBrSize, point.x + m_fBrSize, point.y + m_fBrSize);

			pDC->SelectObject(pOldPen);
			Pen.DeleteObject();

			SetCapture();
			ReleaseDC(pDC);
		}
	}
	else if (m_iSelectedWork == 2)
	{
		//Eraze일때 커서 변경
		if (m_bClickedLButtonEraze)
		{
			CDC *pDC = this->GetDC();
			CPen Pen, *pOldPen;
			
			bUpdateSlice = true;
			OnDraw(this->GetDC());
			Pen.CreatePen(PS_NONE, 2, RGB(255, 255, 255));
			pOldPen = pDC->SelectObject(&Pen);
			pDC->SelectStockObject(NULL_BRUSH);


			pDC->Ellipse(point.x - m_fBrSize, point.y - m_fBrSize, point.x + m_fBrSize, point.y + m_fBrSize);
			pDC->SelectObject(pOldPen);
			Pen.DeleteObject();



			ReleaseDC(pDC);

			m_point_CurDraw = point;
		}
		else
		{
			m_OldCursor = GetCursor();
			SetCursor(NULL);
			CDC *pDC = this->GetDC();
			CPen Pen, *pOldPen;

			bUpdateBuffer = true;
			OnDraw(this->GetDC());

			Pen.CreatePen(PS_NONE, 2, RGB(255, 255, 255));
			pOldPen = pDC->SelectObject(&Pen);

			pDC->SelectStockObject(NULL_BRUSH);

			pDC->Ellipse(point.x - m_fBrSize, point.y - m_fBrSize, point.x + m_fBrSize, point.y + m_fBrSize);

			pDC->SelectObject(pOldPen);
			Pen.DeleteObject();


			SetCapture();
			ReleaseDC(pDC);
		}

	}
	else if (m_bLButtonDown && m_bRButtonDown)
	{
		//Windowing
		if (m_point_WaitCursor.y != point.y)
		{
			if (m_point_WaitCursor.y > point.y)
				m_iImageLevel -= 1;
			else if(m_point_WaitCursor.y < point.y)
				m_iImageLevel += 1;

			m_point_WaitCursor.y = point.y;
		}
		if (m_point_WaitCursor.x != point.x)
		{
			if (m_point_WaitCursor.x > point.x)
				m_iImageWidth += 1;
			else if (m_point_WaitCursor.x < point.x)
				m_iImageWidth -= 1;

			m_point_WaitCursor.x = point.x;
		}


		m_bSetIntensity = true;
		bUpdateSlice = true;
		OnDraw(this->GetDC());
	}
	else if (m_iSelectedWork == 0 /*&& !m_bROIOverflow*/)
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));

		//화면 이동
		if (m_bLButtonDown)
		{
			if (m_bOverside)
			{
				m_bOverside = false;
			}
			else
			{

				m_curPos.x += (int)((double)(m_curTrans.x - point.x) / m_fRatio);
				m_curPos.y += (int)((double)(m_curTrans.y - point.y) / m_fRatio);
				m_curTrans = point;
				bUpdateBuffer = true;
				OnDraw(this->GetDC());
			}
		}
	}

	

	CView::OnMouseMove(nFlags, point);
}


void CsMIShopView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bLButtonDown = false;
	//m_curPos = NULL;
	m_curTrans = point;
	CView::OnLButtonUp(nFlags, point);
	//CView::OnActivateview();

	if (m_bClickedLButtonDraw)
	{
		m_bClickedLButtonDraw = false;
	}
	else if (m_bClickedLButtonEraze)
	{
		m_bClickedLButtonEraze = false;
	}
	else if (m_bChangedBrSize)
	{
		m_bChangedBrSize = false;
	}
}


void CsMIShopView::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CView::OnActivate(nState, pWndOther, bMinimized);

	//MessageBox(_T("d"));
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}




void CsMIShopView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	if (m_bRButtonDown)
	{
		m_bRButtonDown = false;
	}
	else
	{
		ClientToScreen(&point);
		OnContextMenu(this, point);
	}
	
}

void CsMIShopView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	m_bRButtonDown = true;

	m_point_WaitCursor = point;

	CView::OnRButtonDown(nFlags, point);
}


BOOL CsMIShopView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	HWND hWnd = GetSafeHwnd();
	if (TranslateAccelerator(hWnd, m_hAccel, pMsg))
	{
		if (pMsg->wParam == 'H')
		{
			m_iSelectedWork = 0;
			OnPaint();
		}
		else if (pMsg->wParam == 'D')
		{
			if (m_iSelectedWork == 0)
				m_iSelectedWork = 1;
			else if (m_iSelectedWork == 1)
				m_iSelectedWork = 2;
			else if (m_iSelectedWork == 2)
				m_iSelectedWork = 1;
			OnPaint();
		}
	}

	return CView::PreTranslateMessage(pMsg);
}

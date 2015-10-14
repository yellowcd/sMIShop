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

// sMIShopDoc.cpp : implementation of the CsMIShopDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "sMIShop.h"
#endif

#include "sMIShopDoc.h"

#include <propkey.h>

#include "splVoxLib/Voxel.h"
#include "OpticalFlow/project.h"
#include "OpticalFlow/OpticalFlow.h"
#include "OpticalFlow/GaussianPyramid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


cvGF2TParams::cvGF2TParams() : maxCorners(200), qualityLevel(0.1), minDistance(5)
{
}

// CsMIShopDoc

IMPLEMENT_DYNCREATE(CsMIShopDoc, CDocument)

BEGIN_MESSAGE_MAP(CsMIShopDoc, CDocument)
	ON_COMMAND(ID_BUTTON_MOT_FEAT_DET, &CsMIShopDoc::OnButtonMotionFeatDetection)
	ON_COMMAND(ID_BUTTON_MOT_FEAT_DET_GF2T, &CsMIShopDoc::OnButtonMotFeatDetGF2T)
	ON_COMMAND(ID_BUTTON_MOT_FEAT_DET_SIFT, &CsMIShopDoc::OnButtonMotFeatDetSIFT)
	ON_COMMAND(ID_BUTTON_MOT_FEAT_DET_FAST, &CsMIShopDoc::OnButtonMotFeatDetFAST)
	ON_COMMAND(ID_BUTTON_MOT_FEAT_MATCH, &CsMIShopDoc::OnButtonMotFeatMatch)
END_MESSAGE_MAP()


// CsMIShopDoc construction/destruction

CsMIShopDoc::CsMIShopDoc() : m_pVoxel(NULL)
{
	// TODO: add one-time construction code here

}

CsMIShopDoc::~CsMIShopDoc()
{
	// collect memory
	if ( m_pVoxel ) 
		delete m_pVoxel;
}

BOOL CsMIShopDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CsMIShopDoc serialization

void CsMIShopDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CsMIShopDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CsMIShopDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CsMIShopDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CsMIShopDoc diagnostics

#ifdef _DEBUG
void CsMIShopDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CsMIShopDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CsMIShopDoc commands


BOOL CsMIShopDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// open CVoxel object
	
	m_pVoxel = new CVoxel<short>(lpszPathName);

	m_pDrawLayerZ = new cv::Mat[m_pVoxel->m_nZ];
	m_pDrawLayerY = new cv::Mat[m_pVoxel->m_nY];
	m_pDrawLayerX = new cv::Mat[m_pVoxel->m_nX];

	
	for (int i = 0; i < m_pVoxel->m_nZ; i++)
		m_pDrawLayerZ[i].create(m_pVoxel->m_nY,m_pVoxel->m_nX,CV_8UC1);
	for (int i = 0; i < m_pVoxel->m_nY; i++)
		m_pDrawLayerY[i].create(m_pVoxel->m_nX, m_pVoxel->m_nZ, CV_8UC1);
	for (int i = 0; i < m_pVoxel->m_nX; i++)
		m_pDrawLayerX[i].create(m_pVoxel->m_nZ,m_pVoxel->m_nY,CV_8UC1);

	
	m_iMaxValue = m_pVoxel->m_nMaxI;
	m_iMinValue = m_pVoxel->m_nMinI;

	//for(int z = 0; z < m_pVoxel->m_nZ; z++)
	//for(int y = 0; y < m_pVoxel->m_nY; y++)
	//for(int x = 0; x < m_pVoxel->m_nX; x++)
	//{
	//	if(m_pVoxel->m_pData[z*m_pVoxel->m_nY*m_pVoxel->m_nX + y*m_pVoxel->m_nX + x] > m_iMaxValue)
	//		m_iMaxValue = m_pVoxel->m_pData[z*m_pVoxel->m_nY*m_pVoxel->m_nX + y*m_pVoxel->m_nX + x];
	//	if (m_pVoxel->m_pData[z*m_pVoxel->m_nY*m_pVoxel->m_nX + y*m_pVoxel->m_nX + x] < m_iMinValue)
	//		m_iMinValue = m_pVoxel->m_pData[z*m_pVoxel->m_nY*m_pVoxel->m_nX + y*m_pVoxel->m_nX + x];
	//	
	//}

#if 0
	// TEST 3: to warp with thin-plate spline - layer separation
#endif 

#if 0
	// TEST 2: try to extract bone (which has no motion) 
	int imgw = m_pVoxel->m_nX;
	int imgh = m_pVoxel->m_nY;
	int nfrm = m_pVoxel->m_nZ;
	//		construct average image
	cv::Mat avg = cv::Mat::zeros(imgh, imgw, CV_32F);
	for ( int y = 0; y < m_pVoxel->m_nY; y++ ) {
		float *avgp = avg.ptr<float>(y);
		for ( int x = 0; x < m_pVoxel->m_nX; x++ ) {
			for ( int i = 0; i < m_pVoxel->m_nZ; i++ ) {
				avgp[x] += m_pVoxel->GetAt(x,y,i);
			}
		}
	}
	avg = avg/(float)nfrm;
	//		save average image
	cv::Mat avg_8u;
	avg.convertTo(avg_8u, CV_8U);
	cv::imwrite("avg_img.png", avg_8u);

	//		compute subtraction image for all frames
	for ( int i = 0; i < nfrm; i++ ) {
		cv::Mat frm_idx = m_pVoxel->VoxelZSlice2cvMat<short>(i);
		cv::Mat frm_idx_f;
		frm_idx.convertTo(frm_idx_f, CV_32F);
		cv::Mat frm_sub = cv::abs(frm_idx_f - avg);
		double minf, maxf;
		cv::minMaxIdx(frm_sub, &minf, &maxf);
		cv::Mat frm_sub_8u;
		frm_sub.convertTo(frm_sub_8u, CV_8U, 255.0/(maxf-minf), -minf);
		char path[MAX_PATH];
		sprintf(path, "frm_sub_%04d.png", i);
		cv::imwrite(path, frm_sub_8u);
	}
#endif

	// TEST 1: compare optical flow between consecutive and synchronized frames
	// 20140825 ****** OPTICAL FLOW FAILS TOTALLY FOR 2D+t CARDIAC X-RAY IMAGES ****** // 
#if 0
	//////////////////////////////////////////////////////////
	// 20140820 *** ASSUME VESSEL IMAGE HAS BEEN LOADED *** //
	// set the parameters
	//     para(1)--alpha (1), the regularization weight
	//     para(2)--ratio (0.5), the downsample ratio
	//     para(3)--minWidth (40), the width of the coarsest level
	//     para(4)--nOuterFPIterations (3), the number of outer fixed point iterations
	//     para(5)--nInnerFPIterations (1), the number of inner fixed point iterations
	//     para(6)--nSORIterations (20), the number of SOR iterations
	double alpha= 0.012;
	double ratio=0.75;
	int minWidth= 32;
	int nOuterFPIterations = 5;//5;
	int nInnerFPIterations = 1;//3;
	int nSORIterations= 20;//40;

	// compute optical flow between consecutive frames
	int n_src = 24;
	int n_dst = 36;
	DImage frm_src = m_pVoxel->VoxelZSlice2DImage(n_src);
	DImage frm_dst = m_pVoxel->VoxelZSlice2DImage(n_dst);
	DImage cvx,cvy,cwarpI2;
	OpticalFlow::Coarse2FineFlow(cvx,cvy,cwarpI2,frm_src,frm_dst,
		alpha,ratio,minWidth,nOuterFPIterations,nInnerFPIterations,nSORIterations);
	char str[MAX_PATH], str_dst[MAX_PATH];
	sprintf(str, "frm%04d.png", n_src);
	frm_src.imwrite(str);
	sprintf(str, "frm%04d.png", n_dst);
	frm_dst.imwrite(str);
	sprintf(str, "warp_%04d_%04d.png", n_src, n_dst);
	cwarpI2.imwrite(str);
	DImage vimgc = OptFlowVec2Color_DImage(&cvx, &cvy);
	sprintf(str, "motvec_%04d_%04d.png", n_src, n_dst);
	vimgc.imwrite(str);
	//DImage nimgc = OptFlowVecNorm2DImage(&cvx, &cvy, 0.5);
	DImage nimgc = OptFlowVecNorm2DImage(&cvx, &cvy);
	sprintf(str, "motnorm_%04d_%04d.png", n_src, n_dst);
	nimgc.imwrite(str);
#endif
	//////////////////////////////////////////////////////////

	return TRUE;
}

// detect keypoints - default
void CsMIShopDoc::OnButtonMotionFeatDetection()
{
	
}

// detect keypoints - GoodFeatures2Track
void CsMIShopDoc::OnButtonMotFeatDetGF2T()
{

}

// detect keypoints - FAST
void CsMIShopDoc::OnButtonMotFeatDetFAST()
{

}

// detect keypoints - SIFT
void CsMIShopDoc::OnButtonMotFeatDetSIFT()
{

}


// MATCH KEYPOINTS
void CsMIShopDoc::OnButtonMotFeatMatch()
{
}

//#define SAVE_KPT_TRACK_CANDIDATE_PROC
int CsMIShopDoc::InitKPtTrackCandidates(int n_st_frm)
{
	//	FIRST, try to estimate motion by KLT tracking
	std::vector<cv::Point2f> pts_start, pts_prev, pts_next;
	cv::Mat frm_p = m_pVoxel->VoxelZSlice2cvMat<short>(n_st_frm);
	cv::Mat frm_p_8u;
	frm_p.convertTo(frm_p_8u, CV_8U);

	// detect keypoints
	cv::goodFeaturesToTrack(frm_p_8u, pts_start, 
		m_params_gf2t.maxCorners, m_params_gf2t.qualityLevel, m_params_gf2t.minDistance);
	// clear previous tracks
	for ( int i = 0; i < (int)m_kptTracks.size(); i++ ) {
		m_kptTracks[i].clear();
	}
	m_kptTracks.clear();
	// prepare array to store the keypoint tracks
	int nkpts = (int)pts_start.size();
	int nslices = m_pVoxel->m_nZ;
	m_kptTracks.resize(nslices);
	for ( int i = 0; i < nslices; i++ ) {
		m_kptTracks[i].resize(nkpts);
	}
	for ( int i = 0; i < nkpts; i++ ) {
		m_kptTracks[n_st_frm][i] = pts_start[i];
	}

#ifdef SAVE_KPT_TRACK_CANDIDATE_PROC
	char path[MAX_PATH];
	cv::Mat f1c; 
	cv::cvtColor(frm_p_8u, f1c, CV_GRAY2BGR);
	DrawPoints2cvMat(f1c, pts_prev, 1, cv::Scalar(0,255,0), 1);
	sprintf_s(path, "klt_kpts_%04d.png", n_st_frm);
	cv::imwrite(path, f1c);	
#endif
	
	// backwards in time
	pts_prev = pts_start;
	for ( int i = n_st_frm; i > 0; i-- ) {
		int pidx = i;
		int nidx = i-1;
		cv::Mat frm_n = m_pVoxel->VoxelZSlice2cvMat<short>(nidx);
		cv::Mat frm_n_8u;
		frm_n.convertTo(frm_n_8u, CV_8U);

		// try KLT tracker
		std::vector<unsigned char> status;
		std::vector<float> err;
		cv::calcOpticalFlowPyrLK(
		  frm_p_8u, frm_n_8u, // 2 consecutive images
		  pts_prev, // input point positions in first im
		  pts_next, // output point positions in the 2nd
		  status,    // tracking success
		  err      // tracking error
		);
		// backup
		frm_p = frm_n;
		frm_p_8u = frm_n_8u;
		pts_prev = pts_next;
		// store tracks
		for ( int i = 0; i < nkpts; i++ ) {
			m_kptTracks[i][nidx] = pts_next[i];
		}

#ifdef SAVE_KPT_TRACK_CANDIDATE_PROC
		cv::Mat kltimg = DrawMatchingPoints2cvMat(frm_p_8u, pts_prev, frm_n_8u, pts_next, 1, 1);
		sprintf_s(path, "klt_match_%04d_%04d.png", pidx, nidx);
		cv::imwrite(path, kltimg);

		cv::cvtColor(frm_n_8u, f1c, CV_GRAY2BGR);
		DrawPoints2cvMat(f1c, pts_next, 1, cv::Scalar(0,255,0), 1);
		sprintf_s(path, "klt_kpts_%04d.png", nidx);
		cv::imwrite(path, f1c);
#endif
	}

	// forward in time
	pts_prev = pts_start;
	for ( int i = n_st_frm; i < m_pVoxel->m_nZ-1; i++ ) {
		int pidx = i;
		int nidx = i+1;
		cv::Mat frm_n = m_pVoxel->VoxelZSlice2cvMat<short>(nidx);
		cv::Mat frm_n_8u;
		frm_n.convertTo(frm_n_8u, CV_8U);

		// try KLT tracker
		std::vector<unsigned char> status;
		std::vector<float> err;
		cv::calcOpticalFlowPyrLK(
		  frm_p_8u, frm_n_8u, // 2 consecutive images
		  pts_prev, // input point positions in first im
		  pts_next, // output point positions in the 2nd
		  status,    // tracking success
		  err      // tracking error
		);
		// backup
		frm_p = frm_n;
		frm_p_8u = frm_n_8u;
		pts_prev = pts_next;
		// store tracks
		for ( int i = 0; i < nkpts; i++ ) {
			m_kptTracks[nidx][i] = pts_next[i];
		}

#ifdef SAVE_KPT_TRACK_CANDIDATE_PROC
		cv::Mat kltimg = DrawMatchingPoints2cvMat(frm_p_8u, pts_prev, frm_n_8u, pts_next, 1, 1);
		sprintf_s(path, "klt_match_%04d_%04d.png", pidx, nidx);
		cv::imwrite(path, kltimg);

		cv::cvtColor(frm_n_8u, f1c, CV_GRAY2BGR);
		DrawPoints2cvMat(f1c, pts_next, 1, cv::Scalar(0,255,0), 1);
		sprintf_s(path, "klt_kpts_%04d.png", nidx);
		cv::imwrite(path, f1c);
#endif
	}

	return nkpts;
}

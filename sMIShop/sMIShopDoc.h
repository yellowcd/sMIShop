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

// sMIShopDoc.h : interface of the CsMIShopDoc class
//


#pragma once

#include "splVoxLib/Voxel.h"

class cvGF2TParams
{
public:
	int maxCorners; 
	double qualityLevel;
	double minDistance;
    //cv::Mat mask;//=noArray(); 
	//int blockSize;//=3;
	cvGF2TParams();
};

class CsMIShopDoc : public CDocument
{
protected: // create from serialization only
	CsMIShopDoc();
	DECLARE_DYNCREATE(CsMIShopDoc)

// Attributes
public:
	CVoxel<short int> *m_pVoxel;
	cvGF2TParams m_params_gf2t;
	std::vector<std::vector<cv::Point2f>> m_kptTracks;
// Operations
public:
	int InitKPtTrackCandidates(int n_st_frm);

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CsMIShopDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg void OnButtonMotionFeatDetection();
	afx_msg void OnButtonMotFeatDetFAST();
	afx_msg void OnButtonMotFeatDetSIFT();
	afx_msg void OnButtonMotFeatDetGF2T();
	afx_msg void OnButtonMotFeatMatch();
	cv::Mat *m_pDrawLayerZ;
	cv::Mat *m_pDrawLayerY;
	cv::Mat *m_pDrawLayerX;
	int m_iMaxValue;
	int m_iMinValue;
};

#if !defined(VOXELSLICEVIEWER)
#define VOXELSLICEVIEWER

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Viewer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewer window
#include <math.h>
#include "sMIShopSettings.h"
#include "Voxel.h"

class CVoxelSliceViewer : public CStatic
{
// Construction
public:
	CVoxelSliceViewer();

// Attributes
public:
	HDC m_hDC;
	int m_wndWidth;
	int m_wndHeight;

	CVoxel<short int>	*m_pVoxel;
	CVoxel<BYTE>		*m_pGT;
	CVoxel<BYTE>		*m_pSegRes;
	CVoxel<BYTE>		*m_pMK;
	CVoxel<BYTE>		*m_pROI;
	CVoxel<float>		*m_pFGUProb;
	CVoxel<float>		*m_pFGPProb;

	int m_mode;
	short m_curI;
	short m_curOrigI;
	int m_nCurCoordX;
	int m_nCurCoordY;
	int m_nCurCoordZ;
	short int m_nMinI;
	short int m_nMaxI;
	int m_ishowMax;
	int m_ishowMin;
	int m_pInputType;

	CStatic *m_ctrlStaticMinMaxI;
	CStatic *m_ctrlStaticCurI;
	CStatic *m_ctrlStaticCurCoord;
	CSpinButtonCtrl *m_ctrlStaticZoomRatio;

	bool m_bInit;
	CPoint m_PntLeftTop; // Left-top point of the rectangular showing the interacting BITMAP
	double m_iRatio;
	double m_iProbTH;
	float m_brSize;
	// current position of viewer inside voxel space
	CPoint m_curPos;
	CPoint m_curTrans;

	int m_iCurSliceZ;
	int m_iCurSliceY;
	int m_iCurSliceX;

	bool m_bLButtonDown;
	bool m_bRButtonDown;
	BOOL m_bViewGT;
	BOOL m_bViewSegRes;
	BOOL m_bViewProb;
	BOOL m_bMarking;
	BOOL m_bMk3D;

	BITMAPINFO	m_bmi; // for erasing the displays
	BYTE *m_pSlice; // to show the current slice as BITMAP

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewer)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Initialize();
	virtual ~CVoxelSliceViewer();
	void LoadVoxel(CVoxel *pVoxel);
	void SetMode(int n);
	void SetSlice(int n);
	bool SetupDisplayBuffer();
	void SetZoomRatio(int zoom);
	void SetViewProbTH(int th);
	void SetSliceIdxX(int n);
	void SetSliceIdxY(int n);
	void SetSliceIdxZ(int n);
	int   GetZoomRatio();
	void InitZoomRatio();
	void InitCurPos();
	void GetMinMaxIntensities();
	void LoadInfoParams( CStatic *ctrlStaticCurCoord, CStatic *ctrlStaticCurI, 
									CStatic *ctrlStaticMinMaxI, CSpinButtonCtrl *ctrlStaticZoomRatio);
	void LoadInfoIRange( int isMin, int isMax );
	bool LoadGT(CVoxel *pGT);
	bool LoadSegRes(CVoxel *pSegRes);
	bool LoadUP(CFVoxel *pUP);
	bool LoadPP(CFVoxel *pPP);
	bool LoadMarkingVxl( CVoxel* mkVxl );
	bool LoadROIVxl( CVoxel* roiVxl );

	void ClearGT();
	void ClearSegRes();
	void ClearUP();
	void ClearPP();
	void ClearMK();

	void SetInputType( int inputType );

	cv::Mat GetCurrentSlice(CVoxel *pVoxel);
	void SaveSlice(cv::Mat img, bool norm=true);
	void SaveMRSlice();
	void SaveGTSlice();
	void SaveSRSlice();

	void LoadMarkingInfo( BOOL bMarking, BOOL b3D, float brSize);
	void LoadBrSize(float brSize);
	bool Splat( int x, int y, BYTE byInLabel, BOOL bMk3D );

// Generated message map functions
protected:
	//{{AFX_MSG(CVoxelSliceViewer)
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWER_H__E95EB5E3_31D8_48A6_B318_50362D22D554__INCLUDED_)

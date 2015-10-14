#pragma once
// Viewer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewer window

#include <gl\gl.h>
#include <gl\glu.h>
//#include <gl\glaux.h>

#include <math.h>
#include "Voxel.h"
//#include "..\image.h"
//#include "..\3D_Prim\Model.h"

enum { LBUTTON = 1, RBUTTON = 4, BOTHBUTTON = 8 };

/*
class CCaptureImg 
{
public:
	BYTE *pBuffer;
	int width;
	int height;

	CCaptureImg();
	CCaptureImg(int w, int h);
	virtual ~CCaptureImg();
	void SetImg(int w, int h);

	void SaveCaptureImg(CString file);
};
*/

class CVoxelViewer : public CStatic
{
// Construction
public:
	CVoxelViewer();

// Attributes
public:
	HGLRC m_hRC;
	HDC m_hDC;

	int m_Mode;
	double m_Azimuth;
	double m_Elevation;
	double m_Distance;	
	double m_TransX;
	double m_TransY;
	double m_FOV;
	double m_Near;
	double m_Far;

private:
	double m_fEyePositionX;
	double m_fEyePositionY;
	double m_fEyePositionZ;
	double m_fEyeNormX;
	double m_fEyeNormY;
	double m_fEyeNormZ;

public:
	double m_stepAzimuth;
    double m_stepElevation;
    double m_stepTranX;
    double m_stepTranY;
    double m_stepDistance;
	CPoint m_PrevPoint;

	BOOL m_bAxis;
	
	// temp

	CVoxel	*m_pVoxel;
	CVoxel	*m_pVoxel2;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewer)
	//}}AFX_VIRTUAL

// Implementation
public:
	void DrawAxis();
	void DrawScene();
	void Initialize();
	virtual ~CVoxelViewer();
	void DrawObject();
	void LoadVoxel(CVoxel *pVoxel);
	void LoadVoxel2(CVoxel*pVoxel);
	//void ClearModel();
	void SetupAngle();
	void DrawVoxel(CVoxel *pVoxel, bool b = false);
	void DrawCube(float a, float b, float c);
	void DrawWireCube(float a, float b, float c);
	void SetupVoxelDistance(float scale);
	//CCaptureImg *CaptureView();

	double GetEyePositionX();
	double GetEyePositionY();
	double GetEyePositionZ();

	void SetEyePositionX(double x);
	void SetEyePositionY(double y);
	void SetEyePositionZ(double z);

	double GetEyeNormX();
	double GetEyeNormY();
	double GetEyeNormZ();

	void SetEyeNormX(double x);
	void SetEyeNormY(double y);
	void SetEyeNormZ(double z);

	void SetViewPoint();
	

// Generated message map functions
protected:
	//{{AFX_MSG(CVoxelViewer)
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


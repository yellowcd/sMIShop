// FVoxel.h: interface for the CFVoxel class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "F3DPoint.h"

class CVoxel;
typedef CArray<C3DPoint> C3DPointArray;

class CFVoxel  
{
public:
	float *m_pfData;
	
	int m_nX ;
	int m_nY ;
	int m_nZ ;

	float xSp;
	float ySp;
	float zSp;
	
	CFVoxel( ) ;
	CFVoxel( int XSize, int YSize, int ZSize, bool alloc = TRUE ) ;
	CFVoxel(float *arr, int XSize, int YSize, int ZSize);
	CFVoxel( C3DPoint pt );
	CFVoxel( CFVoxel & Source ) ;
	CFVoxel( CString fileName ) ;
	virtual ~CFVoxel( ) ;

	void Allocate( ) ;
	void AllocateNoInitialize( );
	void Allocate( int XSize, int YSize, int ZSize );
	void Free( ) ;
	void ClearToZeros( ) ;
	void Copy( CFVoxel &source );
	CFVoxel *Copy();
	CFVoxel *Crop(const C3DPoint &pt, const C3DPoint &size);
	void CropFilled(C3DPoint &st, C3DPoint &end, double thres = 0, int BUF = 1);
	int GetNumber();
	
	inline float GetAt( int x, int y, int z )
	{
		return m_pfData[x + y * m_nX + z * m_nX * m_nY] ;
	}
	
	inline int CheckGetAt( int x, int y, int z ) // iDir determines the viewing direction along one of x,y,z axes
	{
		if ( 0 <= x && x < m_nX && 0 <= y && y < m_nY && 0 <= z && z < m_nZ )
			return m_pfData[x + y * m_nX + z * m_nX * m_nY];
		else {
			//CString str;	str.Format(_T("You must check range"));
			//AfxMessageBox(str);
			return 1000;
		}
	}

	inline int CheckGetAt( C3DPoint &pt ) // iDir determines the viewing direction along one of x,y,z axes
	{
		if ( 0 <= pt.x && pt.x < m_nX && 0 <= pt.y && pt.y < m_nY && 0 <= pt.z && pt.z < m_nZ )
			return m_pfData[pt.x + pt.y * m_nX + pt.z * m_nX * m_nY];
		else {
			//CString str;	str.Format(_T("You must check range"));
			//AfxMessageBox(str);
			return 1000;
		}
	}

	inline float GetAt( C3DPoint Point )
	{
		return m_pfData[Point.x + Point.y * m_nX + Point.z * m_nX * m_nY] ;
	}
	inline void SetAt( int x, int y, int z, float fValue )
	{
		m_pfData[x + y * m_nX + z * m_nX * m_nY] = fValue ;
	}
	inline void SetAt( C3DPoint Point, float Value )
	{
		m_pfData[Point.x + Point.y * m_nX + Point.z * m_nX * m_nY] = Value ;
	}
	inline C3DPoint GetSize() {
		return C3DPoint(m_nX, m_nY, m_nZ);
	}
	
	bool operator ==(const CVoxel& v) const;
	bool operator ==(const CFVoxel& v) const;
	bool operator !=(const CVoxel& v) const;
	bool operator !=(const CFVoxel& v) const;

	bool FromReverseVoxel( CVoxel *pVoxel );
	bool FromVoxel( CVoxel *pVoxel ) ;
	bool ToVoxel( CVoxel *pVoxel ) ;
	bool Load( CString FileName ) ;
	void Save( CString FileName ) ;
	void SaveFabs( CString FileName );
	bool ReadFromMhd(CString PathName);
	
	void PlusConstant( float fOperand ) ;
	void MinusConstant( float fOperand ) ;

	void Differentiate( CFVoxel* lu );
	void LinearScale( float fLower, float fUpper ) ;
	float GetTriLinearInterpolation( CF3DPoint fPoint ) ;
	//bool HystresisThresholding( float fLower, float fUpper ) ;
	bool Thresholding2Bounds( float fLower, float fUpper ) ;

	bool Get3DFixedGaussianSmoothing( CFVoxel &Source ) ;
	bool Get3DSobel( CFVoxel *pfGradMag, CFVoxel *pfGradX, CFVoxel *pfGradY, CFVoxel *pfGradZ ) ;
	bool Get3DNonMaximumSuppression( CFVoxel *pfGradX, CFVoxel *pfGradY, CFVoxel *pfGradZ ) ;

	///////////////////////////////////////////////////////////////////////////
	// SCLEE, 20091204
	void AddFVoxel( CFVoxel *addvxl );
	inline BOOL IsSameDimension(CFVoxel *vox)
	{
		if ( m_nX == vox->m_nX && m_nY == vox->m_nY && m_nZ == vox->m_nZ )
			return TRUE;
		else 
			return FALSE;
	}
	inline BOOL IsInBounds(const C3DPoint &pt)
	{
		if ( 0 <= pt.x && pt.x < m_nX && 0 <= pt.y && pt.y < m_nY && 0 <= pt.z && pt.z < m_nZ )
			return TRUE;
		else 
			return FALSE;
	}
	BOOL IsLocalMaximum( C3DPoint &pt, C3DPoint &localReg=C3DPoint(2,2,1) );
	void SetData(float f);
	inline int GetTotVoxelNum() {return m_nX*m_nY*m_nZ;}
	double Mean();
	// SCLEE, 20100225
	void IncrementSumProduct(CFVoxel *v1, CFVoxel *v2);
	void IncrementExpSumProduct(CFVoxel *v1, int r, CFVoxel *v2);
	void WeightedMeanAndVar(CFVoxel *w, double &m, double &v,  int nAnchor, int  idx);
	void WeightedMeanAndVar(CFVoxel *w, double &m, double &v);
	CFVoxel *GetGradient(const C3DPoint &npt);
	CFVoxel *Subtract(CFVoxel *vin);
	double GetZeroCrossingBoundaryEdgeLength(C3DPointArray &nPtArr);
	double GetZeroCrossingBoundaryEdgeLength(C3DPointArray &nPtArr, CVoxel *exc);
	void DivideConst( int nVal );
	void DivideConst( float nVal );
	void MultiplyConst( float fv );
	void MultiplyConst( double fv );
	bool CheckValRange( double s, double e );
	void SaveImagePerSlice( CString DirName, CString FileName );
	void SaveBCIImagePerSlice( C3DPointArray *bciPtArr, C3DPointArray *boiPtArr, 
		CF3DPoint *originPtArr, double *meanDistArr, int nDB, 
		C3DPoint &maporig, double mapSize, CString DirName, CString FileName);
	void SaveAsCVoxel( CString FileName );
	float GetMax();
	CVoxel *MakeDTShell(double dist);
	///////////////////////////////////////////////////////////////////////////
	// by shpark
	bool WeightToVoxel( CVoxel *pVoxel );
	void MultiplyConst( int nVal );
	void SaveweightAsCVoxel( CString FileName );
	void ResetPairwiseProbUsingGradientDT(CFVoxel *weight, int r);
	double GetTemplateHammingDistance( CFVoxel* temp, C3DPoint stpt );
	void InsertCVoxelIntoCenter(CVoxel* vxl);
};

extern void NormalizeFVoxelArr(CTypedPtrArray<CPtrArray, CFVoxel*> &vArr);

extern void ConvertFLOATtoCVOXEL(float* farray, CFVoxel* voxel);
extern void ConvertCVOXELtoFLOAT(CFVoxel* voxel, float* farray);

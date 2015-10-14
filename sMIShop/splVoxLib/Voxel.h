// Voxel.h: interface for the CVoxel class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "sMIShopSettings.h"
#include "../OpticalFlow/Image.h"
#include "Util.h"

class CHistogram;
class CBilayerHistogram;
class CVoxelShell;

enum VoxViewMode{XV = 2, YV = 1, ZV = 0};

template <typename T>
class CVoxel  
{
public:
	T *m_pData;
	
	int m_nX;
	int m_nY;
	int m_nZ;

	float xSp ; // 06/25/007
	float ySp ;
	float zSp ;

	float xOrg ;
	float yOrg ;
	float zOrg ;

	double m_nMaxI;
	double m_nMinI;

	// ***** constructors / destructors ***** //
	CVoxel();
	CVoxel( int XSize, int YSize, int ZSize );
	CVoxel( const cv::Point3i &pt );
	template <typename TI>
	CVoxel( CVoxel<TI> *src );
	template <typename TI>
	CVoxel( TI *pData, int XSize, int YSize, int ZSize );
	CVoxel( CString fn );
	~CVoxel();

	// ***** operators ***** //
	template <typename TI>
	bool operator ==(const CVoxel<TI>& v) const;
	template <typename TI>
	bool operator !=(const CVoxel<TI>& v) const;

	// ***** member functions: primitive-inline- ***** //
	inline void SetOrg(int ox, int oy, int oz) {
		xOrg = ox;
		yOrg = oy;
		zOrg = oz;
	}
	inline void SetOrg(CVoxel *pVoxel) {
		SetOrg(pVoxel->xOrg, pVoxel->yOrg, pVoxel->zOrg); 
	}
	inline void SetSpace(int sx, int sy, int sz) {
		xSp = sx;
		ySp = sy;
		zSp = sz;
	}
	inline void SetSpace(CVoxel *pVoxel) {
		SetOrg(pVoxel->xSp, pVoxel->ySp, pVoxel->zSp); 
	}
	inline void CopySpOrg(CVoxel *pVoxel) {
		SetOrg(pVoxel);
		SetSpace(pVoxel);
	}

	inline int GetXSize() const { return m_nX ; }
	inline int GetYSize() const { return m_nY ; }
	inline int GetZSize() const { return m_nZ ; }

	// iDir determines the viewing direction along one of x,y,z axes
	inline int GetAt( int x, int y, int z )	{
		return m_pData[x + y*m_nX + z*m_nX*m_nY];
	}
	inline int GetAt( int i )	{
		return m_pData[i];
	}
	inline int GetAt( cv::Point3i pt)	{
		return m_pData[pt.x + pt.y*m_nX + pt.z*m_nX*m_nY] ;
	}
	inline int CheckGetAt( int x, int y, int z ) // iDir determines the viewing direction along one of x,y,z axes
	{
		if ( 0 <= x && x < m_nX && 0 <= y && y < m_nY && 0 <= z && z < m_nZ )
			return m_pData[x + y * m_nX + z * m_nX * m_nY];
		else {
			//CString str;	str.Format(_T("You must check range"));
			//AfxMessageBox(str);
			return -1;
		}
	}
	inline void GetBicubeValues(int x, int y, int z, T *arr)
	{
		arr[0] = CheckGetAt(2*x,2*y,2*z); 
		arr[1] = CheckGetAt(2*x+1,2*y,2*z);
		arr[2] = CheckGetAt(2*x,2*y+1,2*z);
		arr[3] = CheckGetAt(2*x+1,2*y+1,2*z); 
		arr[4] = CheckGetAt(2*x,2*y,2*z+1);
		arr[5] = CheckGetAt(2*x+1,2*y,2*z+1);
		arr[6] = CheckGetAt(2*x,2*y+1,2*z+1);
		arr[7] = CheckGetAt(2*x+1,2*y+1,2*z+1);
	}
	inline void SetAt( int x, int y, int z, T iValue )	{
		m_pData[x + y * m_nX + z * m_nX * m_nY] = iValue;
	}
	inline void SetAt( cv::Point3i pt, T v )	{
		m_pData[pt.x + pt.y*m_nX + pt.z*m_nX*m_nY] = v;
	}
	inline BOOL IsInBounds( int x, int y, int z )
	{
		return ( 0 <= x && x < m_nX && 
				 0 <= y && y < m_nY && 
				 0 <= z && z < m_nZ );
	}
	inline BOOL IsInBounds( cv::Point3i pt )
	{
		return IsInBounds(pt.x, pt.y, py.z);
	}
	inline cv::Point3i GetSize() {
		return cv::Point3i(m_nX, m_nY, m_nZ);
	}
	template <typename TI>
	inline bool IsSameSize( CVoxel<TI> *src ) {
		return ( m_nX == src.m_nX && m_nY == src.m_nY && m_nZ == src.m_nZ );
	}
	template <typename TI>
	inline bool IsSameDim( CVoxel<TI> *src ) {
		return ( m_nX == src.m_nX && m_nY == src.m_nY && m_nZ == src.m_nZ &&
			xOrg == src.xOrg && yOrg == src.yOrg && zOrg == src.zOrg &&
			xSp == src.xSp && ySp == src.ySp && zSp == src.zSp );
	}
	void CVoxel::SetDefaultSmoothingFilter()
	{
		m_nSmoothingFilter = 3;
		m_pSmoothingFilter = new double[m_nSmoothingFilter];
		m_pSmoothingFilter[0] = 0.25;
		m_pSmoothingFilter[1] = 0.5;
		m_pSmoothingFilter[2] = 0.25;
		m_pSFIdx = new int[m_nSmoothingFilter];
		m_pSFIdx[0] = -1;
	}
	void SetToZero( );
	void SetTo( T iValue );
	double GetMax();
	double GetMin();

	// ***** member functions: memory allocation ***** //
	void Allocate();
	void Allocate(int XSize, int YSize, int ZSize);
	void FreeData();
	
	// ***** member functions: copying ***** //
	template <typename TI>
	void CopyData( TI *pData );
	template <typename TI>
	void CopyData( CVoxel<TI>* src, CVoxel<BYTE> *mask );
	template <typename TI>
	void Copy( CVoxel<TI> *src );
	template <typename TO>
	CVoxel<TO> *Copy();

	// ***** member functions: IO ***** //
	bool Read(CString filename);
	bool ReadFromMsk( CString FileName, bool bBottomUp = false );
	bool ReadFrom2DImg(CString FileName);
	bool ReadFromBin( CString FileName );
	bool ReadFromRaw( CString FileName ) ;
	bool ReadFromMhd(CString PathName);
	//bool ReadFromHdr(CString PathName);
	bool ReadFromImg(CString PathName);
	bool ReadFromDCM(CString PathName);
	//bool ReadFromDcm( CString PathName );
	//bool ReadFromDcmFolder( CString PathName );
	bool WriteToMsk( CString FileName, bool bBottomUp = false ) ;
	bool WriteToBin( CString FileName ) ;
	bool WriteToRaw( CString FileName ) ;
	//void WriteToGz( CString FileName, CString standardName ) ;
	//void WriteToNiftiFile( CString savePath );
	bool WriteToMhd( CString PathName );
	bool WriteToUcharMhd( CString PathName );

	// ***** member functions: conversion to other types ***** //
	template<typename TO> 
	cv::Mat VoxelXSlice2cvMat(int x);
	template<typename TO> 
	cv::Mat VoxelYSlice2cvMat(int y);
	template<typename TO> 
	cv::Mat VoxelZSlice2cvMat(int z);
	DImage VoxelZSlice2DImage(int z);

	// ***** member functions: UI ***** //
	void DrawCircleX(int cx, int cy, int sx, float rad, BYTE bv);
	void DrawCircleY(int cx, int cy, int sy, float rad, BYTE bv);
	void DrawCircleZ(int cx, int cy, int sz, float rad, BYTE bv);

	// ***** member functions: manipulation/processing ***** //	template <typename T>
	CVoxel<T> *MakeSmoothedX_raw(double *k_v, int *k_idx, int k_sz);
	CVoxel<T> *MakeSmoothedY_raw(double *k_v, int *k_idx, int k_sz);
	CVoxel<T> *MakeSmoothedZ_raw(double *k_v, int *k_idx, int k_sz);

	template <typename OP>
	void GetROI_Op(OP op, cv::Point3i &st, cv::Point3i &end, cv::Point3i &sz=cv::Point3i());
	void Crop(const cv::Point3i st, const cv::Point3i end, int BUF=1);
	template <typename TI>
	void Crop( CVoxel<TI> *src, const cv::Point3i st, const cv::Point3i &end, int BUF=1);
	template <typename TO>
	CVoxel<TO>* MakeCropped(const cv::Point3i st, const cv::Point3i end, int BUF=1);
	template <typename TO>
	CVoxel<TO>* MakeCropped(const cv::Point3i st, const cv::Point3i end, const cv::Point3i BUF);
	template <typename TI>
	void Crop( CVoxel<TI> *src, const cv::Point3i st, const cv::Point3i end, const cv::Point3i BUF );
	template <typename TO>
	CVoxel<TO>* GetCropped(const cv::Point3i st, const cv::Point3i end, int BUF);
	template <typename TO>
	CVoxel<TO>* GetCropped(const cv::Point3i st, const cv::Point3i end, const cv::Point3i BUF);

	template <typename TO, typename OP>
	CVoxel<TO>* Mask(T vi, TO vo, OP op);
	template <typename TO, typename OP>
	void Mask(T vi, TO vo, CVoxel<TO>* pV, OP op);
	template <typename TO>
	CVoxel<TO> *GetGradient(const cv::Point3i &npt, double sigma=10);
	template <typename TO>
	void GetGradient(CVoxel<TO> *pV, const cv::Point3i &npt, double sigma=10);
	void SampleSurfacePts(cv::Point3i intv, cvPoint3iArray &sptarr);
	template <typename TO>
	CVoxel<TO>* MakeDownsampleBy2_raw();
	template <typename TO>
	CVoxel<TO>* MakeDownsampleXBy2_raw();
	template <typename TO>
	CVoxel<TO>* MakeDownsampleYBy2_raw();
	template <typename TO>
	CVoxel<TO>* MakeDownsampleZBy2_raw();
	CVoxel<BYTE>* MakeDownsampleBy2_bin(int nz_th);

	// *** 
	// CVoxel* GetLabel(int label, int val);
	// *** -> replace with: 
	// ***	Mask(T label, TO val, iop_mask_eq<T>())
	// CVoxel* Get2Label(int label1, int label2, int val1, int val2);
	// *** -> replace with:
	// ***	CVoxel<T> oVox = pVox->Mask(T label, TO val, iop_mask_eq<T>());
	// ***	pVox->Mask(T label, TO val, oVox, iop_mask_eq<T>());
	// *** 

	/*
	void RemoveRegion(CVoxel* removevoxel);
	BOOL IsHoleBtwBoneAndCartilage( cv::Point3i pt, CVoxel* bone, short BT, cv::Point3i nr = cv::Point3i(2,2,0), int bthres = 5, int cthres = 5 );


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// by SCLee
	void InsertCropRegion( CVoxel* cropMR, cv::Point3i st );
	void InsertLabel( CVoxel* label);
	void Align();
	void Align( float *v, cv::Point3f &mean );
	void PCA( float *v, cv::Point3f &mean, BOOL bSort=FALSE );
	void CartesianRotate( float *v, cv::Point3f &mean, cv::Point3i &minPT = cv::Point3i() );
	void CartesianRotate( CVoxel *src, float *v, cv::Point3f &mean );
	void GetRotatedBoundingBox( float *v, cv::Point3f &mean, cv::Point3i &minpt, cv::Point3i &maxpt );
	void GetRotatedBoundingBox( CVoxel *src, float *v, cv::Point3f &mean, cv::Point3i &minpt, cv::Point3i &maxpt );
	void CleanBoundaries();
	bool TripleUnion(CVoxel *a, CVoxel *b, CVoxel *c);
	void SetFilledVoxelValues(short int v);
	void CreateByMerge( CVoxel *a, CVoxel *b );
	bool Thresholding( short int i );
	CVoxel* Insert(int nx, int ny, int nz, int px, int py, int pz);
	CVoxel* Insert(cv::Point3i size, cv::Point3i pos);
	CVoxel( int *vxl, int nx, int ny, int nz );
	int ComputeNumberOfNonZeroVxls();
// 2009 12 03
	void ComputeCovariance(float *a, cv::Point3f &mean=cv::Point3f());
	void ComputeMaxCrossSectionCOG(cv::Point3f &pt, BOOL bAlign);
	void HistogramNormalize(CHistogram* inHist, CHistogram *outHist);
	void SaveImagePerSlice( CString DirName, CString FileName );
// 2010 02 18
	void GetBinaryVxl( CFVoxel* fvxl, double thres );
	void GetPathRoi(cvPoint3iArray& ptArr, cv::Point3i size);
	CVoxel* ComputeIMinusMean(float m);
	float ComputeIMean();
	void ComputeSubRegionIMean(CVoxel* label, float &m);
	float ComputeIVar(float mean);
	void ComputeIMeanAndVar(float &m, float &v);
	void ComputeIMeanAndVar(cv::Point3i stpt, cv::Point3i edpt, float &m, float &v);
	void ComputeSubRegionIMeanAndVar(cv::Point3i stpt, cv::Point3i edpt, float &m, float &v);
	void ComputeSubRegionIMeanAndVar(cv::Point3i stpt, cv::Point3i edpt, CVoxel* label, float &m, float &v);
	void ComputeSubRegionIMeanAndVar(cv::Point3i stpt, cv::Point3i edpt, CFVoxel* prob, float thres, float &m, float &v);
	BOOL ComputeSubRegionIMeanAndVarWithRegionCheck(cv::Point3i stpt, cv::Point3i edpt, float &m, float &v);
	void ComputeWeightedIMeanAndVar(float &m, float &v, CFVoxel *w);
	void ComputeWeightedSubRegionIMeanAndVar(cv::Point3i stpt, cv::Point3i edpt, float &m, float &v, CFVoxel *w);
	void ComputeWeightedSubRegionIMeanAndVar(cv::Point3i stpt, cv::Point3i edpt, float &m, float &v, CVoxel* roi, CFVoxel *w);
	void GetMeanSigma( float& mean, float& sigma );
	void VarianceNormalize( float appMean, float appSigma );
	void LinearNormalize( int minVal, int maxVal, int mintVal, int maxtVal );
	void Transform(vtkMatrix4x4 *T);
	void Transform(vtkMatrix4x4 *T, CVoxel *orig, cv::Point3i &pt);
	void Transform(vtkMatrix4x4 *T, CVoxel *orig, cv::Point3i &pt, CVoxel **tv);
	bool CheckTrueNeighborLabels(const cv::Point3i pt, const cv::Point3i nr, int BT);
	bool CheckTrueNeighborLabels(const cv::Point3i pt, const cv::Point3i nr );
	CFVoxel *GetFastDistanceTransform( );

	void ComputeSubRegionFGBGMean(cv::Point3i patStPos, cv::Point3i patEdPos, CVoxel* fgbgPat, int fgIdx, int bgIdx, float& fmean, float& bmean);
	float ComputeSubRegionAppDistribution(cv::Point3i patStPos, cv::Point3i patEdPos, CFVoxel* probPat, CBilayerHistogram* hist );
	//CVoxelShell *GetROIVoxelShell(cv::Point3f &roiOrig, cv::Point3i &roiSize);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	vtkContourFilter* ConvertToVTP( int label );
	vtkPolyData* MakePolyData( );
	void SavePoints(CString path, vtkPoints* pt);
	vtkPolyData* ExtractLandmarkPoints( vtkPolyData* pTarPolyCT, cv::Point3i range );
	int GetNormalizeThresValue( int interval, float thresRatio);

	void MakeBallSe( float fRadius ) ;
	int FastErode(CVoxel & SE);
	int FastDilate(CVoxel & SE);
	void Opening(CVoxel & SE);
	void Closing(CVoxel & SE);
	void Opening();
	void Closing();
	void ClosingWithErode();
	int GetNumber( ) ;
	int GetNumberWithIdx(int val);
	
	bool DrawRect( CRect *pRect, int iValue, int iZ ) ;

	CVoxel* GetDeformedlabel3D(CVoxel* stVxl, CFVoxel* deformX, CFVoxel* deformY, CFVoxel* deformZ);
	CVoxel* GetGradientMagnitudeMap();
	CVoxel*	GetSubGradientMagnitudeMap(cv::Point3i stpt, cv::Point3i edpt);
	void GetDistanceTransform( CVoxel *pDt ) ;
	void GetDistanceTransformFastNarrow( CVoxel *pDt ) ; // 02/04/008	
	void GetFastDistanceTransform( CFVoxel *pfDT ) ;
	int deleteSmallRegionsByCca( CVoxel *pLabeledCca, int iConnection, int MinSize ) ;
	bool KeepOnlySkin( ) ;

	bool GenerateModelP01( CVoxel *pModel, int iMinY, int iZ0, int iZ1, cv::Point3i &vMid ) ;
	//bool MatchByTranslation( CVoxel *pModel, CVoxel *pMatched, CVoxel *pDt, cv::Point3i *pntTranss, int nParts ) ;
	bool Thresholding2Bounds( int iLower, int iUpper ) ;

	// compute mean and standard deviation of marked voxels
	bool GetMeanStdDev( CVoxel *pMarkings, BYTE byColor, int iCropX0, int iCropY0, int iCropZ0, float &fMean, float &fStdDev ) ;
	bool FillInClosedContour( CVoxel *pBoundary );


	//by sanghyun
	void TakeForgroundValue( CVoxel* src );
	void TakeForgroundValue( CVoxel* src, int val );
	void ComputeCenterofMassCOG(cv::Point3f &pt);
	void ComputeCenterofMassCOG(cv::Point3i &pt);
	CVoxel* ExtractShell();
	CVoxelShell* ExtractBoundaryShell();
	CVoxelShell* ExtractBCIshell( CVoxel *carti, int bonetype );
	bool ReadFromLiverRaw(CString FileName);
	void GVF3D( double mu, int nIter, CFVoxel* u, CFVoxel* v, CFVoxel* w );

	void GetMeanIntensityInsideShape( CVoxel* shape, int label, double& mean, double& standard, int& min, int& max );

	void NearPointRemoveSampling( cv::Point3i range );

	CVoxel* CreateSuperVoxels(int nSP, int nCompact, int & numlabels);

	int GetTemplateHammingDistance( CVoxel* temp, cv::Point3i stpt );
	double GetTemplateHammingDistance( CFVoxel* temp, cv::Point3i stpt );
	double GetTemplateVarianceDistance( CFVoxel* temp, cv::Point3i stpt );
	void FlipImageAxisX();
	void FlipImageAxisY();
	void FlipImageAxisZ();

	CVoxel* SpacingNormalization(float spX, float spY, float spZ);
	*/
};

typedef CTypedPtrArray<CPtrArray, CVoxel<float>*> CFVoxelPtrArray;



//////////////////////////////////////////////////////////////////////
// ***** Construction/Destruction ***** //
template <typename T>
CVoxel<T>::CVoxel()
{
	m_pData = NULL ;
	m_nX = 0;
	m_nY = 0;
	m_nZ = 0;

	xSp = -1.0f ; // 06/25/007
	ySp = -1.0f ;
	zSp = -1.0f ;

	xOrg = -1.0f ;
	yOrg = -1.0f ;
	zOrg = -1.0f ;
}

template <typename T>
CVoxel<T>::CVoxel(int XSize, int YSize, int ZSize)
{
	m_pData = NULL ;
	m_nX = XSize;
	m_nY = YSize;
	m_nZ = ZSize;
	
	xSp = -1.0f ; // 06/25/007
	ySp = -1.0f ;
	zSp = -1.0f ;

	xOrg = -1.0f ;
	yOrg = -1.0f ;
	zOrg = -1.0f ;

	Allocate();
}

template <typename T>
CVoxel<T>::CVoxel(const cv::Point3i &pt)
{
	m_pData = NULL ;
	m_nX = pt.x;
	m_nY = pt.y;
	m_nZ = pt.z;

	xSp = -1.0f ; // 06/25/007
	ySp = -1.0f ;
	zSp = -1.0f ;

	xOrg = -1.0f ;
	yOrg = -1.0f ;
	zOrg = -1.0f ;
	Allocate();
}

template <typename T>
template <typename TI>
CVoxel<T>::CVoxel(CVoxel<TI> *src)
{	
    Allocate(src->GetXSize(), src->GetYSize(), src->GetZSize());
	CopyData<TI>(src->m_pData);
	CopySpOrg(src);
}

template <typename T>
template <typename TI>
CVoxel<T>::CVoxel( TI *vxl, int nx, int ny, int nz )
{
	m_nX = nx;
	m_nY = ny;
	m_nZ = nz;

	xSp = -1.0f ; // 06/25/007
	ySp = -1.0f ;
	zSp = -1.0f ;

	xOrg = -1.0f ;
	yOrg = -1.0f ;
	zOrg = -1.0f ;

	int i, x, y, z;
	m_pData = new T[m_nX*m_nY*m_nZ] ;
	for( z = 0, i = 0 ; z < m_nZ ; z++ )
		for( y = 0 ; y < m_nY ; y++ )
			for( x = 0 ; x < m_nX ; x++, i++ ) 
				m_pData[i] = (T)vxl[i];
}

template <typename T>
CVoxel<T>::CVoxel( CString fn )
{
	m_pData = NULL;
	Read(fn);
}

template <typename T>
CVoxel<T>::~CVoxel()
{
	FreeData();
}
// ***** end of constructors / destructors ***** //
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// ***** operators ***** //
template <typename T>
template <typename TI>
bool CVoxel<T>::operator ==(const CVoxel<TI>& v) const {
	if ( (m_nX == v.m_nX) && (m_nY == v.m_nY) && (m_nZ == v.m_nZ) )
		return true;
	else
		return false;
}
template <typename T>
template <typename TI>
bool CVoxel<T>::operator !=(const CVoxel<TI>& v) const {
	if ( (m_nX != v.m_nX) || (m_nY != v.m_nY) || (m_nZ != v.m_nZ) )
		return true;
	else
		return false;
}
// ***** end of operators ***** //	
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// ***** member functions: memory allocation ***** //	
template <typename T>
void CVoxel<T>::Allocate( )
{
	int tSize = m_nX * m_nY * m_nZ ;
	SAFE_DELETE_ARRAY(m_pData);
	m_pData = new T[tSize] ;
	SetToZero( );
}

template <typename T>
void CVoxel<T>::Allocate(int XSize, int YSize, int ZSize)
{
	m_nX = XSize;
	m_nY = YSize;
	m_nZ = ZSize;
	Allocate();	
}

template <typename T>
void CVoxel<T>::FreeData()
{
	SAFE_DELETE_ARRAY(m_pData);
}
// ***** end of member functions: memory allocation ***** //	
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// ***** end of member functions: primitives ***** //	-
template <typename T>
void CVoxel<T>::SetToZero()
{
	SetTo(0);
}

template <typename T>
void CVoxel<T>::SetTo( T iValue )
{
	int nv = m_nX*m_nY*m_nZ;
	for ( int i = 0; i < nv; i++ ) {
		m_pData[i] = iValue;
	}
}

template <typename T>
double CVoxel<T>::GetMax()
{
	double maxv = -FLT_MAX;
	int i, n = m_nX*m_nY*m_nZ;
	for ( i = 0; i < n; i++ ) {
		if ( m_pData[i] > maxv )
			maxv = m_pData[i];
	}
	return maxv;
}

template <typename T>
double CVoxel<T>::GetMin()
{
	double minv = FLT_MAX;
	int i, n = m_nX*m_nY*m_nZ;
	for ( i = 0; i < n; i++ ) {
		if ( m_pData[i] < minv )
			minv = m_pData[i];
	}
	return minv;
}
// ***** end of member functions: primitives ***** //	
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// ***** member functions: copying ***** //	
template <typename T>
template <typename TI>
void CVoxel<T>::CopyData( TI *pData )
{
	int x, y, z, i ;
	for( z = 0, i = 0; z < m_nZ ; z++ )
		for( y = 0 ; y < m_nY ; y++ )
			for( x = 0 ; x < m_nX ; x++, i++ )
				m_pData[i] = (T)pData[i];
}

template <typename T>
template <typename TI>
void CVoxel<T>::CopyData( CVoxel<TI>* src, CVoxel<BYTE> *mask )
{
	int x, y, z, i;
	for( z = 0; z < m_nZ ; z++ )
		for( y = 0; y < m_nY ; y++ )
			for( x = 0; x < m_nX ; x++, i++ )
				if (mask->m_pData[i] != 0)	
					m_pData[i] = (T)src->m_pData[i];
}

template <typename T>
template <typename TI>
void CVoxel<T>::Copy( CVoxel<TI> *src )
{
	if( m_nX == src->m_nX && m_nY == src->m_nY && m_nZ == src->m_nZ ) 	{
		CopyData<TI>(src->m_pData);
	}
	else {
		FreeData() ;
		Allocate(src->m_nX, src->m_nY, src->m_nZ);
		CopyData<TI>(src->m_pData);
	}
	CopySpOrg(src);
}

template <typename T>
template <typename TO>
CVoxel<TO> *CVoxel<T>::Copy()
{
	return new CVoxel<TO>(*this);
}
// ***** member functions: copying ***** //	
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// ***** member functions: conversions to other types ***** //	
template <typename T>
DImage CVoxel<T>::VoxelZSlice2DImage(int z)
{
	double den = 1.0/255.0;
	DImage dimg(m_nX, m_nY);
	for ( int y = 0; y < m_nY; y++ ) {
		for ( int x = 0; x < m_nX; x++ ) {
		dimg.pData[y*dimg.width()+x] = 
			(double)m_pData[x + y * m_nX + z * m_nX * m_nY]*den;
		}
	}
	return dimg;
}

template <typename T>
template <typename TO>	
cv::Mat CVoxel<T>::VoxelXSlice2cvMat(int x)
{
	cv::Mat dimg(m_nZ, m_nY, CV_MAKETYPE(cv::DataType<TO>::type, 1));
	for ( int z = 0; z < m_nZ; z++ ) {
		TO *ptr = dimg.ptr<TO>(z);
		for ( int y = 0; y < m_nY; y++ ) {
			ptr[y] = (TO)m_pData[x + y * m_nX + z * m_nX * m_nY];
		}
	}
	return dimg;
}

template <typename T>
template <typename TO>	
cv::Mat CVoxel<T>::VoxelYSlice2cvMat(int y)
{
	cv::Mat dimg(m_nX, m_nZ, CV_MAKETYPE(cv::DataType<TO>::type, 1));
	for ( int x = 0; x < m_nX; x++ ) {
		TO *ptr = dimg.ptr<TO>(x);
		for ( int z = 0; z < m_nZ; z++ ) {
			ptr[z] = (TO)m_pData[x + y * m_nX + z * m_nX * m_nY];
		}
	}
	return dimg;
}

template <typename T>
template <typename TO>	
cv::Mat CVoxel<T>::VoxelZSlice2cvMat(int z)
{
	cv::Mat dimg(m_nY, m_nX, CV_MAKETYPE(cv::DataType<TO>::type, 1));

	for ( int y = 0; y < m_nY; y++ ) {
		TO *ptr = dimg.ptr<TO>(y);
		for ( int x = 0; x < m_nX; x++ ) {
			ptr[x] = (TO)m_pData[x + y * m_nX + z * m_nX * m_nY];
		}
	}
	return dimg;
}
// ***** member functions: conversions to other types ***** //	
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// ***** member functions: UI ***** //	
template <typename T>
void CVoxel<T>::DrawCircleX(int cx, int cy, int sx, float rad, BYTE bv)
{
	if (sx < 0 || sx >= m_nX)
		return;
	int left = std::max((int)Round((float)cx - rad), 0);
	int right = std::min((int)Round((float)cx + rad), m_nY);
	int top = std::max((int)Round((float)cy - rad), 0);
	int bottom = std::min((int)Round((float)cy + rad), m_nZ);

	for (int i = left; i < right; i++)	{
		for (int j = top; j < bottom; j++)	{
			if ((i - cx)*(i - cx) + (j - cy)*(j - cy) < rad*rad) {
				m_pData[sx + i*m_nX + j*m_nX*m_nY] = bv;
			}
		}
	}
}

template <typename T>
void CVoxel<T>::DrawCircleY(int cx, int cy, int sy, float rad, BYTE bv)
{
	if ( sy < 0 || sy >= m_nY )
		return;
	int left	= std::max((int)Round((float)cx-rad),0);
	int right	= std::min((int)Round((float)cx+rad),m_nZ);
	int top		= std::max((int)Round((float)cy-rad),0);
	int bottom	= std::min((int)Round((float)cy+rad),m_nX);

	for ( int i = left; i < right; i++ )	{
		for ( int j = top; j < bottom; j++ )	{
			if ( (i-cx)*(i-cx) + (j-cy)*(j-cy) < rad*rad ) {
				m_pData[j + sy*m_nX + i*m_nX*m_nY] = bv;
			}
		}
	}
}

template <typename T>
void CVoxel<T>::DrawCircleZ(int cx, int cy, int sz, float rad, BYTE bv)
{
	if ( sz < 0 || sz >= m_nZ )
		return;
	int left	= std::max((int)Round((float)cx-rad),0);
	int right	= std::min((int)Round((float)cx+rad),m_nX);
	int top		= std::max((int)Round((float)cy-rad),0);
	int bottom	= std::min((int)Round((float)cy+rad),m_nY);

	for ( int i = left; i < right; i++ )	{
		for ( int j = top; j < bottom; j++ )	{
			if ( (i-cx)*(i-cx) + (j-cy)*(j-cy) < rad*rad ) {
				m_pData[i + j*m_nX + sz*m_nX*m_nY] = bv;
			}
		}
	}
}
// ***** end of member functions: UI ***** //	
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// ***** end of member functions: IO ***** //	
template <typename T>
bool CVoxel<T>::Read(CString fn)
{
	CString Ext ;
	Ext = fn.Right( 3 ) ;
	if( Ext == "bin" || Ext == "BIN" ) 
		return ReadFromBin( fn ) ;
	else if( Ext == "raw" || Ext == "RAW" || Ext == "lab" || Ext == "LAB" ) 
		return ReadFromRaw( fn ) ;
		//ReadFromLiverRaw( fn ) ;
	else if( Ext == "mhd" || Ext == "MHD" ) 
		return ReadFromMhd( fn ) ;
	/*
	else if( Ext == "hdr" || Ext == "HDR" || Ext == "nii" || Ext == "NII" || Ext == ".gz" || Ext == ".GZ" ) 
		ReadFromHdr( fn ) ;
	*/
	else if( Ext == "img" || Ext == "IMG" ) 
		return ReadFromImg( fn ) ;

	else if( Ext == "bmp" || Ext == "jpg" || Ext == "png") 
		return ReadFrom2DImg( fn ) ;

	else if( Ext == "dcm" || Ext == "DCM" ) 
		return ReadFromDCM( fn ) ;
		
	/*else if( Ext == "faw" || Ext == "FAW" )
	{
		CFVoxel *pfVoxel = new CFVoxel( fn ) ;
		
		m_nX = pfVoxel->m_nX ;
		m_nY = pfVoxel->m_nY ;
		m_nZ = pfVoxel->m_nZ ;
		
		xSp = -1.0f ; // 06/25/007
		ySp = -1.0f ;
		zSp = -1.0f ;
		xOrg = -1.0f ;
		yOrg = -1.0f ;
		zOrg = -1.0f ;

		Allocate( ) ;
		pfVoxel->ToVoxel( this ) ;
		delete pfVoxel ;
	}*/
	else return false;
}

template <typename T>
bool CVoxel<T>::ReadFromMsk( CString fileName, bool bBottomUp )
{
	int i, n, index, iSpOrg, nVoxel, nCh ;
	BYTE *pCh, ch ;
	CFile FP ;
	CFileException FE ;

	if( !FP.Open( fileName, CFile::modeRead|CFile::shareDenyNone, &FE ) ) {
		FP.Close( ) ;
		return false ;
	}

	FP.Read( &iSpOrg, sizeof( int ) );
	if( iSpOrg < 0 )	{
		FP.Read( &xSp, sizeof( float ) ) ;
		FP.Read( &ySp, sizeof( float ) ) ;
		FP.Read( &zSp, sizeof( float ) ) ;
		FP.Read( &xOrg, sizeof( float ) ) ;
		FP.Read( &yOrg, sizeof( float ) ) ;
		FP.Read( &zOrg, sizeof( float ) ) ;
		FP.Read( &m_nX, sizeof( int ) ) ;
	}
	else
		m_nX = iSpOrg ;

	FP.Read( &m_nY, sizeof( int ) ) ;
	FP.Read( &m_nZ, sizeof( int ) ) ;

	Allocate( ) ;

	nVoxel = m_nX * m_nY * m_nZ ;
	nCh = ( ( nVoxel % 8 ) == 0 ) ? ( nVoxel / 8 ) : ( nVoxel / 8 + 1 ) ;
	pCh = new BYTE[nCh] ;
	FP.Read( pCh, nCh ) ;

	for( n = 0 ; n < nCh ; n++ ) {
		ch = pCh[n] ;
		for( i = 0 ; i < 8 ; i++ ) {
			index = n * 8 + i ;
			if( index < nVoxel ) {
				if( ( ch & ( 0x80 >> i ) ) != 0 )
					m_pData[index] = 1 ;
				else
					m_pData[index] = 0 ;
			}
		}
	}

	FP.Close( ) ;
	delete [] pCh ;
	
	if( bBottomUp == true ) {
		int x, y, z, iValue ;
		CVoxel *pTemp = new CVoxel( m_nX, m_nY, 1 ) ;
		for( z = 0 ; z < m_nZ ; z++ ) {
			for( y = 0 ; y < m_nY ; y++ )
			for( x = 0 ; x < m_nX ; x++ ) {
				iValue = GetAt( x, y, z ) ;
				pTemp->SetAt( x, m_nY - y - 1, 0, iValue ) ;
			}
			for( y = 0 ; y < m_nY ; y++ )
			for( x = 0 ; x < m_nX ; x++ ) {
				iValue = pTemp->GetAt( x, y, 0 ) ;
				SetAt( x, y, z, iValue ) ;
			}
		}
		delete pTemp ;
	}

	return true ;
}

template <typename T>
bool CVoxel<T>::ReadFrom2DImg(CString FileName)
{
	cv::Mat label = cv::imread(((CStringA)FileName).GetBuffer(),0);

	m_nX = label.cols;
	m_nY = label.rows;
	m_nZ = 1;

	m_pData = new T[m_nX*m_nY*m_nZ];
	for ( int y = 0 ; y < m_nY ; y++ ) {
		uchar *ptr = label.ptr(y);
		for ( int x = 0 ; x < m_nX ; x++ ) {
			SetAt( x, y, 0, ptr[x] ) ;
		}
	}
	
	return true;
}

template <typename T>
bool CVoxel<T>::ReadFromBin(CString FileName)
{
	int x, y, z, iSpOrg ;
	char *pChar ;
	CFile FP ;
	CFileException FE ;

	if( !FP.Open( FileName, CFile::modeRead|CFile::shareDenyNone, &FE ) ) // 007/06/04
	{
		FP.Close( ) ;
		return false ;
	}

	FP.Read( &iSpOrg, sizeof( int ) ) ;
	if( iSpOrg < 0 )
	{
		FP.Read( &xSp, sizeof( float ) ) ;
		FP.Read( &ySp, sizeof( float ) ) ;
		FP.Read( &zSp, sizeof( float ) ) ;
		FP.Read( &xOrg, sizeof( float ) ) ;
		FP.Read( &yOrg, sizeof( float ) ) ;
		FP.Read( &zOrg, sizeof( float ) ) ;
		FP.Read( &m_nX, sizeof( int ) ) ;
	}
	else
		m_nX = iSpOrg ;

	FP.Read( &m_nY, sizeof( int ) ) ;
	FP.Read( &m_nZ, sizeof( int ) ) ;

	Allocate( ) ;

	pChar = new char[m_nX*m_nY] ;

	for ( z = 0 ; z < m_nZ ; z++ )
	{
		FP.Read( ( void FAR *)pChar, m_nX * m_nY ) ;
		for ( y = 0 ; y < m_nY ; y++ )
		for ( x = 0 ; x < m_nX ; x++ )
		{
			char ch = pChar[y*m_nX + x] ;
			SetAt( x, y, z, (int)ch ) ;
		}
	}

	FP.Close( ) ;
	delete [] pChar ;
	return true ;
}

template <typename T>
bool CVoxel<T>::ReadFromRaw(CString FileName)
{
	int x, y, z, iSpOrg, ImageSize, ImageBytes, Offset ;
	short *Origin ;
	CFile FP ;
	CFileException FE ;

	FP.Open( FileName, CFile::modeRead|CFile::shareDenyNone, &FE ) ;

	FP.Read( &iSpOrg, sizeof( int ) ) ;
	if( iSpOrg < 0 )
	{
		FP.Read( &xSp, sizeof( float ) ) ;
		FP.Read( &ySp, sizeof( float ) ) ;
		FP.Read( &zSp, sizeof( float ) ) ;
		FP.Read( &xOrg, sizeof( float ) ) ;
		FP.Read( &yOrg, sizeof( float ) ) ;
		FP.Read( &zOrg, sizeof( float ) ) ;
		FP.Read( &m_nX, sizeof( int ) ) ;
	}
	else
		m_nX = iSpOrg ;

	FP.Read( &m_nY, sizeof( int ) ) ;
	FP.Read( &m_nZ, sizeof( int ) ) ;
	Allocate( ) ;

	ImageSize = m_nX * m_nY ;
	ImageBytes = 2 * ImageSize ;
	Origin = new short[ ImageSize ] ;

	for( z = 0 ; z < m_nZ ; z++ )
	{
		FP.Read( ( void FAR *)Origin, ImageBytes ) ;
		for( y = 0 ; y < m_nY ; y++ )
		for( x = 0 ; x < m_nX ; x++ )
		{
			Offset = y * m_nX + x ;
			SetAt( x, y, z, (int)Origin[Offset] ) ;
		}
	}

	delete [] Origin ;
	FP.Close( ) ;
	return true ;
}

template <typename T>
bool CVoxel<T>::WriteToMsk( CString FileName, bool bBottomUp )
{
	if( bBottomUp == true )
	{
		int x, y, z, iValue ;
		CVoxel *pTemp = new CVoxel( m_nX, m_nY, 1 ) ;
		for( z = 0 ; z < m_nZ ; z++ )
		{
			for( y = 0 ; y < m_nY ; y++ )
			for( x = 0 ; x < m_nX ; x++ )
			{
				iValue = GetAt( x, y, z ) ;
				pTemp->SetAt( x, m_nY - y - 1, 0, iValue ) ;
			}
			for( y = 0 ; y < m_nY ; y++ )
			for( x = 0 ; x < m_nX ; x++ )
			{
				iValue = pTemp->GetAt( x, y, 0 ) ;
				SetAt( x, y, z, iValue ) ;
			}
		}
		delete pTemp ;
	}
	
	int i, n, index, iSpOrg, nVoxel, nCh ;
	BYTE *pCh, ch ;
	CFile FP ;

	FP.Open( FileName, CFile::modeCreate | CFile::modeWrite, NULL ) ;
	iSpOrg = -1 ;
	FP.Write( &iSpOrg, sizeof( int ) ) ;
	FP.Write( &xSp, sizeof( float ) ) ;
	FP.Write( &ySp, sizeof( float ) ) ;
	FP.Write( &zSp, sizeof( float ) ) ;
	FP.Write( &xOrg, sizeof( float ) ) ;
	FP.Write( &yOrg, sizeof( float ) ) ;
	FP.Write( &zOrg, sizeof( float ) ) ;

	FP.Write( &m_nX, sizeof( int ) ) ;
	FP.Write( &m_nY, sizeof( int ) ) ;
	FP.Write( &m_nZ, sizeof( int ) ) ;

	nVoxel = m_nX * m_nY * m_nZ ;
	nCh = ( ( nVoxel % 8 ) == 0 ) ? ( nVoxel / 8 ) : ( nVoxel / 8 + 1 ) ;
	pCh = new BYTE[nCh] ;
	for( n = 0 ; n < nCh ; n++ )
	{
		ch = 0x00 ;
		for( i = 0 ; i < 8 ; i++ )
		{
			index = n * 8 + i ;
			if( index < nVoxel )
			{
				if( m_pData[index] != 0 )
					ch |= ( 0x80 ) >> i ;
			}
		}
		pCh[n] = ch ;
	}

	FP.Write( pCh, nCh ) ;

	FP.Close( ) ;
	delete [] pCh ;
	return true ;
}

template <typename T>
bool CVoxel<T>::WriteToBin( CString FileName )
{
	int x, y, z, iSpOrg ;
	char *TempString = new char[5] ;
	char *pChar ;
	CFile FP ;

	FP.Open( FileName, CFile::modeCreate | CFile::modeWrite, NULL ) ;
	iSpOrg = -1 ;
	FP.Write( &iSpOrg, sizeof( int ) ) ;
	FP.Write( &xSp, sizeof( float ) ) ;
	FP.Write( &ySp, sizeof( float ) ) ;
	FP.Write( &zSp, sizeof( float ) ) ;
	FP.Write( &xOrg, sizeof( float ) ) ;
	FP.Write( &yOrg, sizeof( float ) ) ;
	FP.Write( &zOrg, sizeof( float ) ) ;

	FP.Write( &m_nX, sizeof( int ) ) ;
	FP.Write( &m_nY, sizeof( int ) ) ;
	FP.Write( &m_nZ, sizeof( int ) ) ;

	pChar = new char[ m_nX * m_nY ] ;
	
	for( z = 0 ; z < m_nZ ; z++ )
	{
		for( y = 0 ; y < m_nY ; y++ ) 
		for( x = 0 ; x < m_nX ; x++ ) 
			pChar[ y * m_nX + x ] = (char)GetAt( x, y, z ) ;
		FP.Write( (void FAR *)pChar, m_nX * m_nY ) ;
	}

	FP.Close( ) ;
	delete [] pChar ;
	delete [] TempString ;
	return true ;
}

template <typename T>
bool CVoxel<T>::WriteToRaw( CString FileName )
{
	int x, y, z, iSpOrg ;
	int ImageSize, ImageBytes, Offset ;
	short *Buffer ;
	CFile FP ;

	FP.Open( FileName, CFile::modeCreate | CFile::modeWrite, NULL ) ;

	iSpOrg = -1 ;
	FP.Write( &iSpOrg, sizeof( int ) ) ;
	FP.Write( &xSp, sizeof( float ) ) ;
	FP.Write( &ySp, sizeof( float ) ) ;
	FP.Write( &zSp, sizeof( float ) ) ;
	FP.Write( &xOrg, sizeof( float ) ) ;
	FP.Write( &yOrg, sizeof( float ) ) ;
	FP.Write( &zOrg, sizeof( float ) ) ;

	FP.Write( &m_nX, sizeof( int ) ) ;
	FP.Write( &m_nY, sizeof( int ) ) ;
	FP.Write( &m_nZ, sizeof( int ) ) ;

	ImageSize = m_nX * m_nY ;
	ImageBytes = 2 * ImageSize ;
	Buffer = new short[ ImageSize ] ;

	for( z = 0 ; z < m_nZ ; z++ )
	{
		for( y = 0 ; y < m_nY ; y++ )
		for( x = 0 ; x < m_nX ; x++ )
		{
			Offset = y * m_nX + x ; // 06/21/007
			Buffer[Offset] = (short)GetAt( x, y, z ) ;
		}
		FP.Write( (void FAR *)Buffer, ImageBytes ) ;
	}
	delete [] Buffer ;
	FP.Close( ) ;
	return true ;
}

template <typename T>
bool CVoxel<T>::ReadFromDCM(CString PathName)
{
	// Instantiate the reader
	vtkSmartPointer <vtkDICOMReader> reader =
	vtkSmartPointer <vtkDICOMReader>:: New ();
	if ( !(reader->CanReadFile ( (CStringA)PathName )) )
		return false;
	// Provide a multi-frame, multi-stack file
	reader->SetFileName ( (CStringA)PathName );
	// Read the meta data, get a list of stacks
	reader->UpdateInformation();

	// Update the reader
	reader->Update();
	vtkImageData *img = reader->GetOutput();

	/*
	VTK_CREATE( vtkDICOMImageReader, dreader );
	
	CStringA directory;
	directory = (CStringA)GetDirectoryPath(PathName);
	dreader->SetDirectoryName(directory);

	//CStringA fn = (CStringA)PathName;
	//dreader->SetFileName(fn);
	//dreader->Update();
	vtkImageData* img = dreader->GetOutput();
	*/

	int dims[3];
	double spacing[3];
	double origin[3];
	img->GetDimensions(dims);
	img->GetSpacing(spacing);
	img->GetOrigin(origin);

	m_nX = dims[0];
	m_nY = dims[1];
	m_nZ = dims[2];
	xSp = (float)spacing[0];
	ySp = (float)spacing[1];
	zSp = (float)spacing[2];
	xOrg = (float)origin[0];
	yOrg = (float)origin[1];
	zOrg = (float)origin[2];

	int IMGTYPE = img->GetScalarType();
	if( IMGTYPE == VTK_SHORT || IMGTYPE == VTK_UNSIGNED_SHORT )
	{
		Allocate( ) ;
		short* imgdata = ( short* )img->GetScalarPointer();
		int x,y,z;
		short int val;
		for( z = 0 ; z < m_nZ ; z++ ) {
			for( y = 0 ; y < m_nY ; y++ ) {
				for( x = 0 ; x < m_nX ; x++ ) {
					val = (T)imgdata[m_nX*m_nY*z+m_nX*y+x];
					SetAt(x,m_nY-1-y,z,val);
				}
			}
		}
	}
	else if ( IMGTYPE == VTK_UNSIGNED_CHAR ) {
		Allocate( ) ;
		unsigned char* imgdata = ( unsigned char* )img->GetScalarPointer();
		int x,y,z;
		short int val;
		for( z = 0 ; z < m_nZ ; z++ ) {
			for( y = 0 ; y < m_nY ; y++ ) {
				for( x = 0 ; x < m_nX ; x++ ) {
					val = (T)imgdata[m_nX*m_nY*z+m_nX*y+x];
					SetAt(x,m_nY-1-y,z,val);
				}
			}
		}
	}
	else if ( IMGTYPE == VTK_INT ) {
		Allocate( ) ;
		int* imgdata = ( int* )img->GetScalarPointer();
		int x,y,z;
		short int val;
		for( z = 0 ; z < m_nZ ; z++ ) {
			for( y = 0 ; y < m_nY ; y++ ) {
				for( x = 0 ; x < m_nX ; x++ ) {
					val = (T)imgdata[m_nX*m_nY*z+m_nX*y+x];
					SetAt(x,m_nY-1-y,z,val);
				}
			}
		}
	}
	else {
		Allocate( ) ;
		float* imgdata = ( float* )img->GetScalarPointer();
		int x,y,z;
		short int val;
		for( z = 0 ; z < m_nZ ; z++ ) {
			for( y = 0 ; y < m_nY ; y++ ) {
				for( x = 0 ; x < m_nX ; x++ ) {
					val = (T)Round(imgdata[m_nX*m_nY*z+m_nX*y+x]);
					SetAt(x,m_nY-1-y,z,val);
				}
			}
		}
		return false;
	}
	return true;
}

template <typename T>
bool CVoxel<T>::WriteToUcharMhd( CString PathName )
{
	//int iSpOrg;
	int x, y, z ;
	int ImageSize, ImageBytes, Offset ;
	uchar* Buffer ;

	//
	CStdioFile fpMeta;
	CString strMeta = _T(" ");
	CString strMetaData = _T(" ");
	if( !fpMeta.Open( PathName, CStdioFile::modeCreate | CStdioFile::modeWrite ) ) {
		return false;
	}

	fpMeta.WriteString(_T("ObjectType = Image\n"));
	fpMeta.WriteString(_T("NDims = 3\n"));
	fpMeta.WriteString(_T("BinaryData = True\n"));
	fpMeta.WriteString(_T("BinaryDataByteOrderMSB = False\n"));
	fpMeta.WriteString(_T("CompressedData = False\n"));
	fpMeta.WriteString(_T("TransformMatrix = 1 0 0 0 1 0 0 0 1\n"));
	fpMeta.WriteString(_T("Offset = 0 0 0\n"));
	fpMeta.WriteString(_T("CenterOfRotation = 0 0 0\n"));
	fpMeta.WriteString(_T("AnatomicalOrientation = RAI\n"));
	strMeta.Format(_T("ElementSpacing = %f %f %f\n"), xSp, ySp, zSp);
	fpMeta.WriteString(strMeta);
	strMeta.Format(_T("DimSize = %d %d %d\n"), m_nX, m_nY, m_nZ);
	fpMeta.WriteString(strMeta);
	fpMeta.WriteString(_T("ElementType = MET_UCHAR\n"));
	strMetaData = PathName.Left(PathName.ReverseFind('.')) + _T(".raw");
	strMeta = strMetaData.Right(strMetaData.GetLength() - 1 - strMetaData.ReverseFind('\\'));
	fpMeta.WriteString(_T("ElementDataFile = ") + strMeta + _T("\n"));

	fpMeta.Close();

	CFile FP ;
	FP.Open( strMetaData, CFile::modeCreate | CFile::modeWrite, NULL ) ;


	ImageSize = m_nX * m_nY ;
	ImageBytes = ImageSize ;
	Buffer = new uchar[ ImageSize ] ;

	for( z = 0 ; z < m_nZ ; z++ )
	{
		for( y = 0 ; y < m_nY ; y++ )
			for( x = 0 ; x < m_nX ; x++ )
			{
				Offset = y * m_nX + x ; // 06/21/007
				Buffer[Offset] = (uchar)GetAt( x, y, z ) ;
			}

			FP.Write( (void FAR *)Buffer, ImageBytes ) ;
	}

	delete [] Buffer ;
	FP.Close( ) ;
	return true ;
}

template <typename T>
bool CVoxel<T>::ReadFromImg(CString PathName)
{
	VTK_CREATE( vtkImageReader, readerS );
	readerS->SetFileDimensionality( 2 );
	readerS->SetDataExtent( 0, 2047, 0, 2047, 0, 0 );
	readerS->SetNumberOfScalarComponents( 1 );
	readerS->SetDataScalarTypeToUnsignedShort();
	readerS->SetDataByteOrderToBigEndian();
	readerS->SetFileName( (CStringA)PathName );
	readerS->Update();

	m_nX = 2048;
	m_nY = 2048;
	m_nZ = 1;
	m_pData = new T[m_nX*m_nY*m_nZ];

	unsigned short* imgdata = (unsigned short*) readerS->GetOutput()->GetScalarPointer();

	int x,y,z;
	for ( x = 0 ; x < m_nX; x++) {
		for ( y = 0 ; y < m_nY; y++) {
			for ( z = 0 ; z < m_nZ; z++) {
				T val = (T)imgdata[x+y*m_nX+z*m_nX*m_nY];
				SetAt(x,y,z, val);
			}
		}
	}

	return true ;
}

template <typename T>
bool CVoxel<T>::ReadFromMhd(CString PathName)
{
	//int iSpOrg;
	int ImageSize, ImageBytes, Offset ;

	CFile FP ;
	CFileException FE ;
	CString strDataFile = _T(" ");

	//
	CStdioFile fpMeta;
	if( !fpMeta.Open( PathName, CStdioFile::modeRead|CFile::shareDenyNone ) ) {
		return false;
	}

	CString strMetaType = _T(" ");
	CString strMetaData = _T(" ");
	CString strLine = _T(" ");
	CString strElementType = _T(" ");
	while( strLine.GetLength() > 0)
	{
		fpMeta.ReadString(strLine);
		if (strLine.GetLength() < 0) break; 

		int sep = strLine.Find(_T("="));
		strMetaType = strLine.Left(sep-1);
		strMetaData = strLine.Right(strLine.GetLength() - 2 - sep);

		if( strMetaType == "DimSize" ) {
			//sscanf((const char*)strMetaData, "%d %d %d", &m_nX, &m_nY, &m_nZ);
			sscanf_s((CStringA)strMetaData, "%d %d %d", &m_nX, &m_nY, &m_nZ);
		} else if ( strMetaType == "ElementSpacing" ) {
			//sscanf((const char*)strMetaData, "%f %f %f", &xSp, &ySp, &zSp);
			sscanf_s((CStringA)strMetaData, "%f %f %f", &xSp, &ySp, &zSp);
		} else if ( strMetaType == "ElementDataFile" ) {
			strDataFile = PathName.Left(PathName.ReverseFind('\\') + 1) + strMetaData;
		} 
		else if ( strMetaType == "ElementType" ) {
			strElementType =  strMetaData;
		} 
		else {

		}
	}
	fpMeta.Close();


	if( !FP.Open( strDataFile, CFile::modeRead|CFile::shareDenyNone, &FE ) ) // 007/06/04
	{
		CString str;
		switch ( FE.m_cause ) {
			case CFileException::none:
				str = "none"; break;
			case CFileException::genericException:
				str = "genericException"; break;
			case CFileException::fileNotFound:
				str = "fileNotFound"; break;
			case CFileException::badPath:
				str = "badPath"; break;
			case CFileException::tooManyOpenFiles:
				str = "tooManyOpenFiles"; break;
			case CFileException::accessDenied:
				str = "accessDenied"; break;
			case CFileException::invalidFile:
				str = "invalidFile"; break;
			case CFileException::removeCurrentDir:
				str = "removeCurrentDir"; break;
			case CFileException::directoryFull:
				str = "directoryFull"; break;
			case CFileException::badSeek:
				str = "badSeek"; break;
			case CFileException::hardIO:
				str = "hardIO"; break;
			case CFileException::sharingViolation:
				str = "sharingViolation"; break;
			case CFileException::lockViolation:
				str = "lockViolation"; break;
			case CFileException::diskFull:
				str = "diskFull"; break;
			case CFileException::endOfFile:
				str = "endOfFile"; break;
		}
		CString str_out; str_out.Format(_T("Cannot open mhd file %s"), str);
		AfxMessageBox(str_out);
		return false ;
	}
	Allocate( ) ;
	if ( strElementType == "MET_SHORT") {
		short *Origin ;
		ImageSize = m_nX * m_nY ;
		ImageBytes = 2*ImageSize ;
		Origin = new short[ ImageSize ] ;

		for( int z = 0 ; z < m_nZ ; z++ )
		{
			FP.Read( ( void FAR *)Origin, ImageBytes ) ;
			for( int i = 0, y = 0 ; y < m_nY ; y++ )
				for( int x = 0 ; x < m_nX ; x++, i++ )
				{
					Offset = y * m_nX + x ;
					SetAt( x, y, z, (T)Origin[i] ) ;
				}
		}

		delete [] Origin ;
	}
	else if ( strElementType == "MET_FLOAT") {
		float *Origin ;
		ImageSize = m_nX * m_nY ;
		ImageBytes = 4*ImageSize ;
		Origin = new float[ ImageSize ] ;
		
		for( int z = 0 ; z < m_nZ ; z++ )
		{
			FP.Read( ( void FAR *)Origin, ImageBytes ) ;
			for( int i = 0, y = 0 ; y < m_nY ; y++ )
				for( int x = 0 ; x < m_nX ; x++, i++ )
				{
					Offset = y * m_nX + x ;
					SetAt( x, y, z, (T)Origin[i] ) ;
				}
		}
		delete [] Origin ;
	}
	else {
		uchar *Origin ;
		ImageSize = m_nX * m_nY ;
		ImageBytes = ImageSize ;
		Origin = new uchar[ ImageSize ] ;

		for( int z = 0 ; z < m_nZ ; z++ )
		{
			FP.Read( ( void FAR *)Origin, ImageBytes ) ;
			for( int i = 0, y = 0 ; y < m_nY ; y++ )
				for( int x = 0 ; x < m_nX ; x++, i++ )
				{
					Offset = y * m_nX + x ;
					SetAt( x, y, z, (uchar)Origin[i] ) ;
				}
		}
		delete [] Origin ;
	}
	FP.Close( ) ;
	m_nMaxI = GetMax();
	m_nMinI = GetMin();

	return true;
}

template <typename T>
bool CVoxel<T>::WriteToMhd( CString PathName )
{
	//int iSpOrg;
	int x, y, z ;S
	int ImageSize, ImageBytes, Offset ;
	short* Buffer ;

	//
	CStdioFile fpMeta;
	CString strMeta = _T(" ");
	CString strMetaData = _T(" ");
	if( !fpMeta.Open( PathName, CStdioFile::modeCreate | CStdioFile::modeWrite ) ) {
		return false;
	}

	fpMeta.WriteString(_T("ObjectType = Image\n"));
	fpMeta.WriteString(_T("NDims = 3\n"));
	fpMeta.WriteString(_T("BinaryDataByteOrderMSB = False\n"));
	fpMeta.WriteString(_T("BinaryData = True\n"));
	fpMeta.WriteString(_T("CompressedData = False\n"));
	fpMeta.WriteString(_T("TransformMatrix = 1 0 0 0 1 0 0 0 1\n"));
	fpMeta.WriteString(_T("Offset = 0 0 0\n"));
	fpMeta.WriteString(_T("CenterOfRotation = 0 0 0\n"));
	fpMeta.WriteString(_T("AnatomicalOrientation = RAI\n"));
	strMeta.Format(_T("ElementSpacing = %f %f %f\n"), xSp, ySp, zSp);
	fpMeta.WriteString(strMeta);
	strMeta.Format(_T("DimSize = %d %d %d\n"), m_nX, m_nY, m_nZ);
	fpMeta.WriteString(strMeta);
	fpMeta.WriteString(_T("ElementType = MET_SHORT\n"));
	strMetaData = PathName.Left(PathName.ReverseFind('.')) + _T(".raw");
	strMeta = strMetaData.Right(strMetaData.GetLength() - 1 - strMetaData.ReverseFind('\\'));
	fpMeta.WriteString(_T("ElementDataFile = ") + strMeta + _T("\n"));

	fpMeta.Close();

	CFile FP ;
	FP.Open( strMetaData, CFile::modeCreate | CFile::modeWrite, NULL ) ;

	ImageSize = m_nX * m_nY ;
	ImageBytes = 2 * ImageSize ;
	Buffer = new short[ ImageSize ] ;

	for( z = 0 ; z < m_nZ ; z++ )
	{
		for( y = 0 ; y < m_nY ; y++ )
			for( x = 0 ; x < m_nX ; x++ )
			{
				Offset = y * m_nX + x ; // 06/21/007
				Buffer[Offset] = (short)GetAt( x, y, z ) ;
			}

			FP.Write( (void FAR *)Buffer, ImageBytes ) ;
	}

	delete [] Buffer ;
	FP.Close( ) ;
	return true ;
}

/*
bool CVoxel::ReadFromDcm( CString PathName )
{
	int iSpOrg, ImageSize, ImageBytes, Offset ;

	// GDCM library 

	GDCM_NAME_SPACE::File *dcmFile;
	GDCM_NAME_SPACE::FileHelper *dcmFileHelper;

	unsigned char* imageData;
	int dataSize;

	//std::string fname((LPCTSTR)PathName);
	CT2CA pszConvertedAnsiString(PathName);
	std::string fname(pszConvertedAnsiString);

	//	TODO: when filename contains UNICODE(Korean) characters...
	//	std::ifstream* Fp = new std::ifstream(fname.c_str(), std::ios::in | std::ios::binary);
	//	if ( ! *Fp ) {
	//		AfxMessageBox(_T("NULL"));
	//	}

	dcmFile = GDCM_NAME_SPACE::File::New( );
	dcmFile->SetFileName( fname );
	dcmFile->Load();
	if (!dcmFile->IsReadable()) {
		AfxMessageBox(_T("Not a Readable DICOM / ACR File"));
		dcmFile->Delete();
		return FALSE;
	}

	dcmFileHelper = GDCM_NAME_SPACE::FileHelper::New(dcmFile);
	imageData = dcmFileHelper->GetImageData();
	dataSize = dcmFileHelper->GetImageDataSize();

	m_nX = dcmFile->GetXSize();
	m_nY = dcmFile->GetYSize();
	m_nZ = dcmFile->GetZSize();
	Allocate( );

#ifdef SHOW_PROGRESSBAR
	CGProgressBar Bar(_T("Loding Voxel Data..."), 100);
#endif

	ImageSize = m_nX ;
	ImageBytes = 2 * ImageSize ;
	short* sp = (short*) imageData;

	m_nMaxI = SHRT_MIN;
	m_nMinI = SHRT_MAX;

	for(int i = 0, z = 0 ; z < m_nZ ; z++ )
	{
		for(int y = 0 ; y < m_nY ; y++ ) 
		{
			for(int x = 0; x < m_nX; x++, i++ ) 
			{
				SetAt(x, y, z, sp[i]);

				if(imageData[i] > m_nMaxI) m_nMaxI = sp[i];
				if(imageData[i] < m_nMinI) m_nMinI = sp[i];
			}

			// faster mode cannot calculate min/max intensity.
			//	memcpy( m_pData[z][y], sp, ImageBytes);
			//	sp += ImageSize;
		}

#ifdef SHOW_PROGRESSBAR
		Bar.SetPos(100*z/m_nZ);
#endif
	}

	dcmFile->Delete();
	dcmFileHelper->Delete();

	return TRUE;

}
*/
/*
bool CVoxel::ReadFromDcmFolder( CString PathName )
{
	int i, j, k;
	FILE* fp_data = NULL;
	FILE* fp_mask = NULL;
	FILE* fp_rng = NULL;
	short buf_data[2048];

	int vd_x, vd_y, vd_z, vd_sz, vd_z1, vd_z2;

	GDCM_NAME_SPACE::SerieHelper *dcmSerieHelper;

	CT2CA pszConvertedAnsiString(PathName);
	std::string dirName(pszConvertedAnsiString);

	dcmSerieHelper = GDCM_NAME_SPACE::SerieHelper::New();
	dcmSerieHelper->SetLoadMode(GDCM_NAME_SPACE::LD_ALL);     // Load everything for each File
	//GDCM_NAME_SPACE::TagKey t(0x0020,0x0013);
	//dcmSerieHelper->AddRestriction(t, "340", GDCM_NAME_SPACE::GDCM_LESS); 
	dcmSerieHelper->SetDirectory(dirName, FALSE); // true : recursive exploration

	GDCM_NAME_SPACE::FileList::const_iterator it;
	GDCM_NAME_SPACE::FileList *dcmFileList;
	GDCM_NAME_SPACE::FileHelper *dcmFileHelper;
	uint8_t * imageData;
	int dataSize;
	int nbFiles;
	double zspacing = 0.;

	// For all the Single SerieUID Files Sets of the GDCM_NAME_SPACE::Serie
	dcmFileList = dcmSerieHelper->GetFirstSingleSerieUIDFileSet();
	while (dcmFileList)
	{ 
		nbFiles = dcmFileList->size() ;
		//---------------------------------------------------------
		dcmSerieHelper->SetDropDuplicatePositions(false);
		dcmSerieHelper->OrderFileList(dcmFileList);  // sort the list (and compute ZSpacing !)
		//---------------------------------------------------------

#ifdef SHOW_PROGRESSBAR
		CGProgressBar Bar(_T("Loading Voxel Data..."), 100);
#endif

		zspacing = dcmSerieHelper->GetZSpacing();
		int k = 0;
		for ( std::vector<GDCM_NAME_SPACE::File* >::iterator it2 =  dcmFileList->begin();
			it2 != dcmFileList->end();
			++it2, ++k)
		{
			if (k == 0) {
				vd_x = (*it2)->GetXSize();
				vd_y = (*it2)->GetYSize();
				vd_z = nbFiles;

			}

			dcmFileHelper = GDCM_NAME_SPACE::FileHelper::New((*it2));
			imageData = dcmFileHelper->GetImageData();
			dataSize = dcmFileHelper->GetImageDataSize();

			short * imageDataTV = new short[vd_x * vd_y];
			memcpy(imageDataTV, imageData, dataSize);

#ifdef SHOW_PROGRESSBAR
			Bar.SetPos((100*(k))/vd_sz);
#endif

			int i,j;
			for (j = 0; j < vd_y; j++) {
				for (i = 0; i < vd_x; i++) {
					m_pData[k][j][i] = imageDataTV[j*vd_x + i];
				}
			}

			dcmFileHelper->Delete();
			delete imageDataTV;
		}  

		break; // we only deal with the first one, TODO: next ones
		dcmFileList = dcmSerieHelper->GetNextSingleSerieUIDFileSet();
	} 

	dcmSerieHelper->Delete();

	if (fp_mask) fclose(fp_mask);
	if (fp_rng)	 fclose(fp_rng);

	m_nX = vd_x;
	m_nY = vd_y;
	m_nZ = vd_sz;

	return TRUE;
}*/

/*
bool CVoxel::ReadFromHdr(CString PathName)
{
	nifti_image* img = nifti_image_read( CStringA(PathName), true );

	int x,y,z;
	m_nX = img->nx;
	m_nY = img->ny;
	m_nZ = img->nz;
	m_pData = new short int[m_nX*m_nY*m_nZ];

	xSp = img->dx; // 06/25/007
	ySp = img->dy;
	zSp = img->dz;

	xOrg = img->qoffset_x;
	yOrg = img->qoffset_y;
	zOrg = img->qoffset_z;

	if ( img->nbyper == 2 ) {
		short* darr = (short*) img->data;

		for ( x = 0 ; x < m_nX; x++) {
			for ( y = 0 ; y < m_nY; y++) {
				for ( z = 0 ; z < m_nZ; z++) {
					SetAt(x,y,z, darr[x+y*m_nX+z*m_nX*m_nY]);
				}
			}
		}

		delete [] darr;
	}

	else if ( img->nbyper == 1 ) {
		byte* darr = (byte*) img->data;

		for ( x = 0 ; x < m_nX; x++) {
			for ( y = 0 ; y < m_nY; y++) {
				for ( z = 0 ; z < m_nZ; z++) {
					SetAt(x,y,z, (short)darr[x+y*m_nX+z*m_nX*m_nY]);
				}
			}
		}

		delete [] darr;
	}

	else if ( img->nbyper == 4 ) {
		float* darr = (float*) img->data;

		
		for ( x = 0 ; x < m_nX; x++) {
			for ( y = 0 ; y < m_nY; y++) {
				for ( z = 0 ; z < m_nZ; z++) {
					int val = Round(darr[x+y*m_nX+z*m_nX*m_nY]);
					SetAt(x,y,z, val);
				}
			}
		}

		delete [] darr;
	}
	else {}
	SAFE_DELETE(img);	
	return true ;
}
*/

/*
template <typename T>
void CVoxel<T>::WriteToGz( CString FileName, CString standardName )
{
	nifti_image* img = nifti_image_read( CStringA(standardName), true );
	
	img->nx = m_nX;
	img->ny = m_nY;
	img->nz = m_nZ;
	
	int nAll = m_nX*m_nY*m_nZ;
	img->nvox = nAll;

	img->dim[1] = m_nX;
	img->dim[2] = m_nY;
	img->dim[3] = m_nZ;
	img->nbyper = 1;

	byte* nimgdata = new byte[nAll];
	for( int j = 0; j < nAll; j++ ) {
		nimgdata[j] = (byte)m_pData[j];
	}

	CStringA buffer1;
	buffer1 = (CStringA)FileName;
	char* buffer2 = new char[200];
	strcpy( buffer2, buffer1 );

	delete [] img->data;
	img->data = (void*)nimgdata ;
	img->fname = buffer2;
	img->iname = buffer2;
	nifti_image_write( img );

	SAFE_DELETE_ARRAY(buffer2);
	delete [] nimgdata;
	SAFE_DELETE(img);
}
*/
// ***** end of member functions: IO ***** //	
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// ***** member functions: manipulations / processing ***** //	
template <typename T>
CVoxel<T> *CVoxel<T>::MakeSmoothedX_raw(double *k_v, int *k_idx, int k_sz)
{
	CVoxel<T> *vxl = new CVoxel<T>(m_nX, m_nY, m_nZ);
	int x, y, z, i, x_;
	double sum, count;
	for ( z = 0; z < m_nZ ; z++ ) {
		int iz = z*m_nX*m_nY;
		for ( y = 0; y < m_nY; y++ ) {
			int izy = iz + y*m_nX;
			for ( x = 0; x < m_nX; x++ ) {
				sum = count = 0;
				for ( i = 0; i < k_sz; i++ ) {
					x_ = x+k_idx[i];
					if ( 0 <= x_ && x_ < m_nX ) {
						sum += m_pData[izy+x_]*k_v[i];
						count += k_v[i];
					}
				}
				vxl->m_pData[izy+x] = (T)Round(sum/count);
			}
		}
	}
	return vxl;
}

template <typename T>
CVoxel<T> *CVoxel<T>::MakeSmoothedY_raw(double *k_v, int *k_idx, int k_sz)
{
	CVoxel<T> *vxl = new CVoxel<T>(m_nX, m_nY, m_nZ);
	int x, y, z, i, y_;
	double sum, count;
	for ( z = 0; z < m_nZ ; z++ ) {
		int iz = z*m_nX*m_nY;
		for ( x = 0; x < m_nX; x++ ) {
			int izx = iz + x;
			for ( y = 0; y < m_nY; y++ ) {
				sum = count = 0;
				for ( i = 0; i < k_sz; i++ ) {
					y_ = y+k_idx[i];
					if ( 0 <= y_ && y_ < m_nY ) {
						sum += m_pData[izx+y_*m_nX]*k_v[i];
						count += k_v[i];
					}
				}
				vxl->m_pData[izx+y*m_nX] = (T)Round(sum/count);
			}
		}
	}
	return vxl;
}

template <typename T>
CVoxel<T> *CVoxel<T>::MakeSmoothedZ_raw(double *k_v, int *k_idx, int k_sz)
{
	CVoxel<T> *vxl = new CVoxel<T>(m_nX, m_nY, m_nZ);
	int x, y, z, i, z_;
	double sum, count;
	for ( y = 0; y < m_nY; y++ ) {
		int iy = y*m_nX;
		for ( x = 0; x < m_nX; x++ ) {
			int iyx = iy+x;
			for ( z = 0; z < m_nZ ; z++ ) {
				sum = count = 0;
				for ( i = 0; i < k_sz; i++ ) {
					z_ = z+k_idx[i];
					if ( 0 <= z_ && z_ < m_nZ ) {
						sum += m_pData[iyx+z_*m_nX*m_nY]*k_v[i];
						count += k_v[i];
					}
				}
				vxl->m_pData[iyx+z*m_nX*m_nY] = (T)Round(sum/count);
			}
		}
	}
	return vxl;
}

template <typename T>
template <typename TO>
CVoxel<TO>* CVoxel<T>::MakeDownsampleBy2_raw()
{
	int nX = (int) ceil(m_nX*0.5);
	int nY = (int) ceil(m_nY*0.5);
	int nZ = (int) ceil(m_nZ*0.5);

	CVoxel<TO> *hv = new CVoxel<TO>(nX,nY,nZ);
	T bvs[8];
	for (int z = 0; z < nZ; z++){
		for (int y = 0; y < nY; y++) {
			for (int x = 0; x < nX; x++) {
				GetBicubeValues(x, y, z, bvs);
				hv->SetAt(x,y,z, (TO)arrAvgNonZero<T>(bvs, 8));
			}
		}
	}
	return hv;
}

template <typename T>
template <typename TO>
CVoxel<TO>* CVoxel<T>::MakeDownsampleXBy2_raw()
{
	const int k_sz = 3;
	int k_idx[k_sz] = {-1,0,1};
	double k_v[k_sz] = {0.25,0.5,0.25};
	CVoxel<T> *smvxl = MakeSmoothedX_raw(k_v, k_idx, k_sz);
	int nX = (int) ceil(m_nX*0.5);
	CVoxel<TO> *vxl = new CVoxel<TO>(nX, m_nY, m_nZ);
	for ( int z = 0; z < m_nZ; z++ ) {
		for ( int y = 0; y < m_nY; y++ ) {
			for ( int x = 0; x < nX; x++ ) {
				vxl->SetAt(x, y, z, (TO)smvxl->GetAt(2*x, y, z));
			}
		}
	}
	SAFE_DELETE(smvxl);
	return vxl;
}
template <typename T>
template <typename TO>
CVoxel<TO>* CVoxel<T>::MakeDownsampleYBy2_raw()
{
	const int k_sz = 3;
	int k_idx[k_sz] = {-1,0,1};
	double k_v[k_sz] = {0.25,0.5,0.25};
	CVoxel<T> *smvxl = MakeSmoothedY_raw(k_v, k_idx, k_sz);
	int nY = (int) ceil(m_nY*0.5);
	CVoxel<TO> *vxl = new CVoxel<TO>(m_nX, nY, m_nZ);
	for ( int z = 0; z < m_nZ; z++ ) {
		for ( int y = 0; y < nY; y++ ) {
			for ( int x = 0; x < m_nX; x++ ) {
				vxl->SetAt(x, y, z, (TO)smvxl->GetAt(x, 2*y, z));
			}
		}
	}
	SAFE_DELETE(smvxl);
	return vxl;
}
template <typename T>
template <typename TO>
CVoxel<TO>* CVoxel<T>::MakeDownsampleZBy2_raw()
{
	const int k_sz = 3;
	int k_idx[k_sz] = {-1,0,1};
	double k_v[k_sz] = {0.25,0.5,0.25};
	CVoxel<T> *smvxl = MakeSmoothedZ_raw(k_v, k_idx, k_sz);
	int nZ = (int) ceil(m_nZ*0.5);
	CVoxel<TO> *vxl = new CVoxel<TO>(m_nX, m_nY, nZ);
	for ( int z = 0; z < nZ; z++ ) {
		for ( int y = 0; y < m_nY; y++ ) {
			for ( int x = 0; x < m_nX; x++ ) {
				vxl->SetAt(x, y, z, (TO)smvxl->GetAt(x, y, 2*z));
			}
		}
	}
	SAFE_DELETE(smvxl);
	return vxl;
}

template <typename T>
CVoxel<BYTE>* CVoxel<T>::MakeDownsampleBy2_bin(int nz_th)
{
	int nX = (int) ceil(m_nX*0.5);
	int nY = (int) ceil(m_nY*0.5);
	int nZ = (int) ceil(m_nZ*0.5);

	CVoxel<BYTE> *hv = new CVoxel<BYTE>(nX,nY,nZ);
	int bvs[8];
	int colorcount[16];
	for (int z = 0; z<nZ ; z++){
		for (int y = 0; y < nY; y++) {
			for (int x = 0; x < nX; x++) {
				GetBicubeValues(x, y, z, bvs);
				for (int i = 0; i < 16; i++)	
					colorcount[i] = 0;
				int count=0;
				int val=0;
				for (int i = 0; i < 8; i++) {
					if ( bvs[i] > 0 && bvs[i] <= 16 ) {
						count++;
						colorcount[bvs[i]]++;
					}	
				}
				int maxidx, maxval=0;
				if (count > nz_th) {
					for (int i = 1; i < 16; i++ ) {
						if (colorcount[i] > maxval) {
							maxval = colorcount[i];
							maxidx = i;
						}
					}
					hv->SetAt(x,y,z, maxidx);
				}
				else {
					hv->SetAt(x,y,z,0);
				}
			}
		}
	}
	return hv;
}

template <typename T>
template <typename TO, typename OP>
CVoxel<TO>* CVoxel<T>::Mask(T vi, TO vo, OP op)
{	
	CVoxel<T>* ov = new CVoxel(m_nX, m_nY, m_nZ);
	ov->SetToZero();
	Mask(vi, vo, ov, op);
	return ov;
}
template <typename T>
template <typename TO, typename OP>
void CVoxel<T>::Mask(T vi, TO vo, CVoxel<TO>* pV, OP op)
{	
	VERIFY(IsSameSize(pV));
	int x,y,z;
	for (int z = 0; z < m_nZ; z++ ) {
		for (int  y = 0; y < m_nY; y++ ) {
			for (int x = 0; x < m_nX; x++ ) {
				if (op(GetAt(x,y,z)))
					pV->SetAt(x,y,z,vo);
			}
		}
	}
}

// Sample points on the 3d object surface
// input
//	cv::Point3i intv: sampling interval in 3 dimensions
// output
//	cvPoint3iArray &sptarr: resulting point array
template <typename T>
void CVoxel<T>::SampleSurfacePts(cv::Point3i intv, cvPoint3iArray &sptarr)
{
	sptarr.RemoveAll();
	// construct voxel shell
	CVoxelShell shl(this);
	// convert voxel shell in vertex form back to voxel form 
	CVoxel *shlv = shl.Voxellize(m_nX,m_nY,m_nZ);
	int x, y, z, i;
	cv::Point3i ih = intv*0.5;
	for ( i = shl.npts-1; i >= 0; i-- ) {
		if ( shlv->GetAt(shl.pts[i]) ) {
			// include shell point
			sptarr.Add(shl.pts[i]);
			// remove adjacent points
			int sx = std::max(shl.pts[i].x-ih.x, 0);
			int sy = std::max(shl.pts[i].y-ih.y, 0);
			int sz = std::max(shl.pts[i].z-ih.z, 0);
			int ex = std::min(st.x+intv.x, m_nX);
			int ey = std::min(st.y+intv.y, m_nY);
			int ez = std::min(st.z+intv.z, m_nZ);
			for ( z = sz; z < ez; z++ ) {
				for ( y = sy; y < ey; y++ ) {
					for ( x= sx; x < ex; x++ ) {
						shlv->SetAt(x, y ,z, 0);
					}
				}
			}
		}
	}
	delete shlv;
}

template <typename T>
template <typename TO>
CVoxel<TO> *CVoxel<T>::GetGradient(const cv::Point3i &npt, double sigma)
{
	CVoxel<TO> *grad = new CFVoxel(m_nX, m_nY, m_nZ);
	grad->ClearToZeros();
	GetGradient(grad, npt, sigma);
	return grad;
}
template <typename T>
template <typename TO>
void CVoxel<T>::GetGradient(CVoxel<TO> *pV, const cv::Point3i &npt, double sigma)
{
	VERIFY(IsSameSize(pV));
	int x, y, z, i;
	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				cv::Point3i cnpt = cv::Point3i(x,y,z)+npt;
				if ( IsInBounds(cnpt) ) {
					grad->m_pfData[i] = 1 - exp(-fabs((double)m_pData[i]-GetAt(cnpt))/(sigma*sigma));
				}
			}
		}
	}
}

template <typename T>
template <typename OP>
void CVoxel<T>::GetROI_Op(OP op, cv::Point3i &st, cv::Point3i &end, cv::Point3i &sz=cv::Point3i())
{
	int i, x, y, z;
	// determine [min std::max] range in x, y, z directions
	int minX, maxX, minY, maxY, minZ, maxZ;
	minX = minY = minZ = INT_MAX;
	maxX = maxY = maxZ = INT_MIN;
	for( z = 0, i = 0; z < src->m_nZ ; z++ ) {
		for( y = 0 ; y < src->m_nY ; y++ ) {
			for( x = 0 ; x < src->m_nX ; x++, i++ ) {
				if ( op(src->m_pData[i]) ) {
					if ( x < minX ) minX = x;
					if ( y < minY ) minY = y;
					if ( z < minZ ) minZ = z;
					if ( x > maxX ) maxX = x;
					if ( y > maxY ) maxY = y;
					if ( z > maxZ ) maxZ = z;
				}
			}
		}
	}
	st = cv::Point3i(minX, minY, minZ);
	end = cv::Point3i(maxX, maxY, maxZ);
	sz = end-st;
}
template <typename T>
void CVoxel<T>::Crop(const cv::Point3i st, const cv::Point3i end, int BUF )
{
	CropFilled(this, st, end, BUF);
}
template <typename T>
template <typename TI>
void CVoxel<T>::Crop( CVoxel<TI> *src, const cv::Point3i st, const cv::Point3i &end, int BUF )
{
	CropFilled(src, sr, end, cv::Point3i(BUF, BUF, BUF), op);
}	
template <typename T>
template <typename TO>
CVoxel<TO>* CVoxel<T>::GetCropped(const cv::Point3i st, const cv::Point3i end, int BUF)
{
	return CropFilledwithReturn(st, end, cv::Point3i(BUF, BUF, BUF));
}
template <typename T>
template <typename TO>
CVoxel<TO>* CVoxel<T>::GetCropped(const cv::Point3i st, const cv::Point3i end, const cv::Point3i BUF)
{
	CVoxel<TO> *pVoxel = new CVoxel<TO>;
	pVoxel->CropFilled(this, st, end, BUF);
	return pVoxel;
}
template <typename T>
template <typename TI>
void CVoxel<T>::Crop( CVoxel<TI> *src, const cv::Point3i st, const cv::Point3i end, const cv::Point3i BUF )
{
	int i, x, y, z;
	// assign buffers for shape templates
	int minX = std::max(0, minX-BUF.x);
	int minY = std::max(0, minY-BUF.y);
	int minZ = std::max(0, minZ-BUF.z);
	int maxX = std::min(src->m_nX-1, maxX+BUF.x);
	int maxY = std::min(src->m_nY-1, maxY+BUF.y);
	int maxZ = std::min(src->m_nZ-1, maxZ+BUF.z);

	// crop only relevant voxels
	m_nX = maxX-minX+1;
	m_nY = maxY-minY+1;
	m_nZ = maxZ-minZ+1;

	T *pData = new T[m_nX*m_nY*m_nZ] ;
	for( z = minZ, i = 0; z <= maxZ; z++ ) {
		for( y = minY; y <= maxY; y++ ) {
			for( x = minX; x <= maxX; x++, i++ ) {
				pData[i] = (T)src->GetAt(x,y,z);
			}
		}
	}

	SAFE_DELETE_ARRAY(m_pData);
	m_pData = pData;
	SetOrg(src);
	SetSpace(src);
}

// ***** member functions: manipulations / processing ***** //	
//////////////////////////////////////////////////////////////////////

#pragma once

#include "sMIShopSettings.h"

#define INF 1e10
#ifndef PI
#define PI 3.141592654
#endif
#define UPOT_MAX 30
//#define UPOT_MAX 10000

#define VAL_EPSILON 1e-6

// histogram related constants
//#define I_MIN -1500
//#define I_MAX 3000

//#define I_MIN 0
//#define I_MAX 800
//#define HIST_BIN 400

//
//#define I_MIN -1100
//#define I_MAX 1700

#define I_MIN 0
#define I_MAX 400

//#define I_MAX 5000

//#define I_MAX 500
#define HIST_BIN 200


/////////////////////////////////////////////
// Math functions
/////////////////////////////////////////////
inline double Round( double x )
{
	return floor(x+0.5);
}

inline float Round( float x )
{
	return floor(x+0.5f);
}

inline double logB(double x, double base) {
  return log(x) / log(base);
}

#define SQR(x)   ( (x)*(x) )
#define LNorm(x)   ( fabs(x) )

template <class T>
inline T square(const T &x) { return x*x; };

template <class T>
inline int sign(const T &x) { if ( x > 0 ) return 1; else return -1; };

template <class T>
inline int MaxIdx3( const T &n1, const T &n2, const T &n3 ) 
{
	if (n1 > n2) {
		if (n1 > n3) {
			return 1;
		}
		else {
			return 3;
		}
	}
	else {
		if (n2 > n3) {
			return 2;
		}
		else {
			return 3;
		}
	}
}

inline int Max3( int n1, int n2, int n3 ) 
{
	if (n1 > n2) {
		if (n1 > n3) {
			return n1;
		}
		else {
			return n3;
		}
	}
	else {
		if (n2 > n3) {
			return n2;
		}
		else {
			return n3;
		}
	}
}

inline short int Max3(short int n1, short int n2, short int n3 ) 
{
	if (n1 > n2) {
		if (n1 > n3) {
			return n1;
		}
		else {
			return n3;
		}
	}
	else {
		if (n2 > n3) {
			return n2;
		}
		else {
			return n3;
		}
	}
}

template <class T>
bool CheckValRange(T &v, T &s, T&e)
{
	return ( s-VAL_EPSILON  <= v && v <= e+VAL_EPSILON  );
}

template <class T>
double arrAvg(T *arr, int narr)
{
	double avg = 0;
	for ( int i = 0; i < narr; i++ ) {
		avg += arr[i];
	}
	return avg/(double)narr;
}

template <class T>
double arrAvgNonZero(T *arr, int narr)
{
	double avg = 0;
	int nzcount = 0;
	for ( int i = 0; i < narr; i++ ) {
		if ( arr[i] > 0 ) {
			avg += arr[i];
			nzcount++;
		}
	}
	return nzcount > 0 ? avg/(double)nzcount : 0;
}

/////////////////////////////////////////////
// Directory manipulation
/////////////////////////////////////////////
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

/////////////////////////////////////////////
// File read/write
/////////////////////////////////////////////
extern char* wctoc(const WCHAR* wch, int strLength);

//////////////////////////////////////////////////////////////////////////
// string functions
extern void PadPathBackslash2String(CString &str);
extern void WritePathArray2OutStream(std::wofstream &outstream, CStringArray &strArr);
extern void WritePath2OutStream(std::wofstream &outstream, CString &str);
extern void ReadPathArrayFromInStream(std::wifstream &instream, int nArr, CStringArray &strArr);
extern void ReadPathFromInStream(std::wifstream &instream, CString &strArr);
extern CString GetDirectoryPath(CString str);
extern CString GetDCMDirectoryPath(CString str);
extern CString GetFileName(CString str);
extern CString GetFileDirName(CString path);
extern CString ExtendPathFileNameSetDir(CString path,  CString dir);
extern CString ExtendPathFileNameSetDir(CString path, CString extstr, CString dir);
extern CString ExtendPathFileNameSetDirExt(CString path, CString extstr, CString dir, CString ext);
extern CString GetFilenameWithoutExt(CString str);
CString ExtendPathFileName(CString path, CString extstr);
CString GetFileExtName(CString str);

//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////
// Point, Vector, Matrix Operations
/////////////////////////////////////////////
inline double RotatePtX( double x, double y, double centerX, double centerY, double sin_val, double cos_val )
{
	return (x-centerX)*cos_val - (y-centerY)*sin_val + centerX;
}
inline double RotatePtY( double x, double y, double centerX, double centerY, double sin_val, double cos_val )
{
	return (x-centerX)*sin_val + (y-centerY)*cos_val + centerY;
}
inline double RotatePtXOffset( double x, double y, double centerX, double centerY, double sin_val, double cos_val, double offsetX )
{
	return (x-centerX)*cos_val - (y-centerY)*sin_val + offsetX;
}
inline double RotatePtYOffset( double x, double y, double centerX, double centerY, double sin_val, double cos_val, double offsetY )
{
	return (x-centerX)*sin_val + (y-centerY)*cos_val + offsetY;
}
void MultFloat3x3Mat(float *a, float *b, float *v);
float *GetXRotMat( double rotx );
float *GetYRotMat( double roty );
float *GetZRotMat( double rotz );

/* dt of 1d function using squared distance */
float *dt(float *f, int n);

/////////////////////////////////////////////
// Estimation, Optimization Methods
/////////////////////////////////////////////

/////////////////////////////////////////////
// Linear Least-Square Solutions
/////////////////////////////////////////////

/////////////////////////////////////////////
// Quaternion Rotation
/////////////////////////////////////////////


/////////////////////////////////////////////
// Cost(Error) functions
/////////////////////////////////////////////


/////////////////////////////////////////////
// Display Methods
/////////////////////////////////////////////


/////////////////////////////////////////////
// Image Processing Methods
/////////////////////////////////////////////
void SaveBYTE2Image(BYTE *pB, int w, int h, int c, CStringA str);
void DrawKeypoints2cvMat(cv::Mat img, std::vector<cv::KeyPoint> kpts, 
	int rad, cv::Scalar col, int th);
void DrawPoints2cvMat(cv::Mat img, std::vector<cv::Point2f> kpts, 
	int rad, cv::Scalar col, int th);
void DrawPoints2cvMat(cv::Mat img, std::vector<cv::Point2f> kpts, 
	int rad, std::vector<cv::Scalar> cols, int th);
cv::Mat DrawMatchingPoints2cvMat(
	cv::Mat img1, std::vector<cv::Point2f> kpts1, 
	cv::Mat img2, std::vector<cv::Point2f> kpts2, 
	int rad, int th, bool bVertical=false);


//////////////////////////////////////////////////////////////////////////////////////////
// memory related methods
//////////////////////////////////////////////////////////////////////////////////////////
#define SAFE_DELETE(pt) if ( pt ) delete pt; pt = NULL;
#define SAFE_DELETE_ARRAY(pt) if ( pt ) delete [] pt; pt = NULL;
#define SAFE_DELETE_IPLIMAGE(pt) if (pt) cvReleaseImage(&pt); pt = NULL;
#define SAVE_DELETE_CVMAT(mat) if ( mat ) cvReleaseMat(&mat); mat = NULL;


template <class T>
inline void SAFE_DELETE_2DARRAY( T** arr, int n )
{
	if (n) {
		for ( int i = 0; i < n; i++ )
		{
			SAFE_DELETE_ARRAY(arr[i]);
		}
		SAFE_DELETE_ARRAY(arr);
	}
}


template <class T>
inline void ClearArrayPtrArray(CTypedPtrArray<CPtrArray, T*> &ptrArr)
{
	int size = (int) ptrArr.GetSize();
	if ( size ) {
		for (int i = 0; i < size; i++ ) {
			T *ptr = ptrArr.GetAt(i);
			SAFE_DELETE_ARRAY(ptr);
		}
		ptrArr.RemoveAll();
	}
}

template <class T>
inline void ClearPtrArray(CTypedPtrArray<CPtrArray, T*> &ptrArr)
{
	int size = (int) ptrArr.GetSize();
	if ( size ) {
		for (int i = 0; i < size; i++ ) {
			T *ptr = ptrArr.GetAt(i);
			SAFE_DELETE(ptr);
		}
		ptrArr.RemoveAll();
	}
}

template <class T>
inline void ClearPtr2DArray(CTypedPtrArray<CPtrArray, T*> **pptrArr, int n)
{
	CTypedPtrArray<CPtrArray, T*> *ptrArr = *pptrArr;
	if ( ptrArr ) {
		for ( int i = 0; i < n; i++ ) {
			int size = (int) ptrArr[i].GetSize();
			if ( size ) {
				for (int j = 0; j < size; j++ ) {
					T *ptr = ptrArr[i].GetAt(j);
					SAFE_DELETE(ptr);
				}
				ptrArr[i].RemoveAll();
			}
		}
		SAFE_DELETE_ARRAY(ptrArr);
		*pptrArr = ptrArr;
	}
}

// concatenate two CTypedPtrArrays
template <class T>
void ConcatenatePtrArr(CTypedPtrArray<CPtrArray, T*> &a1, 
									 CTypedPtrArray<CPtrArray, T*> &a2, 
									 CTypedPtrArray<CPtrArray, T*> &aout)
{
	// clear out array
	if ( (int) aout.GetSize() ) 
		ClearPtrArray(aout);
	// add elements of first array into out array
	for ( int i = 0; i < (int) a1.GetSize(); i++ ) {
		aout.Add(a1.GetAt(i));
	}
	// add elements of second array into out array
	for ( int i = 0; i < (int) a2.GetSize(); i++ ) {
		aout.Add(a2.GetAt(i));
	}
}

// concatenate two CArrays
template <class T>
void ConcatenateArr(CArray<T> &a1, CArray<T> &a2, CArray<T> &aout)
{
	// clear out array
	if ( (int) aout.GetSize() ) 
		aout.RemoveAll();
	// add elements of first array into out array
	for ( int i = 0; i < (int) a1.GetSize(); i++ ) {
		aout.Add(a1.GetAt(i));
	}
	// add elements of second array into out array
	for ( int i = 0; i < (int) a2.GetSize(); i++ ) {
		aout.Add(a2.GetAt(i));
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
// Clustering vectors by affinity propagation 
//double** APClusterVectors( double** inputVectorArr, int nVec, int nVecDim, int &nExemplars,
//						  double (*func)(double *v1, double *v2, int nDim), int nMinIter=1, int nConvIter=200, int nMaxIter=2000, double fLambda=0.6 );
void APClusterVectors( double** inputVectorArr, int nVec, int nVecDim, 
					  int &nExemplars, int **clusterIdxs, int **exList, double ***exemplars,
					  double (*func)(double *v1, double *v2, int nDim), 
					  int nMinIter=1, int nConvIter=200, int nMaxIter=2000, double fLambda=0.9 );
double VectorL2Dist(double* v1, double* v2, int size);
double VectorL2Dist( cv::Point3i& v1, cv::Point3i& v2);
double VectorL2Dist( cv::Point3f& v1, cv::Point3f& v2);

double *CopyDoubleVector(double *ptr, int n);
float *CvMat2FloatPtr(CvMat *mat);
CvMat *FloatPtr2CvMat(float *fptr, int nRow, int nCol);
void CvMat2DoublePtrArray(CvMat *mat, DoublePtrArray &coeffArr, int nPC, BOOL rowVect=TRUE);

extern void ReadDirFiles(CStringArray &fileArr, const CString ext);
extern void ReadDirFiles(CStringArray &fileArr, const CString ext, const CString fnkey);
extern void ReadDirFiles(CStringArray &fileArr, const CString ext, const CString fnkey, const CString fnkey2);
extern void ReadDirFiles(CStringArray &fileArr, CStringArray &extArr, const CString fnkey);
extern void ReadDirFiles(CStringArray &fileArr, CStringArray &extArr, CStringArray &fnkeyArr);
extern void ReadDirFiles(CStringArray &fileArr, CStringArray &extArr);
extern void ReadDirDCMFiles(CStringArray &fileArr, const CString ext);
//extern void ReadDirFiles(CStringArray &fileArr, CString &extArr);


template <class T>
int GetIdxNum( T *ptArr, T &pt, int npt ) 
{
	int idx;
	for ( int i = 0; i < npt; i++ ) {
		if ( ptArr[i] == pt ) {
			idx = i;
			break;
		}
	}
	return idx;
}

////////////////////////////////////////////////////////////////////////
// operator classes to be used as parameters for masking voxels/images
template<typename _MaskValT>
class iop_mask_zero
{
public:
	bool operator()(_MaskValT mvx) const
	{
		return mvx==0;
	}
};

template<typename _MaskValT>
class iop_mask_nzero //not-zero
{
public:
	
	bool operator()(_MaskValT mvx) const
	{
		return mvx!=0;
	}
};

template<typename _MaskValT>
class iop_mask_ge
{
	const _MaskValT m_mval;
public:
	iop_mask_ge(const _MaskValT mval)
		:m_mval(mval)
	{
	}
	bool operator()(_MaskValT mvx) const
	{
		return mvx>=m_mval;
	}
};

template<typename _MaskValT>
class iop_mask_eq
{
	const _MaskValT m_mval;
public:
	iop_mask_eq(const _MaskValT mval)
		:m_mval(mval)
	{
	}
	bool operator()(_MaskValT mvx) const
	{
		return mvx==m_mval;
	}
};
template<typename _MaskValT>
class iop_mask_le
{
	const _MaskValT m_mval;
public:
	iop_mask_le(const _MaskValT mval)
		:m_mval(mval)
	{
	}
	bool operator()(_MaskValT mvx) const
	{
		return mvx<=m_mval;
	}
};

template<typename _MaskValT>
class iop_mask_neq
{
	const _MaskValT m_mval;
public:
	iop_mask_neq(const _MaskValT mval)
		:m_mval(mval)
	{
	}
	bool operator()(_MaskValT mvx) const
	{
		return mvx!=m_mval;
	}
};

template<typename _MaskValT>
class iop_mask_rng //In-Range
{
	const _MaskValT m_mvbeg,m_mvend;
public:
	iop_mask_rng(const _MaskValT mvbeg,const _MaskValT mvend)
		:m_mvbeg(mvbeg),m_mvend(mvend)
	{
	}
	bool operator()(_MaskValT mvx) const
	{
		return mvx>=m_mvbeg&&mvx<m_mvend;
	}
};

template<typename _MaskValT>
class iop_mask_nrng //Not-In-Range
{
	const _MaskValT m_mvbeg,m_mvend;
public:
	iop_mask_nrng(const _MaskValT mvbeg,const _MaskValT mvend)
		:m_mvbeg(mvbeg),m_mvend(mvend)
	{
	}
	bool operator()(_MaskValT mvx) const
	{
		return mvx<m_mvbeg||mvx>=m_mvend;
	}
};
// end of operator classes
////////////////////////////////////////////////////////////////////////

extern "C" void Trace ( char *_fmt, ... );

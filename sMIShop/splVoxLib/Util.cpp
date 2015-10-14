#include "stdafx.h"
#include "Util.h"
//#include "ap_globals.h"
#include "Voxel.h"
//#include <fstream>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////
// File read/write
/////////////////////////////////////////////
//=====================================================================================
/*
|| ::DESCRIPTION::
|| This function will convert a WCHAR string to a CHAR string.
||
|| Param 1 :: Pointer to a buffer that will contain the converted string. Ensure this
|| buffer is large enough; if not, buffer overrun errors will occur.
|| Param 2 :: Constant pointer to a source WCHAR string to be converted to CHAR
*/
char* wctoc(const WCHAR* wch, int strLength)
{
	size_t convertedChars = 0;
	size_t  sizeInBytes = ((strLength + 1) * 2);
	errno_t err = 0;
	char    *ch = (char *)malloc(sizeInBytes);

	err = wcstombs_s(&convertedChars, 
								 ch, sizeInBytes,
								 wch, sizeInBytes);
	if (err != 0)
		AfxMessageBox(_T("wcstombs_s  failed!\n"));

	return ch;
}

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg) {
	case BFFM_INITIALIZED:
		char buffer[MAX_PATH];
		_getcwd(buffer, MAX_PATH);
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM) buffer);
		break;
	}
	return 0;
}

void PadPathBackslash2String(CString &str)
{
	int length = str.GetLength();
	for ( int i = length-1; i > 0; i-- ) {
		if( str[i] == 92 ) {
			str.Insert(i, 92);
		}
	}
}

CString GetDirectoryPath(CString str)
{
	return str.Left(str.ReverseFind(92));
}

CString GetDCMDirectoryPath(CString str)
{
	return str.Left(str.ReverseFind(92)-1);
}

CString GetFileName(CString str)
{
	return str.Right(str.GetLength()-str.ReverseFind(92)-1);
}

CString GetFileDirName(CString path)
{
	CString dirPath = path.Left(path.ReverseFind(92)-1);
	CString dirName = GetFileName(dirPath);
	return dirName;
}

CString ExtendPathFileNameSetDir(CString path, CString extstr, CString dir)
{
	CString fileName = GetFilenameWithoutExt(path);
	CString ext = GetFileExtName(path);
	CString extpath; extpath.Format(_T("%s\\%s%s.%s"), dir, fileName, extstr, ext);
	return extpath;
}

CString ExtendPathFileNameSetDir(CString path,  CString dir)
{
	CString fileName = GetFilenameWithoutExt(path);
	CString extpath; extpath.Format(_T("%s\\%s"), dir, fileName);
	return extpath;
}

CString ExtendPathFileNameSetDirExt(CString path, CString extstr, CString dir, CString ext)
{
	CString fileName = GetFilenameWithoutExt(path);
	CString extpath; extpath.Format(_T("%s\\%s%s.%s"), dir, fileName, extstr, ext);
	return extpath;
}

CString GetFilenameWithoutExt(CString str)
{
	CString strfile = str.Right(str.GetLength()- str.ReverseFind(92)-1);
	if ( strfile.ReverseFind('.') == -1 )
		return strfile;
	else 
		return strfile.Left(strfile.ReverseFind('.'));
}

/////////////////////////////////////////////
// Memory alloc, dealloc
/////////////////////////////////////////////

void MultFloat3x3Mat(float *a, float *b, float *v)
{
	v[0] = a[0]*b[0] + a[1]*b[3] + a[2]*b[6];
	v[1] = a[0]*b[1] + a[1]*b[4] + a[2]*b[7];
	v[2] = a[0]*b[2] + a[1]*b[5] + a[2]*b[8];
	v[3] = a[3]*b[0] + a[4]*b[3] + a[5]*b[6];
	v[4] = a[3]*b[1] + a[4]*b[4] + a[5]*b[7];
	v[5] = a[3]*b[2] + a[4]*b[5] + a[5]*b[8];
	v[6] = a[6]*b[0] + a[7]*b[3] + a[8]*b[6];
	v[7] = a[6]*b[1] + a[7]*b[4] + a[8]*b[7];
	v[8] = a[6]*b[2] + a[7]*b[5] + a[8]*b[8];
}

float *GetXRotMat( double rotx )
{
	float *v = new float[9];
	v[0] = 1.f;
	v[1] = 0.f;
	v[2] = 0.f;
	v[3] = 0.f;
	v[4] = (float)cos(rotx);
	v[5] = -(float)sin(rotx);
	v[6] = 0.f;
	v[7] = (float)sin(rotx);
	v[8] = (float)cos(rotx);
	return v;
}

float *GetYRotMat( double roty )
{
	float *v = new float[9];
	v[0] = (float)cos(roty);
	v[1] = 0.f;
	v[2] = (float)sin(roty);
	v[3] = 0.f;
	v[4] = 1.f;
	v[5] = 0.f;
	v[6] = -(float)sin(roty);
	v[7] = 0.f;
	v[8] = (float)cos(roty);
	return v;
}

float *GetZRotMat( double rotz )
{
	float *v = new float[9];
	v[0] = (float)cos(rotz);
	v[1] = -(float)sin(rotz);
	v[2] = 0.f;
	v[3] = (float)sin(rotz);
	v[4] = (float)cos(rotz);
	v[5] = 0.f;
	v[6] = 0.f;
	v[7] = 0.f;
	v[8] = 1.f;
	return v;
}

/////////////////////////////////////////////
// Image Processing Methods
/////////////////////////////////////////////
void SaveBYTE2Image(BYTE *pB, int w, int h, int c, CStringA str)
{
	cv::Mat img(h, w, CV_MAKETYPE(cv::DataType<BYTE>::type, c), pB);
	cv::imwrite(str.GetBuffer(), img);
}

void DrawKeypoints2cvMat(cv::Mat img, std::vector<cv::KeyPoint> kpts, 
	int rad, cv::Scalar col, int th)
{
	for (int i = 0; i < (int)kpts.size(); i++ ) {
		cv::circle(img, kpts[i].pt, rad, col, th);
	}
}
void DrawPoints2cvMat(cv::Mat img, std::vector<cv::Point2f> kpts, 
	int rad, cv::Scalar col, int th)
{
	for (int i = 0; i < (int)kpts.size(); i++ ) {
		cv::circle(img, kpts[i], rad, col, th);
	}
}
void DrawPoints2cvMat(cv::Mat img, std::vector<cv::Point2f> kpts, 
	int rad, std::vector<cv::Scalar> cols, int th)
{
	for (int i = 0; i < (int)kpts.size(); i++ ) {
		cv::circle(img, kpts[i], rad, cols[i], th);
	}
}
cv::Mat DrawMatchingPoints2cvMat(
	cv::Mat img1, std::vector<cv::Point2f> kpts1, 
	cv::Mat img2, std::vector<cv::Point2f> kpts2, 
	int rad, int th, bool bVertical)
{
	// compute concatenated image dimension
	int cimgr, cimgc, osx, osy;
	if ( bVertical ) {
		cimgr = img1.rows + img2.rows;
		cimgc = std::max(img1.cols, img2.cols);
		osx = 0;
		osy = img1.rows;
	}
	else {
		cimgr = std::max(img1.rows, img2.rows);
		cimgc = img1.cols + img2.cols;
		osx = img1.cols;
		osy = 0;
	}

	cv::Mat cimg = cv::Mat::zeros(cimgr, cimgc, CV_8UC3);
	cv::Mat cimg_c1(cimg, cv::Rect(0,0, img1.cols, img1.rows));
	cv::Mat img1_copy = img1;
	if ( img1.channels() != 3 ) {
		cv::cvtColor(img1, img1_copy, CV_GRAY2BGR);
	}
	img1_copy.copyTo(cimg_c1);
	cv::Mat cimg_c2(cimg, cv::Rect(osx,osy, img2.cols, img2.rows));
	cv::Mat img2_copy = img2;
	if ( img2.channels() != 3 ) {
		cv::cvtColor(img2, img2_copy, CV_GRAY2BGR);
	}
	img2_copy.copyTo(cimg_c2);
	int nkpts = kpts1.size();
	if ( kpts2.size() != nkpts ) {
		printf("ERROR at Util::DrawMatchingPoints2cvMat(): keypoint array sizes do not match\n");
		return cv::Mat();
	}
	srand(NULL);
	for ( int i = 0; i < nkpts; i++ ) {
		uchar rr = (int)Round((double)rand()/double(RAND_MAX+1)*255.0);
		uchar rg = (int)Round((double)rand()/double(RAND_MAX+1)*255.0);
		uchar rb = (int)Round((double)rand()/double(RAND_MAX+1)*255.0);
		cv::Scalar col(rb, rg, rr);
		cv::Point pt1((int)Round(kpts1[i].x), (int)Round(kpts1[i].y));
		cv::Point pt2((int)Round(kpts2[i].x+osx), (int)Round(kpts2[i].y+osy));
		cv::circle(cimg, pt1, rad, col);
		cv::circle(cimg, pt2, rad, col);
		cv::line(cimg, pt1, pt2, col, 1);
	}
	return cimg;
}

float *dt(float *f, int n)
{
	float *d = new float[n];
	int *v = new int[n];
	float *z = new float[n+1];
	int k = 0, q ;
	v[0] = 0;
	z[0] = -FLT_MAX + 1 ;
	z[1] = +FLT_MAX;
	for( q = 1 ; q <= n-1 ; q++ )
	{
		float s  = ( ( f[q] + square(q) ) - ( f[v[k]] + square(v[k]) ) ) / ( 2 * q - 2 * v[k] ) ;
		while( s <= z[k] )
		{
			k--;
			s  = ( ( f[q] + square(q) ) - ( f[v[k]] + square( v[k] ) ) ) / ( 2 * q - 2 * v[k] ) ;
		}
		k++;
		v[k] = q;
		z[k] = s;
		z[k+1] = +FLT_MAX;
	}

	k = 0;
	for( q = 0; q <= n-1; q++) {
		while (z[k+1] < q)
			k++;
		d[q] = square(q-v[k]) + f[v[k]];
	}

	delete [] v;
	delete [] z;
	return d;
}

/*
void APClusterVectors( double** inputVectorArr, int nVec, int nVecDim, 
								  int &nExemplars, int **clusterIdxs, int **exList, double ***exemplars,
								  double (*func)(double *v1, double *v2, int nDim), 
								  int nMinIter, int nConvIter, int nMaxIter, double fLambda )
{
	// **** getting ready to apply affinity propagation functions **** //
	importFunction apcluster32;
	HMODULE dlh = NULL;
	if (!(dlh=LoadLibrary(_T("apclusterwin.dll")))) {
		TRACE("LoadLibrary() failed: %d\n", GetLastError()); 
	}
	if (!(apcluster32=(importFunction)(GetProcAddress(dlh, "apcluster32")))) {
		TRACE("GetProcAddress() failed: %d\n", GetLastError()); 
	}

	APOPTIONS apoptions={0};
	apoptions.cbSize = sizeof(APOPTIONS);
	apoptions.lambda = fLambda;//0.6;
	apoptions.minimum_iterations = nMinIter;//1;
	apoptions.converge_iterations = nConvIter;// 200;
	apoptions.maximum_iterations = nMaxIter;// 2000;
	apoptions.nonoise = 0;
	apoptions.progress=callback; apoptions.progressf=NULL;

	// **** cluster components by affinity propagation **** //
	int n = nVec; 
	int n_sqr = n*n;
	double *S_SUM = new double[n_sqr];
	unsigned int *idx_i = new unsigned int[n_sqr];
	unsigned int *idx_j = new unsigned int[n_sqr];
	double min_sim = DBL_MAX;
	double mean_sim = 0;

	// **** compute similarities **** //
	//double** distMat = new double*[nVec];
	for (int i=0;i<n;i++)
	{
		//distMat[i] = new double[nVec];
		for (int j=0;j<n;j++)
		{
			//distMat[i][j] = (*func)(inputVectorArr[i], inputVectorArr[j], nVecDim);
			//S_SUM[i*n+j] = -distMat[i][j];
			S_SUM[i*n+j] = -(*func)(inputVectorArr[i], inputVectorArr[j], nVecDim);
		}
	}
	//for (int i=0;i<n;i++)	delete [] distMat[i];
	//delete [] distMat;

	for (int i = 0; i < n; i++ ) {
		for (int j = 0; j < n; j++ ) {
			int ij = i*n+j;	
			int ji = j*n+i;
			if ( S_SUM[ij] < min_sim ) min_sim = S_SUM[ij];
			mean_sim += S_SUM[ij];
		}
		// store i & j index values
		for (int j = 0; j < n; j++ ) {
			idx_i[i*n+j] = i;	idx_j[i*n+j] = j;
		}
	}

	// **** set preferences as the minimum of the similarities **** //
	mean_sim /= n_sqr;
	//mean_sim /= n_sqr-n;
	for (int i = 0; i < n; i++ ) {
		//S_SUM[i*n+i] = min_sim;
		S_SUM[i*n+i] = mean_sim;
	}

	// * variable for returning the final net similarity * /
	int niter; 
	double netsim = 0.0; 

	// * actual function calls * /
	//int* clusterIdx; 
	//clusterIdx = (int*)calloc(n,sizeof(*clusterIdx));
	*clusterIdxs = new int[n];
	//int apcluster32(double *sij, unsigned int *i, unsigned int *j, unsigned int N, int *idx, double *netsim, APOPTIONS *apoptions);

	//niter = apcluster32(S_SUM, idx_i, idx_j, n_sqr, clusterIdx, &netsim, &apoptions); 
	niter = apcluster32(S_SUM, idx_i, idx_j, n_sqr, *clusterIdxs, &netsim, &apoptions); 
	delete [] idx_i;
	delete [] idx_j;
	delete [] S_SUM;

	// **** success: print out the indices of the foreground patches **** //
	nExemplars = 0; 
	//int* exList = new int[nVec];
	(*exList) = new int[nVec];
	if ( niter > 0 ) {
		for (int i = 0; i < n; i++ ) {
			BOOL new_cluster_idx = TRUE;
			for (int j = 0; j < nExemplars; j++ ) {
				//if ( clusterIdx[i] == exList[j] ) {
				if ( (*clusterIdxs)[i] == (*exList)[j] ) {
					new_cluster_idx = FALSE;
				}
			}
			if ( new_cluster_idx ) {
				//exList[nExemplars++] = clusterIdx[i];
				(*exList)[nExemplars++] = (*clusterIdxs)[i];
			}
		}
	}
#if 0
	double** Exampler = new double*[nExemplars];
	for (int i = 0; i < nExemplars; i++ ) {
		Exampler[i] = new double[nVecDim];
		for (int j = 0; j < nVecDim; j++ ) {
			Exampler[i][j] = inputVectorArr[exList[i]][j];
		}
	}
#endif
	*exemplars = new double*[nExemplars];
	for (int i = 0; i < nExemplars; i++ ) {
		(*exemplars)[i] = new double[nVecDim];
		for (int j = 0; j < nVecDim; j++ ) {
			(*exemplars)[i][j] = inputVectorArr[(*exList)[i]][j];
		}
	}
	//delete [] exList;
	//delete clusterIdx;
	//return Exampler;
}
*/

double VectorL2Dist(double* v1, double* v2, int size)
{
	double dist=0;

	for (int i=0;i<size;i++)
	{
		dist += (v1[i]-v2[i])*(v1[i]-v2[i]);
	}
	dist = sqrt(dist);

	return dist;
}

double VectorL2Dist( cv::Point3i& v1, cv::Point3i& v2)
{
	double dist=0;
	double x= v1.x;	double y= v1.y;	double z= v1.z;
	double cx= v2.x;	double cy= v2.y;	double cz= v2.z;

	dist = (cx-x)*(cx-x) + (cy-y)*(cy-y) + (cz-z)*(cz-z); 
	dist = sqrt(dist);
	return dist;
}

double VectorL2Dist( cv::Point3f& v1, cv::Point3f& v2)
{
	double dist=0;
	double x= v1.x;		double y= v1.y;	double z= v1.z;
	double cx= v2.x;	double cy= v2.y;	double cz= v2.z;

	dist = (cx-x)*(cx-x) + (cy-y)*(cy-y) + (cz-z)*(cz-z); 
	dist = sqrt(dist);
	return dist;
}
void WritePathArray2OutStream(std::wofstream &outstream, CStringArray &strArr)
{
	CString str;
	for ( int i = 0; i < (int) strArr.GetSize(); i++ ) {
		str = strArr[i]; 
		outstream << str.GetBuffer() << endl; 
		str.ReleaseBuffer();
	}
}

void WritePath2OutStream(std::wofstream &outstream, CString &str)
{
	outstream << str.GetBuffer() << endl; 
	str.ReleaseBuffer();
}

void ReadPathArrayFromInStream(std::wifstream &instream, int nArr, CStringArray &strArr)
{
	for ( int i = 0; i < nArr; i++ ) {
		WCHAR strtemp[MAX_PATH];
		instream >> strtemp; 
		strArr.Add(CString(strtemp));
	}
}

void ReadPathFromInStream(std::wifstream &instream, CString &str)
{
	WCHAR strtemp[MAX_PATH];
	instream >> strtemp; 
	str = CString(strtemp);
}

double *CopyDoubleVector(double *ptr, int n)
{
	double *cptr = new double[n];
	for ( int i = 0; i < n; i++ ) {
		cptr[n] = ptr[n];
	}
	return cptr;
}

float *CvMat2FloatPtr(CvMat *mat)
{
	float *res = new float[mat->rows*mat->cols];
	int x, y, i;
	for ( y = 0, i=0; y < mat->rows; y++ ) {
		for ( x = 0; x < mat->cols; x++, i++ ) {
			res[i] = (float)cvmGet( mat, y, x );
		}
	}
	return res;
}

void CvMat2DoublePtrArray(CvMat *mat, DoublePtrArray &coeffArr, int nPC, BOOL rowVect)
{
	if ( rowVect ) {
		VERIFY(nPC <= mat->cols);
		int i, j, nVect = mat->rows;
		double *vec = NULL;
		for ( i = 0; i < nVect; i++ ) {
			vec = new double[nPC];
			for ( j = 0; j < nPC; j++ ) {
				vec[j] = cvmGet(mat, i, j);
			}
			coeffArr.Add(vec);
		}
	}
	else {
		VERIFY(nPC <= mat->rows);
		int i, j, nVect = mat->cols;
		double *vec = NULL;
		for ( i = 0; i < nVect; i++ ) {
			vec = new double[nPC];
			for ( j = 0; j < nPC; j++ ) {
				vec[j] = cvmGet(mat, j, i);
			}
			coeffArr.Add(vec);
		}
	}
}

CvMat *FloatPtr2CvMat(float *fptr, int nRow, int nCol)
{
	CvMat *mat = cvCreateMat(nRow, nCol, CV_32F);
	int x, y, i;
	for ( y = 0, i = 0; y < nRow; y++ ) {
		for ( x = 0; x < nCol; x++, i++ ) {
			cvmSet(mat, y, x, fptr[i]);
		}
	}
	return mat;
}


void ReadDirDCMFiles(CStringArray &fileArr, const CString ext)
{
	CString filename, pathname;
	CFileFind file_finder;
	BOOL bWorking = file_finder.FindFile(NULL);

	while (bWorking) {
		bWorking = file_finder.FindNextFile();
		filename = file_finder.GetFileName();
		pathname = file_finder.GetFilePath();
		if ( !(file_finder.IsDirectory()) ) {
			if ( filename.Find(ext) != -1 ) {
				int length = pathname.GetLength();
				for ( int i = length-1; i > 0; i-- ) {
					if( pathname[i] == 92 ) {
						pathname.Insert(i, 92);
					}
				}
				fileArr.Add(pathname);
				break;
			}
		}
	}

	CFileFind dir_finder;
	BOOL bIsDir = dir_finder.FindFile(NULL);

	while (bIsDir) {
		bIsDir   = dir_finder.FindNextFile();
		filename = dir_finder.GetFileName();
		if ( dir_finder.IsDirectory() ) {
			if ( !dir_finder.IsDots() ) {
				_wchdir((WCHAR *)filename.GetString());
				ReadDirDCMFiles(fileArr, ext);
				_chdir("..");
			}
		} 
	}
}

void ReadDirFiles(CStringArray &fileArr, const CString ext, const CString fnkey)
{
	CString filename, pathname;
	CFileFind file_finder;
	BOOL bWorking = file_finder.FindFile(NULL);

	while (bWorking) {
		bWorking = file_finder.FindNextFile();
		filename = file_finder.GetFileName();
		pathname = file_finder.GetFilePath();
		if ( !(file_finder.IsDirectory()) ) {
			if ( filename.Find(ext) != -1 ) {
				int length = pathname.GetLength();
				for ( int i = length-1; i > 0; i-- ) {
					if( pathname[i] == 92 ) {
						pathname.Insert(i, 92);
					}
				}
				if ( filename.Find(fnkey) != -1 )
					fileArr.Add(pathname);
			}
		}
	}

	CFileFind dir_finder;
	BOOL bIsDir = dir_finder.FindFile(NULL);

	while (bIsDir) {
		bIsDir   = dir_finder.FindNextFile();
		filename = dir_finder.GetFileName();
		if ( dir_finder.IsDirectory() ) {
			if ( !dir_finder.IsDots() ) {
				_wchdir((WCHAR *)filename.GetString());
				ReadDirFiles(fileArr, ext, fnkey);
				_chdir("..");
			}
		} 
	}
}

void ReadDirFiles(CStringArray &fileArr, const CString ext, const CString fnkey, const CString fnkey2)
{
	CString filename, pathname;
	CFileFind file_finder;
	BOOL bWorking = file_finder.FindFile(NULL);

	while (bWorking) {
		bWorking = file_finder.FindNextFile();
		filename = file_finder.GetFileName();
		pathname = file_finder.GetFilePath();
		if ( !(file_finder.IsDirectory()) ) {
			if ( filename.Find(ext) != -1 ) {
				int length = pathname.GetLength();
				for ( int i = length-1; i > 0; i-- ) {
					if( pathname[i] == 92 ) {
						pathname.Insert(i, 92);
					}
				}
				if ( filename.Find(fnkey) != -1 && filename.Find(fnkey2) != -1 )
					fileArr.Add(pathname);
			}
		}
	}

	CFileFind dir_finder;
	BOOL bIsDir = dir_finder.FindFile(NULL);

	while (bIsDir) {
		bIsDir   = dir_finder.FindNextFile();
		filename = dir_finder.GetFileName();
		if ( dir_finder.IsDirectory() ) {
			if ( !dir_finder.IsDots() ) {
				_wchdir((WCHAR *)filename.GetString());
				ReadDirFiles(fileArr, ext, fnkey, fnkey2);
				_chdir("..");
			}
		} 
	}
}

void ReadDirFiles(CStringArray &fileArr, const CString ext)
{
	CString filename, pathname;
	CFileFind file_finder;
	BOOL bWorking = file_finder.FindFile(NULL);

	while (bWorking) {
		bWorking = file_finder.FindNextFile();
		filename = file_finder.GetFileName();
		pathname = file_finder.GetFilePath();
		if ( !(file_finder.IsDirectory()) ) {
			if ( filename.Find(ext) != -1 ) {
				int length = pathname.GetLength();
				for ( int i = length-1; i > 0; i-- ) {
					if( pathname[i] == 92 ) {
						pathname.Insert(i, 92);
					}
				}
				fileArr.Add(pathname);
				//if ( filename.Find(fnkey) != -1 )
				//	fileArr.Add(pathname);
			}
		}
	}

	CFileFind dir_finder;
	BOOL bIsDir = dir_finder.FindFile(NULL);

	while (bIsDir) {
		bIsDir   = dir_finder.FindNextFile();
		filename = dir_finder.GetFileName();
		if ( dir_finder.IsDirectory() ) {
			if ( !dir_finder.IsDots() ) {
				_wchdir((WCHAR *)filename.GetString());
				ReadDirFiles(fileArr, ext);
				_chdir("..");
			}
		} 
	}
}

void ReadDirFiles(CStringArray &fileArr, CStringArray &extArr, CString fnkey)
{
	CString filename, pathname;
	CFileFind file_finder;
	BOOL bWorking = file_finder.FindFile(NULL);
	int nExt = (int) extArr.GetSize();
	int x;

	while (bWorking) {
		bWorking = file_finder.FindNextFile();
		filename = file_finder.GetFileName();
		pathname = file_finder.GetFilePath();
		if ( !(file_finder.IsDirectory()) ) {
			for ( x = 0; x < nExt; x++ ) {
				if ( filename.Find(extArr[x]) != -1 ) {
					int length = pathname.GetLength();
					for ( int i = length-1; i > 0; i-- ) {
						if( pathname[i] == 92 ) {
							pathname.Insert(i, 92);
						}
					}
					if ( filename.Find(fnkey) != -1 )
						fileArr.Add(pathname);
				}
			}
		}
	}

	CFileFind dir_finder;
	BOOL bIsDir = dir_finder.FindFile(NULL);

	while (bIsDir) {
		bIsDir   = dir_finder.FindNextFile();
		filename = dir_finder.GetFileName();
		if ( dir_finder.IsDirectory() ) {
			if ( !dir_finder.IsDots() ) {
				_wchdir((WCHAR *)filename.GetString());
				ReadDirFiles(fileArr, extArr, fnkey);
				_chdir("..");
			}
		} 
	}
}

void ReadDirFiles(CStringArray &fileArr, CStringArray &extArr, CStringArray &fnkeyArr)
{
	CString filename, pathname;
	CFileFind file_finder;
	BOOL bWorking = file_finder.FindFile(NULL);
	int nExt = (int) extArr.GetSize();
	int nKw = (int) fnkeyArr.GetSize();
	int x, y;

	while (bWorking) {
		bWorking = file_finder.FindNextFile();
		filename = file_finder.GetFileName();
		pathname = file_finder.GetFilePath();
		if ( !(file_finder.IsDirectory()) ) {
			for ( x = 0; x < nExt; x++ ) {
				if ( filename.Find(extArr[x]) != -1 ) {
					int length = pathname.GetLength();
					for ( int i = length-1; i > 0; i-- ) {
						if( pathname[i] == 92 ) {
							pathname.Insert(i, 92);
						}
					}
					for ( y = 0; y < nKw; y++ ) {
						if ( filename.Find(fnkeyArr[y]) != -1 )
							fileArr.Add(pathname);
					}
				}
			}
		}
	}

	CFileFind dir_finder;
	BOOL bIsDir = dir_finder.FindFile(NULL);

	while (bIsDir) {
		bIsDir   = dir_finder.FindNextFile();
		filename = dir_finder.GetFileName();
		if ( dir_finder.IsDirectory() ) {
			if ( !dir_finder.IsDots() ) {
				_wchdir((WCHAR *)filename.GetString());
				ReadDirFiles(fileArr, extArr, fnkeyArr);
				_chdir("..");
			}
		} 
	}
}


void ReadDirFiles(CStringArray &fileArr, CStringArray &extArr)
{
	CString filename, pathname;
	CFileFind file_finder;
	BOOL bWorking = file_finder.FindFile(NULL);
	int nExt = (int) extArr.GetSize();
	int x;

	while (bWorking) {
		bWorking = file_finder.FindNextFile();
		filename = file_finder.GetFileName();
		pathname = file_finder.GetFilePath();
		if ( !(file_finder.IsDirectory()) ) {
			for ( x = 0; x < nExt; x++ ) {
				if ( filename.Find(extArr[x]) != -1 ) {
					int length = pathname.GetLength();
					for ( int i = length-1; i > 0; i-- ) {
						if( pathname[i] == 92 ) {
							pathname.Insert(i, 92);
						}
					}
					fileArr.Add(pathname);
				}
			}
		}
	}

	CFileFind dir_finder;
	BOOL bIsDir = dir_finder.FindFile(NULL);

	while (bIsDir) {
		bIsDir   = dir_finder.FindNextFile();
		filename = dir_finder.GetFileName();
		if ( dir_finder.IsDirectory() ) {
			if ( !dir_finder.IsDots() ) {
				_wchdir((WCHAR *)filename.GetString());
				ReadDirFiles(fileArr, extArr);
				_chdir("..");
			}
		} 
	}
}

/*
void ReadDirFiles(CStringArray &fileArr, CString &ext)
{
	CString filename, pathname;
	CFileFind file_finder;
	BOOL bWorking = file_finder.FindFile(NULL);

	while (bWorking) {
		bWorking = file_finder.FindNextFile();
		filename = file_finder.GetFileName();
		pathname = file_finder.GetFilePath();
		if ( !(file_finder.IsDirectory()) ) {
			if ( filename.Find(ext) != -1 ) {
				int length = pathname.GetLength();
				for ( int i = length-1; i > 0; i-- ) {
					if( pathname[i] == 92 ) {
						pathname.Insert(i, 92);
					}
				}
				fileArr.Add(pathname);
			}
		}
	}

	CFileFind dir_finder;
	BOOL bIsDir = dir_finder.FindFile(NULL);

	while (bIsDir) {
		bIsDir   = dir_finder.FindNextFile();
		filename = dir_finder.GetFileName();
		if ( dir_finder.IsDirectory() ) {
			if ( !dir_finder.IsDots() ) {
				_wchdir((WCHAR *)filename.GetString());
				ReadDirFiles(fileArr, ext);
				_chdir("..");
			}
		} 
	}
}
*/

CString ExtendPathFileName(CString path, CString extstr)
{
	CString dirPath = path.Left(path.ReverseFind(46));
	CString ext = GetFileExtName(path);
	CString extpath; extpath.Format(_T("%s%s.%s"), dirPath, extstr, ext);
	return extpath;
}

CString GetFileExtName(CString str)
{
	return str.Right(str.GetLength()-str.ReverseFind(46)-1);
}


void Trace ( char *_fmt, ... )
{
	char out[256];
	va_list body;
	va_start ( body, _fmt );
	vsprintf_s ( out, _fmt, body );
	va_end ( body );
	int dsize = MultiByteToWideChar ( CP_ACP, 0, (char*)out, -1, NULL, NULL );
	WCHAR *des = new WCHAR[dsize];
	MultiByteToWideChar ( CP_ACP, 0, (char*)out, dsize, des, dsize );
	OutputDebugString ( des );
	delete[] des;
}

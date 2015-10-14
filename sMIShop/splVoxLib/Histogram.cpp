// Histogram.cpp: implementation of the CBilayerHistogram class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "Histogram.h"
#include "Voxel.h"
#include "Util.h"
#include "VoxelShell.h"
//#include <fstream>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


int WND = 1;
double w[3] = {0.15, 0.7, 0.15};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CHistogram implementation

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
CHistogram::CHistogram()
{
	m_nHistBin = 256 ;

	m_iMin = SHRT_MAX;
	m_iMax = SHRT_MIN;
	
	m_hist = new double[m_nHistBin] ;
	for( int j = 0 ; j < m_nHistBin ; j++ )
		m_hist[j] = 0.0f ;
}

CHistogram::CHistogram(int n)
{
	m_nHistBin = n;

	m_iMin = SHRT_MAX;
	m_iMax = SHRT_MIN;

	m_hist = new double[m_nHistBin] ;
	for( int j = 0 ; j < m_nHistBin ; j++ )
		m_hist[j] = 0.0f ;
}

CHistogram::CHistogram(short imin, short imax, int n)
{
	m_nHistBin = n;

	m_iMin = imin;
	m_iMax = imax;

	m_hist = new double[m_nHistBin] ;
	for( int j = 0 ; j < m_nHistBin ; j++ )
		m_hist[j] = 0.0f ;
}

CHistogram::CHistogram(CVoxel *vxl) : 
	m_hist(NULL)
{
	ComputeFixedLengthHistogram(vxl);
}

CHistogram::CHistogram(CVoxel *vxl, CVoxel *roi) : 
	m_hist(NULL)
{
	ComputeFixedLengthHistogramWithROI(vxl, roi);
}

CHistogram::CHistogram(CString str) : 
	m_hist(NULL)
{
	Load(str);
}

CHistogram::CHistogram(CTypedPtrArray<CPtrArray, CHistogram*> &histArr)
	: m_hist(NULL), m_iMin(SHRT_MAX), m_iMax(SHRT_MIN)
{
	ComputeMeanHistogram(histArr);
}


CHistogram::~CHistogram()
{
	ClearHistogram();
}
//////////////////////////////////////////////////////////////////////
// initialization, recollection
void CHistogram::InitHistogram()
{
	SAFE_DELETE_ARRAY(m_hist);
	m_hist = new double[m_nHistBin] ;
	for( int j = 0 ; j < m_nHistBin ; j++ )
		m_hist[j] = 0.0f ;
}

void CHistogram::ClearHistogram()
{
	SAFE_DELETE_ARRAY( m_hist ) ;
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// get, set methods
int CHistogram::GetNumHistBin()
{
	return m_nHistBin;
}

void CHistogram::SetNumHistBin(int n)
{
	m_nHistBin = n;
}

void CHistogram::SetIMax(short i)
{
	m_iMax = i;
}

void CHistogram::SetIMin(short i)
{
	m_iMin = i;
}

short CHistogram::GetIMax()
{
	return m_iMax;
}

short CHistogram::GetIMin()
{
	return m_iMin;
}
//////////////////////////////////////////////////////////////////////////

void CHistogram::ComputeBdHistogram(CVoxel *pat, CVoxelShell *bd, cv::Point3i pPt, int nSmoothHist)
{
	int nx = pat->m_nX;
	int ny = pat->m_nY;
	int nz = pat->m_nZ;
	int ngbx, ngby, ngbz;
	int count = 0;
	for ( int i = 0; i < bd->npts; i++ )  {
		short pI, pnI; 
		pI = pat->GetAt(bd->pts[i]);
		
		ngbx = bd->pts[i].m_x-pPt.m_x;
		ngby = bd->pts[i].m_y-pPt.m_y;
		ngbz = bd->pts[i].m_z-pPt.m_z;
		if ( ngbx >= 0 && ngbx < nx && ngby >= 0 && ngby < ny && ngbz >= 0 && ngbz < nz ) {
			if ( pI != (pnI=pat->GetAt(bd->pts[i]-pPt)) ) {
				short iV = std::max(std::min(abs(pI-pnI), I_MAX), I_MIN);
				float intensity = ( (float)iV - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
				int binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
				// increment related bin values
				m_hist[binidx] += 1.0f ;
				count++;
			}
		}

		ngbx = bd->pts[i].m_x+pPt.m_x;
		ngby = bd->pts[i].m_y+pPt.m_y;
		ngbz = bd->pts[i].m_z+pPt.m_z;
		if ( ngbx >= 0 && ngbx < nx && ngby >= 0 && ngby < ny && ngbz >= 0 && ngbz < nz ) {
			if ( pI != (pnI=pat->GetAt(bd->pts[i]+pPt)) ) {
				short iV = std::max(std::min(abs(pI-pnI), I_MAX), I_MIN);
				float intensity = ( (float)iV - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
				int binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
				// increment related bin values
				m_hist[binidx] += 1.0f ;
				count++;
			}
		}
		
	}

	// normalize histogram
	float denom = 1.0f / (float)count ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[j] *= denom;
	}

	// smooth histogram
	for ( int i = 0; i < nSmoothHist; i++ ) {
		SmoothHistogram();
	}
}

void CHistogram::ComputeFGGradHistogram(CVoxel *pat, CVoxel *lbl, int fg, 
															CVoxel *exc, cv::Point3i pPt, int nSmoothHist)
{
	InitHistogram();
	// compute histogram
	int count = 0; 
	short iValue;
	float intensity;
	int binidx;
	int x, y, z, i;
	int nx = pat->m_nX;
	int ny = pat->m_nY;
	int nz = pat->m_nZ;
	cv::Point3i edpt(nx, ny, nz);
	for( z = 0; z < nz; z++ ) {
		for( y = 0 ; y < ny; y++ ) {
			for( x = 0 ; x < nx; x++ ) {
				cv::Point3i xpt(x,y,z);
				cv::Point3i npt = xpt+pPt;
				if ( ZERO_3DPT <= npt && npt < edpt ) {
					if ( lbl->GetAt(xpt)==fg && lbl->GetAt(npt)==fg && exc->GetAt(xpt)==0 ) {
						int _i = abs(pat->GetAt(x, y, z)-pat->GetAt(npt));
						iValue = std::max(std::min((int)_i, I_MAX), I_MIN);
						intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
						binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
						// increment related bin values
						m_hist[binidx] += 1.0f ;
						count++;
					}
				}
			}
		}
	}

	// normalize histogram
	float denom = 1.0f / (float)count ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[j] *= denom;
	}

	// smooth histogram
	for ( i = 0; i < nSmoothHist; i++ ) {
		SmoothHistogram();
	}

}

void CHistogram::ComputeBGGradHistogram(CVoxel *pat, CVoxel *lbl, int fg, 
															CVoxel *exc, cv::Point3i pPt, int nSmoothHist)
{
	InitHistogram();
	// compute histogram
	int count = 0; 
	short iValue;
	float intensity;
	int binidx;
	int x, y, z, i;
	int nx = pat->m_nX;
	int ny = pat->m_nY;
	int nz = pat->m_nZ;
	cv::Point3i edpt(nx, ny, nz);
	for( z = 0; z < nz; z++ ) {
		for( y = 0 ; y < ny; y++ ) {
			for( x = 0 ; x < nx; x++ ) {
				cv::Point3i xpt(x,y,z);
				cv::Point3i npt = xpt+pPt;
				if ( ZERO_3DPT <= npt && npt < edpt ) {
					if ( lbl->GetAt(xpt)!=fg && lbl->GetAt(npt)!=fg && exc->GetAt(xpt)==0 ) {
						int _i = abs(pat->GetAt(x, y, z)-pat->GetAt(npt));
						iValue = std::max(std::min(_i, I_MAX), I_MIN);
						intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
						binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
						// increment related bin values
						m_hist[binidx] += 1.0f ;
						count++;
					}
				}
			}
		}
	}

	// normalize histogram
	float denom = 1.0f / (float)count ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[j] *= denom;
	}

	// smooth histogram
	for ( i = 0; i < nSmoothHist; i++ ) {
		SmoothHistogram();
	}

}

void CHistogram::MakeCombinePairwiseHist( CHistogram *inputHist, CHistogram *bdHist )
{
	m_nHistBin = inputHist->m_nHistBin;
	double inputVal, bdVal, resultVal;
	for ( int i = 0; i < m_nHistBin; i++ ) {
		inputVal = inputHist->m_hist[i];
		bdVal = bdHist->m_hist[i];
		if (bdVal+inputVal>0) resultVal = bdVal / (inputVal+bdVal);
		else resultVal = 0;

		m_hist[i] = resultVal;
	}
}


void CHistogram::ComputeHistogram( CVoxel *mr, int nHistBin, int nSmoothHist )
{
	int x, y, z, i, xSize, ySize, zSize;
	xSize = mr->m_nX;
	ySize = mr->m_nY;
	zSize = mr->m_nZ;

	// initialize m_hist
	ClearHistogram();

	// determine minimum and maximum intensity values
	short iValue;
	for( z = 0, i = 0; z < zSize ; z++ ) {
		for( y = 0 ; y < ySize ; y++ ) {
			for( x = 0 ; x < xSize ; x++, i++ ) {
				iValue = mr->m_pData[i];
				if( iValue > m_iMax )
					m_iMax = iValue;
				if( iValue < m_iMin )
					m_iMin = iValue;
			}
		}
	}

	// initialize m_hist
	if ( nHistBin <= 0 )
		m_nHistBin = m_iMax-m_iMin+1;
	else
		m_nHistBin = nHistBin;
	InitHistogram();

	// compute histogram
	float intensity;
	int binidx;
	for( z = 0, i = 0; z < zSize; z++ )	{
		for( y = 0; y < ySize; y++ ) {
			for( x = 0; x < xSize; x++, i++ ) {
				iValue = mr->m_pData[i];
				intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
				binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
				// increment related bin values
				m_hist[binidx] += 1.0f ;
			}
		}
	}

	// normalize histogram
	double denom = 1/(xSize*ySize*zSize);
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[j] *= denom;
	}

	// smooth histogram
	for ( i = 0; i < nSmoothHist; i++ ) {
		SmoothHistogram();
	}
}

void CHistogram::ComputeHistogram( double *arr, int N, int nSmoothHist, double *w )
{
	// initialize m_hist
	ClearHistogram();
	InitHistogram();

	// determine minimum and maximum intensity values
	int i;
	double iValue;
	for( i = 0; i < N; i++ ) {
		iValue = arr[i];
		if( iValue > m_iMax )
			m_iMax = iValue;
		if( iValue < m_iMin )
			m_iMin = iValue;
	}

	// compute histogram
	float intensity;
	int binidx;
	double wsum = 0;
	if ( w ) {
		for( i = 0; i < N; i++ ) {
			iValue = arr[i];
			intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
			binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
			// increment related bin values
			m_hist[binidx] += w[i];
			wsum += w[i];
		}
		// normalize histogram
		double denom = 1/wsum;
		for ( int j = 0; j < m_nHistBin; j++ )	{
			m_hist[j] *= denom;
		}
	}
	else {
		for( i = 0; i < N; i++ ) {
			iValue = arr[i];
			intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
			binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
			// increment related bin values
			m_hist[binidx] += 1.0;
		}
		// normalize histogram
		double denom = 1.0/N;
		for ( int j = 0; j < m_nHistBin; j++ )	{
			m_hist[j] *= denom;
		}
	}

	// smooth histogram
	for ( i = 0; i < nSmoothHist; i++ ) {
		SmoothHistogram();
	}
}

void CHistogram::ComputeHistogram( CVoxel *mr, cv::Point3i &stpt, cv::Point3i &edpt, int nSmoothHist )
{
	VERIFY( 0 <= stpt.m_x && stpt.m_x < mr->m_nX );
	VERIFY( 0 <= stpt.m_y && stpt.m_y < mr->m_nY );
	VERIFY( 0 <= stpt.m_z && stpt.m_z < mr->m_nZ );
	VERIFY( 0 <= edpt.m_x && edpt.m_x < mr->m_nX );
	VERIFY( 0 <= edpt.m_y && edpt.m_y < mr->m_nY );
	VERIFY( 0 <= edpt.m_z && edpt.m_z < mr->m_nZ );
	int x, y, z, i;

	// initialize m_hist
	ClearHistogram();
	InitHistogram();

	// determine minimum and maximum intensity values
	short iValue;
	int nCount = 0;
	for( z = stpt.m_z; z <= edpt.m_z; z++ ) {
		for( y = stpt.m_y; y <= edpt.m_y; y++ ) {
			for( x = stpt.m_x; x <= edpt.m_x; x++ ) {
				i = z*mr->m_nX*mr->m_nY+y*mr->m_nX+x;
				iValue = mr->m_pData[i];
				if( iValue > m_iMax )
					m_iMax = iValue;
				if( iValue < m_iMin )
					m_iMin = iValue;
				nCount++;
			}
		}
	}

	// compute histogram
	float intensity;
	int binidx;
	for( z = stpt.m_z; z <= edpt.m_z; z++ ) {
		for( y = stpt.m_y; y <= edpt.m_y; y++ ) {
			for( x = stpt.m_x; x <= edpt.m_x; x++ ) {
				i = z*mr->m_nX*mr->m_nY+y*mr->m_nX+x;
				iValue = mr->m_pData[i];
				intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
				binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
				// increment related bin values
				m_hist[binidx] += 1.0f ;
			}
		}
	}

	// normalize histogram
	double denom = 1/nCount;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[j] *= denom;
	}

	// smooth histogram
	for ( i = 0; i < nSmoothHist; i++ ) {
		SmoothHistogram();
	}
}

void CHistogram::ComputeFixedLengthHistogram( CVoxel *mr, int nHistBin, int iMin, int iMax, int nSmoothHist )
{
	m_nHistBin = nHistBin;
	m_iMin = iMin;
	m_iMax = iMax;
	// initialize m_hist
	ClearHistogram();
	InitHistogram();
	int xSize = mr->m_nX;
	int ySize = mr->m_nY;
	int zSize = mr->m_nZ;

	// compute histogram
	int count = 0; 
	short iValue;
	float intensity;
	int binidx;
	int x, y, z, i;
	for( z = 0; z < zSize ; z++ ) {
		for( y = 0 ; y < ySize ; y++ ) {
			for( x = 0 ; x < xSize ; x++ ) {
				iValue = std::max(std::min(mr->GetAt(x, y, z), I_MAX), I_MIN);
				intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
				binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
				// increment related bin values
				m_hist[binidx] += 1.0f ;
				count++;
			}
		}
	}

	// normalize histogram
	float denom = 1.0f / (float)count ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[j] *= denom;
	}

	// smooth histogram
	for ( i = 0; i < nSmoothHist; i++ ) {
		SmoothHistogram();
	}
}

void CHistogram::ComputeFixedLengthHistogramWithROI( CVoxel *mr, CVoxel* roi, 
											   int nHistBin, int iMin, int iMax, int nSmoothHist )
{
	m_nHistBin = nHistBin;
	m_iMin = iMin;
	m_iMax = iMax;
	// initialize m_hist
	ClearHistogram();
	InitHistogram();

	// compute histogram
	int count = 0; 
	short iValue;
	float intensity;
	int binidx;
	int x, y, z;
	for( z = 0; z < mr->m_nZ; z++ ) {
		for( y = 0; y < mr->m_nY; y++ ) {
			for( x = 0; x < mr->m_nX; x++ ) {
				if ( roi->GetAt(x,y,z) ) {
					iValue = std::max(std::min(mr->GetAt(x, y, z), I_MAX), I_MIN);
					intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
					binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
					// increment related bin values

					m_hist[binidx] += 1.0f ;
					count++;
				}
			}
		}
	}

	// normalize histogram
	float denom = 1.0f / (float)count ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[j] *= denom;
	}

	// smooth histogram
	for ( int i = 0; i < nSmoothHist; i++ ) {
		SmoothHistogram();
	}
}

void CHistogram::ComputeFixedLengthHistogram( CVoxel *mr, cv::Point3i &stpt, cv::Point3i &edpt, 
																		 int nHistBin, int iMin, int iMax, int nSmoothHist )
{
	VERIFY( 0 <= stpt.m_x && stpt.m_x < mr->m_nX );
	VERIFY( 0 <= stpt.m_y && stpt.m_y < mr->m_nY );
	VERIFY( 0 <= stpt.m_z && stpt.m_z < mr->m_nZ );
	VERIFY( 0 <= edpt.m_x && edpt.m_x < mr->m_nX );
	VERIFY( 0 <= edpt.m_y && edpt.m_y < mr->m_nY );
	VERIFY( 0 <= edpt.m_z && edpt.m_z < mr->m_nZ );

	m_nHistBin = nHistBin;
	m_iMin = iMin;
	m_iMax = iMax;
	// initialize m_hist
	ClearHistogram();
	InitHistogram();

	// compute histogram
	int count = 0; 
	short iValue;
	float intensity;
	int binidx;
	int x, y, z;
	for( z = stpt.m_z; z <= edpt.m_z; z++ ) {
		for( y = stpt.m_y; y <= edpt.m_y; y++ ) {
			for( x = stpt.m_x; x <= edpt.m_x; x++ ) {
				iValue = std::max(std::min(mr->GetAt(x, y, z), I_MAX), I_MIN);
				intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
				binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
				// increment related bin values
				m_hist[binidx] += 1.0f ;
				count++;
			}
		}
	}

	// normalize histogram
	float denom = 1.0f / (float)count ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[j] *= denom;
	}

	// smooth histogram
	for ( int i = 0; i < nSmoothHist; i++ ) {
		SmoothHistogram();
	}
}


double CHistogram::ComputeChiSquareDistance(CHistogram& thist)
{
	double val, histdist;
	histdist = 0;
	for ( int i = 0; i < m_nHistBin; i++ ) {
		val = m_hist[i] + thist.m_hist[i];
		
		if ( val != 0 ) {
			double tmp = (m_hist[i] - thist.m_hist[i])*(m_hist[i] - thist.m_hist[i]) / val;
			tmp = tmp/2.0;
			histdist += tmp;
		}
	}
	
	return histdist;
}


void CHistogram::SmoothHistogram()
{
	int j, m;
	// in the y direction
	for ( j = 0; j < m_nHistBin; j++ )
	{
		int s = ( j-WND <  0 ) ? -j    : -WND; 
		int e = ( j+WND >= m_nHistBin ) ? m_nHistBin-j-1 :  WND;
		double sum = 0;
		for ( m = s; m <= e; m++ ) {
			sum += m_hist[j+m]*w[m+WND];
		}
		m_hist[j] = sum;
	}
}

void CHistogram::ComputeHistogramWithSmoothRound( double *arr, int N, int nSmoothHist, double *w )
{
	// initialize m_hist
	ClearHistogram();
	InitHistogram();

	// determine minimum and maximum intensity values
	int i;
	double iValue;
	for( i = 0; i < N; i++ ) {
		iValue = arr[i];
		if( iValue > m_iMax )
			m_iMax = iValue;
		if( iValue < m_iMin )
			m_iMin = iValue;
	}

	// compute histogram
	float intensity;
	int binidx;
	double wsum = 0;
	if ( w ) {
		for( i = 0; i < N; i++ ) {
			iValue = arr[i];
			intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
			binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
			// increment related bin values
			m_hist[binidx] += w[i];
			wsum += w[i];
		}
		// normalize histogram
		double denom = 1/wsum;
		for ( int j = 0; j < m_nHistBin; j++ )	{
			m_hist[j] *= denom;
		}
	}
	else {
		for( i = 0; i < N; i++ ) {
			iValue = arr[i];
			intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
			binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
			// increment related bin values
			m_hist[binidx] += 1.0;
		}
		// normalize histogram
		double denom = 1.0/N;
		for ( int j = 0; j < m_nHistBin; j++ )	{
			m_hist[j] *= denom;
		}
	}

	// smooth histogram
	for ( i = 0; i < nSmoothHist; i++ ) {
		SmoothHistogramRound();
	}
}

void CHistogram::SmoothHistogramRound()
{
	int j, m;
	// in the y direction
	for ( j = 0; j < m_nHistBin; j++ )
	{
		double sum = 0;
		if ( j == 0 ) {
			sum += m_hist[m_nHistBin-1]*w[0];
			sum += m_hist[0]*w[1];
			sum += m_hist[1]*w[2];
			m_hist[j] = sum;
		}
		else if ( j == m_nHistBin-1 ) {
			sum += m_hist[j-1]*w[0];
			sum += m_hist[j]*w[1];
			sum += m_hist[0]*w[2];
			m_hist[j] = sum;
		}
		else {
			for ( m = -1; m <= 1; m++ ) {
				sum += m_hist[j+m]*w[m+WND];
			}
			m_hist[j] = sum;
		}
	}
}


void CHistogram::ComputeMeanHistogram(CTypedPtrArray<CPtrArray, CHistogram*> &histArr)
{
	int i,j, nHist = (int) histArr.GetSize();
	double imin, imax;
	imin = FLT_MAX;
	imax = -FLT_MAX;
	int histBinMin = INT_MAX;
	for ( i = 0; i < nHist; i++ ) {
		CHistogram *hist = histArr[i];
		if ( hist->m_iMin < imin )
			imin = hist->m_iMin;
		if ( hist->m_iMax > imax )
			imax = hist->m_iMax;
		if ( hist->m_nHistBin < histBinMin ) 
			histBinMin = hist->m_nHistBin;
	}
	m_nHistBin = histBinMin;
	m_iMin = imin;
	m_iMax = imax;
	InitHistogram();
	for ( i = 0; i < nHist; i++ ) {
		double rBinW = (double)m_nHistBin/(double)histArr[i]->m_nHistBin;
		CHistogram *hist = histArr[i];
		for ( j = 0; j < hist->m_nHistBin; j++ ) {
			int b = std::min((int)Round((double)j*rBinW), m_nHistBin-1);
			m_hist[b] += hist->m_hist[j];
		}
	}
	double histSum = 0;
	for ( j = 0; j < m_nHistBin; j++ ) {
		histSum += m_hist[j];
	}
	double denom = 1/ histSum;
	for ( j = 0; j < m_nHistBin; j++ ) {
		m_hist[j] *= denom;
	}
}

void CHistogram::Save(CString str)
{
	ofstream outstream;
	outstream.open(str, ios::out|ios::binary|ios::trunc);
	if ( outstream.is_open() ) {
		outstream.write((char*)&m_nHistBin, sizeof(int));
		outstream.write((char*)&m_iMin, sizeof(float));
		outstream.write((char*)&m_iMax, sizeof(float));
		outstream.write((char*)m_hist, m_nHistBin*sizeof(double));
		outstream.close();
	}
	else { 
		AfxMessageBox(_T("Error while saving CHistogram object!"));
	}
}

void CHistogram::SaveToWofstream(std::wofstream& outstream)
{
	outstream << m_nHistBin << endl;
	outstream << m_iMin << endl;
	outstream << m_iMax << endl;

	for ( int  i = 0; i < m_nHistBin; i++ ) {
		outstream << m_hist[i] << endl;
	}
}


void CHistogram::SavefloatHist(CString str)
{
	ofstream outstream;
	outstream.open(str, ios::out|ios::binary|ios::trunc);
	if ( outstream.is_open() ) {
		outstream.write((char*)&m_nHistBin, sizeof(int));
		outstream.write((char*)&m_iMin, sizeof(float));
		outstream.write((char*)&m_iMax, sizeof(float));
		outstream.write((char*)m_hist, m_nHistBin*sizeof(double));
		outstream.close();
	}
	else { 
		AfxMessageBox(_T("Error while saving CHistogram object!"));
	}
}

void CHistogram::LoadShortHist(CString str) 
{
	ifstream instream;
	instream.open(str, ios::in|ios::binary);
	if ( instream.is_open() ) {
		instream.read((char*)&m_nHistBin, sizeof(int));
		short iMin, iMax;
		instream.read((char*)&iMin, sizeof(short));
		instream.read((char*)&iMax, sizeof(short));
		m_iMin = iMin;
		m_iMax = iMax;
		SAFE_DELETE_ARRAY(m_hist);
		m_hist = new double[m_nHistBin];
		instream.read((char*)m_hist, m_nHistBin*sizeof(double));
		instream.close();
	}
	else { 
		AfxMessageBox(_T("Error while loading CHistogram object!"));
	}
}

void CHistogram::LoadfloatHist(CString str) 
{
	ifstream instream;
	instream.open(str, ios::in|ios::binary);
	if ( instream.is_open() ) {
		instream.read((char*)&m_nHistBin, sizeof(int));
		instream.read((char*)&m_iMin, sizeof(float));
		instream.read((char*)&m_iMax, sizeof(float));
		SAFE_DELETE_ARRAY(m_hist);
		m_hist = new double[m_nHistBin];
		instream.read((char*)m_hist, m_nHistBin*sizeof(double));
		instream.close();
	}
	else { 
		AfxMessageBox(_T("Error while loading CHistogram object!"));
	}
}

void CHistogram::Load(CString str) 
{

	ifstream instream;
	instream.open(str, ios::in|ios::binary);
	if ( instream.is_open() ) {
		instream.read((char*)&m_nHistBin, sizeof(int));
		instream.read((char*)&m_iMin, sizeof(float));
		instream.read((char*)&m_iMax, sizeof(float));
		SAFE_DELETE_ARRAY(m_hist);
		m_hist = new double[m_nHistBin];
		instream.read((char*)m_hist, m_nHistBin*sizeof(double));
		instream.close();
	}
	else { 
		AfxMessageBox(_T("Error while loading CHistogram object!"));
	}
}

double *CHistogram::CumulativeVector()
{
	double *cumulVec = new double[m_nHistBin];
	cumulVec[0] = m_hist[0];
	for ( int i = 1; i < m_nHistBin; i++ ) {
		cumulVec[i] = cumulVec[i-1]+m_hist[i];
	}
	return cumulVec;
}

double *CHistogram::CumulativeVectorNormBin()
{
	double *cumulVec = new double[m_nHistBin];
	cumulVec[0] = m_hist[0];
	for ( int i = 1; i < m_nHistBin; i++ ) {
		cumulVec[i] = cumulVec[i-1]+m_hist[i];
	}
	for ( int i = 0; i < m_nHistBin; i++ ) {
		//cumulVec[i] *= m_nHistBin;
		cumulVec[i] *= m_iMax-m_iMin;
		cumulVec[i] += m_iMin;
	}
	return cumulVec;
}

void CHistogram::ComputeGradHistogram(CVoxel *pat, CVoxel *lbl, int fg, cv::Point3i pPt, int nSmoothHist)
{
	InitHistogram();
	// compute histogram
	int count = 0; 
	short iValue;
	float intensity;
	int binidx;
	int x, y, z, i;
	int nx = pat->m_nX;
	int ny = pat->m_nY;
	int nz = pat->m_nZ;
	cv::Point3i edpt(nx, ny, nz);
	for( z = 0; z < nz; z++ ) {
		for( y = 0 ; y < ny; y++ ) {
			for( x = 0 ; x < nx; x++ ) {
				cv::Point3i xpt(x,y,z);
				cv::Point3i npt = xpt+pPt;
				if ( ZERO_3DPT <= npt && npt < edpt ) {
					if ( lbl->GetAt(xpt)==fg && lbl->GetAt(npt)==fg ) {
						int _i = abs(pat->GetAt(x, y, z)-pat->GetAt(npt));
						iValue = std::max(std::min(_i, I_MAX), I_MIN);
						intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
						binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
						// increment related bin values
						m_hist[binidx] += 1.0f ;
						count++;
					}
				}
			}
		}
	}

	// normalize histogram
	float denom = 1.0f / (float)count ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[j] *= denom;
	}

	// smooth histogram
	for ( i = 0; i < nSmoothHist; i++ ) {
		SmoothHistogram();
	}
}

void CHistogram::ComputeBDGradHistogram(CVoxel *pat, CVoxel *lbl, int fg, int bg, cv::Point3i pPt, int nSmoothHist)
{
	InitHistogram();
	// compute histogram
	int count = 0; 
	short iValue;
	float intensity;
	int binidx;
	int x, y, z, i;
	int nx = pat->m_nX;
	int ny = pat->m_nY;
	int nz = pat->m_nZ;
	cv::Point3i edpt(nx, ny, nz);
	for( z = 0; z < nz; z++ ) {
		for( y = 0 ; y < ny; y++ ) {
			for( x = 0 ; x < nx; x++ ) {
				cv::Point3i xpt(x,y,z);
				cv::Point3i npt = xpt+pPt;
				if ( ZERO_3DPT <= npt && npt < edpt ) {
					if ( (lbl->GetAt(xpt)==fg && lbl->GetAt(npt)==bg) || (lbl->GetAt(xpt)==bg && lbl->GetAt(npt)==fg)) {
						int _i = abs(pat->GetAt(x, y, z)-pat->GetAt(npt));
						iValue = std::max(std::min(_i, I_MAX), I_MIN);
						intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
						binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
						// increment related bin values
						m_hist[binidx] += 1.0f ;
						count++;
					}
				}
			}
		}
	}

	// normalize histogram
	float denom = 1.0f / (float)count ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[j] *= denom;
	}

	// smooth histogram
	for ( i = 0; i < nSmoothHist; i++ ) {
		SmoothHistogram();
	}
}



short CHistogram::GetBinIValue(int n)
{
	return (short)Round((double)(m_iMax-m_iMin)/(double)m_nHistBin*(double)n);
}

#define VIEW_BIN_WIDTH 1
#define VIEW_HIST_HEIGHT 240
#define MAX_H 20
void CHistogram::SaveHistImage(CStringA str)
{
	IplImage *histimg = cvCreateImage(cvSize(m_nHistBin+1*VIEW_BIN_WIDTH, VIEW_HIST_HEIGHT), 8, 1);
	cvSet(histimg, cvScalar(255));
	for ( int i = 0; i < m_nHistBin; i++ ) {
		int h = (int)(VIEW_HIST_HEIGHT-m_hist[i]*VIEW_HIST_HEIGHT*MAX_H);
		if ( h < VIEW_HIST_HEIGHT )
		cvRectangle( histimg, 
					 cvPoint(VIEW_BIN_WIDTH*(i), h),
					 cvPoint(VIEW_BIN_WIDTH*(i+1)-1, VIEW_HIST_HEIGHT),
					 cvScalar(0), CV_FILLED);
	}
	cvSaveImage(str, histimg);
	cvReleaseImage(&histimg);
}

void CHistogram::SaveVariableLengthImage(CStringA str)
{
	int vbw = std::max(1, cvRound((m_iMax-m_iMin)/(float)m_nHistBin));
	IplImage *histimg = cvCreateImage(cvSize(m_nHistBin+1*vbw, VIEW_HIST_HEIGHT), 8, 1);
	cvSet(histimg, cvScalar(255));
	for ( int i = 0; i < m_nHistBin; i++ ) {
		int h = (int)(VIEW_HIST_HEIGHT-m_hist[i]*VIEW_HIST_HEIGHT*MAX_H);
		if ( h < VIEW_HIST_HEIGHT )
		cvRectangle( histimg, 
					 cvPoint(vbw*(i), h),
					 cvPoint(vbw*(i+1)-1, VIEW_HIST_HEIGHT),
					 cvScalar(0), CV_FILLED);
	}
	cvSaveImage(str, histimg);
	cvReleaseImage(&histimg);
}

void CHistogram::SaveHistImage(CStringA str, double magnitude)
{
	IplImage *histimg = cvCreateImage(cvSize(m_nHistBin+1*VIEW_BIN_WIDTH, VIEW_HIST_HEIGHT), 8, 1);
	cvSet(histimg, cvScalar(255));
	for ( int i = 0; i < m_nHistBin; i++ ) {
		int h = (int)(VIEW_HIST_HEIGHT-m_hist[i]*VIEW_HIST_HEIGHT*magnitude);
		if ( h < VIEW_HIST_HEIGHT )
			cvRectangle( histimg, 
			cvPoint(VIEW_BIN_WIDTH*(i), h),
			cvPoint(VIEW_BIN_WIDTH*(i+1)-1, VIEW_HIST_HEIGHT),
			cvScalar(0), CV_FILLED);
	}
	cvSaveImage(str, histimg);
	cvReleaseImage(&histimg);
}

// END of CHistogram implementation
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
// START CBilayerHistogram implementation
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
CBilayerHistogram::CBilayerHistogram()
{
	m_nHistBin = 256 ;
	m_hist[0] = NULL; // for BG
	m_hist[1] = NULL; // for FG

	m_iMin = SHRT_MAX ;
	m_iMax = SHRT_MIN ;
}

CBilayerHistogram::CBilayerHistogram(int n)
{
	m_nHistBin = n;
	m_hist[0] = NULL; // for BG
	m_hist[1] = NULL; // for FG

	m_iMin = SHRT_MAX ;
	m_iMax = SHRT_MIN ;
}

CBilayerHistogram::CBilayerHistogram(CString str)
{
	m_hist[0] = NULL; // for BG
	m_hist[1] = NULL; // for FG
	Load(str);
}


CBilayerHistogram::~CBilayerHistogram()
{
	ClearHistogram();
}
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
int CBilayerHistogram::GetNumHistBin()
{
	return m_nHistBin;
}

void CBilayerHistogram::SetNumHistBin(int n)
{
	m_nHistBin = n;
}

void CBilayerHistogram::SetIMax(short i)
{
	m_iMax = i;
}

void CBilayerHistogram::SetIMin(short i)
{
	m_iMin = i;
}

short CBilayerHistogram::GetIMax()
{
	return m_iMax;
}

short CBilayerHistogram::GetIMin()
{
	return m_iMin;
}

void CBilayerHistogram::InitHistogram()
{
	for ( int i = 0; i < 2; i++ )
	{
		m_hist[i] = new double[m_nHistBin] ;
		for( int j = 0 ; j < m_nHistBin ; j++ )
			m_hist[i][j] = 0.0f ;
	}
}

void CBilayerHistogram::ClearHistogram()
{
	for( int i = 0; i < 2 ; i++ ) {
		SAFE_DELETE_ARRAY( m_hist[i] ) ;
	}
}

CBilayerHistogram* CBilayerHistogram::Copy()
{
	CBilayerHistogram* newHist = new CBilayerHistogram();

	newHist->m_nHistBin = m_nHistBin;
	newHist->m_iMin = m_iMin;
	newHist->m_iMax = m_iMax;
	
	newHist->m_hist[0] = new double[m_nHistBin];
	newHist->m_hist[1] = new double[m_nHistBin];

	for ( int i = 0; i < m_nHistBin; i++ ) {
		newHist->m_hist[0][i] = m_hist[0][i];
		newHist->m_hist[1][i] = m_hist[1][i];
	}

	return newHist;
}

bool CBilayerHistogram::ComputeHistogram( CVoxel *pCrop, CVoxel *pMarkings, int iCropX0, int iCropY0, int iCropZ0, BOOL bGc3D, int iZ, BYTE byColor )
{
	short iValue ;
	int x, y, z, xSize, ySize, zSize ;
	xSize = pCrop->m_nX ;
	ySize = pCrop->m_nY ;
	zSize = bGc3D ? pCrop->m_nZ : 1 ;
	int nTotal = xSize*ySize*zSize ;
	
	// let's first try three 1-D histograms
	// initialize m_hist
	ClearHistogram();
	InitHistogram();

	for( z = 0 ; z < zSize ; z++ )
	{
		z = bGc3D ? z : iZ ;
		for( y = 0 ; y < ySize ; y++ )
		for( x = 0 ; x < xSize ; x++ )
		{
			iValue = pCrop->GetAt( x, y, z ) ;
			if( iValue > m_iMax )
				m_iMax = iValue ;
			if( iValue < m_iMin )
				m_iMin = iValue ;
		}
	}

	// compute histogram
	int count_f = 0; int count_b = 0;
	for( z = 0 ; z < zSize ; z++ )
	{
		z = bGc3D ? z : iZ ;
		for( y = 0 ; y < ySize ; y++ )
		for( x = 0 ; x < xSize ; x++ )
		{
			iValue = pCrop->GetAt( x, y, z ) ;
			int iMarking = pMarkings->GetAt( x + iCropX0, y + iCropY0, z + iCropZ0 ) ;
			float intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
	
//			if ( iMarking == 0 || iMarking == 1) // very fatal bug!! found 11/13/007
			if ( iMarking > 0 && iMarking < BYTE_WHITE )
			{
				int int_bin = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
				
				if ( iMarking == byColor )
				{
					// increment related bin values
					m_hist[1][int_bin] += 1.0f ;
					count_f++;
				}
				else
				{
					// increment related bin values
					m_hist[0][int_bin] += 1.0f ;
					count_b++;
				}
			}
		}
	}

	if( count_b == 0 || count_f == 0 )
	{
		AfxMessageBox(_T("only background or foreground??"));
		return false;
	}
	
	// normalize histogram
	float denom_b = 1.0f / (float)count_b ;
	float denom_f = 1.0f / (float)count_f ;
	for ( int j = 0; j < m_nHistBin; j++ )
	{
		m_hist[0][j] *= denom_b;
		m_hist[1][j] *= denom_f;
	}

	// smooth histogram
	SmoothHistogram();
	SmoothHistogram();
	return true;
}

bool CBilayerHistogram::ComputeHistogram( CVoxel *mr, CVoxel *st )
{
	VERIFY( mr->m_nX == st->m_nX && mr->m_nY == st->m_nY && mr->m_nZ == st->m_nZ );
	int x, y, z, i, xSize, ySize, zSize;
	xSize = mr->m_nX;
	ySize = mr->m_nY;
	zSize = mr->m_nZ;

// initialize m_hist
	ClearHistogram();
	InitHistogram();

// determine minimum and maximum intensity values
	short iValue;
	for( z = 0, i = 0; z < zSize ; z++ ) {
		for( y = 0 ; y < ySize ; y++ ) {
			for( x = 0 ; x < xSize ; x++, i++ ) {
				iValue = mr->m_pData[i];
				if( iValue > m_iMax )
					m_iMax = iValue;
				if( iValue < m_iMin )
					m_iMin = iValue;
			}
		}
	}

// compute histogram
	int count_f = 0; int count_b = 0;
	short iST;
	float intensity;
	int binidx;
	for( z = 0, i = 0; z < zSize; z++ )	{
		for( y = 0; y < ySize; y++ ) {
			for( x = 0; x < xSize; x++, i++ ) {
				iValue = mr->m_pData[i];
				iST = st->m_pData[i];
				intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
				binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
			// increment related bin values
				if ( iST == FG )	{
					m_hist[FGHIST][binidx] += 1.0f ;
					count_f++;
				}
				else if ( iST == BG )	{
					m_hist[BGHIST][binidx] += 1.0f ;
					count_b++;
				}
			}
		}
	}

	if( count_b == 0 || count_f == 0 )
	{
		//AfxMessageBox(_T("insufficient shape template for histogram construction"));
		return false;
	}

	// normalize histogram
	float denom_b = 1.0f / (float)count_b ;
	float denom_f = 1.0f / (float)count_f ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[0][j] *= denom_b;
		m_hist[1][j] *= denom_f;
	}

	// smooth histogram
	SmoothHistogram();
	SmoothHistogram();
	return true;
}

bool CBilayerHistogram::ComputeHistogram( CVoxel *mr, CVoxel *st, int fg, int nSmooth  )
{
	return ComputeHistogram( mr, st, cv::Point3i(0,0,0), 
		cv::Point3i(mr->m_nX-1, mr->m_nY-1, mr->m_nZ-1), fg, 0, nSmooth );
}

bool CBilayerHistogram::ComputeHistogram( CVoxel *mr, CVoxel *st, cv::Point3i &stpt, cv::Point3i &edpt, int fg, int bg, int nSmooth  )
{
	VERIFY( mr->m_nX == st->m_nX && mr->m_nY == st->m_nY && mr->m_nZ == st->m_nZ );

	// initialize m_hist
	ClearHistogram();
	InitHistogram();

	// determine minimum and maximum intensity values
	int x, y, z;
	short iValue;
	for( z = stpt.m_z; z <= edpt.m_z; z++ ) {
		for( y = stpt.m_y; y <= edpt.m_y; y++ ) {
			for( x = stpt.m_x; x <= edpt.m_x; x++ ) {
				iValue = mr->GetAt(x, y, z);
				if( iValue > m_iMax )
					m_iMax = iValue;
				if( iValue < m_iMin )
					m_iMin = iValue;
			}
		}
	}

	// compute histogram
	int count_f = 0; int count_b = 0;
	short iST;
	float intensity;
	int binidx;
	for( z = stpt.m_z; z <= edpt.m_z; z++ ) {
		for( y = stpt.m_y; y <= edpt.m_y; y++ ) {
			for( x = stpt.m_x; x <= edpt.m_x; x++ ) {
				iValue = mr->GetAt(x, y, z);
				iST = st->GetAt(x, y, z);
				intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
				binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
				// increment related bin values
				if ( iST == fg )	{
					m_hist[FGHIST][binidx] += 1.0f ;
					count_f++;
				}
				else if ( iST == bg )	{
					m_hist[BGHIST][binidx] += 1.0f ;
					count_b++;
				}
			}
		}
	}

	if( count_b == 0 || count_f == 0 )
	{
		//AfxMessageBox(_T("insufficient shape template for histogram construction"));
		return false;
	}

	// normalize histogram
	float denom_b = 1.0f / (float)count_b ;
	float denom_f = 1.0f / (float)count_f ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[0][j] *= denom_b;
		m_hist[1][j] *= denom_f;
	}

	// smooth histogram
	for ( int i = 0; i < nSmooth; i++ ) {
		SmoothHistogram();
	}
	return true;
}

bool CBilayerHistogram::ComputeFixedLengthHistogram( CVoxel *mr, CFVoxel *st, CVoxel *exc, int fg, int nSmooth )
{
	VERIFY( mr->m_nX == st->m_nX && mr->m_nY == st->m_nY && mr->m_nZ == st->m_nZ );
	VERIFY( mr->m_nX == exc->m_nX && mr->m_nY == exc->m_nY && mr->m_nZ == exc->m_nZ );

	m_nHistBin = HIST_BIN;
	m_iMin = I_MIN;
	m_iMax = I_MAX;
	// initialize m_hist
	ClearHistogram();
	InitHistogram();

	// compute histogram
	float count_f = 0; float count_b = 0;
	short iST, iValue;
	float intensity;
	int binidx;
	int x, y, z;
	cv::Point3i stpt(0,0,0);
	cv::Point3i edpt(mr->m_nX-1,mr->m_nY-1,mr->m_nZ-1);
	for( z = stpt.m_z; z <= edpt.m_z; z++ ) {
		for( y = stpt.m_y; y <= edpt.m_y; y++ ) {
			for( x = stpt.m_x; x <= edpt.m_x; x++ ) {
				iValue = std::max(std::min(mr->GetAt(x, y, z), I_MAX), I_MIN);
				iST = st->GetAt(x, y, z);
				intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
				binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
				// increment related bin values
				if ( exc->GetAt(x, y, z)==0 ) {
					m_hist[FGHIST][binidx] += st->GetAt(x,y,z);
					count_f += st->GetAt(x,y,z);
					m_hist[BGHIST][binidx] += 1.f-st->GetAt(x,y,z);
					count_b += 1.f-st->GetAt(x,y,z);
				}
			}
		}
	}

	if( count_b == 0 || count_f == 0 )
	{
		//AfxMessageBox(_T("insufficient shape template for histogram construction"));
		return false;
	}

	// normalize histogram
	float denom_b = 1.0f / count_b ;
	float denom_f = 1.0f / count_f ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[BGHIST][j] *= denom_b;
		m_hist[FGHIST][j] *= denom_f;
	}

	// smooth histogram
	for ( int i = 0; i < nSmooth; i++ ) {
		SmoothHistogram();
	}
	return true;	
}

void CBilayerHistogram::SaveFHistImage( CStringA str )
{
	IplImage *histimg = cvCreateImage(cvSize(m_nHistBin+1*VIEW_BIN_WIDTH, VIEW_HIST_HEIGHT), 8, 1);
	cvSet(histimg, cvScalar(255));
	for ( int i = 0; i < m_nHistBin; i++ ) {

		int h = (int)(VIEW_HIST_HEIGHT-m_hist[1][i]*VIEW_HIST_HEIGHT*MAX_H);
		if ( h < VIEW_HIST_HEIGHT )
			cvRectangle( histimg, 
			cvPoint(VIEW_BIN_WIDTH*(i), h),
			cvPoint(VIEW_BIN_WIDTH*(i+1)-1, VIEW_HIST_HEIGHT),
			cvScalar(0), CV_FILLED);
	}
	cvSaveImage(str, histimg);
	cvReleaseImage(&histimg);
}

void CBilayerHistogram::SaveBHistImage( CStringA str )
{
	IplImage *histimg = cvCreateImage(cvSize(m_nHistBin+1*VIEW_BIN_WIDTH, VIEW_HIST_HEIGHT), 8, 1);
	cvSet(histimg, cvScalar(255));
	for ( int i = 0; i < m_nHistBin; i++ ) {
		int h = (int)(VIEW_HIST_HEIGHT-m_hist[0][i]*VIEW_HIST_HEIGHT*MAX_H);
		if ( h < VIEW_HIST_HEIGHT )
			cvRectangle( histimg, 
			cvPoint(VIEW_BIN_WIDTH*(i), h),
			cvPoint(VIEW_BIN_WIDTH*(i+1)-1, VIEW_HIST_HEIGHT),
			cvScalar(0), CV_FILLED);
	}
	cvSaveImage(str, histimg);
	cvReleaseImage(&histimg);
}

bool CBilayerHistogram::ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, int fg, int bg, int nSmooth )
{
	return ComputeFixedLengthHistogram(mr, st, cv::Point3i(0,0,0), cv::Point3i(mr->m_nX-1, mr->m_nY-1, mr->m_nZ-1), fg, bg, nSmooth);
}

bool CBilayerHistogram::ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, int fg, int nSmooth )
{
	return ComputeFixedLengthHistogram(mr, st, cv::Point3i(0,0,0), cv::Point3i(mr->m_nX-1, mr->m_nY-1, mr->m_nZ-1), fg, nSmooth);
}


bool CBilayerHistogram::ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, cv::Point3i &stpt, cv::Point3i &edpt, int fg, int bg, int nSmooth )
{
	VERIFY( mr->m_nX == st->m_nX && mr->m_nY == st->m_nY && mr->m_nZ == st->m_nZ );

	m_nHistBin = HIST_BIN;
	m_iMin = I_MIN;
	m_iMax = I_MAX;
	// initialize m_hist
	ClearHistogram();
	InitHistogram();

	// compute histogram
	int count_f = 0; int count_b = 0;
	short iST, iValue;
	float intensity;
	int binidx;
	int x, y, z;

	for( z = stpt.m_z; z <= edpt.m_z; z++ ) {
		for( y = stpt.m_y; y <= edpt.m_y; y++ ) {
			for( x = stpt.m_x; x <= edpt.m_x; x++ ) {
				iValue = std::max(std::min(mr->GetAt(x, y, z), I_MAX), I_MIN);
				iST = st->GetAt(x, y, z);
				intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
				binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
				// increment related bin values
				if ( iST == fg )	{
					m_hist[FGHIST][binidx] += 1.0f ;
					count_f++;
				}
				else if ( iST == bg )	{
					m_hist[BGHIST][binidx] += 1.0f ;
					count_b++;
				}
			}
		}
	}

	if( count_b == 0 || count_f == 0 )
	{
		//AfxMessageBox(_T("insufficient shape template for histogram construction"));
		return false;
	}

	// normalize histogram
	float denom_b = 1.0f / (float)count_b ;
	float denom_f = 1.0f / (float)count_f ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[BGHIST][j] *= denom_b;
		m_hist[FGHIST][j] *= denom_f;
	}

	// smooth histogram
	for ( int i = 0; i < nSmooth; i++ ) {
		SmoothHistogram();
	}
	return true;	
}

bool CBilayerHistogram::ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, cv::Point3i &stpt, cv::Point3i &edpt, int fg, int nSmooth )
{
	VERIFY( mr->m_nX == st->m_nX && mr->m_nY == st->m_nY && mr->m_nZ == st->m_nZ );

	m_nHistBin = HIST_BIN;
	m_iMin = I_MIN;
	m_iMax = I_MAX;
	// initialize m_hist
	ClearHistogram();
	InitHistogram();

	// compute histogram
	int count_f = 0; int count_b = 0;
	short iST, iValue;
	float intensity;
	int binidx;
	int x, y, z;

	for( z = stpt.m_z; z <= edpt.m_z; z++ ) {
		for( y = stpt.m_y; y <= edpt.m_y; y++ ) {
			for( x = stpt.m_x; x <= edpt.m_x; x++ ) {
				iValue = std::max(std::min(mr->GetAt(x, y, z), I_MAX), I_MIN);
				iST = st->GetAt(x, y, z);
				intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
				binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
				// increment related bin values
				if ( iST == fg )	{
					m_hist[FGHIST][binidx] += 1.0f ;
					count_f++;
				}
				else {
					m_hist[BGHIST][binidx] += 1.0f ;
					count_b++;
				}
			}
		}
	}

	if( count_b == 0 || count_f == 0 )
	{
		//AfxMessageBox(_T("insufficient shape template for histogram construction"));
		return false;
	}

	// normalize histogram
	float denom_b = 1.0f / (float)count_b ;
	float denom_f = 1.0f / (float)count_f ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[BGHIST][j] *= denom_b;
		m_hist[FGHIST][j] *= denom_f;
	}

	// smooth histogram
	for ( int i = 0; i < nSmooth; i++ ) {
		SmoothHistogram();
	}
	return true;	
}


bool CBilayerHistogram::ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, CVoxel *exc, int fg, int bg, int nSmooth )
{
	return ComputeFixedLengthHistogram(mr, st, exc, cv::Point3i(0,0,0), cv::Point3i(mr->m_nX-1,mr->m_nY-1,mr->m_nZ-1), fg, bg, 2 );
}

bool CBilayerHistogram::ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, CVoxel *exc, int fg, int nSmooth )
{
	return ComputeFixedLengthHistogram(mr, st, exc, cv::Point3i(0,0,0), cv::Point3i(mr->m_nX-1,mr->m_nY-1,mr->m_nZ-1), fg, 2 );
}


bool CBilayerHistogram::ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, CVoxel *exc, 
																				  cv::Point3i &stpt, cv::Point3i &edpt, int fg, int nSmooth )
{
	VERIFY( mr->m_nX == st->m_nX && mr->m_nY == st->m_nY && mr->m_nZ == st->m_nZ );

	m_nHistBin = HIST_BIN;
	m_iMin = I_MIN;
	m_iMax = I_MAX;
	// initialize m_hist
	ClearHistogram();
	InitHistogram();

	// compute histogram
	int count_f = 0; int count_b = 0;
	short iST, iValue;
	float intensity;
	int binidx;
	int x, y, z;

	if ( exc ) {
		for( z = stpt.m_z; z <= edpt.m_z; z++ ) {
			for( y = stpt.m_y; y <= edpt.m_y; y++ ) {
				for( x = stpt.m_x; x <= edpt.m_x; x++ ) {
					iValue = std::max(std::min(mr->GetAt(x, y, z), I_MAX), I_MIN);
					iST = st->GetAt(x, y, z);
					intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
					binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
					// increment related bin values
					if ( exc->GetAt(x, y, z)==0 ) {
						if ( iST == fg )	{
							m_hist[FGHIST][binidx] += 1.0f ;
							count_f++;
						}
						else {
							m_hist[BGHIST][binidx] += 1.0f ;
							count_b++;
						}
					}
				}
			}
		}
	}
	else {
		return false;
	}

	if( count_b == 0 || count_f == 0 )
	{
		AfxMessageBox(_T("insufficient shape template for histogram construction"));
		return false;
	}

	// normalize histogram
	float denom_b = 1.0f / (float)count_b ;
	float denom_f = 1.0f / (float)count_f ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[BGHIST][j] *= denom_b;
		m_hist[FGHIST][j] *= denom_f;
	}

	// smooth histogram
	for ( int i = 0; i < nSmooth; i++ ) {
		SmoothHistogram();
	}
	return true;	
}


bool CBilayerHistogram::ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, CVoxel *exc, 
																				  cv::Point3i &stpt, cv::Point3i &edpt, int fg, int bg, int nSmooth )
{
	VERIFY( mr->m_nX == st->m_nX && mr->m_nY == st->m_nY && mr->m_nZ == st->m_nZ );

	m_nHistBin = HIST_BIN;
	m_iMin = I_MIN;
	m_iMax = I_MAX;
	// initialize m_hist
	ClearHistogram();
	InitHistogram();

	// compute histogram
	int count_f = 0; int count_b = 0;
	short iST, iValue;
	float intensity;
	int binidx;
	int x, y, z;

	if ( exc ) {
		for( z = stpt.m_z; z <= edpt.m_z; z++ ) {
			for( y = stpt.m_y; y <= edpt.m_y; y++ ) {
				for( x = stpt.m_x; x <= edpt.m_x; x++ ) {
					iValue = std::max(std::min(mr->GetAt(x, y, z), I_MAX), I_MIN);
					iST = st->GetAt(x, y, z);
					if( iST != 0 ){
						int a=0;
					}

					intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
					binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
					// increment related bin values
					if ( exc->GetAt(x, y, z)==0 ) {
						if ( iST == fg )	{
							m_hist[FGHIST][binidx] += 1.0f ;
							count_f++;
						}
						else if ( iST == bg )	{
							m_hist[BGHIST][binidx] += 1.0f ;
							count_b++;
						}
					}
				}
			}
		}
	}
	else {
		return false;
	}

	if( count_b == 0 || count_f == 0 )
	{
		//AfxMessageBox(_T("insufficient shape template for histogram construction"));
		return false;
	}

	// normalize histogram
	float denom_b = 1.0f / (float)count_b ;
	float denom_f = 1.0f / (float)count_f ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[BGHIST][j] *= denom_b;
		m_hist[FGHIST][j] *= denom_f;
	}

	// smooth histogram
	for ( int i = 0; i < nSmooth; i++ ) {
		SmoothHistogram();
	}
	return true;	
}


bool CBilayerHistogram::ComputeFixedLengthHistogramWithROI( CVoxel *mr, CVoxel *st, CVoxel *roi, 
													cv::Point3i &stpt, cv::Point3i &edpt, int fg, int nSmooth )
{
	VERIFY( mr->m_nX == st->m_nX && mr->m_nY == st->m_nY && mr->m_nZ == st->m_nZ );

	m_nHistBin = HIST_BIN;
	m_iMin = I_MIN;
	m_iMax = I_MAX;
	// initialize m_hist
	ClearHistogram();
	InitHistogram();

	// compute histogram
	int count_f = 0; int count_b = 0;
	short iST, iValue;
	float intensity;
	int binidx;
	int x, y, z;

	if ( roi ) {
		for( z = stpt.m_z; z <= edpt.m_z; z++ ) {
			for( y = stpt.m_y; y <= edpt.m_y; y++ ) {
				for( x = stpt.m_x; x <= edpt.m_x; x++ ) {
					iValue = std::max(std::min(mr->GetAt(x, y, z), I_MAX), I_MIN);
					iST = st->GetAt(x, y, z);
					if( iST != 0 ){
						int a=0;
					}

					intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
					binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
					// increment related bin values
					if ( roi->GetAt(x, y, z) ) {
						if ( iST == fg )	{
							m_hist[FGHIST][binidx] += 1.0f ;
							count_f++;
						}
						else {
							m_hist[BGHIST][binidx] += 1.0f ;
							count_b++;
						}
					}
				}
			}
		}
	}
	else {
		return false;
	}

	if( count_b == 0 || count_f == 0 )
	{
		//AfxMessageBox(_T("insufficient shape template for histogram construction"));
		return false;
	}

	// normalize histogram
	float denom_b = 1.0f / (float)count_b ;
	float denom_f = 1.0f / (float)count_f ;
	for ( int j = 0; j < m_nHistBin; j++ )	{
		m_hist[BGHIST][j] *= denom_b;
		m_hist[FGHIST][j] *= denom_f;
	}

	// smooth histogram
	for ( int i = 0; i < nSmooth; i++ ) {
		SmoothHistogram();
	}
	return true;	
}


void CBilayerHistogram::SmoothHistogram()
{
	int j, m;
	// in the y direction
	for ( j = 0; j < m_nHistBin; j++ )
	{
		int s = ( j-WND <  0 ) ? -j    : -WND; 
		int e = ( j+WND >= m_nHistBin ) ? m_nHistBin-j-1 :  WND;
		double sum_b = 0; double sum_f = 0; 
		for ( m = s; m <= e; m++ ) {
			sum_b += m_hist[0][j+m]*w[m+WND];
			sum_f += m_hist[1][j+m]*w[m+WND];
		}
		m_hist[0][j] = sum_b;
		m_hist[1][j] = sum_f;
	}
}

void CBilayerHistogram::ComputeVUnaries(CVoxel *v, CFVoxel *fgU, CFVoxel  *bgU)
{
	int x, y, z, i;
	int voxX = v->m_nX;
	int voxY = v->m_nY;
	int voxZ = v->m_nZ;
	float fu, bu;
	for ( z = 0, i = 0; z < voxZ; z++ ) {
		for ( y = 0; y < voxY; y++ ) {
			for ( x = 0; x < voxX; x++, i++ ) {
				ComputeFUnary(v->GetAt(x,y,z), fu, bu);
				fgU->SetAt(x, y, z, fu);
				bgU->SetAt(x, y, z, bu);
			}
		}
	}
}

void CBilayerHistogram::ComputeVProbabilities(CVoxel *v, CFVoxel *fgU, CFVoxel  *bgU)
{
	int x, y, z, i;
	int voxX = v->m_nX;
	int voxY = v->m_nY;
	int voxZ = v->m_nZ;
	float fu, bu;
	for ( z = 0, i = 0; z < voxZ; z++ ) {
		for ( y = 0; y < voxY; y++ ) {
			for ( x = 0; x < voxX; x++, i++ ) {
				ComputeFProbability(v->GetAt(x,y,z), fu, bu);
				fgU->SetAt(x, y, z, fu);
				bgU->SetAt(x, y, z, bu);
			}
		}
	}
}

CBilayerHistogram *MeanHistogram(CBilayerHistogram *h1, CBilayerHistogram *h2)
{
// simple averaging of two histograms with same min, max intensities and bin number
	VERIFY (h1->GetNumHistBin() == h2->GetNumHistBin());
	int nb = h1->GetNumHistBin();
	CBilayerHistogram *avg = new CBilayerHistogram(nb);
	avg->InitHistogram();
	avg->SetIMax(h1->GetIMax());
	avg->SetIMin(h1->GetIMin());
	for ( int i = 0; i < 2; i++ ) {
		for ( int n = 0; n < nb; n++ ) {
			avg->m_hist[i][n] = (h1->m_hist[i][n]+h2->m_hist[i][n])*0.5f;
		}
	}
	return avg;
}

CBilayerHistogram *CombineHistogram(CBilayerHistogram *h1, CBilayerHistogram *h2)
{
// combination of two histograms with different min, max intensities and bin numbers
// get data of each input histogram
	int imin1 = h1->GetIMin();
	int imax1 = h1->GetIMax();
	int imin2 = h2->GetIMin();
	int imax2 = h2->GetIMax();

	int nb1 = h1->GetNumHistBin();
	int nb2 = h2->GetNumHistBin();

	double binitv1 = (double)(imax1-imin1+1)/(double)nb1;
	double binitv2 = (double)(imax2-imin2+1)/(double)nb2;
// determine new bin intensity interval and min, max intensity range 
	double newitv = std::min(binitv1, binitv2);
	int newimin = std::min(imin1, imin2);
	int newimax = std::max(imax1, imax2);
// determine number of bins for new histogram
	int newb = (int)Round((double)(newimax-newimin+1)/newitv);
// initialize new histogram
	CBilayerHistogram *newhist = new CBilayerHistogram(newb);
	newhist->InitHistogram();
// interpolate histogram values according to each bin's intensity interval
	double nis, nie, h1is, h1ie, h2is, h2ie;
	int n1s, n2s, k1, k2;
	n1s = n2s = 0;
	for ( int i = 0; i < 2; i++ ) {
		for ( int n = 0; n < newb; n++ ) {
		// get intensity interval for current bin
			nis = (double)newimin+newitv*n;
			nie = nis+newitv;
		// determine position of new start and end of intensity range in each input histogram
			for ( k1 = n1s; k1 < nb1; k1++ ) {
				h1is = (double)imin1+binitv1*k1;
				h1ie = h1is+binitv1;
				if ( nis <= h1is && h1is <= nie ) {
					if ( h1ie < nie ) {
						newhist->m_hist[i][n]+=(h1->m_hist[i][k1]);
					}
					else {
						newhist->m_hist[i][n]+=(h1->m_hist[i][k1])*(nie-h1is)/binitv1;
						if ( n+1 < newb )
							newhist->m_hist[i][n+1]+=(h1->m_hist[i][k1])*(h1ie-nie)/binitv1;
						n1s = k1;
						break;
					}
				}
			}
			for ( k2 = 0; k2 < nb2; k2++ ) {
				h2is = (double)imin2+binitv2*k2;
				h2ie = h2is+binitv2;
				if ( nis <= h2is && h2is <= nie ) {
					if ( h2ie < nie ) {
						newhist->m_hist[i][n]+=(h2->m_hist[i][k2]);
					}
					else {
						newhist->m_hist[i][n]+=(h2->m_hist[i][k2])*(nie-h2is)/binitv2;
						if ( n+1 < newb )
							newhist->m_hist[i][n+1]+=(h2->m_hist[i][k2])*(h2ie-nie)/binitv2;
						n2s = k2;
						break;
					}
				}
			}
		}
	}
	// normalize
	for ( int i = 0; i < 2; i++ ) {
		for ( int n = 0; n < newb; n++ ) {
			newhist->m_hist[i][n] *= 0.5f;
		}
	}
	return newhist;
}

CBilayerHistogram *AggregateFixedLengthHistogram(CBilayerHistogram *h1, CBilayerHistogram *h2)
{
	// construct aggregation of two fixed length histograms
	CBilayerHistogram *newhist = new CBilayerHistogram;
	newhist->SetNumHistBin(HIST_BIN);
	newhist->SetIMin(I_MIN);
	newhist->SetIMax(I_MAX);
	newhist->InitHistogram();
	for ( int j = 0; j < HIST_BIN; j++ )	{
		newhist->m_hist[0][j] = std::max(h1->m_hist[0][j], h2->m_hist[0][j]);
		newhist->m_hist[1][j] = std::max(h1->m_hist[1][j], h2->m_hist[1][j]);
	}
	return newhist;
}

void CBilayerHistogram::ComputeMeanHistogram(CTypedPtrArray<CPtrArray, CBilayerHistogram*> &histArr)
{
	// initialize histogram
	double imin, imax;
	imin = FLT_MAX;
	imax = -FLT_MAX;
	int histBinMin = INT_MAX;
	int i, k, n;
	int nHist = (int) histArr.GetSize();
	for ( i = 0; i < nHist; i++ ) {
		CBilayerHistogram *hist = histArr[i];
		if ( hist->m_iMin < imin )
			imin = hist->m_iMin;
		if ( hist->m_iMax > imax )
			imax = hist->m_iMax;
		if ( hist->m_nHistBin < histBinMin ) 
			histBinMin = hist->m_nHistBin;
	}
	m_nHistBin = histBinMin;
	m_iMin = imin;
	m_iMax = imax;
	ClearHistogram();
	InitHistogram();
	// compute mean histogram
	for ( k = 0; k < nHist; k++ ) {
		CBilayerHistogram *hist = histArr[k];
		double rBinW = (double)m_nHistBin/(double)hist->m_nHistBin;
		for ( n = 0; n < m_nHistBin; n++ ) {
			int b = std::min((int)Round((double)n*rBinW), m_nHistBin-1);
			for ( i = 0; i < 2; i++ ) {
				m_hist[i][b] += hist->m_hist[i][n];
			}
		}
	}
	for ( i = 0; i < 2; i++ ) {
		for ( n = 0; n < m_nHistBin; n++ ) {
			m_hist[i][n] /= (double)nHist;
		}
	}
}

void CBilayerHistogram::Save(CString str)
{
	ofstream outstream;
	outstream.open(str, ios::out|ios::binary|ios::trunc);
	if ( outstream.is_open() ) {
		outstream.write((char*)&m_nHistBin, sizeof(int));
		outstream.write((char*)&m_iMin, sizeof(float));
		outstream.write((char*)&m_iMax, sizeof(float));
		outstream.write((char*)m_hist[0], m_nHistBin*sizeof(double));
		outstream.write((char*)m_hist[1], m_nHistBin*sizeof(double));
		outstream.close();
	}
	else { 
		AfxMessageBox(_T("Error while saving CHistogram object!"));
	}
}

void CBilayerHistogram::SaveToWofstream(std::wofstream& outstream)
{
	outstream << m_nHistBin << endl;
	outstream << m_iMin << endl;
	outstream << m_iMax << endl;

	for ( int i = 0; i < m_nHistBin; i++ ) {
		outstream << m_hist[0][i] << endl;
		outstream << m_hist[1][i] << endl;
	}
}

void CBilayerHistogram::Load(CString str)
{
	ifstream instream;
	instream.open(str, ios::in|ios::binary);
	if ( instream.is_open() ) {
		instream.read((char*)&m_nHistBin, sizeof(int));
		instream.read((char*)&m_iMin, sizeof(float));
		instream.read((char*)&m_iMax, sizeof(float));
		ClearHistogram();
		InitHistogram();
		instream.read((char*)m_hist[0], m_nHistBin*sizeof(double));
		instream.read((char*)m_hist[1], m_nHistBin*sizeof(double));
		instream.close();
	}
	else { 
		AfxMessageBox(_T("Error while loading CHistogram object!"));
	}
}


void CBilayerHistogram::SaveHistImage(CString str)
{
	CString fgstr = ExtendPathFileName(str, _T("-FG"));
	CString bgstr = ExtendPathFileName(str, _T("-BG"));
	IplImage *fhistimg = cvCreateImage(cvSize((m_nHistBin+1)*VIEW_BIN_WIDTH, VIEW_HIST_HEIGHT), 8, 1);
	IplImage *bhistimg = cvCreateImage(cvSize((m_nHistBin+1)*VIEW_BIN_WIDTH, VIEW_HIST_HEIGHT), 8, 1);
	cvSet(fhistimg, cvScalar(255));
	cvSet(bhistimg, cvScalar(255));
	int h;
	for ( int i = 0; i < m_nHistBin; i++ ) {
		h = VIEW_HIST_HEIGHT-(int)Round(m_hist[0][i]*(double)VIEW_HIST_HEIGHT*(double)MAX_H);
		if ( h < VIEW_HIST_HEIGHT )
		cvRectangle( bhistimg, 
					 cvPoint(VIEW_BIN_WIDTH*(i), h),
					 cvPoint(VIEW_BIN_WIDTH*(i+1)-1, VIEW_HIST_HEIGHT),
					 cvScalar(0), CV_FILLED);
		h = VIEW_HIST_HEIGHT-(int)Round(m_hist[1][i]*(double)VIEW_HIST_HEIGHT*(double)MAX_H);
		if ( h < VIEW_HIST_HEIGHT )
		cvRectangle( fhistimg, 
					 cvPoint(VIEW_BIN_WIDTH*(i), h),
					 cvPoint(VIEW_BIN_WIDTH*(i+1)-1, VIEW_HIST_HEIGHT),
					 cvScalar(0), CV_FILLED);
	}
	cvSaveImage(CStringA(fgstr), fhistimg);
	cvReleaseImage(&fhistimg);
	cvSaveImage(CStringA(bgstr), bhistimg);
	cvReleaseImage(&bhistimg);
}



/*
void CBilayerHistogram::ComputeHistRgnLikelihood( CVoxel *pVoxel, BOOL bGc3D, int iZ )
{
	int i, x, y, z, nX, nY, nZ, nTotal ;
	nX = pVoxel->m_nX ;
	nY = pVoxel->m_nY ;
	nZ = bGc3D ? pVoxel->m_nZ : 1 ;
	nTotal = nX * nY * nZ ;
	SAFE_DELETE_ARRAY( m_lh[0] ) ;
	SAFE_DELETE_ARRAY( m_lh[1] ) ;
	m_lh[0] = new float[nTotal];
	m_lh[1] = new float[nTotal];
	
	i = 0 ;
	for( z = 0 ; z < nZ ; z++ )
	{
		z = bGc3D ? z : iZ ;
		for( y = 0 ; y < nY ; y++ )
		for( x = 0 ; x < nX ; x++ )
		{
			short iValue = pVoxel->GetAt( x, y, z ) ;
			float intensity = ( (float)iValue - m_iMin ) / ( (float)m_iMax - m_iMin ) ;
			ComputeHistPixelLikelihood( intensity, m_lh[0][i], m_lh[1][i] ) ;
			i++ ;
		}	
	}
}
*/

/*
void CBilayerHistogram::ComputeHistPixelLikelihood( float intensity, float &l_b, float &l_f )
{
	int int_bin = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
	l_b = ( m_hist[0][int_bin] == 0 ) ? MAX_INF + 1 : -(float)log( m_hist[0][int_bin] ) * rho ;
	l_f = ( m_hist[1][int_bin] == 0 ) ? MAX_INF + 1 : -(float)log( m_hist[1][int_bin] ) * rho ;
}
*/

// END of CBilayerHistogram implementation
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Voxel.cpp: implementation of the CVoxel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Voxel.h"
#include "VoxelShell.h"
#include "math.h"
#include "float.h"
#include "Util.h"
//#include "Histogram.h"
//#include "itkNiftiImageIO.h"

// GDCM files
//#include "gdcmFile.h"
//#include "gdcmFileHelper.h"
//#include "gdcmSerieHelper.h"
//#include "gdcmDirList.h"
//#include "gdcmUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#if 0
int CVoxel::GetNormalizeThresValue( int interval, float thresRatio)
{
	int maxVal = GetMax();
	int nAll = m_nX*m_nY*m_nZ;
	int nBin = (int)((float)maxVal/interval)+1; 

	int* hist = new int[nBin];
	for ( int j = 0; j < nBin; j++ ) {
		hist[j] = 0;
	}
	int count = 0; 
	short iValue;
	float intensity;
	int binidx;
	int x, y, z;
	for( z = 0; z < m_nZ ; z++ ) {
		for( y = 0 ; y < m_nY; y++ ) {
			for( x = 0 ; x < m_nX; x++ ) {
				iValue = GetAt(x, y, z);
				intensity = (float)iValue/(float)maxVal;
				binidx = (int)( (float)( nBin - 1 ) * intensity + 0.5f ) ;
				// increment related bin values
				hist[binidx] += 1.0;
				count++;
			}
		}
	}

	int thres = nAll*thresRatio;
	int idx;
	int histSum=0;
	for ( int j = 0; j < nBin; j++ ) {
		histSum = histSum+hist[j];
		if ( histSum > thres ) {
			idx = j;
			break;
		}
	}
	SAFE_DELETE_ARRAY(hist);

	int val = idx*interval;

	return val;
}


void CVoxel::RemoveRegion(CVoxel* removevoxel)
{
	int x,y,z;
	for(z=0;z<m_nZ;z++){
		for(y=0;y<m_nY;y++){
			for(x=0;x<m_nX;x++){
				if(removevoxel->GetAt(x,y,z)){
					SetAt(x,y,z,0);
				}
			}
		}
	}
}

void CVoxel::MakeBallSe( float fRadius )
{
	float fNowRadius ;
	int i, j, k, nRadius, nSize ;

	if( fRadius == floor( fRadius ) )
		nRadius = (int)fRadius ;
	else
		nRadius = (int)fRadius + 1 ;

	nSize = 2 * nRadius + 1 ;
	Allocate( nSize, nSize, nSize ) ;

	for ( i = 0 ; i < nSize ; i++ )
	for ( j = 0 ; j < nSize ; j++ )
	for ( k = 0 ; k < nSize ; k++ )
	{
		fNowRadius = (float)sqrt( pow( double(i - nRadius), 2 ) + pow( double(j - nRadius), 2 ) + pow( double(k - nRadius), 2 ) ) ;
		if ( fNowRadius <= fRadius )
		{
			SetAt( i, j, k, 1 ) ;
		}
	}

}

int CVoxel::FastDilate(CVoxel & SE)
{
	int i, j, k;
	int x, y, z;
	int x_o, y_o, z_o;
	int x_d, y_d, z_d;

    x_o = SE.GetXSize()/2;
    y_o = SE.GetYSize()/2;
	z_o = SE.GetZSize()/2;

	int XSize = SE.GetXSize();
	int YSize = SE.GetYSize();
	int ZSize = SE.GetZSize();

	CVoxel bound;
	bound.Allocate(m_nX, m_nY, m_nZ);

	//bool test = false;
	int count = 0;
	for ( i = 0 ; i < m_nX ; i++ )
	for ( j = 0 ; j < m_nY ; j++ )
	for ( k = 0 ; k < m_nZ ; k++ )
	{
		if ( GetAt( i, j, k ) )
		{
			x = -1; y = 0; z = 0;
			if ( !(i + x < 0 || i + x >= m_nX)
				&& !(j + y < 0 || j + y >= m_nY)
				&& !(k + z < 0 || k + z >= m_nZ)
				&& !GetAt( i+x, j+y, k+z ) )
			{
				bound.SetAt( i, j, k, 1 ) ;
				count++;
			}
			else
			{
				x = 1; y = 0; z = 0;
				if ( !(i + x < 0 || i + x >= m_nX)
					&& !(j + y < 0 || j + y >= m_nY)
					&& !(k + z < 0 || k + z >= m_nZ)
					&& !GetAt( i+x, j+y, k+z ) )
				{
					bound.SetAt( i, j, k, 1) ;
					count++;
				}
				else
				{
					x = 0; y = -1; z = 0;
					if ( !(i + x < 0 || i + x >= m_nX)
						&& !(j + y < 0 || j + y >= m_nY)
						&& !(k + z < 0 || k + z >= m_nZ)
						&& !GetAt( i+x, j+y, k+z ) )
					{
						bound.SetAt( i, j, k, 1 ) ;
						count++;
					}
					else
					{
						x = 0; y = 1; z = 0;
						if ( !(i + x < 0 || i + x >= m_nX)
							&& !(j + y < 0 || j + y >= m_nY)
							&& !(k + z < 0 || k + z >= m_nZ)
							&& !GetAt( i+x, j+y, k+z ) )
						{
							bound.SetAt( i, j, k, 1 ) ;
							count++;
						} 
						else
						{
							x = 0; y = 0; z = -1;
							if ( !(i + x < 0 || i + x >= m_nX)
								&& !(j + y < 0 || j + y >= m_nY)
								&& !(k + z < 0 || k + z >= m_nZ)
								&& !GetAt( i+x, j+y, k+z ) )
							{
								bound.SetAt( i, j, k, 1 ) ;
								count++;
							} 
							else
							{
								x = 0; y = 0; z = 1;
								if ( !(i + x < 0 || i + x >= m_nX)
									&& !(j + y < 0 || j + y >= m_nY)
									&& !(k + z < 0 || k + z >= m_nZ)
									&& !GetAt( i+x, j+y, k+z ) )
								{
									bound.SetAt( i, j, k, 1 ) ;
									count++;
								}
							}
						}
					}
				}	
			}
		}
	}

	if ( !count )
	{
		for ( i = 0 ; i < m_nX ; i++ )
		for ( j = 0 ; j < m_nY ; j++ )
		for ( k = 0 ; k < m_nZ ; k++ )
		{
			bound.SetAt( i, j, k, GetAt( i, j, k ) ) ;
		}
	}
	
	for ( i = 0 ; i < m_nX ; i++ )
	for ( j = 0 ; j < m_nY ; j++ )
	for ( k = 0 ; k < m_nZ ; k++ )
	{
		if ( bound.GetAt( i, j, k ) )
		{
			x_d = i - x_o; y_d = j - y_o; z_d = k - z_o;
			for ( x = 0 ; x < XSize ; x++ )
			for ( y = 0 ; y < YSize ; y++ )
			for ( z = 0 ; z < ZSize ; z++ )
			{
				if ( !(x_d + x < 0 || x_d + x >= m_nX)
					&& !(y_d + y < 0 || y_d + y >= m_nY)
					&& !(z_d + z < 0 || z_d + z >= m_nZ)
					&& SE.GetAt( x, y, z ) )
				{
					SetAt( x_d+x, y_d+y, z_d+z, 1 ) ;
				}
			}
		}
	}

	return GetNumber();
}

int CVoxel::FastErode(CVoxel & SE)
{	
	int i, j, k;
	int x, y, z;
	int x_o, y_o, z_o;
	int x_d, y_d, z_d;

    x_o = SE.GetXSize()/2;
    y_o = SE.GetYSize()/2;
	z_o = SE.GetZSize()/2;

	int XSize = SE.GetXSize();
	int YSize = SE.GetYSize();
	int ZSize = SE.GetZSize();

	CVoxel bound;
	bound.Allocate(m_nX, m_nY, m_nZ);

	//bool test = false;
	int count = 0;
	for ( i = 0 ; i < m_nX ; i++ )
	for ( j = 0 ; j < m_nY ; j++ )
	for ( k = 0 ; k < m_nZ ; k++ )
	{
		if ( !GetAt(i, j, k ) )
		{
			x = -1; y = 0; z = 0;
			if ( !(i + x < 0 || i + x >= m_nX)
				&& !(j + y < 0 || j + y >= m_nY)
				&& !(k + z < 0 || k + z >= m_nZ)
				&& GetAt( i+x, j+y, k+z ) )
			{
				bound.SetAt( i, j, k, 1 ) ;
				count++;
			}
			else
			{
				x = 1; y = 0; z = 0;
				if ( !(i + x < 0 || i + x >= m_nX)
					&& !(j + y < 0 || j + y >= m_nY)
					&& !(k + z < 0 || k + z >= m_nZ)
					&& GetAt( i+x, j+y, k+z ) )
				{
					bound.SetAt( i, j, k, 1 ) ;
					count++;
				}
				else
				{
					x = 0; y = -1; z = 0;
					if ( !(i + x < 0 || i + x >= m_nX)
					&& !(j + y < 0 || j + y >= m_nY)
					&& !(k + z < 0 || k + z >= m_nZ)
					&& GetAt( i+x, j+y, k+z ) )
					{
						bound.SetAt( i, j, k, 1 ) ;
						count++;
					}
					else
					{
						x = 0; y = 1; z = 0;
						if ( !(i + x < 0 || i + x >= m_nX)
							&& !(j + y < 0 || j + y >= m_nY)
							&& !(k + z < 0 || k + z >= m_nZ)
							&& GetAt( i+x, j+y, k+z ) )
						{
							bound.SetAt( i, j, k, 1 ) ;
							count++;
						}
						else
						{
							x = 0; y = 0; z = -1;
							if ( !(i + x < 0 || i + x >= m_nX)
								&& !(j + y < 0 || j + y >= m_nY)
								&& !(k + z < 0 || k + z >= m_nZ)
								&& GetAt( i+x, j+y, k+z ) )
							{
								bound.SetAt( i, j, k, 1 ) ;
								count++;
							}
							else
							{
								x = 0; y = 0; z = 1;
								if ( !(i + x < 0 || i + x >= m_nX)
									&& !(j + y < 0 || j + y >= m_nY)
									&& !(k + z < 0 || k + z >= m_nZ)
									&& GetAt( i+x, j+y, k+z ) )
								{
									bound.SetAt( i, j, k, 1 ) ;
									count++;
								}
							}
						}
					}
				}
			}
		}
	}
	
	if( !count )
	{
		for ( i = 0 ; i < m_nX ; i++ )
		for ( j = 0 ; j < m_nY ; j++ )
		for ( k = 0 ; k < m_nZ ; k++ )
			bound.SetAt( i, j, k, GetAt(i, j, k ) ) ;
	}
	
	for ( i = 0 ; i < m_nX ; i++ )
	for ( j = 0 ; j < m_nY ; j++ )
	for ( k = 0 ; k < m_nZ ; k++ )
	{
		if ( bound.GetAt(i, j, k ) )
		{
			x_d = i - x_o; y_d = j - y_o; z_d = k - z_o;
			for ( x = 0 ; x < XSize ; x++ )
			for ( y = 0 ; y < YSize ; y++ )
			for ( z = 0 ; z < ZSize ; z++ )
			{
				if ( !(x_d + x < 0 || x_d + x >= m_nX)
					&& !(y_d + y < 0 || y_d + y >= m_nY)
					&& !(z_d + z < 0 || z_d + z >= m_nZ)
					&& SE.GetAt(x, y,z ) )
				{
					SetAt( x_d+x, y_d+y, z_d+z, 0 ) ; 
				}
			}
		}
	}

	return GetNumber();
}

void CVoxel::Opening()
{
	int i, j, k;
	CVoxel SE;
	SE.Allocate(3,3,3);

	for ( i = 0 ; i < 3 ; i++ ) {
		for ( j = 0 ; j < 3 ; j++ ) {
			for ( k = 0 ; k < 3 ; k++ ) {
				SE.SetAt( i, j, k, 1 ) ;
			}
		}
	}

	FastErode(SE);
	FastDilate(SE);
}

void CVoxel::Opening(CVoxel & SE)
{	
	FastErode(SE);
	FastDilate(SE);
}

void CVoxel::Closing()
{
	int i, j, k;
	CVoxel SE;
	int size = 5;
	SE.Allocate(size,size,size);

	for ( i = 0 ; i < size ; i++ ) {
		for ( j = 0 ; j < size ; j++ ) {
			for ( k = 0 ; k < size ; k++ ) {
				SE.SetAt( i, j, k, 1 ) ;
			}
		}
	}

	FastDilate(SE);
	FastErode(SE);	
}

void CVoxel::ClosingWithErode()
{
	int i, j, k;
	CVoxel SE;
	int size = 5;
	SE.Allocate(size,size,size);

	for ( i = 0 ; i < size ; i++ ) {
		for ( j = 0 ; j < size ; j++ ) {
			for ( k = 0 ; k < size ; k++ ) {
				SE.SetAt( i, j, k, 1 ) ;
			}
		}
	}

	FastDilate(SE);
	FastErode(SE);	
	FastErode(SE);	
	FastErode(SE);	
	FastErode(SE);	
}

void CVoxel::Closing(CVoxel & SE)
{
	FastDilate(SE);
	FastErode(SE);	
}

int CVoxel::GetNumber()
{
	int i, j, k;
	int count = 0;
	for ( i = 0 ; i < m_nX ; i++ ) {
		for ( j = 0 ; j < m_nY ; j++ ) {
			for ( k = 0 ; k < m_nZ ; k++ ) {
				if ( (*this).GetAt(i, j, k ) ) count++;
			}
		}
	}

	return count;
}

int CVoxel::GetNumberWithIdx(int val)
{
	int i, j, k;
	int count = 0;
	for ( i = 0 ; i < m_nX ; i++ ) {
		for ( j = 0 ; j < m_nY ; j++ ) {
			for ( k = 0 ; k < m_nZ ; k++ ) {
				if ( (*this).GetAt(i, j, k ) == val ) count++;
			}
		}
	}

	return count;
}

bool CVoxel::DrawRect( CRect *pRect, int iValue, int iZ )
{
	if( iZ < 0 || iZ >= m_nZ )
		return false ;

	int x, x0, x1, y, y0, y1 ;
	CPoint pnt0, pnt1 ;
	pnt0 = pRect->TopLeft( ) ;
	pnt1 = pRect->BottomRight( ) ;
	x0 = std::min( pnt0.x, pnt1.x ) ;
	x0 = std::max( 0, x0 ) ;
	x1 = std::max( pnt0.x, pnt1.x ) ;
	x1 = std::min( m_nX - 1, x1 ) ;
	y0 = std::min( pnt0.y, pnt1.y ) ;
	y0 = std::max( 0, y0 ) ;
	y1 = std::max( pnt0.y, pnt1.y ) ;
	y1 = std::min( m_nY - 1, y1 ) ;

	for( y = y0 ; y <= y1 ; y++ )
	for( x = x0 ; x <= x1 ; x++ )
	{
		SetAt( x, y, iZ, iValue ) ;
	}


	return true ;
}

// the reference voxels are "1",  // 02/04/008
// and the other voxels ( p in S ) whose DT values should be calculated are "0".
void CVoxel::GetDistanceTransform( CVoxel *pDt )
{
	int i, j, k, dtIncrement, dtU, dtV, iValue, iValue2, nBVs ;
	cv::Point3i u, v ;
	CArray< cv::Point3i, cv::Point3i& > BVs ; // boundary voxels
	
	// For all voxels p in S, assign a DT as infinity
	for( v.z = 0 ; v.z < m_nZ ; v.z++ )
	for( v.y = 0 ; v.y < m_nY ; v.y++ )
	for( v.x = 0 ; v.x < m_nX ; v.x++ )
	{
		iValue = GetAt( v ) ;
		if( iValue == 0 ) 
			pDt->SetAt( v, SHRT_MAX ) ;
	}
	
	// Calculate DT values for v
	// and initialze the array of bounday voxels, BVs
	int kk = 1 ;
	for( v.z = 0 ; v.z < m_nZ ; v.z++ )
	for( v.y = 0 ; v.y < m_nY ; v.y++ )
	for( v.x = 0 ; v.x < m_nX ; v.x++ )
	{
		iValue = GetAt( v ) ;
		if( iValue == 0 ) // voxels in S
		{
			for( k = -1 ; k <= 1 && v.z+k >= 0 && v.z+k < m_nZ ; k++ )
			for( j = -1 ; j <= 1 && v.y+j >= 0 && v.y+j < m_nY ; j++ )
			for( i = -1 ; i <= 1 && v.x+i >= 0 && v.x+i < m_nX ; i++ )
			{
				iValue2 = GetAt( v.x+i, v.y+j, v.z+k ) ;
				if( iValue2 == 1 ) // reference voxels
				{
					dtV = pDt->GetAt( v ) ;
					dtIncrement = i*i + j*j + k*k + 2 ; // <3,4,5> the weighted metric
					if( dtV == SHRT_MAX )
					{
						BVs.Add( v ) ; // add to BVs
					}
					if( dtIncrement < dtV )
						pDt->SetAt( v, dtIncrement ) ; // set DT value for the voxel 
				}
			}
		}
	}
	
	// Propagate the boundary
	while( BVs.GetSize( ) )
	{
		nBVs = (int) BVs.GetSize( ) ;
		v = BVs[0] ; // Repeat for all v in BVs
		dtV = pDt->GetAt( v ) ; // DT value for v
		
		for( k = -1 ; k <= 1 && v.z+k >= 0 && v.z+k < m_nZ ; k++ ) // find all voxels u in S which are neighbors of v
		for( j = -1 ; j <= 1 && v.y+j >= 0 && v.y+j < m_nY ; j++ ) // assing DtU as min{ DtU, Dtv+( 3 for face/ 4 for edge / 5 for vertex ) }
		for( i = -1 ; i <= 1 && v.x+i >= 0 && v.x+i < m_nX ; i++ )
		{
			u.x = v.x + i ;
			u.y = v.y + j ;
			u.z = v.z + k ;
			if( GetAt( u ) == 0 )
			{
				dtU = pDt->GetAt( u ) ;
				dtIncrement =  i*i + j*j + k*k + 2 ; // <3,4,5> the weighted metric
				if( dtU == SHRT_MAX )
					BVs.Add( u ) ; // add u to BVs
				if( dtV + dtIncrement < dtU )
					pDt->SetAt( u, dtV + dtIncrement ) ;
			}
		}
		BVs.RemoveAt( 0 ) ; // remove v from BVs
	}
}

void CVoxel::GetDistanceTransformFastNarrow( CVoxel *pDt ) // 02/04/008
{
	int i, j, k, dtIncrement, dtU, dtV, iValue, iValue2, nBVs ;
	cv::Point3i u, v ;
	CArray< cv::Point3i, cv::Point3i& > BVs ; // boundary voxels
	
	// For all voxels p in S, assign a DT as infinity
	for( v.z = 0 ; v.z < m_nZ ; v.z++ )
	for( v.y = 0 ; v.y < m_nY ; v.y++ )
	for( v.x = 0 ; v.x < m_nX ; v.x++ )
	{
		iValue = GetAt( v ) ;
		if( iValue == 0 ) 
			pDt->SetAt( v, SHRT_MAX ) ;
	}
	
	// Calculate DT values for v
	// and initialze the array of bounday voxels, BVs
	int kk = 1 ;
	for( v.z = 0 ; v.z < m_nZ ; v.z++ )
	for( v.y = 0 ; v.y < m_nY ; v.y++ )
	for( v.x = 0 ; v.x < m_nX ; v.x++ )
	{
		iValue = GetAt( v ) ;
		if( iValue == 0 ) // voxels in S
		{
			for( k = -1 ; k <= 1 && v.z+k >= 0 && v.z+k < m_nZ ; k++ )
			for( j = -1 ; j <= 1 && v.y+j >= 0 && v.y+j < m_nY ; j++ )
			for( i = -1 ; i <= 1 && v.x+i >= 0 && v.x+i < m_nX ; i++ )
			{
				iValue2 = GetAt( v.x+i, v.y+j, v.z+k ) ;
				if( iValue2 == 1 ) // reference voxels
				{
					dtV = pDt->GetAt( v ) ;
					dtIncrement = i*i + j*j + k*k + 2 ; // <3,4,5> the weighted metric
					if( dtV == SHRT_MAX )
					{
						BVs.Add( v ) ; // add to BVs
					}
					if( dtIncrement < dtV )
						pDt->SetAt( v, dtIncrement ) ; // set DT value for the voxel 
				}
			}
		}
	}
	
	// Propagate the boundary
	while( BVs.GetSize( ) )
	{
		nBVs = (int) BVs.GetSize( ) ;
		v = BVs[0] ; // Repeat for all v in BVs
		dtV = pDt->GetAt( v ) ; // DT value for v
		
		for( k = -1 ; k <= 1 && v.z+k >= 0 && v.z+k < m_nZ ; k++ ) // find all voxels u in S which are neighbors of v
		for( j = -1 ; j <= 1 && v.y+j >= 0 && v.y+j < m_nY ; j++ ) // assing DtU as min{ DtU, Dtv+( 3 for face/ 4 for edge / 5 for vertex ) }
		for( i = -1 ; i <= 1 && v.x+i >= 0 && v.x+i < m_nX ; i++ )
		{
			u.x = v.x + i ;
			u.y = v.y + j ;
			u.z = v.z + k ;
			if( GetAt( u ) == 0 )
			{
				dtU = pDt->GetAt( u ) ;
				dtIncrement =  i*i + j*j + k*k + 2 ; // <3,4,5> the weighted metric
				if( dtU == SHRT_MAX )
					BVs.Add( u ) ; // add u to BVs
				if( dtV + dtIncrement < dtU )
					pDt->SetAt( u, dtV + dtIncrement ) ;
			}
		}
		BVs.RemoveAt( 0 ) ; // remove v from BVs
	}
}

void CVoxel::GetFastDistanceTransform( CFVoxel *pfDT )
{
	int x, y, z, iValue ;
	float fValue ;

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		iValue = GetAt( x, y, z ) ;
		if( iValue == 0 )
			pfDT->SetAt( x, y, z, FLT_MAX ) ;
		else
			pfDT->SetAt( x, y, z, 0.0f ) ;
	}

	float *fValues = new float[ std::max( std::max( m_nX, m_nY ), m_nZ ) ] ;
	float *fDists ;

	// transform along Z-direction
	for( x = 0 ; x < m_nX ; x++ )
	{
		for( y = 0 ; y < m_nY ; y++ )
		{
			for( z = 0 ; z < m_nZ ; z++ )
				fValues[z] = pfDT->GetAt( x, y, z ) ;

			fDists = dt( fValues, m_nZ ) ;

			for( z = 0 ; z < m_nZ ; z++ )
			  pfDT->SetAt( x, y, z, fDists[z] ) ;

			delete [] fDists ;
		}
	}
	
	// transform along Y-direction
	for( z = 0 ; z < m_nZ ; z++ )
	{
		for( x = 0 ; x < m_nX ; x++ )
		{
			for( y = 0 ; y < m_nY ; y++ )
				fValues[y] = pfDT->GetAt( x, y, z ) ;

			fDists = dt( fValues, m_nY ) ;

			for( y = 0 ; y < m_nY ; y++ )
			  pfDT->SetAt( x, y, z, fDists[y] ) ;

			delete [] fDists ;
		}
	}

	// transform along X-direction
	for( y = 0 ; y < m_nY ; y++ )
	{
		for( z = 0 ; z < m_nZ ; z++ )
		{
			for( x = 0 ; x < m_nX ; x++ )
				fValues[x] = pfDT->GetAt( x, y, z ) ;

			fDists = dt( fValues, m_nX ) ;

			for( x = 0 ; x < m_nX ; x++ )
			  pfDT->SetAt( x, y, z, fDists[x] ) ;

			delete [] fDists ;
		}
	}

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		fValue = pfDT->GetAt( x, y, z ) ;
		pfDT->SetAt( x, y, z, (float)sqrt( fValue ) ) ;
	}

	delete [] fValues ;
}

CFVoxel *CVoxel::GetFastDistanceTransform()
{
	CFVoxel *dtv = new CFVoxel(m_nX, m_nY, m_nZ, FALSE);
	GetFastDistanceTransform(dtv);
	return dtv;
}

int CVoxel::deleteSmallRegionsByCca( CVoxel *pLabeledCca, int iConnection, int MinSize )
{
	int i, j, k, n, x, y, z, oldN, count, label, Value, CompNo, VoxelCount ;
	cv::Point3i U, V, Seed ;
	CArray< int, int& > VoxelCounts, voxelLabels ;
	CArray< cv::Point3i, cv::Point3i& > Lambda ;
	CVoxel *pCopy = new CVoxel( *this ) ;  // added 07/14/008

	CompNo = LABEL_BASE ;
	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		pLabeledCca->SetAt( x, y, z, 0 ) ;
		Value = GetAt( x, y, z ) ;
		if( Value == 0 || Value >= LABEL_BASE )
			continue ;
		
		Seed.x = x ;
		Seed.y = y ;
		Seed.z = z ;
		SetAt( Seed, CompNo ) ;
		Lambda.Add( Seed ) ;
		VoxelCount = 1 ;

		while( Lambda.GetSize( ) )
		{
			U = Lambda[0] ;
			Lambda.RemoveAt(0) ;
			if( iConnection == 26 )
			{
				for( k = -1 ; k <= 1 ; k++ )
				for( j = -1 ; j <= 1 ; j++ )
				for( i = -1 ; i <= 1 ; i++ )
				{
					V.x = U.x + i ;
					V.y = U.y + j ;
					V.z = U.z + k ;
					if( V.x < 0 || V.x >= m_nX ||
						V.y < 0 || V.y >= m_nY ||
						V.z < 0 || V.z >= m_nZ )
						continue ;
					Value = GetAt( V ) ;
					if( Value == 0 || Value >= LABEL_BASE )
						continue ;
					else // if( Value == 1 ) 
					{
						SetAt( V, CompNo ) ;
						Lambda.Add( V ) ;
						VoxelCount++ ;
					}
				}
			}
			else // 6-connectedness
			{
				V.x = U.x ;
				V.y = U.y ;
				V.z = U.z - 1 ;
				if( V.z >= 0 )
				{
					Value = GetAt( V ) ;
					if( Value > 0 && Value < LABEL_BASE )
					{
						SetAt( V, CompNo ) ;
						Lambda.Add( V ) ;
						VoxelCount++ ;
					}
				}
				V.z = U.z + 1 ;
				if( V.z < m_nZ )
				{
					Value = GetAt( V ) ;
					if( Value > 0 && Value < LABEL_BASE )
					{
						SetAt( V, CompNo ) ;
						Lambda.Add( V ) ;
						VoxelCount++ ;
					}
				}

				V.x = U.x ;
				V.y = U.y - 1 ;
				V.z = U.z ;
				if( V.y >= 0 )
				{
					Value = GetAt( V ) ;
					if( Value > 0 && Value < LABEL_BASE )
					{
						SetAt( V, CompNo ) ;
						Lambda.Add( V ) ;
						VoxelCount++ ;
					}
				}
				V.y = U.y + 1 ;
				if( V.y < m_nY )
				{
					Value = GetAt( V ) ;
					if( Value > 0 && Value < LABEL_BASE )
					{
						SetAt( V, CompNo ) ;
						Lambda.Add( V ) ;
						VoxelCount++ ;
					}
				}

				V.x = U.x - 1 ;
				V.y = U.y ;
				V.z = U.z ;
				if( V.x >= 0 )
				{
					Value = GetAt( V ) ;
					if( Value > 0 && Value < LABEL_BASE )
					{
						SetAt( V, CompNo ) ;
						Lambda.Add( V ) ;
						VoxelCount++ ;
					}
				}
				V.x = U.x + 1 ;
				if( V.x < m_nX )
				{
					Value = GetAt( V ) ;
					if( Value > 0 && Value < LABEL_BASE )
					{
						SetAt( V, CompNo ) ;
						Lambda.Add( V ) ;
						VoxelCount++ ;
					}
				}
			}
		}

		voxelLabels.Add( CompNo ) ;
		VoxelCounts.Add( VoxelCount ) ;
		CompNo++ ;
	}

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		Value = GetAt( x, y, z ) ;
		if( Value == 0 )
			continue ;
		else
		{
			VoxelCount = VoxelCounts[ Value - LABEL_BASE ] ;
			if( VoxelCount < MinSize ) // remove regions of less than MinSize
				SetAt( x, y, z, 0 ) ;
			else
			{
				SetAt( x, y, z, 1 ) ;
				pLabeledCca->SetAt( x, y, z, Value ) ;
			}
		}
	}

	// remove the elements whose numbers of voxels less than MinSize from the arrays
	oldN = (int) VoxelCounts.GetSize( ) ;
	for( j = 0 ; j < VoxelCounts.GetSize( ) ; j++ )
	{
		count = VoxelCounts[j] ;
		if( count < MinSize )
		{
			VoxelCounts.RemoveAt( j ) ;
			voxelLabels.RemoveAt( j ) ;
			j-- ; // not increase j as 1 element was removed
		}
	}

	// sorting by straight inserstion in descending order
	n = (int) VoxelCounts.GetSize( ) ;
	for( j = 1 ; j < n ; j++ ) 
	{
		count = VoxelCounts[j] ;
		label = voxelLabels[j] ;
		i = j - 1 ;
		while( i >= 0 && VoxelCounts[i] < count ) 
		{
			VoxelCounts[i+1] = VoxelCounts[i] ;
			voxelLabels[i+1] = voxelLabels[i] ;
			i--;
		}
		VoxelCounts[i+1] = count ;
		voxelLabels[i+1] = label ;
	}

	// get the inverse mapping of voxeLabels
	int *invLabels = new int[ oldN ] ;
	for( j = 0 ; j < oldN ; j++ )
	{
		for( i = 0 ; i < n ; i++ )
			if( voxelLabels[i] == j + LABEL_BASE )
				break ;
		invLabels[j] = i + LABEL_BASE ;
	}

	// relabel as descending order
	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		label = pLabeledCca->GetAt( x, y, z ) ;
		if( label != 0 )
			pLabeledCca->SetAt( x, y, z, invLabels[ label - LABEL_BASE ] ) ;
	}

	delete [] invLabels ;
	this->Copy( *pCopy ) ; // added 07/14/008
	delete pCopy ;
	return n ;
}

/*
int CVoxel::deleteSmallRegionsByCcaAndGetParts( CVoxel *pLabeledCca, CParts &parts, int iConnection, int MinSize )
{
	int i, j, k, n, x, y, z, oldN, count, label, Value, CompNo, VoxelCount ;
	cv::Point3i U, V, Seed ;
	CArray< int, int& > VoxelCounts, voxelLabels ;
	CConnectedComponent Lambda, cc ;
	CVoxel *pCopy = new CVoxel( *this ) ;  // added 07/14/008

	parts.RemoveAll( ) ;
	CompNo = LABEL_BASE ;
	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		pLabeledCca->SetAt( x, y, z, 0 ) ;
		Value = GetAt( x, y, z ) ;
		if( Value == 0 || Value >= LABEL_BASE )
			continue ;
		
		Seed.x = x ;
		Seed.y = y ;
		Seed.z = z ;
		SetAt( Seed, CompNo ) ;
		Lambda.Add( Seed ) ;
		cc.RemoveAll( ) ;
		parts.Add( cc ) ;
		parts[CompNo-LABEL_BASE].Add( Seed ) ; // 07/14/008
		VoxelCount = 1 ;

		while( Lambda.GetSize( ) )
		{
			U = Lambda[0] ;
			Lambda.RemoveAt(0) ;
			if( iConnection == 26 )
			{
				for( k = -1 ; k <= 1 ; k++ )
				for( j = -1 ; j <= 1 ; j++ )
				for( i = -1 ; i <= 1 ; i++ )
				{
					V.x = U.x + i ;
					V.y = U.y + j ;
					V.z = U.z + k ;
					if( V.x < 0 || V.x >= m_nX ||
						V.y < 0 || V.y >= m_nY ||
						V.z < 0 || V.z >= m_nZ )
						continue ;
					Value = GetAt( V ) ;
					if( Value == 0 || Value >= LABEL_BASE )
						continue ;
					else // if( Value == 1 ) 
					{
						SetAt( V, CompNo ) ;
						Lambda.Add( V ) ;
						parts[CompNo-LABEL_BASE].Add( V ) ; // 07/14/008
						VoxelCount++ ;
					}
				}
			}
			else // 6-connectedness
			{
				V.x = U.x ;
				V.y = U.y ;
				V.z = U.z - 1 ;
				if( V.z >= 0 )
				{
					Value = GetAt( V ) ;
					if( Value > 0 && Value < LABEL_BASE )
					{
						SetAt( V, CompNo ) ;
						Lambda.Add( V ) ;
						parts[CompNo-LABEL_BASE].Add( V ) ; // 07/14/008
						VoxelCount++ ;
					}
				}
				V.z = U.z + 1 ;
				if( V.z < m_nZ )
				{
					Value = GetAt( V ) ;
					if( Value > 0 && Value < LABEL_BASE )
					{
						SetAt( V, CompNo ) ;
						Lambda.Add( V ) ;
						parts[CompNo-LABEL_BASE].Add( V ) ; // 07/14/008
						VoxelCount++ ;
					}
				}

				V.x = U.x ;
				V.y = U.y - 1 ;
				V.z = U.z ;
				if( V.y >= 0 )
				{
					Value = GetAt( V ) ;
					if( Value > 0 && Value < LABEL_BASE )
					{
						SetAt( V, CompNo ) ;
						Lambda.Add( V ) ;
						parts[CompNo-LABEL_BASE].Add( V ) ; // 07/14/008
						VoxelCount++ ;
					}
				}
				V.y = U.y + 1 ;
				if( V.y < m_nY )
				{
					Value = GetAt( V ) ;
					if( Value > 0 && Value < LABEL_BASE )
					{
						SetAt( V, CompNo ) ;
						Lambda.Add( V ) ;
						parts[CompNo-LABEL_BASE].Add( V ) ; // 07/14/008
						VoxelCount++ ;
					}
				}

				V.x = U.x - 1 ;
				V.y = U.y ;
				V.z = U.z ;
				if( V.x >= 0 )
				{
					Value = GetAt( V ) ;
					if( Value > 0 && Value < LABEL_BASE )
					{
						SetAt( V, CompNo ) ;
						Lambda.Add( V ) ;
						parts[CompNo-LABEL_BASE].Add( V ) ; // 07/14/008
						VoxelCount++ ;
					}
				}
				V.x = U.x + 1 ;
				if( V.x < m_nX )
				{
					Value = GetAt( V ) ;
					if( Value > 0 && Value < LABEL_BASE )
					{
						SetAt( V, CompNo ) ;
						Lambda.Add( V ) ;
						parts[CompNo-LABEL_BASE].Add( V ) ; // 07/14/008
						VoxelCount++ ;
					}
				}
			}
		}

		voxelLabels.Add( CompNo ) ;
		VoxelCounts.Add( VoxelCount ) ;
		CompNo++ ;
	}

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		Value = GetAt( x, y, z ) ;
		if( Value == 0 )
			continue ;
		else
		{
			VoxelCount = VoxelCounts[ Value - LABEL_BASE ] ;
			if( VoxelCount < MinSize ) // remove regions of less than MinSize
			{
				SetAt( x, y, z, 0 ) ;
			}
			else
			{
				SetAt( x, y, z, 1 ) ;
				pLabeledCca->SetAt( x, y, z, Value ) ;
			}
		}
	}

	// remove the elements whose numbers of voxels less than MinSize from the arrays
	oldN = VoxelCounts.GetSize( ) ;
	for( j = 0 ; j < VoxelCounts.GetSize( ) ; j++ )
	{
		count = VoxelCounts[j] ;
		if( count < MinSize )
		{
			VoxelCounts.RemoveAt( j ) ;
			voxelLabels.RemoveAt( j ) ;
			parts.RemoveAt( j ) ;
			j-- ; // not increase j as 1 element was removed
		}
	}

	// sorting by straight inserstion in descending order
	n = VoxelCounts.GetSize( ) ;
	for( j = 1 ; j < n ; j++ ) 
	{
		count = VoxelCounts[j] ;
		label = voxelLabels[j] ;
		cc.RemoveAll( ) ;
		cc = parts[j] ; // 07/14/008
		i = j - 1 ;
		while( i >= 0 && VoxelCounts[i] < count ) 
		{
			VoxelCounts[i+1] = VoxelCounts[i] ;
			voxelLabels[i+1] = voxelLabels[i] ;
			parts[i+1].RemoveAll( ) ;
			parts[i+1] = parts[i] ; // 07/14/008
			i--;
		}
		VoxelCounts[i+1] = count ;
		voxelLabels[i+1] = label ;
		parts[i+1].RemoveAll( ) ;
		parts[i+1] = cc ; // 07/14/008
	}

	// get the inverse mapping of voxeLabels
	int *invLabels = new int[ oldN ] ;
	for( j = 0 ; j < oldN ; j++ )
	{
		for( i = 0 ; i < n ; i++ )
			if( voxelLabels[i] == j + LABEL_BASE )
				break ;
		invLabels[j] = i + LABEL_BASE ;
	}

	// relabel as descending order
	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		label = pLabeledCca->GetAt( x, y, z ) ;
		if( label != 0 )
			pLabeledCca->SetAt( x, y, z, invLabels[ label - LABEL_BASE ] ) ;
	}

	// 07/15/008
/ *	int nParts, nPnts ;
	cv::Point3i pntFrom ;
	CConnectedComponent cc2 ;
	nParts = parts.GetSize( ) ;
	for( n = 0 ; n < nParts ; n++ )
	{
		nPnts = parts[n].GetSize( ) ;
		pntFrom = parts[n][nPnts-1] ;
		pCopy->GetAPart( cc2, pntFrom, 26 ) ;
		retParts.Add( cc2 ) ;
	} * /

	delete [] invLabels ;
	this->Copy( *pCopy ) ; // added 07/14/008
	delete pCopy ;
	return n ;
}
*/
/*
void CVoxel::GetParts( CParts &parts, int iConnection, int minCcSize )
{
	int n, x, y, z, nCcs, iValue ;
	cv::Point3i U ;
	CConnectedComponent cc ;
	CVoxel *pLabel = new CVoxel( m_nX, m_nY, m_nZ ) ;
	
	nCcs = deleteSmallRegionsByCca( pLabel, iConnection, minCcSize ) ;
	
	parts.RemoveAll( ) ; // 007/06/05
	for( n = 0 ; n < nCcs ; n++ )
		parts.Add( cc ) ;

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		iValue = pLabel->GetAt( x, y, z ) - LABEL_BASE ;
		if( iValue >= 0 )
		{
			U.x = x ;
			U.y = y ;
			U.z = z ;
			parts[iValue].Add( U ) ;
		}
	}

	delete pLabel ;
}
*/
/*
void CVoxel::GetAPart( CConnectedComponent &cc, cv::Point3i pntSeed, int iConnection )
{
	int i, j, k, x, y, z, iValue ;
	cv::Point3i U, V ;
	CConnectedComponent Lambda ;
	CVoxel *pLabel = new CVoxel( *this ) ;

	pLabel->SetAt( pntSeed, LABEL_BASE ) ;
	Lambda.Add( pntSeed ) ;

	while( Lambda.GetSize( ) )
	{
		U = Lambda[0] ;
		Lambda.RemoveAt(0) ;
		if( iConnection == 26 )
		{
			for( k = -1 ; k <= 1 ; k++ )
			for( j = -1 ; j <= 1 ; j++ )
			for( i = -1 ; i <= 1 ; i++ )
			{
				V.x = U.x + i ;
				V.y = U.y + j ;
				V.z = U.z + k ;
				if( V.x < 0 || V.x >= m_nX ||
					V.y < 0 || V.y >= m_nY ||
					V.z < 0 || V.z >= m_nZ )
					continue ;
				iValue = pLabel->GetAt( V ) ;
				if( iValue == 0 || iValue >= LABEL_BASE )
					continue ;
				else // if( Value == 1 ) 
				{
					pLabel->SetAt( V, LABEL_BASE ) ;
					Lambda.Add( V ) ;
				}
			}
		}
		else // 6-connectedness
		{
			V.x = U.x ;
			V.y = U.y ;
			V.z = U.z - 1 ;
			if( V.z >= 0 )
			{
				iValue = pLabel->GetAt( V ) ;
				if( iValue > 0 && iValue < LABEL_BASE )
				{
					pLabel->SetAt( V, LABEL_BASE ) ;
					Lambda.Add( V ) ;
				}
			}
			V.z = U.z + 1 ;
			if( V.z < m_nZ )
			{
				iValue = pLabel->GetAt( V ) ;
				if( iValue > 0 && iValue < LABEL_BASE )
				{
					pLabel->SetAt( V, LABEL_BASE ) ;
					Lambda.Add( V ) ;
				}
			}

			V.x = U.x ;
			V.y = U.y - 1 ;
			V.z = U.z ;
			if( V.y >= 0 )
			{
				iValue = pLabel->GetAt( V ) ;
				if( iValue > 0 && iValue < LABEL_BASE )
				{
					pLabel->SetAt( V, LABEL_BASE ) ;
					Lambda.Add( V ) ;
				}
			}
			V.y = U.y + 1 ;
			if( V.y < m_nY )
			{
				iValue = pLabel->GetAt( V ) ;
				if( iValue > 0 && iValue < LABEL_BASE )
				{
					pLabel->SetAt( V, LABEL_BASE ) ;
					Lambda.Add( V ) ;
				}
			}

			V.x = U.x - 1 ;
			V.y = U.y ;
			V.z = U.z ;
			if( V.x >= 0 )
			{
				iValue = pLabel->GetAt( V ) ;
				if( iValue > 0 && iValue < LABEL_BASE )
				{
					pLabel->SetAt( V, LABEL_BASE ) ;
					Lambda.Add( V ) ;
				}
			}
			V.x = U.x + 1 ;
			if( V.x < m_nX )
			{
				iValue = pLabel->GetAt( V ) ;
				if( iValue > 0 && iValue < LABEL_BASE )
				{
					pLabel->SetAt( V, LABEL_BASE ) ;
					Lambda.Add( V ) ;
				}
			}
		}
	}

	cc.RemoveAll( ) ;
	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		iValue = pLabel->GetAt( x, y, z ) ;
		if( iValue >= LABEL_BASE )
		{
			U.x = x ;
			U.y = y ;
			U.z = z ;
			cc.Add( U ) ;
		}
	}

	delete pLabel ;
}
*/

bool CVoxel::KeepOnlySkin( )
{
	int x, y, z, iValue ;

	CVoxel *pTemp = new CVoxel( *this ) ;
	this->ClearToZeroes( ) ;

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		iValue = pTemp->GetAt( x, y, z ) ;
		if( iValue > 0 )
		{
			if( x == 0 || x == m_nX - 1 || y == 0 || y == m_nY - 1 || z == 0 || z == m_nZ - 1 )
				SetAt( x, y, z, 1 ) ;
			else if( pTemp->GetAt( x - 1, y, z ) == 0 || pTemp->GetAt( x + 1, y, z ) == 0 || 
					 pTemp->GetAt( x, y - 1, z ) == 0 || pTemp->GetAt( x, y + 1, z ) == 0 || 
					 pTemp->GetAt( x, y, z - 1 ) == 0 || pTemp->GetAt( x, y, z + 1 ) == 0 )
				SetAt( x, y, z, 1 ) ;
		}
	}
	
	delete pTemp ;
	return true ;
}

bool CVoxel::GenerateModelP01( CVoxel *pModel, int iMinY, int iZ0, int iZ1, cv::Point3i &vMid )
{
	bool bMinTouch, bMaxTouch ;
	int x, y, z, iMaxY, iValue ;
	cv::Point3i v ;

	for( z = iZ0 ; z <= iZ1 ; z++ )
	{
		// first find out the concave extremum in the mid-line
		bMaxTouch = false ;
		for( y = vMid.y; y < m_nY && bMaxTouch == false ; y++ )
		{
			iValue = GetAt( vMid.x, y, z ) ;
			if( iValue > 0 )
			{
				iMaxY = y ;
				bMaxTouch = true ;
			}
		}

		int kk = 0 ;
		// second, scan horizontally moving vertically downward
		for( y = iMinY ; y <= iMaxY ; y++ )
		{
			bMinTouch = false ;
			for( x = vMid.x ; x >= 0 && bMinTouch == false ; x-- )
			{
				iValue = GetAt( x, y, z ) ;
				if( iValue > 0 )
				{
					bMinTouch = true ;
					pModel->SetAt( x, y, z, 1 ) ;
				}
			}
			bMaxTouch = false ;
			for( x = vMid.x ; x < m_nX && bMaxTouch == false ; x++ )
			{
				iValue = GetAt( x, y, z ) ;
				if( iValue > 0 )
				{
					bMaxTouch = true ;
					pModel->SetAt( x, y, z, 1 ) ;
				}
			}
		}
		
		// third, scan vertically moving horizontally toward right
		for( x = vMid.x - 50 ; x <= vMid.x + 50 ; x++ )
		{
			bMaxTouch = false ;
			for( y = iMinY ; y < m_nY && bMaxTouch == false ; y++ )
			{
				iValue = GetAt( x, y, z ) ;
				if( iValue > 0 )
				{
					bMaxTouch = true ;
					pModel->SetAt( x, y, z, 1 ) ;
				}
			}
		}
	}

	return true ;
}

/*
bool CVoxel::MatchByTranslation( CVoxel *pModel, CVoxel *pMatched, CVoxel *pDt, cv::Point3i *pntTranss, int nParts )
{
	int iMatchW, iMatchH, iMatchD, iMatchW2, iMatchH2, iMatchD2 ;
	iMatchW = iMatchH = 60 ;
	iMatchD = 80 ;
	iMatchW2 = iMatchW / 2 ;
	iMatchH2 = iMatchH / 2 ;
	iMatchD2 = iMatchD / 2 ;

	int w, h, d, m, n, x, y, z, iValue, nPnts, iTruncThresh, iChamf ;
	cv::Point3i v, vMin, vMax ;
	CConnectedComponent cc, cci ;

	iTruncThresh = 100 ;
	for( n = 0 ; n < nParts ; n++ )
	{
		cc.RemoveAll( ) ;
		for( z = 0 ; z < m_nZ ; z++ )
		for( y = 0 ; y < m_nY ; y++ )
		for( x = 0 ; x < m_nX ; x++ )
		{
			iValue = pModel->GetAt( x, y, z ) ;
			if( iValue == n + 1 ) // parts "n+1"
			{
				v.x = x ;
				v.y = y ;
				v.z = z ;
				cc.Add( v ) ;
			}
		}
		nPnts = cc.GetSize( ) ;
		
		cc.EvaluateMinMaxMean( ) ;
		vMin.x = std::max( cc.m_minX - iMatchW, 0 ) ;
		vMin.y = std::max( cc.m_minY - iMatchH, 0 ) ;
		vMin.z = std::max( cc.m_minZ - iMatchD, 0 ) ;
		vMax.x = std::min( cc.m_maxX + iMatchW, m_nX - 1 ) ;
		vMax.y = std::min( cc.m_maxY + iMatchH, m_nY - 1 ) ;
		vMax.z = std::min( cc.m_maxZ + iMatchD, m_nZ - 1 ) ;

/ *		pDt->FilWithOneValue( iTruncThresh ) ;
		CVoxel *pCrDt = new CVoxel( vMax.x - vMin.x + 1, vMax.y - vMin.y + 1, vMax.z - vMin.z + 1 ) ;
		CVoxel *pCrEdge = new CVoxel( vMax.x - vMin.x + 1, vMax.y - vMin.y + 1, vMax.z - vMin.z + 1 ) ;
		this->GetSubVoxel( pCrEdge, vMin.x, vMin.y, vMin.z ) ;
		pCrEdge->GetDistanceTransform( pCrDt ) ;
		pDt->PutSubVoxel( pCrDt, vMin.x, vMin.y, vMin.z ) ;
		delete pCrDt ;
		delete pCrEdge ;* /

		float fSumChamf, fMinSumChamf = FLT_MAX ;
		for( d = -iMatchD2 ; d <= iMatchD2 ; d += 2 )
		for( h = -iMatchH2 ; h <= iMatchH2 ; h += 2 )
		for( w = -iMatchW2 ; w <= iMatchW2 ; w += 2 )
		{
			fSumChamf = 0 ;
			for( m = 0 ; m < nPnts ; m++ )
			{
				v.x = cc[m].x + w ;
				v.y = cc[m].y + h ;
				v.z = cc[m].z + d ;
				iChamf = std::min( pDt->GetAt( v ), iTruncThresh ) ;
				fSumChamf += (float)iChamf ;
			}
			if( fSumChamf < fMinSumChamf )
			{
				fMinSumChamf = fSumChamf ;
				pntTranss[n].x = w ;
				pntTranss[n].y = h ;
				pntTranss[n].z = d ;
			}
			int kk = 0 ;
		}

		for( m = 0 ; m < nPnts ; m++ )
		{
			v.x = cc[m].x + pntTranss[n].x ;
			v.y = cc[m].y + pntTranss[n].y ;
			v.z = cc[m].z + pntTranss[n].z ;
			if( v.x >= 0 && v.x < m_nX && v.y >= 0 && v.y < m_nY && v.z >= 0 && v.z < m_nZ )
				pMatched->SetAt( v, n + 1 ) ;
		}
	}

	return true ;
}
*/

void CVoxel::InsertLabel( CVoxel* label)
{
	int x, y, z;
	for( z = 0 ; z < m_nZ ; z++ ) { 
		for( y = 0 ; y < m_nY ; y++ ) {
			for( x = 0 ; x < m_nX ; x++ ) {
				if ( label->GetAt(x,y,z) != 0 ) {
					SetAt(x,y,z,label->GetAt(x,y,z));
				}
			}
		}
	}
}

bool CVoxel::Thresholding2Bounds( int iLower, int iUpper )
{
	int x, y, z, iValue ;

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		iValue = GetAt( x, y, z ) ;
		if( iValue < iLower || iValue > iUpper )
			SetAt( x, y, z, 0 ) ; // suppress!!
	}
	return true ;
}

bool CVoxel::Thresholding( short int i )
{
	int x, y, z, iValue ;

	for( z = 0 ; z < m_nZ ; z++ )
		for( y = 0 ; y < m_nY ; y++ )
			for( x = 0 ; x < m_nX ; x++ )
			{
				iValue = GetAt( x, y, z ) ;
				if( iValue > i )
					SetAt( x, y, z, FG ) ; // suppress!!
				else
					SetAt( x, y, z, BG ) ; // suppress!!
			}
			return true ;
}

void CVoxel::GetBinaryVxl( CFVoxel* fvxl, double thres )
{
	int x, y, z;
	for ( z = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++ ) {
				if ( fvxl->GetAt(x,y,z) >= thres ) {
					SetAt(x,y,z,1);
				}
				else {
					SetAt(x,y,z,0);
				}
			}
		}
	}
}

void CVoxel::SavePoints(CString path, vtkPoints* pt)
{
	int nPt = pt->GetNumberOfPoints();
	for( int i = 0; i < nPt; i++ )
	{
		double tmp[3];
		pt->GetPoint( i , tmp );
		if ( tmp[0] >= 0 && tmp[0] < m_nX && tmp[1] >= 0 && tmp[1] < m_nY && tmp[2] >= 0 && tmp[2] < m_nZ ) {
			SetAt(tmp[0],tmp[1],tmp[2],1);
		}
	}
	
	WriteToBin(path);
}


vtkPolyData* CVoxel::MakePolyData( )
{
	vtkPolyData* pSrcPolyCT = vtkPolyData::New();

	int srcX = m_nX;			
	int srcY = m_nY;			
	int srcZ = m_nZ;
	float srcSpX = xSp;	
	float srcSpY = ySp;	
	float srcSpZ = zSp;
	float srcOrgX = xOrg;	
	float srcOrgY = yOrg; 	
	float srcOrgZ = zOrg; 

	vtkShortArray* pSrcArray = vtkShortArray::New();
	vtkShortArray* pSrcArrayB = vtkShortArray::New();
	pSrcArray->SetArray( m_pData, srcX * srcY * srcZ, 1 );
	pSrcArrayB->SetNumberOfValues( srcX * srcY * srcZ );

	int x,y,z;
	for( x = 0; x < srcX; x++ ) {
		for( y = 0; y < srcY; y++ ) {
			for( z = 0; z < srcZ; z++ ) {
				int idx = x + y * srcX + z * srcX * srcY;
				int k = m_pData[ idx ];
				if( k == BYTE_RED ) 
					pSrcArrayB->SetValue( idx, 255 );
				else 
					pSrcArrayB->SetValue( idx, 0 );
			}
		}
	}
	vtkImageData* pSrcImg = vtkImageData::New();
	vtkImageData* pSrcBase = vtkImageData::New();
	pSrcImg->GetPointData()->SetScalars( pSrcArray );
	pSrcImg->SetDimensions( srcX, srcY, srcZ );
	pSrcImg->SetSpacing( 1, 1, 1 );
	pSrcImg->SetOrigin( 0, 0, 0 );
#if VTK_MAJOR_VERSION <= 5
	pSrcImg->SetScalarTypeToUnsignedShort();
#else
	pSrcImg->SetScalarType(VTK_UNSIGNED_SHORT,pSrcImg->GetInformation());
#endif

	pSrcBase->GetPointData()->SetScalars( pSrcArrayB );
	pSrcBase->SetDimensions( srcX, srcY, srcZ );
	pSrcBase->SetSpacing( 1, 1, 1 );
	pSrcBase->SetOrigin( 0, 0, 0 );
#if VTK_MAJOR_VERSION <= 5
	pSrcBase->SetScalarTypeToUnsignedShort();
#else
	pSrcBase->SetScalarType(VTK_UNSIGNED_SHORT,pSrcBase->GetInformation());
#endif

	int		srcDim[3];
	double	srcSp[3];
	double	srcOrg[3];
	pSrcImg->GetDimensions( srcDim );
	pSrcImg->GetSpacing( srcSp );
	pSrcImg->GetOrigin( srcOrg );

	// Transform vtkImageData(Volume) to vtkPolyData(Contour) : Begin
	vtkLookupTable* pLUT = vtkLookupTable::New();
	pLUT->SetNumberOfColors( 256 );
	pLUT->SetTableRange( 0.0, 255.0 );
	pLUT->SetHueRange( 0.0 , 1.0 );
	pLUT->SetSaturationRange( 0.0 , 1.0 );
	pLUT->SetTableValue( BYTE_WHITE, 1.0, 1.0, 1.0, 1 );
	pLUT->SetTableValue( BYTE_RED, 1.0, 0.0, 0.0, 1 );
	pLUT->SetTableValue( BYTE_GREEN, 0.0, 1.0, 0.0, 1 );
	pLUT->SetTableValue( BYTE_BLUE, 0.0, 0.0, 1.0, 1 );
	pLUT->SetTableValue( BYTE_GREENBLUE, 0.0, 1.0, 1.0, 1 );
	pLUT->SetTableValue( BYTE_REDGREEN, 1.0, 1.0, 0.0, 1 );
	pLUT->SetTableValue( BYTE_BLUERED, 1.0, 0.0, 1.0, 1 );
	pLUT->SetTableValue( BYTE_GREEN2BLUE, 0.0, 1.0, 0.4, 1 );
	pLUT->SetTableValue( BYTE_RED2GREEN, 1.0, 0.4, 0.0, 1 );
	pLUT->SetTableValue( BYTE_BLUE2RED, 0.4, 0.0, 1.0, 1 );
	pLUT->SetTableValue( BYTE_GREENBLUE2, 0.0, 0.4, 1.0, 1 );
	pLUT->SetTableValue( BYTE_REDGREEN2, 0.4, 1.0, 0.0, 1 );
	pLUT->SetTableValue( BYTE_BLUERED2, 1.0, 0.0, 0.4, 1 );
	pLUT->Build();

	vtkContourFilter* pSrcCT = vtkContourFilter::New();
#if VTK_MAJOR_VERSION <= 5
	pSrcCT->SetInput( pSrcBase );
#else
	pSrcCT->SetInputData( pSrcBase );
#endif
	pSrcCT->ComputeNormalsOn();
	pSrcCT->SetValue( 0, 255 );
	pSrcCT->Update();

	vtkImageMapToColors* pSrcMC = vtkImageMapToColors::New();
	pSrcMC->SetLookupTable( pLUT );
#if VTK_MAJOR_VERSION <= 5
	pSrcMC->SetInput( pSrcImg );
#else
	pSrcMC->SetInputData( pSrcImg );
#endif

	vtkProbeFilter* pSrcPr = vtkProbeFilter::New();
#if VTK_MAJOR_VERSION <= 5
	pSrcPr->SetInput( pSrcCT->GetOutput() );
	pSrcPr->SetSource( pSrcMC->GetOutput() );
#else
	pSrcPr->SetInputData( pSrcCT->GetOutput() );
	pSrcPr->SetSourceData( pSrcMC->GetOutput() );
#endif

	//vtkPolyData* pSrcPolyCT = vtkPolyData::New();
	vtkPoints* pSrcPt = pSrcCT->GetOutput()->GetPoints();
	vtkCellArray* pSrcVert = pSrcCT->GetOutput()->GetVerts();

	// the number of points of the target and src
	int nPt = pSrcPt->GetNumberOfPoints();
	vtkPoints* pSrcPolyPt = vtkPoints::New();
	vtkCellArray* pSrcPolyVert = vtkCellArray::New();
	vtkIdType pidT[1];
	vtkDataArray* pSrcNorm = pSrcCT->GetOutput()->GetPointData()->GetNormals();

	int i;
	for( i = 0; i < nPt; i++ )
	{
		double pt[3];
		pSrcPt->GetPoint( i, pt );
		pidT[0] = pSrcPolyPt->InsertNextPoint( ( pt[0] ) , ( pt[1] ), ( pt[2] ) );
		pSrcPolyVert->InsertNextCell( 1, pidT );
	}

	pSrcPolyCT->SetPoints( pSrcPolyPt );
	pSrcPolyCT->SetVerts( pSrcPolyVert );
	pSrcPolyCT->GetPointData()->SetNormals( pSrcCT->GetOutput()->GetPointData()->GetNormals() );

	pSrcArray->Delete();
	pSrcArrayB->Delete();
	pSrcImg->Delete();
	pSrcBase->Delete();
	pLUT->Delete();
	pSrcCT->Delete();
	pSrcMC->Delete();
	pSrcPr->Delete();
	pSrcPolyPt->Delete();
	pSrcPolyVert->Delete();

	return pSrcPolyCT;
}


vtkPolyData* CVoxel::ExtractLandmarkPoints( vtkPolyData* pTarPolyCT, cv::Point3i range )
{
	int i;
	vtkIdType pidT[1];
	vtkPoints* pTarPt = pTarPolyCT->GetPoints();
	int nPt = pTarPt->GetNumberOfPoints();
	int nX = m_nX;
	int nY = m_nY;
	int nZ = m_nZ;

	CVoxel* landVxl = new CVoxel(nX,nY,nZ);

	for( i = 0; i < nPt; i++ ) {
		double pt[3];
		pTarPt->GetPoint( i, pt );
		landVxl->SetAt(pt[0],pt[1],pt[2],1);
	}

	int bx,by,bz,fx,fy,fz;
	int x,y,z, l,m,n;
	cvPoint3iArray selectPtArr;

	//CVoxel* check = new CVoxel(nX,nY,nZ);

	range = range/2.0;
	for ( z = 0; z < nZ; z++ ) {
		for ( y = 0; y < nY; y++ ) {
			for ( x = 0; x < nX; x++ ) {
				if ( landVxl->GetAt(x,y,z) ) {
					cv::Point3i pt = cv::Point3i(x,y,z);
					selectPtArr.Add(pt);

					bx = std::max(pt.x-range.x,0);	fx = std::min(nX-1, pt.x+range.x);
					by = std::max(pt.y-range.y,0);	fy = std::min(nY-1, pt.y+range.y);
					bz = std::max(pt.z-range.z,0);	fz = std::min(nZ-1, pt.z+range.z);

					for ( n = bz; n <= fz; n++ ) {
						for ( m = by; m <= fy; m++ ) {
							for ( l = bx; l <= fx; l++ ) {
								//check->SetAt(l,m,n,1);
								if ( landVxl->GetAt(l,m,n) ) {
									landVxl->SetAt(l,m,n,0);
								}
							}
						}
					}
				}

			}
		}
	}

	//check->WriteToBin(_T("F:\\Experimentation\\Bone_2012\\BoneTrainOut\\check.bin"));
	delete landVxl;
	//delete check;

	int nLandMark = selectPtArr.GetSize();

	vtkPoints* pLandPt = vtkPoints::New(); 
	vtkCellArray* pLandVert = vtkCellArray::New();
	vtkPolyData* pLandPoly = vtkPolyData::New();
	vtkDoubleArray* pNrmArr = vtkDoubleArray::New();
	pNrmArr->SetNumberOfComponents( 3 );
	pNrmArr->SetNumberOfTuples( nLandMark );

	for( i = 0; i < nLandMark; i++ ) {

		cv::Point3i selectPt = selectPtArr[i];
		int idxPos = pTarPolyCT->FindPoint( selectPt.x, selectPt.y, selectPt.z );

		double pt[3];
		pTarPt->GetPoint( idxPos, pt );
		pidT[0] = pLandPt->InsertNextPoint( ( pt[0] ) , ( pt[1] ), ( pt[2] ) );
		pLandVert->InsertNextCell( 1, pidT );

		double pn[3];
		pTarPolyCT->GetPointData()->GetNormals()->GetTuple( idxPos, pn );
		pNrmArr->SetTuple( i, pn );
	}


	pLandPoly->SetPoints( pLandPt );
	pLandPoly->SetVerts( pLandVert );
	pLandPoly->GetPointData()->SetNormals( pNrmArr );

	pLandPt->Delete();
	pLandVert->Delete();
	pNrmArr->Delete();

	return pLandPoly;
}

void CVoxel::GetPathRoi(cvPoint3iArray& ptArr, cv::Point3i size)
{
	int x, y, z, i;

	int nPT = ptArr.GetSize();

	int minX,minY,minZ,maxX,maxY,maxZ;
	cv::Point3i pt;
	cv::Point3i halfSize = size/2.0;

	CFVoxel* tmp = new CFVoxel(m_nX,m_nY,m_nZ);
	CFVoxel* tmpTrain = new CFVoxel(_T("F:\\Experimentation\\Bone_2012\\BoneTrainOut\\29patch.fvx"));
	for ( i = 0; i < nPT; i++ ) {

		if ( i == 29) {
			pt = ptArr.GetAt(i);

			minX = std::max(pt.x-halfSize.x, 0);
			minY = std::max(pt.y-halfSize.y, 0);
			minZ = std::max(pt.z-halfSize.z, 0);
			maxX = std::min(pt.x+halfSize.x, m_nX-1);
			maxY = std::min(pt.y+halfSize.y, m_nY-1);
			maxZ = std::min(pt.z+halfSize.z, m_nZ-1);

			for ( z = minZ; z <= maxZ; z++ ) {
				for ( y = minY; y <= maxY; y++ ) {
					for ( x = minX; x <= maxX; x++ ) {
						SetAt(x,y,z,1);
						tmp->SetAt(x,y,z,tmpTrain->GetAt(x-minX, y-minY, z-minZ));
					}
				}
			}
		}
	}
	tmp->Save(_T("F:\\Experimentation\\Bone_2012\\Test\\Others\\29patch.fvx"));

}

CVoxel* CVoxel::ComputeIMinusMean(float m)
{
	CVoxel* dVxl = new CVoxel(m_nX,m_nY,m_nZ);
	int x, y, z;
	for ( z = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++ ) {
				dVxl->SetAt(x,y,z,GetAt(x,y,z)-m);
			}
		}
	}
	return dVxl;
}

float CVoxel::ComputeIMean()
{
	int x, y, z, i, npv=m_nX*m_nY*m_nZ;
	float msum = 0;
	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				msum += m_pData[i];
			}
		}
	}
	return msum/(float)npv;
}

float CVoxel::ComputeIVar(float mean)
{
	int x, y, z, i, npv=m_nX*m_nY*m_nZ;
	float vsum = 0;
	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				vsum += (m_pData[i]-mean)*(m_pData[i]-mean);
			}
		}
	}
	// and variance of input patch
	return vsum/(float)npv;
}

void CVoxel::ComputeIMeanAndVar(float &m, float &v)
{
	int x, y, z, i, npv=m_nX*m_nY*m_nZ;
	float msum, vsum;
	msum = vsum = 0;
	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				msum += m_pData[i];
			}
		}
	}
	// compute mean
	m = msum/(float)npv;

	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				vsum += (m_pData[i]-m)*(m_pData[i]-m);
			}
		}
	}

	// and variance of input patch
	v = vsum/(float)npv;
}

void CVoxel::ComputeIMeanAndVar(cv::Point3i stpt, cv::Point3i edpt, float &m, float &v)
{
	cv::Point3i size = edpt-stpt;
	//size = size-1;
	int x, y, z, npv=size.x*size.y*size.z;
	float msum, vsum;
	msum = vsum = 0;
	int count = 0;
	for ( z = stpt.z; z < edpt.z; z++ ) {
		for ( y = stpt.y; y < edpt.y; y++ ) {
			for ( x = stpt.x; x < edpt.x; x++ ) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ ) {
					msum += GetAt(x,y,z);
					count++;
				}
				else {

				}
			}
		}
	}
	// compute mean
	//m = msum/(float)npv;
	m = msum/(float)count;

	for ( z = stpt.z; z < edpt.z; z++ ) {
		for ( y = stpt.y; y < edpt.y; y++ ) {
			for ( x = stpt.x; x < edpt.x; x++ ) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ ) {
					vsum += (GetAt(x,y,z)-m)*(GetAt(x,y,z)-m);
				}
				else {
					//vsum += (-m)*(-m);
				}
			}
		}
	}

	// and variance of input patch
	//v = vsum/(float)npv;
	v = vsum/(float)count;
}

void CVoxel::ComputeSubRegionIMeanAndVar(cv::Point3i stpt, cv::Point3i edpt, float &m, float &v)
{
	cv::Point3i size = edpt-stpt;
	//size = size-1;
	int x, y, z, npv=size.x*size.y*size.z;
	float msum, vsum;
	msum = vsum = 0;
	int count = 0;
	for ( z = stpt.z; z < edpt.z; z++ ) {
		for ( y = stpt.y; y < edpt.y; y++ ) {
			for ( x = stpt.x; x < edpt.x; x++ ) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ ) {
					//if ( GetAt(x,y,z) > 120 ) {
						msum += GetAt(x,y,z);
						count++; 
					//}
				}
			}
		}
	}
	// compute mean
	m = msum/(float)count;

	for ( z = stpt.z; z < edpt.z; z++ ) {
		for ( y = stpt.y; y < edpt.y; y++ ) {
			for ( x = stpt.x; x < edpt.x; x++ ) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ ) {
					//if ( GetAt(x,y,z) > 120 ) {
						vsum += (GetAt(x,y,z)-m)*(GetAt(x,y,z)-m);
					//}
				}
				else {
					
				}
			}
		}
	}

	// and variance of input patch
	v = vsum/(float)count;
}

void CVoxel::ComputeSubRegionIMeanAndVar(cv::Point3i stpt, cv::Point3i edpt, CVoxel* label, float &m, float &v)
{
	cv::Point3i size = edpt-stpt;
	//size = size-1;
	int x, y, z, npv=size.x*size.y*size.z;
	float msum, vsum;
	msum = vsum = 0;
	int count = 0;
	//int idx;
	int k;
	for ( z = stpt.z, k = 0; z < edpt.z; z++ ) {
		for ( y = stpt.y; y < edpt.y; y++ ) {
			for ( x = stpt.x; x < edpt.x; x++, k++ ) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ ) {
					if ( label->m_pData[k] ) {
						msum += GetAt(x,y,z);
						count++; 
					}
				}
			}
		}
	}
	// compute mean
	m = msum/(float)count;

	for ( z = stpt.z, k = 0; z < edpt.z; z++ ) {
		for ( y = stpt.y; y < edpt.y; y++ ) {
			for ( x = stpt.x; x < edpt.x; x++, k++ ) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ ) {
					if ( label->m_pData[k] ) {
						vsum += (GetAt(x,y,z)-m)*(GetAt(x,y,z)-m);
					}
				}
				else {

				}
			}
		}
	}

	// and variance of input patch
	v = vsum/(float)count;
}

void CVoxel::ComputeSubRegionIMeanAndVar(cv::Point3i stpt, cv::Point3i edpt, CFVoxel* prob, float thres, float &m, float &v)
{
	cv::Point3i size = edpt-stpt;
	//size = size-1;
	int x, y, z, npv=size.x*size.y*size.z;
	float msum, vsum;
	msum = vsum = 0;
	int count = 0;
	//int idx;
	int k;
	for ( z = stpt.z, k = 0; z < edpt.z; z++ ) {
		for ( y = stpt.y; y < edpt.y; y++ ) {
			for ( x = stpt.x; x < edpt.x; x++, k++ ) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ ) {
					if ( prob->m_pfData[k] >= thres ) {
						msum += GetAt(x,y,z);
						count++; 
					}
				}
			}
		}
	}
	// compute mean
	m = msum/(float)count;

	for ( z = stpt.z, k = 0; z < edpt.z; z++ ) {
		for ( y = stpt.y; y < edpt.y; y++ ) {
			for ( x = stpt.x; x < edpt.x; x++, k++ ) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ ) {
					if ( prob->m_pfData[k] >= thres ) {
						vsum += (GetAt(x,y,z)-m)*(GetAt(x,y,z)-m);
					}
				}
				else {

				}
			}
		}
	}

	// and variance of input patch
	v = vsum/(float)count;
}

BOOL CVoxel::ComputeSubRegionIMeanAndVarWithRegionCheck(cv::Point3i stpt, cv::Point3i edpt, float &m, float &v)
{
	BOOL bcheck = FALSE;

	int x, y, z;
	float msum, vsum;
	msum = vsum = 0;
	int count = 0;
	for ( z = stpt.z; z < edpt.z; z++ ) {
		for ( y = stpt.y; y < edpt.y; y++ ) {
			for ( x = stpt.x; x < edpt.x; x++ ) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ ) {
					//if ( GetAt(x,y,z) > 120 ) {
					msum += GetAt(x,y,z);
					count++; 
					//}
				}
				else {
					bcheck = TRUE;
				}
			}
		}
	}
	// compute mean
	m = msum/(float)count;

	for ( z = stpt.z; z < edpt.z; z++ ) {
		for ( y = stpt.y; y < edpt.y; y++ ) {
			for ( x = stpt.x; x < edpt.x; x++ ) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ ) {
					//if ( GetAt(x,y,z) > 120 ) {
					vsum += (GetAt(x,y,z)-m)*(GetAt(x,y,z)-m);
					//}
				}
				else {

				}
			}
		}
	}

	// and variance of input patch
	v = vsum/(float)count;

	return bcheck;
}


void CVoxel::ComputeSubRegionIMean(CVoxel* label, float &m)
{
	int x, y, z;
	float msum = 0;
	int count = 0;
	for ( z = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++ ) {
				if ( label->GetAt(x,y,z) ) {
					msum += GetAt(x,y,z);
					count++; 
				}
			}
		}
	}
	// compute mean
	m = msum/(float)count;
}

void CVoxel::ComputeWeightedSubRegionIMeanAndVar(cv::Point3i stpt, cv::Point3i edpt, float &m, float &v, CFVoxel *w)
{
	cv::Point3i size = edpt-stpt;
	//size = size-1;
	int x, y, z, i, npv=size.x*size.y*size.z;
	float wd, wsum, msum, vsum;
	wsum = msum = vsum = 0;

	for ( z = stpt.z, i=0; z < edpt.z; z++ ) {
		for ( y = stpt.y; y < edpt.y; y++ ) {
			for ( x = stpt.x; x < edpt.x; x++, i++ ) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ ) {
					wsum += w->m_pfData[i];
					wd = w->m_pfData[i]*w->m_pfData[i];
					msum += wd;
				}
			}
		}
	}
	// compute mean
	m = msum/(float)wsum;

	for ( z = stpt.z, i=0; z < edpt.z; z++ ) {
		for ( y = stpt.y; y < edpt.y; y++ ) {
			for ( x = stpt.x; x < edpt.x; x++, i++ ) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ ) {
					wd = w->m_pfData[i]*(GetAt(x,y,z)-m)*(GetAt(x,y,z)-m);
					vsum += wd;
				}
				else {

				}
			}
		}
	}

	// and variance of input patch
	v = vsum/(float)wsum;
}

void CVoxel::ComputeWeightedSubRegionIMeanAndVar(cv::Point3i stpt, cv::Point3i edpt, float &m, float &v, CVoxel* roi, CFVoxel *w)
{
	cv::Point3i size = edpt-stpt;
	//size = size-1;
	int x, y, z, i, npv=size.x*size.y*size.z;
	float wd, wsum, msum, vsum;
	wsum = msum = vsum = 0;

	for ( z = stpt.z, i=0; z < edpt.z; z++ ) {
		for ( y = stpt.y; y < edpt.y; y++ ) {
			for ( x = stpt.x; x < edpt.x; x++, i++ ) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ ) {
					if ( roi->m_pData[i] ) {
						wsum += w->m_pfData[i];
						wd = w->m_pfData[i]*w->m_pfData[i];
						msum += wd;
					}
				}
			}
		}
	}
	// compute mean
	m = msum/(float)wsum;

	for ( z = stpt.z, i=0; z < edpt.z; z++ ) {
		for ( y = stpt.y; y < edpt.y; y++ ) {
			for ( x = stpt.x; x < edpt.x; x++, i++ ) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ ) {
					if ( roi->m_pData[i] ) {
						wd = w->m_pfData[i]*(GetAt(x,y,z)-m)*(GetAt(x,y,z)-m);
						vsum += wd;
					}
				}
				else {

				}
			}
		}
	}

	// and variance of input patch
	v = vsum/(float)wsum;
}


void CVoxel::ComputeWeightedIMeanAndVar(float &m, float &v, CFVoxel *w)
{
	int x, y, z, i, npv=m_nX*m_nY*m_nZ;
	float wd, wsum, msum, vsum;
	wsum = msum = vsum = 0;
	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				wsum += w->m_pfData[i];
				//vsum += m_pData[i]*m_pData[i];
				wd = w->m_pfData[i] * m_pData[i];
				msum += wd;
			}
		}
	}
	m = msum/wsum;

	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				wd = w->m_pfData[i] * (m_pData[i] - m)*(m_pData[i] - m);
				vsum += wd;
			}
		}
	}
	v = vsum/wsum;
}

float CVoxel::ComputeSubRegionAppDistribution(cv::Point3i patStPos, cv::Point3i patEdPos, CFVoxel* probPat, CBilayerHistogram* hist )
{
	float val = 0;
	float intensity;
	float fgP, bgP;
	int count = 0;
	int x,y,z;

	for ( z = patStPos.z; z < patEdPos.z; z++ ) {
		for ( y = patStPos.y; y < patEdPos.y; y++ ) {
			for ( x = patStPos.x; x < patEdPos.x; x++ ) {
				if ( x >=0 && x < m_nX && y >=0 && y < m_nY && z >=0 && z < m_nZ ) {
					if ( probPat->GetAt(x,y,z) > 0.5 ) {
						intensity = GetAt(x,y,z);
						hist->ComputeFProbability(intensity, fgP, bgP);
						float tmp = fgP;
						val += tmp;
						count++;
					}
					else {
						intensity = GetAt(x,y,z);
						hist->ComputeFProbability(intensity, fgP, bgP);
						float tmp = bgP;
						val += tmp;
						count++;
					}
				}
			}
		}
	}

	val /= count;

	return val;
}

void CVoxel::ComputeSubRegionFGBGMean(cv::Point3i patStPos, cv::Point3i patEdPos, CVoxel* fgbgPat, int fgIdx, int bgIdx, float& fmean, float& bmean)
{
	int x, y, z, k;
	int nFG = 0;
	int nBG = 0;

	fmean = 0;
	bmean = 0;

	for ( z = patStPos.z, k=0; z < patEdPos.z; z++ ) {
		for ( y = patStPos.y; y < patEdPos.y; y++ ) {
			for ( x = patStPos.x; x < patEdPos.x; x++, k++ ) {
				if ( x >=0 && x < m_nX && y >=0 && y < m_nY && z >=0 && z < m_nZ ) {
					if (fgbgPat->m_pData[k] == fgIdx) {
						fmean += GetAt(x,y,z);
						nFG++;
					}
					else if (fgbgPat->m_pData[k] == bgIdx) {
						bmean += GetAt(x,y,z);
						nBG++;
					}
					else {
					}
				}
			}
		}
	}
	if ( nFG == 0 || nBG == 0 ) {
		fmean = 0 ;
		bmean = 0;
	}

	else {
		fmean /= nFG;
		bmean /= nBG;
	}
}


CVoxel* CVoxel::GetGradientMagnitudeMap()
{
	CVoxel* grad = new CVoxel(m_nX,m_nY,m_nZ);
	int x,y,z;
	int dx, dy;
	int val;
	for (z = 0; z< m_nZ ; z++) {
		for(y = 0; y < m_nY-1; y++) {
			for(x = 0; x < m_nX-1 ; x++) {
				dx = GetAt(x,y,z) - GetAt(x+1,y,z);
				dy = GetAt(x,y,z) - GetAt(x,y+1,z);

				val = (int)Round(sqrt((double)dx*dx+dy*dy));
				grad->SetAt(x,y,z,val);
			}
		}
	}

	return grad;
}

CVoxel* CVoxel::GetSubGradientMagnitudeMap(cv::Point3i stpt, cv::Point3i edpt)
{
	int nX = edpt.x-stpt.x;
	int nY = edpt.y-stpt.y;
	int nZ = edpt.z-stpt.z;
	CVoxel* grad = new CVoxel(nX,nY,nZ);
	int x,y,z;
	int dx, dy;
	int val;
	for (z = stpt.z; z< edpt.z ; z++) {
		for(y = stpt.y; y < edpt.y-1; y++) {
			for(x = stpt.x; x < edpt.x-1 ; x++) {
				if (x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ) {
					dx = GetAt(x,y,z) - GetAt(x+1,y,z);
					dy = GetAt(x,y,z) - GetAt(x,y+1,z);
					val = (int)Round(sqrt((double)dx*dx+dy*dy));
					grad->SetAt(x-stpt.x,y-stpt.y,z-stpt.z,val);
				}
			}
		}
	}

	return grad;
}

CVoxel* CVoxel::GetDeformedlabel3D(CVoxel* stVxl, CFVoxel* deformX, CFVoxel* deformY, CFVoxel* deformZ)
{
	CVoxel* transVxl = new CVoxel(m_nX,m_nY,m_nZ);

	transVxl->xSp = xSp;
	transVxl->ySp = ySp;
	transVxl->zSp = zSp;

	int x,y,z;
	float tx,ty,tz;
	float dX,dY,dZ;

	for( z = 0 ; z < stVxl->m_nZ ; z++ ) {
		for( y = 0 ; y < stVxl->m_nY ; y++ ) {
			for( x = 0 ; x < stVxl->m_nX ; x++) {

				int val = stVxl->GetAt(x,y,z);

				if ( val != 0) {
					dX = deformX->GetAt(x,y,z) / deformX->xSp;
					dY = deformY->GetAt(x,y,z) / deformX->ySp;
					dZ = deformZ->GetAt(x,y,z) / deformX->zSp;

					tx = (x+dX)*(m_nX/ (float)deformX->m_nX);
					ty = (y+dY)*(m_nY/ (float)deformX->m_nY);
					tz = (z+dZ)*(m_nZ/ (float)deformX->m_nZ);

					int lx = cvRound(tx);
					int ly = cvRound(ty);
					int lz = cvRound(tz);

					if ( lx >= 0 && lx < m_nX && ly >= 0 && ly < m_nY && lz >= 0 && lz < m_nZ ) {
						transVxl->SetAt(lx,ly,lz,val);
					}
					else {
					}
				}
			}
		}
	}

	return transVxl;
}


void CVoxel::VarianceNormalize( float appMean, float appSigma )
{	
	float Cmean, Csigma;
	GetMeanSigma( Cmean,  Csigma);

	CVoxel *tmpMR = new CVoxel(m_nX, m_nY, m_nZ);
	double val;
	for (int z = 0; z< m_nZ ; z++) {
		for(int y = 0; y < m_nY; y++) {
			for(int x = 0; x < m_nX ; x++) {

				val = GetAt(x,y,z) - Cmean; 
				//val = val*(Csigma/normalsigma);
				val = val*(appSigma/Csigma);
				val = Round(val+appMean);
				if (val<0)		
					tmpMR->SetAt(x,y,z, 0);
				else
					tmpMR->SetAt(x,y,z, (short int)val);
			}
		}
	}
	for (int z = 0; z< m_nZ ; z++) {
		for(int y = 0; y < m_nY; y++) {
			for(int x = 0; x < m_nX ; x++) {
				SetAt(x,y,z,tmpMR->GetAt(x,y,z));
			}
		}
	}
	SAFE_DELETE(tmpMR);

	//CString ResultPath;
	//ResultPath.Format(_T("D:\\Research\\Cartilage\\Data_DB\\192_80\\Result\\normal.raw"));
	//m_MRData->WriteToRaw(ResultPath);
}

void CVoxel::LinearNormalize( int minVal, int maxVal, int mintVal, int maxtVal)
{	
	m_nMinI = SHRT_MAX;
	m_nMaxI = SHRT_MIN;

	int i, x, y, z;
	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				if ( GetAt(x,y,z) > m_nMaxI ) {
					m_nMaxI = GetAt(x,y,z);
				}
				if ( GetAt(x,y,z) < m_nMinI ) 
					m_nMinI = GetAt(x,y,z);
			}
		}
	}

	//if ( m_nMaxI < 2000 ) {
	//	maxVal = m_nMaxI*0.5;
	//}
	if ( m_nMinI > minVal ) {
		minVal = m_nMinI;
	}


	//if ( m_nMaxI > maxtVal || m_nMinI < mintVal) {
		int val,newval;
		for (int z = 0; z< m_nZ ; z++){
			for (int y = 0; y < m_nY; y++) {
				for (int x = 0; x < m_nX; x++) {
					val = GetAt(x,y,z);
					if ( val > maxVal ) {
						SetAt(x,y,z, maxtVal);
					}
					else if ( val < minVal ) {
						SetAt(x,y,z, mintVal);
					}
					else {
						newval = mintVal + (int)Round(((double)(val-minVal)/(maxVal-minVal))*(maxtVal-mintVal));
						SetAt(x,y,z,newval);
					}
				}
			}
		}
	//}
}


void CVoxel::GetMeanSigma( float& mean, float& sigma )
{
	double nAll = m_nX*m_nY*m_nZ;
	int val;
	double sum;

	//mean
	sum =0;
	for (int z = 0; z< m_nZ ; z++) {
		for(int y = 0; y < m_nY; y++) {
			for(int x = 0; x < m_nX ; x++) {
				val = GetAt(x,y,z);	
				sum += val;
			}
		}
	}
	mean = sum/nAll;

	//standard deviation
	sum =0;
	for (int z = 0; z< m_nZ ; z++) {
		for(int y = 0; y < m_nY; y++) {
			for(int x = 0; x < m_nX ; x++) {
				val = GetAt(x,y,z);	
				sum += (val-mean)*(val-mean);
			}
		}
	}
	sigma = sqrt(sum/nAll);
}

/*
CVoxelShell *CVoxel::GetROIVoxelShell(cv::Point3f &roiOrig, cv::Point3i &roiSize)
{
	cv::Point3f st, end;
	st = roiOrig-roiSize;
	end = roiOrig+roiSize;
	
}
*/

void CVoxel::Transform(vtkMatrix4x4 *T)
{
	// get the inverse
	vtkMatrix4x4 *imat = vtkMatrix4x4::New();
	vtkMatrix4x4::Invert(T, imat);
	//transform ALL the src points by the ICP solution
	// inverse transform the whole grid and set values according to the src
	double gridpt[4];
	double transpt[4];
	int nx = m_nX;
	int ny = m_nY;
	int nz = m_nZ;
	int vxlsize = nx*ny*nz;
	short int *transvxl = new short int[vxlsize];
	memset(transvxl, 0, sizeof(short int)*vxlsize);
	int tx, ty, tz;
	int i, x, y, z;
	for ( z = 0, i = 0; z < nz; z++ ) {
		for ( y = 0; y < ny; y++ ) {
			for ( x = 0; x < nx; x++, i++ ) {
				// make point of grid
				gridpt[0] = x;		gridpt[1] = y;
				gridpt[2] = z;		gridpt[3] = 1;
				imat->MultiplyPoint(gridpt, transpt);
				tx = (int) Round(transpt[0]/transpt[3]);
				ty = (int) Round(transpt[1]/transpt[3]);
				tz = (int) Round(transpt[2]/transpt[3]);
				if ( 0 <= tx && tx < nx && 0 <= ty && ty < ny && 0 <= tz && tz < nz ) {
					transvxl[i] = GetAt(tx, ty, tz);
				}
			}
		}
	}
	SAFE_DELETE_ARRAY(m_pData);
	m_pData = transvxl;
	imat->Delete();
}

void CVoxel::Transform(vtkMatrix4x4 *T, CVoxel *orig, cv::Point3i &pt)
{
	// get the inverse
	vtkMatrix4x4 *imat = vtkMatrix4x4::New();
	vtkMatrix4x4::Invert(T, imat);
	//transform ALL the src points by the ICP solution
	// inverse transform the whole grid and set values according to the src
	double gridpt[4];
	double transpt[4];
	int nx = m_nX;
	int ny = m_nY;
	int nz = m_nZ;
	int vxlsize = nx*ny*nz;
	short int *transvxl = new short int[vxlsize];
	memset(transvxl, 0, sizeof(short int)*vxlsize);
	int tx, ty, tz;
	int i, x, y, z;
	for ( z = 0, i = 0; z < nz; z++ ) {
		for ( y = 0; y < ny; y++ ) {
			for ( x = 0; x < nx; x++, i++ ) {
				// make point of grid
				gridpt[0] = x;		gridpt[1] = y;
				gridpt[2] = z;		gridpt[3] = 1;
				imat->MultiplyPoint(gridpt, transpt);
				tx = (int) Round(transpt[0]/transpt[3]);
				ty = (int) Round(transpt[1]/transpt[3]);
				tz = (int) Round(transpt[2]/transpt[3]);
				if ( 0 <= tx && tx < nx && 0 <= ty && ty < ny && 0 <= tz && tz < nz ) {
					transvxl[i] = GetAt(tx, ty, tz);
				}
				// if point of transformed voxel space projected into 
				// original voxel PATCH space is out of bounds,
				else if (0 <= tx+pt.x && tx+pt.x < orig->m_nX && 
						   0 <= ty+pt.y && ty+pt.y < orig->m_nY &&
						   0 <= tz+pt.z && tz+pt.z < orig->m_nZ) {
				// retrieve value from original WHOLE voxel space
					transvxl[i] = orig->GetAt(tx+pt.x, ty+pt.y, tz+pt.z);
				}
				else {
					transvxl[i] = -1;
				}
			}
		}
	}
	SAFE_DELETE_ARRAY(m_pData);
	m_pData = transvxl;
	imat->Delete();
}

void CVoxel::Transform(vtkMatrix4x4 *T, CVoxel *orig, cv::Point3i &pt, CVoxel **tv)
{
	SAFE_DELETE(*tv);
	*tv = new CVoxel(*this);
	(*tv)->Transform(T, orig, pt);
}

bool CVoxel::GetMeanStdDev( CVoxel *pMarkings, BYTE byColor, int iCropX0, int iCropY0, int iCropZ0, float &fMean, float &fStdDev )
{
	int x, y, z, nX, nY, nZ, nFG, iMarking ;
	float fValue, fSum ;
	
	nX = m_nX ;
	nY = m_nY ;
	nZ = m_nZ ;
	
	nFG = 0 ;
	fSum = 0.0f ;
	for( z = 0 ; z < nZ ; z++ )
	for( y = 0 ; y < nY ; y++ )
	for( x = 0 ; x < nX ; x++ )
	{
		iMarking = pMarkings->GetAt( x + iCropX0, y + iCropY0, z + iCropZ0 ) ;
		if( iMarking == byColor )
		{
			fValue = (float)GetAt( x, y, z ) ;
			fSum += fValue ;
			nFG++ ;
		}
	}
	fMean = fSum / nFG ;
	
	fSum = 0.0f ;
	for( z = 0 ; z < nZ ; z++ )
	for( y = 0 ; y < nY ; y++ )
	for( x = 0 ; x < nX ; x++ )
	{
		iMarking = pMarkings->GetAt( x + iCropX0, y + iCropY0, z + iCropZ0 ) ;
		if( iMarking == byColor )
		{
			fValue = (float)GetAt( x, y, z ) ;
			fSum += ( fValue - fMean ) * ( fValue - fMean ) ;
		}
	}
	fStdDev = (float)sqrt( fSum / nFG ) ;

	return true ;
}

/*
void CVoxel::WriteToNiftiFile( CString savePath )
{
	//CString tmpPath;	tmpPath.Format(_T("E:\\dataset\\Asan\\DET0000301_avg.nii.gz"));
	CString tmpPath;	tmpPath.Format(_T("DET0000301_avg.nii.gz"));
	nifti_image* img = nifti_image_read( CStringA(tmpPath), true );
	
	img->nx = m_nX;
	img->ny = m_nY;
	img->nz = m_nZ;
	img->nvox = m_nX*m_nY*m_nZ;
	img->dim[1] = m_nX;
	img->dim[2] = m_nY;
	img->dim[3] = m_nZ;
	img->nbyper = 1;

	int nAll = img->nvox;
	byte* nimgdata = new byte[nAll];
	for( int j = 0; j < nAll; j++ ) {
		nimgdata[j] = (byte)m_pData[j];
	}

	CStringA buffer1;
	buffer1 = (CStringA)savePath;
	char* buffer2 = new char[200];
	strcpy( buffer2, buffer1 );

	delete [] img->data;
	img->data = (void*)nimgdata ;
	img->fname = buffer2;
	img->iname = buffer2;
	nifti_image_write( img );

	delete [] nimgdata;
	SAFE_DELETE(img);
}
*/

vtkContourFilter* CVoxel::ConvertToVTP( int label )
{
	int x,y,z;
	for( x = 0; x < m_nX; x++ ) {
		for( y = 0; y < m_nY; y++ ) {
			for( z = 0; z < m_nZ; z++ ) {
				if( GetAt( x, y, z ) == label )
					continue;
				else
					SetAt( x, y, z, 0 );
			}
		}
	}

	VTK_CREATE( vtkShortArray, pArr );
	VTK_CREATE( vtkImageData, pImg );
	pArr->SetArray( m_pData, m_nX * m_nY * m_nZ, TRUE );

	pImg->GetPointData()->SetScalars( pArr );
	pImg->SetDimensions( m_nX, m_nY, m_nZ );
#if VTK_MAJOR_VERSION <= 5
	pImg->SetScalarTypeToUnsignedShort();
#else
	pImg->SetScalarType(VTK_UNSIGNED_SHORT,pImg->GetInformation());
#endif

	pImg->SetSpacing( 1.0, 1.0, 1.0 );
	pImg->SetOrigin( 0.0, 0.0, 0.0 );

	//VTK_CREATE( vtkContourFilter, pConFilter );
	vtkContourFilter* pConFilter = vtkContourFilter::New();
#if VTK_MAJOR_VERSION <= 5
	pConFilter->SetInput( pImg );
#else
	pConFilter->SetInputData( pImg );
#endif
	pConFilter->SetNumberOfContours( 1 );
	pConFilter->SetValue( 0, BYTE_RED );
	pConFilter->Update();

	return pConFilter;
}

bool CVoxel::CheckTrueNeighborLabels(const cv::Point3i pt, const cv::Point3i nr, int BT)
{
	bool isBd = false;
	/*for ( int z = pt.z-nr.z; z <= pt.z+nr.z; z++ ) {
		for ( int y = pt.y-nr.y; y <= pt.y+nr.y; y++ ) {
			for ( int x = pt.x-nr.x; x <= pt.x+nr.x; x++ ) {
				if ( x>=0 && y>=0 && z>=0 && x<m_nX && y<m_nY && z<m_nZ ){
					if ( GetAt(x, y, z) > 0 ) {
						isBd = TRUE;
						goto stop;
					}
				}
			}
		}
	}*/
	int x,y,z;
	x = pt.x;
	y = pt.y;
	for ( z = pt.z-nr.z; z <= pt.z+nr.z; z++ ) {
		if ( z >= 0 && z < m_nZ ){
			if ( GetAt(x, y, z) == BT ) {
				isBd = TRUE;
				goto stop;
			}
		}
	}
	x = pt.x;
	z = pt.z;
	for ( y = pt.y-nr.y; y <= pt.y+nr.y; y++ ) {
		if ( y >= 0 && y < m_nY ){
			if ( GetAt(x, y, z) == BT ) {
				isBd = TRUE;
				goto stop;
			}
		}
	}
	y = pt.y;
	z = pt.z;
	for ( x = pt.x-nr.x; x <= pt.x+nr.x; x++ ) {
		if ( x >= 0 && x < m_nX ){
			if ( GetAt(x, y, z) == BT ) {
				isBd = TRUE;
				goto stop;
			}
		}
	}
	
	stop:
	return isBd;
}

bool CVoxel::CheckTrueNeighborLabels(const cv::Point3i pt, const cv::Point3i nr )
{
	bool isBd = false;
	/*for ( int z = pt.z-nr.z; z <= pt.z+nr.z; z++ ) {
	for ( int y = pt.y-nr.y; y <= pt.y+nr.y; y++ ) {
	for ( int x = pt.x-nr.x; x <= pt.x+nr.x; x++ ) {
	if ( x>=0 && y>=0 && z>=0 && x<m_nX && y<m_nY && z<m_nZ ){
	if ( GetAt(x, y, z) > 0 ) {
	isBd = TRUE;
	goto stop;
	}
	}
	}
	}
	}*/
	int x,y,z;
	x = pt.x;
	y = pt.y;
	for ( z = pt.z-nr.z; z <= pt.z+nr.z; z++ ) {
		if ( z >= 0 && z < m_nZ ){
			if ( GetAt(x, y, z) > 0 ) {
				isBd = TRUE;
				goto stop;
			}
		}
	}
	x = pt.x;
	z = pt.z;
	for ( y = pt.y-nr.y; y <= pt.y+nr.y; y++ ) {
		if ( y >= 0 && y < m_nY ){
			if ( GetAt(x, y, z) > 0 ) {
				isBd = TRUE;
				goto stop;
			}
		}
	}
	y = pt.y;
	z = pt.z;
	for ( x = pt.x-nr.x; x <= pt.x+nr.x; x++ ) {
		if ( x >= 0 && x < m_nX ){
			if ( GetAt(x, y, z) > 0 ) {
				isBd = TRUE;
				goto stop;
			}
		}
	}

stop:
	return isBd;
}



// *this : *pRegion
bool CVoxel::FillInClosedContour( CVoxel *pBoundary )
{
	int i, j, k, x, y, z, iLabel, iValue ;
	int nLabelBase = 0 ;

	CVoxel *pTemp = new CVoxel( *this ) ; // pResgion
	pTemp->deleteSmallRegionsByCca( this, 6, 1 ) ;  // previously 15 02/07/008
	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		iLabel = GetAt( x, y, z ) ;
		switch( iLabel )
		{
		case 0 :
			break ;
		case LABEL_BASE : // delete the larget CC, which is the background
			SetAt( x, y, z, 0 ) ;
			nLabelBase++ ;
			break ;
		default :
			iValue = ( iLabel - LABEL_BASE ) * 100 ;
			SetAt( x, y, z, iValue ) ;
		}
	}

	int iBrSize = 1, nLabel ;
	float fLabelSum ;
	pTemp->Copy( *pBoundary ) ;
	pTemp->deleteSmallRegionsByCca( pBoundary, 26, 1 ) ; // previously 15 02/07/008
	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		iLabel = pBoundary->GetAt( x, y, z ) ;
		if( iLabel >= LABEL_BASE )
		{
			nLabel = 0 ;
			fLabelSum = 0.0f ;
			for( k = -iBrSize ; k <= iBrSize ; k++ )
			for( j = -iBrSize ; j <= iBrSize ; j++ )
			for( i = -iBrSize ; i <= iBrSize ; i++ )
			{
				if( x + i < 0 || x + i >= m_nX || y + j < 0 || y + j >= m_nY || z + k < 0 || z + k >= m_nZ )
					continue ;
				iLabel = this->GetAt( x + i, y + j, z + k ) ; // region
				if( iLabel > 0 )
				{
					fLabelSum += iLabel ;
					nLabel++ ;
				}
			}
			iValue = (int)( ( fLabelSum / nLabel ) / 100.0f  ) * 100 ;
			this->SetAt( x, y, z, iValue ) ; // update pRegion
		}
	}

	delete pTemp ;

	return true ;
}


int CVoxel::GetTemplateHammingDistance( CVoxel* temp, cv::Point3i stpt )
{
	cv::Point3i enpt;
	enpt.x = stpt.x + temp->m_nX;
	enpt.y = stpt.y + temp->m_nY;
	enpt.z = stpt.z + temp->m_nZ;

	int x,y,z, nx,ny,nz;
	int count = 0;
	for (z = stpt.z; z <= enpt.z ; z++) {
		for (y = stpt.y; y <= enpt.y; y++) {
			for (x = stpt.x; x <= enpt.x; x++) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ && GetAt(x,y,z) ) {
					
					nx = x - stpt.x;
					ny = y - stpt.y;
					nz = z - stpt.z;

					if ( nx >= 0 && nx < temp->m_nX && ny >= 0 && ny < temp->m_nY && nz >= 0 && nz < temp->m_nZ && temp->GetAt(nx,ny,nz) > 0) {
						count++;
					}
				}
			}
		}
	}

	return count;
}

double CVoxel::GetTemplateHammingDistance( CFVoxel* temp, cv::Point3i stpt )
{
	cv::Point3i enpt;
	enpt.x = stpt.x + temp->m_nX;
	enpt.y = stpt.y + temp->m_nY;
	enpt.z = stpt.z + temp->m_nZ;

	int x,y,z;
	int nx,ny,nz;
	double count = 0;
	for (z = stpt.z; z <= enpt.z ; z++) {
		for (y = stpt.y; y <= enpt.y; y++) {
			for (x = stpt.x; x <= enpt.x; x++) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ && GetAt(x,y,z) ) {
					
					nx = x - stpt.x;
					ny = y - stpt.y;
					nz = z - stpt.z;

					if ( nx >= 0 && nx < temp->m_nX && ny >= 0 && ny < temp->m_nY && nz >= 0 && nz < temp->m_nZ && temp->GetAt(nx,ny,nz) > 0) {
						count += temp->GetAt(nx,ny,nz);
					}
				}
			}
		}
	}

	return count;
}

double CVoxel::GetTemplateVarianceDistance( CFVoxel* temp, cv::Point3i stpt )
{
	cv::Point3i enpt;
	enpt.x = stpt.x + temp->m_nX;
	enpt.y = stpt.y + temp->m_nY;
	enpt.z = stpt.z + temp->m_nZ;

	int x,y,z;
	int nx,ny,nz;
	double mean = 0;
	double var = 0;
	int count = 0;
	for (z = stpt.z; z <= enpt.z ; z++) {
		for (y = stpt.y; y <= enpt.y; y++) {
			for (x = stpt.x; x <= enpt.x; x++) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ && GetAt(x,y,z) ) {
					
					nx = x - stpt.x;
					ny = y - stpt.y;
					nz = z - stpt.z;

					if ( nx >= 0 && nx < temp->m_nX && ny >= 0 && ny < temp->m_nY && nz >= 0 && nz < temp->m_nZ && temp->GetAt(nx,ny,nz) > 0.8 ) {
						count += GetAt(x,y,z);
						var += GetAt(x,y,z)*GetAt(x,y,z);
						count++;
					}
				}
			}
		}
	}
	mean /= count;
	var /= count;
	var = sqrt(var - (mean*mean));

	return var;
}

void CVoxel::FlipImageAxisX()
{
	int x,y,z;
	int tx;

	CVoxel* tmp = new CVoxel(m_nX,m_nY,m_nZ);
	for( x = 0; x < m_nX ; x++ ) {

		tx = m_nX - 1 - x;

		for( y = 0 ; y < m_nY ; y++ ) {
			for( z = 0 ; z < m_nZ ; z++) {
				tmp->SetAt(tx,y,z, GetAt(x,y,z));
			}
		}
	}

	for( z = 0; z < m_nZ ; z++ ) {
		for( y = 0 ; y < m_nY ; y++ ) {
			for( x = 0 ; x < m_nX ; x++) {
				SetAt(x,y,z,tmp->GetAt(x,y,z));
			}
		}
	}

	SAFE_DELETE(tmp);
}

void CVoxel::FlipImageAxisY()
{
	int x,y,z;
	int ty;

	CVoxel* tmp = new CVoxel(m_nX,m_nY,m_nZ);
	for( y = 0; y < m_nY ; y++ ) {

		ty = m_nY - 1 - y;

		for( z = 0 ; z < m_nZ ; z++ ) {
			for( x = 0 ; x < m_nX ; x++) {
				tmp->SetAt(x,ty,z, GetAt(x,y,z));
			}
		}
	}

	for( z = 0; z < m_nZ ; z++ ) {
		for( y = 0 ; y < m_nY ; y++ ) {
			for( x = 0 ; x < m_nX ; x++) {
				SetAt(x,y,z,tmp->GetAt(x,y,z));
			}
		}
	}

	SAFE_DELETE(tmp);
}

void CVoxel::FlipImageAxisZ()
{
	int x,y,z;
	int tz;

	CVoxel* tmp = new CVoxel(m_nX,m_nY,m_nZ);
	for( z = 0; z < m_nZ ; z++ ) {

		tz = m_nZ - 1 - z;

		for( y = 0 ; y < m_nY ; y++ ) {
			for( x = 0 ; x < m_nX ; x++) {
				tmp->SetAt(x,y,tz, GetAt(x,y,z));
			}
		}
	}

	for( z = 0; z < m_nZ ; z++ ) {
		for( y = 0 ; y < m_nY ; y++ ) {
			for( x = 0 ; x < m_nX ; x++) {
				SetAt(x,y,z,tmp->GetAt(x,y,z));
			}
		}
	}

	SAFE_DELETE(tmp);
}


CVoxel* CVoxel::SpacingNormalization(float spX, float spY, float spZ)
{
	int nX = cvRound((m_nX/xSp)*spX);
	int nY = cvRound((m_nY/ySp)*spY);
	int nZ = cvRound((m_nZ/zSp)*spZ);


	CVoxel* normVxl = new CVoxel(nX,nY,nZ); 
	normVxl->xSp = spX;
	normVxl->ySp = spY;
	normVxl->zSp = spZ;

	int x,y,z;
	int ix,iy,iz;
	for( z = 0; z < nZ ; z++ ) {
		for( y = 0 ; y < nY ; y++ ) {
			for( x = 0 ; x < nX ; x++) {
				ix = (int)cvRound((x/spX)*xSp);
				iy = (int)cvRound((y/spY)*ySp);
				iz = (int)cvRound((z/spZ)*zSp);

				if ( ix >= 0 && ix < m_nX && iy >= 0 && iy < m_nY && iz >= 0 && iz < m_nZ ) {
					normVxl->SetAt(x,y,z,GetAt(ix,iy,iz));
				}
			}
		}
	}

	return normVxl;
}



//////////////////////////////////////////////////////////////////////////
// By SCLee, Feb, 2009
//////////////////////////////////////////////////////////////////////////
void CVoxel::InsertCropRegion( CVoxel* cropMR, cv::Point3i st )
{
	int x,y,z,nx,ny,nz;
	for( z = 0; z < cropMR->m_nZ ; z++ ) {
		for( y = 0 ; y < cropMR->m_nY ; y++ ) {
			for( x = 0 ; x < cropMR->m_nX ; x++) {
				nx = x + st.x;
				ny = y + st.y;
				nz = z + st.z;
				SetAt(nx,ny,nz, cropMR->GetAt(x,y,z));
			}
		}
	}
}



void CVoxel::Align()
{
	cv::Point3f mean;
	float *v = new float[9];
	// compute principal axes
	PCA( v, mean );
	// rotate current model to principal axes
	CartesianRotate( v, mean );
	CropFilled();
}

void CVoxel::Align( float *v, cv::Point3f &mean )
{
	// compute principal axes
	PCA( v, mean );
	// rotate current model to principal axes
	CartesianRotate( v, mean );
	CropFilled();
}

void CVoxel::GetRotatedBoundingBox( float *v, cv::Point3f &mean, cv::Point3i &minpt, cv::Point3i &maxpt )
{
	// find new bounding box
	cv::Point3i r[8];
	r[0].x = (int)Round(v[0]*(-mean.x)+v[1]*(-mean.y)+v[2]*(-mean.z));
	r[0].y = (int)Round(v[3]*(-mean.x)+v[4]*(-mean.y)+v[6]*(-mean.z));
	r[0].z = (int)Round(v[6]*(-mean.x)+v[7]*(-mean.y)+v[8]*(-mean.z));

	r[1].x = (int)Round(v[0]*(m_nX-mean.x)+v[1]*(-mean.y)+v[2]*(-mean.z));
	r[1].y = (int)Round(v[3]*(m_nX-mean.x)+v[4]*(-mean.y)+v[6]*(-mean.z));
	r[1].z = (int)Round(v[6]*(m_nX-mean.x)+v[7]*(-mean.y)+v[8]*(-mean.z));

	r[2].x = (int)Round(v[0]*(-mean.x)+v[1]*(m_nY-mean.y)+v[2]*(-mean.z));
	r[2].y = (int)Round(v[3]*(-mean.x)+v[4]*(m_nY-mean.y)+v[6]*(-mean.z));
	r[2].z = (int)Round(v[6]*(-mean.x)+v[7]*(m_nY-mean.y)+v[8]*(-mean.z));

	r[3].x = (int)Round(v[0]*(-mean.x)+v[1]*(-mean.y)+v[2]*(m_nZ-mean.z));
	r[3].y = (int)Round(v[3]*(-mean.x)+v[4]*(-mean.y)+v[6]*(m_nZ-mean.z));
	r[3].z = (int)Round(v[6]*(-mean.x)+v[7]*(-mean.y)+v[8]*(m_nZ-mean.z));

	r[4].x = (int)Round(v[0]*(m_nX-mean.x)+v[1]*(m_nY-mean.y)+v[2]*(-mean.z));
	r[4].y = (int)Round(v[3]*(m_nX-mean.x)+v[4]*(m_nY-mean.y)+v[6]*(-mean.z));
	r[4].z = (int)Round(v[6]*(m_nX-mean.x)+v[7]*(m_nY-mean.y)+v[8]*(-mean.z));

	r[5].x = (int)Round(v[0]*(m_nX-mean.x)+v[1]*(-mean.y)+v[2]*(m_nZ-mean.z));
	r[5].y = (int)Round(v[3]*(m_nX-mean.x)+v[4]*(-mean.y)+v[6]*(m_nZ-mean.z));
	r[5].z = (int)Round(v[6]*(m_nX-mean.x)+v[7]*(-mean.y)+v[8]*(m_nZ-mean.z));

	r[6].x = (int)Round(v[0]*(-mean.x)+v[1]*(m_nY-mean.y)+v[2]*(m_nZ-mean.z));
	r[6].y = (int)Round(v[3]*(-mean.x)+v[4]*(m_nY-mean.y)+v[6]*(m_nZ-mean.z));
	r[6].z = (int)Round(v[6]*(-mean.x)+v[7]*(m_nY-mean.y)+v[8]*(m_nZ-mean.z));

	r[7].x = (int)Round(v[0]*(m_nX-mean.x)+v[1]*(m_nY-mean.y)+v[2]*(m_nZ-mean.z));
	r[7].y = (int)Round(v[3]*(m_nX-mean.x)+v[4]*(m_nY-mean.y)+v[6]*(m_nZ-mean.z));
	r[7].z = (int)Round(v[6]*(m_nX-mean.x)+v[7]*(m_nY-mean.y)+v[8]*(m_nZ-mean.z));

	minpt.x = minpt.y = minpt.z = INT_MAX;
	maxpt.x = maxpt.y = maxpt.z = INT_MIN;
	for ( int i = 0; i < 8; i++ ) {
		if ( r[i].x < minpt.x ) 	minpt.x = r[i].x;
		if ( r[i].y < minpt.y ) 	minpt.y = r[i].y;
		if ( r[i].z < minpt.z ) 	minpt.z = r[i].z;
		if ( r[i].x > maxpt.x ) 	maxpt.x = r[i].x;
		if ( r[i].y > maxpt.y ) 	maxpt.y = r[i].y;
		if ( r[i].z > maxpt.z ) 	maxpt.z = r[i].z;
	}
}

void CVoxel::GetRotatedBoundingBox( CVoxel *src, float *v, cv::Point3f &mean, cv::Point3i &minpt, cv::Point3i &maxpt )
{
	// find new bounding box
	cv::Point3i r[8];
	r[0].x = (int)Round(v[0]*(-mean.x)+v[1]*(-mean.y)+v[2]*(-mean.z));
	r[0].y = (int)Round(v[3]*(-mean.x)+v[4]*(-mean.y)+v[6]*(-mean.z));
	r[0].z = (int)Round(v[6]*(-mean.x)+v[7]*(-mean.y)+v[8]*(-mean.z));

	r[1].x = (int)Round(v[0]*(src->m_nX-mean.x)+v[1]*(-mean.y)+v[2]*(-mean.z));
	r[1].y = (int)Round(v[3]*(src->m_nX-mean.x)+v[4]*(-mean.y)+v[6]*(-mean.z));
	r[1].z = (int)Round(v[6]*(src->m_nX-mean.x)+v[7]*(-mean.y)+v[8]*(-mean.z));

	r[2].x = (int)Round(v[0]*(-mean.x)+v[1]*(src->m_nY-mean.y)+v[2]*(-mean.z));
	r[2].y = (int)Round(v[3]*(-mean.x)+v[4]*(src->m_nY-mean.y)+v[6]*(-mean.z));
	r[2].z = (int)Round(v[6]*(-mean.x)+v[7]*(src->m_nY-mean.y)+v[8]*(-mean.z));

	r[3].x = (int)Round(v[0]*(-mean.x)+v[1]*(-mean.y)+v[2]*(src->m_nZ-mean.z));
	r[3].y = (int)Round(v[3]*(-mean.x)+v[4]*(-mean.y)+v[6]*(src->m_nZ-mean.z));
	r[3].z = (int)Round(v[6]*(-mean.x)+v[7]*(-mean.y)+v[8]*(src->m_nZ-mean.z));

	r[4].x = (int)Round(v[0]*(src->m_nX-mean.x)+v[1]*(src->m_nY-mean.y)+v[2]*(-mean.z));
	r[4].y = (int)Round(v[3]*(src->m_nX-mean.x)+v[4]*(src->m_nY-mean.y)+v[6]*(-mean.z));
	r[4].z = (int)Round(v[6]*(src->m_nX-mean.x)+v[7]*(src->m_nY-mean.y)+v[8]*(-mean.z));

	r[5].x = (int)Round(v[0]*(src->m_nX-mean.x)+v[1]*(-mean.y)+v[2]*(src->m_nZ-mean.z));
	r[5].y = (int)Round(v[3]*(src->m_nX-mean.x)+v[4]*(-mean.y)+v[6]*(src->m_nZ-mean.z));
	r[5].z = (int)Round(v[6]*(src->m_nX-mean.x)+v[7]*(-mean.y)+v[8]*(src->m_nZ-mean.z));

	r[6].x = (int)Round(v[0]*(-mean.x)+v[1]*(src->m_nY-mean.y)+v[2]*(src->m_nZ-mean.z));
	r[6].y = (int)Round(v[3]*(-mean.x)+v[4]*(src->m_nY-mean.y)+v[6]*(src->m_nZ-mean.z));
	r[6].z = (int)Round(v[6]*(-mean.x)+v[7]*(src->m_nY-mean.y)+v[8]*(src->m_nZ-mean.z));

	r[7].x = (int)Round(v[0]*(src->m_nX-mean.x)+v[1]*(src->m_nY-mean.y)+v[2]*(src->m_nZ-mean.z));
	r[7].y = (int)Round(v[3]*(src->m_nX-mean.x)+v[4]*(src->m_nY-mean.y)+v[6]*(src->m_nZ-mean.z));
	r[7].z = (int)Round(v[6]*(src->m_nX-mean.x)+v[7]*(src->m_nY-mean.y)+v[8]*(src->m_nZ-mean.z));

	minpt.x = minpt.y = minpt.z = INT_MAX;
	maxpt.x = maxpt.y = maxpt.z = INT_MIN;
	for ( int i = 0; i < 8; i++ ) {
		if ( r[i].x < minpt.x ) 	minpt.x = r[i].x;
		if ( r[i].y < minpt.y ) 	minpt.y = r[i].y;
		if ( r[i].z < minpt.z ) 	minpt.z = r[i].z;
		if ( r[i].x > maxpt.x ) 	maxpt.x = r[i].x;
		if ( r[i].y > maxpt.y ) 	maxpt.y = r[i].y;
		if ( r[i].z > maxpt.z ) 	maxpt.z = r[i].z;
	}
}

void CVoxel::CartesianRotate( float *v, cv::Point3f &mean, cv::Point3i &minPT )
{
	// find new bounding box
	cv::Point3i minpt, maxpt;
	GetRotatedBoundingBox( v, mean, minpt, maxpt );
	// new data for rotated voxel model
	int nX, nY, nZ, i;
	nX = maxpt.x-minpt.x;
	nY = maxpt.y-minpt.y;
	nZ = maxpt.z-minpt.z;
	short int *newData = new short int[nX*nY*nZ];

	// get inverse eigenvectors 
	CvMat Evects = cvMat( 3, 3, CV_32F, v );
	float a[9]; 	for ( i = 0 ; i < 3*3 ; i++ ) { a[i] = 0; }
	CvMat Ivects = cvMat( 3, 3, CV_32F, a );
	cvInvert( &Evects, &Ivects, CV_SVD );

	// backproject new voxels to old frame and find values
	int x, y, z, oldx, oldy, oldz;
	for( z = minpt.z, i = 0; z < maxpt.z; z++ ) {
		for( y = minpt.y; y < maxpt.y; y++ ) {
			for( x = minpt.x; x < maxpt.x; x++, i++ ) {
				oldx = (int)Round(a[0]*x+a[1]*y+a[2]*z+mean.x);
				oldy = (int)Round(a[3]*x+a[4]*y+a[5]*z+mean.y);
				oldz = (int)Round(a[6]*x+a[7]*y+a[8]*z+mean.z);
				if ( 0 <= oldx && oldx < m_nX &&
					0 <= oldy && oldy < m_nY && 
					0 <= oldz && oldz < m_nZ && 
					GetAt(oldx, oldy, oldz) )
					newData[i] = GetAt(oldx, oldy, oldz);
				else 
					newData[i] = 0;
			}
		}
	}
	// store new data
	m_nX = nX;
	m_nY = nY;
	m_nZ = nZ;
	minPT.Set(minpt);
	SAFE_DELETE(m_pData);
	m_pData = newData;
}

void CVoxel::CartesianRotate( CVoxel *src, float *v, cv::Point3f &mean )
{
	// find new bounding box
	cv::Point3i minpt, maxpt;
	GetRotatedBoundingBox( src, v, mean, minpt, maxpt );
	// new data for rotated voxel model
	m_nX = maxpt.x-minpt.x;
	m_nY = maxpt.y-minpt.y;
	m_nZ = maxpt.z-minpt.z;
	Allocate();

	// get inverse eigenvectors 
	CvMat Evects = cvMat( 3, 3, CV_32F, v );
	float a[9]; 	for ( int i = 0 ; i < 3*3 ; i++ ) { a[i] = 0; }
	CvMat Ivects = cvMat( 3, 3, CV_32F, a );
	cvInvert( &Evects, &Ivects, CV_SVD );

	// backproject new voxels to old frame and find values
	int i, x, y, z, oldx, oldy, oldz;
	for( z = minpt.z, i = 0; z < maxpt.z; z++ ) {
		for( y = minpt.y; y < maxpt.y; y++ ) {
			for( x = minpt.x; x < maxpt.x; x++, i++ ) {
				oldx = (int)Round(a[0]*x+a[1]*y+a[2]*z+mean.x);
				oldy = (int)Round(a[3]*x+a[4]*y+a[5]*z+mean.y);
				oldz = (int)Round(a[6]*x+a[7]*y+a[8]*z+mean.z);
				if ( 0 <= oldx && oldx < src->m_nX &&
					0 <= oldy && oldy < src->m_nY && 
					0 <= oldz && oldz < src->m_nZ && 
					src->GetAt(oldx, oldy, oldz) )
					m_pData[i] = src->GetAt(oldx, oldy, oldz);
				else 
					m_pData[i] = 0;
			}
		}
	}
}

void CVoxel::ComputeCovariance(float *a, cv::Point3f &mean)
{
	int i, x, y, z;
	// initialize pca vector variables
	for ( i = 0 ; i < 3*3 ; i++ ) { a[i] = 0; }

	// compute center of mass
	int sumx, sumy, sumz, vcount;
	sumx = sumy = sumz = vcount = 0;
	for( z = 0, i = 0; z < m_nZ ; z++ ) {
		for( y = 0 ; y < m_nY ; y++ ) {
			for( x = 0 ; x < m_nX ; x++, i++ ) {
				if ( GetAt(x, y, z) ) {
					sumx += x;
					sumy += y;
					sumz += z;
					++vcount;
				}
			}
		}
	}
	float meanx, meany, meanz;
	meanx = (float)sumx/(float)vcount;
	meany = (float)sumy/(float)vcount;
	meanz = (float)sumz/(float)vcount;

	// compute covariance
	for( z = 0, i = 0; z < m_nZ ; z++ ) {
		for( y = 0 ; y < m_nY ; y++ ) {
			for( x = 0 ; x < m_nX ; x++, i++ ) {
				if ( GetAt(x, y, z) ) {
					a[0] += (float)((x-meanx)*(x-meanx)); 
					a[1] += (float)((x-meanx)*(y-meany)); 
					a[2] += (float)((x-meanx)*(z-meanz));
					a[4] += (float)((y-meany)*(y-meany)); 
					a[5] += (float)((y-meany)*(z-meanz)); 
					a[8] += (float)((z-meanz)*(z-meanz));
				}
			}
		}
	}
	a[3] = a[1]; a[6] = a[2]; a[7] = a[5];

	for ( i = 0 ; i < 3*3 ; i++ ) a[i] /= (float)vcount;

	mean.x = meanx;
	mean.y = meany;
	mean.z = meanz;
}

void CVoxel::PCA( float *v, cv::Point3f &mean, BOOL bSort )
{
	int i;//, x, y, z;
	// initialize pca vector variables
	float a[3*3]; float d[3];
	for ( i = 0 ; i < 3*3 ; i++ ) { a[i] = 0; v[i] = 0; }
	for ( i = 0 ; i < 3 ; i++ ) d[i] = 0;	

	ComputeCovariance(a, mean);
	/*// compute center of mass
	int sumx, sumy, sumz, vcount;
	sumx = sumy = sumz = vcount = 0;
	for( z = 0, i = 0; z < m_nZ ; z++ ) {
		for( y = 0 ; y < m_nY ; y++ ) {
			for( x = 0 ; x < m_nX ; x++, i++ ) {
				if ( GetAt(x, y, z) ) {
					sumx += x;
					sumy += y;
					sumz += z;
					++vcount;
				}
			}
		}
	}
	float meanx, meany, meanz;
	meanx = (float)sumx/(float)vcount;
	meany = (float)sumy/(float)vcount;
	meanz = (float)sumz/(float)vcount;

	// compute covariance
	for( z = 0, i = 0; z < m_nZ ; z++ ) {
		for( y = 0 ; y < m_nY ; y++ ) {
			for( x = 0 ; x < m_nX ; x++, i++ ) {
				if ( GetAt(x, y, z) ) {
					a[0] += (float)((x-meanx)*(x-meanx)); 
					a[1] += (float)((x-meanx)*(y-meany)); 
					a[2] += (float)((x-meanx)*(z-meanz));
					a[4] += (float)((y-meany)*(y-meany)); 
					a[5] += (float)((y-meany)*(z-meanz)); 
					a[8] += (float)((z-meanz)*(z-meanz));
				}
			}
		}
	}
	a[3] = a[1]; a[6] = a[2]; a[7] = a[5];

	for ( i = 0 ; i < 3*3 ; i++ ) a[i] /= (float)vcount;*/

	CvMat A = cvMat( 3, 3, CV_32F, a ); // symmetric matrix A
	CvMat Evects = cvMat( 3, 3, CV_32F, v ); // eigenvectors
	CvMat Evals = cvMat( 3, 1, CV_32F, d ); // eigenvalues
	//cvEigenVV( &A, &Evects, &Evals, 1e-10 );
	cvSVD(&A, &Evals, &Evects, 0, CV_SVD_U_T);

	// sort eigenvectors according to covariances
	if ( !bSort ) {
		int mine, mide, maxe;
		if ( a[0] > a[4] ) {
			if ( a[0] > a[8] ) {
				maxe = 0;
				if ( a[8] > a[4] ) {
					mide = 8;
					mine = 4;
				}
				else {
					mide = 4;
					mine = 8;
				}
			}
			else {
				maxe = 8;
				mide = 0;
				mine = 4;
			}
		}
		else {
			if ( a[4] > a[8] ) {
				maxe = 4;
				if ( a[8] > a[0] ) {
					mide = 8;
					mine = 0;
				}
				else {
					mide = 0;
					mine = 8;
				}
			}
			else {
				maxe = 8;
				mide = 4;
				mine = 0;
			}
		}
		float *vtemp = new float[9];
		for ( int i = 0; i < 9; i++ ) {
			vtemp[i] = v[i];
		}

		switch ( maxe ) {
		case 4:
			v[3] = vtemp[0];
			v[4] = vtemp[1];
			v[5] = vtemp[2];
			break;
		case 8:
			v[6] = vtemp[0];
			v[7] = vtemp[1];
			v[8] = vtemp[2];
			break;
		}
		switch ( mide ) {
		case 0:
			v[0] = vtemp[3];
			v[1] = vtemp[4];
			v[2] = vtemp[5];
			break;
		case 8:
			v[6] = vtemp[3];
			v[7] = vtemp[4];
			v[8] = vtemp[5];
			break;
		}
		switch ( mine ) {
		case 0:
			v[0] = vtemp[6];
			v[1] = vtemp[7];
			v[2] = vtemp[8];
			break;
		case 4:
			v[3] = vtemp[6];
			v[4] = vtemp[7];
			v[5] = vtemp[8];
			break;
		}
		delete [] vtemp;
	}
}

void CVoxel::CleanBoundaries()
{
	int x, y, z;
// for each 6 sides, if within threshold, make 0
	int th = 5;
	// xy-planes
	for ( y = 0; y < m_nY; y++ ) {
		for ( x = 0; x < m_nX; x++ ) {
			for ( z = 0; z < th; z++ ) {
				SetAt(x, y, z, 0);
			}
		}
	}
	for ( y = 0; y < m_nY; y++ ) {
		for ( x = 0; x < m_nX; x++ ) {
			for ( z = m_nZ-1; z >= m_nZ-th; z-- ) {
				SetAt(x, y, z, 0);
			}
		}
	}
	// xz-planes
	for ( z = 0; z < m_nZ; z++ ) {
		for ( x = 0; x < m_nX; x++ ) {
			for ( y = 0; y < th; y++ ) {
				SetAt(x, y, z, 0);
			}
		}
	}
	for ( z = 0; z < m_nZ; z++ ) {
		for ( x = 0; x < m_nX; x++ ) {
			for ( y = m_nY-1; y >= m_nY-th; y-- ) {
				SetAt(x, y, z, 0);
			}
		}
	}
	// yz-planes
	for ( z = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < th; x++ ) {
				SetAt(x, y, z, 0);
			}
		}
	}
	for ( z = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = m_nX-1; x > m_nX-th; x-- ) {
				SetAt(x, y, z, 0);
			}
		}
	}
}

bool CVoxel::TripleUnion(CVoxel *a, CVoxel *b, CVoxel *c)
{
// assuming all three voxelmodels have same size
	if ( a->m_nX == b->m_nX && b->m_nX == c->m_nX &&
		 a->m_nY == b->m_nY && b->m_nY == c->m_nY &&
		 a->m_nZ == b->m_nZ && b->m_nZ == c->m_nZ ) {
		 m_nX = a->m_nX;
		 m_nY = a->m_nY;
		 m_nZ = a->m_nZ;
		 Allocate();
		 int i, x, y, z;
		 for ( z = 0, i = 0; z < m_nZ; z++ ) {
			 for ( y = 0; y < m_nY; y++ ) {
				 for ( x = 0; x < m_nX; x++, i++ ) {
					 if ( a->m_pData[i] || b->m_pData[i] || c->m_pData[i])
						m_pData[i] = Max3(a->m_pData[i], b->m_pData[i], c->m_pData[i]);
					else 
						m_pData[i] = 0;
				 }
			 }
		 }
		 return true;
	}
	else
		return false;
}

void CVoxel::SetFilledVoxelValues(short int v)
{
	int i, x, y, z;
	// determine [std::min max] range in x, y, z directions
	for( z = 0, i = 0; z < m_nZ ; z++ ) {
		for( y = 0 ; y < m_nY ; y++ ) {
			for( x = 0 ; x < m_nX ; x++, i++ ) {
				if ( m_pData[i] > 0 ) {
					m_pData[i] = v;
				}
			}
		}
	}
}

// create new voxel model of size (nx, ny, nz) by inserting current model into position (px, py, pz)
CVoxel* CVoxel::Insert(int nx, int ny, int nz, int px, int py, int pz)
{
	VERIFY( nx >= px+m_nX && ny >=py+m_nY && nz >= pz+m_nZ );
	int x, y, z, i;
	CVoxel *res = new CVoxel(nx, ny, nz);
	short int *resptr = res->m_pData;
	int newx, newy, newz;
	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		newz = z+pz;
		for ( y = 0; y < m_nY; y++ ) {
			newy = y+py;
			for ( x = 0; x < m_nX; x++, i++ ) {
				newx = x+px;
				if( m_pData[i] && 
					0 <= newx && newx < nx &&
					0 <= newy && newy < ny && 
					0 <= newz && newz < nz)
					res->SetAt(newx, newy, newz, m_pData[i]);
			}
		}
	}
	return res;
}

CVoxel* CVoxel::Insert(cv::Point3i size, cv::Point3i pos)
{
	return Insert(size.x, size.y, size.z, pos.x, pos.y, pos.z);
}

int CVoxel::ComputeNumberOfNonZeroVxls() 
{
	int x, y, z, i;
	int nnonzerovxls = 0;
	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				if( m_pData[i] )
					nnonzerovxls++;
			}
		}
	}
	return nnonzerovxls;
}


void CVoxel::ComputeMaxCrossSectionCOG(cv::Point3f &pt, BOOL bAlign)
{
	// crop object
	CVoxel *cropped = new CVoxel;
	cv::Point3i st, end;
	cropped->CropFilled(*this, st, end);
	int x, y, z, i;
	
	float v[9];
	cv::Point3f mean;
	if ( bAlign ) {
		// align object		
		// compute principal axes
		cropped->PCA( v, mean, TRUE );
	}
	else {
		float cov[9];
		ComputeCovariance(cov);
		switch ( MaxIdx3(cov[0], cov[4], cov[8]) ) {
			// x axis has largest variance: no rotation
			case 1:
				v[0] = 1; v[1] = 0; v[2] = 0;
				v[3] = 0; v[4] = 1; v[5] = 0;
				v[6] = 0; v[7] = 0; v[8] = 1;
				break;
			// y axis has largest variance: change x and y-> 
				// rotate 90 degrees with respect to z axis
			case 2:
				v[0] = 0; v[1] = -1; v[2] = 0;
				v[3] = 1; v[4] = 0; v[5] = 0;
				v[6] = 0; v[7] = 0; v[8] = 1;
				break;
			// z axis has largest variance: change x and z
				// rotate 90 degrees with respect to y axis
			case 3:
				v[0] = 0; v[1] = 0; v[2] = 1;
				v[3] = 0; v[4] = 1; v[5] = 0;
				v[6] = -1; v[7] = 0; v[8] = 0;
				break;
		}
		mean.x = cropped->m_nX*0.5f;
		mean.y = cropped->m_nY*0.5f;
		mean.z = cropped->m_nZ*0.5f;
	}

	// rotate current model to principal axes
	cv::Point3i minPT;
	cropped->CartesianRotate( v, mean, minPT );

	// compute section area for each slice
	int *nSecArea = new int[cropped->m_nX];
	for ( x = 0; x < cropped->m_nX; x++ ) {
		nSecArea[x] = 0;
	}
	for ( z = 0, i = 0; z < cropped->m_nZ; z++ ) {
		for ( y = 0; y < cropped->m_nY; y++ ) {
			for ( x = 0; x < cropped->m_nX; x++, i++ ) {
				//i = z*cropped.m_nX*cropped.m_nY+y*cropped.m_nX+x;
				if ( cropped->m_pData[i] )
					nSecArea[x]++;
			}
		}
	}
	// find slice with maximum section area
	int maxSecSlice, maxSecArea = INT_MIN;
	for ( x = 0; x < cropped->m_nX; x++ ) {
		if ( nSecArea[x] > maxSecArea ) {
			maxSecArea = nSecArea[x];
			maxSecSlice = x;
		}
	}

	// compute center of gravity of cross section with maximum area
	int cogy, cogz;
	cogy = cogz = 0;
	for ( z = 0; z < cropped->m_nZ; z++ ) {
		for ( y = 0; y < cropped->m_nY; y++ ) {
			i = z*cropped->m_nX*cropped->m_nY+y*cropped->m_nX+maxSecSlice;
			if ( cropped->m_pData[i] ) {
				cogy+=y;
				cogz+=z;
			}
		}
	}

	// compute point in current CVoxel object
	CvMat eVects = cvMat( 3, 3, CV_32F, v ); // eigenvectors
	float vinv[9];
	CvMat invEVects = cvMat( 3, 3, CV_32F, vinv ); // inverse eigenvectors
	cvInvert( &eVects, &invEVects, CV_SVD );
	float ptinv[3];
	ptinv[0] = (float)(maxSecSlice+minPT.x); 
	ptinv[1] = (float)cogy/(float)maxSecArea+minPT.y;
	ptinv[2] = (float)cogz/(float)maxSecArea+minPT.z;
	CvMat ptinvmat = cvMat( 3, 1, CV_32F, ptinv ); 
	float pttemp[3];
	CvMat ptmat = cvMat( 3, 1, CV_32F, pttemp );
	// inverse PCA rotation
	cvMatMul( &invEVects, &ptinvmat, &ptmat );

	// adding back cropped position
	pt.x = pttemp[0]+mean.x+st.x;
	pt.y = pttemp[1]+mean.y+st.y;
	pt.z = pttemp[2]+mean.z+st.z;

	SAFE_DELETE(cropped);
	SAFE_DELETE_ARRAY(nSecArea);
}


void CVoxel::ComputeCenterofMassCOG(cv::Point3i &pt)
{
	//Center of mass 로 중심을 잡는 방법
	int x,y,z,i;
	int fgcount=0;;
	float sumx=0;
	float sumy=0;
	float sumz=0;

	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				if ( m_pData[i] ) {
					sumx += x;
					sumy += y;
					sumz += z;
					fgcount++;
				}	
			}
		}
	}

	pt.x = (int)cvRound(sumx/ (float)fgcount);
	pt.y = (int)cvRound(sumy/ (float)fgcount);
	pt.z = (int)cvRound(sumz/ (float)fgcount);
}

void CVoxel::ComputeCenterofMassCOG(cv::Point3f &pt)
{
	
	//Center of mass 로 중심을 잡는 방법
	int x,y,z,i;
	int fgcount=0;;
	float sumx=0;
	float sumy=0;
	float sumz=0;

	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				//i = z*cropped.m_nX*cropped.m_nY+y*cropped.m_nX+x;
				if ( m_pData[i] ) {
					sumx += x;
					sumy += y;
					sumz += z;
					fgcount++;
				}	
			}
		}
	}

	pt.x = sumx/ (float)fgcount;
	pt.y = sumy/ (float)fgcount;
	pt.z = sumz/ (float)fgcount;

/*
	// crop object
	CVoxel *cropped = new CVoxel;
	cv::Point3i st, end;
	cropped->CropFilled(*this, st, end);
	int y, z, i;
	
	float v[9];
	cv::Point3f mean;
	if ( bAlign ) {
		// align object		
		// compute principal axes
		cropped->PCA( v, mean, TRUE );
	}
	else {
		float cov[9];
		ComputeCovariance(cov);
		switch ( MaxIdx3(cov[0], cov[4], cov[8]) ) {
			// x axis has largest variance: no rotation
			case 1:
				v[0] = 1; v[1] = 0; v[2] = 0;
				v[3] = 0; v[4] = 1; v[5] = 0;
				v[6] = 0; v[7] = 0; v[8] = 1;
				break;
			// y axis has largest variance: change x and y-> 
				// rotate 90 degrees with respect to z axis
			case 2:
				v[0] = 0; v[1] = -1; v[2] = 0;
				v[3] = 1; v[4] = 0; v[5] = 0;
				v[6] = 0; v[7] = 0; v[8] = 1;
				break;
			// z axis has largest variance: change x and z
				// rotate 90 degrees with respect to y axis
			case 3:
				v[0] = 0; v[1] = 0; v[2] = 1;
				v[3] = 0; v[4] = 1; v[5] = 0;
				v[6] = -1; v[7] = 0; v[8] = 0;
				break;
		}
		mean.x = cropped->m_nX*0.5f;
		mean.y = cropped->m_nY*0.5f;
		mean.z = cropped->m_nZ*0.5f;
	}

	// rotate current model to principal axes
	cv::Point3i minPT;
	cropped->CartesianRotate( v, mean, minPT );


	// compute center of gravity of cross section with maximum area
	int slicenum = (int)Round((double)cropped->m_nX*0.5);
	int slicearea =0;
	int cogy, cogz;
	cogy = cogz = 0;
	for ( z = 0; z < cropped->m_nZ; z++ ) {
		for ( y = 0; y < cropped->m_nY; y++ ) {
			i = z*cropped->m_nX*cropped->m_nY+y*cropped->m_nX+slicenum;
			if ( cropped->m_pData[i] ) {
				cogy+=y;
				cogz+=z;
				slicearea++;
			}
		}
	}

	// compute point in current CVoxel object
	CvMat eVects = cvMat( 3, 3, CV_32F, v ); // eigenvectors
	float vinv[9];
	CvMat invEVects = cvMat( 3, 3, CV_32F, vinv ); // inverse eigenvectors
	cvInvert( &eVects, &invEVects, CV_SVD );
	float ptinv[3];
	ptinv[0] = (float)(slicenum+minPT.x); 
	ptinv[1] = (float)cogy/(float)slicearea+minPT.y;
	ptinv[2] = (float)cogz/(float)slicearea+minPT.z;
	CvMat ptinvmat = cvMat( 3, 1, CV_32F, ptinv ); 
	float pttemp[3];
	CvMat ptmat = cvMat( 3, 1, CV_32F, pttemp );
	// inverse PCA rotation
	cvMatMul( &invEVects, &ptinvmat, &ptmat );

	// adding back cropped position
	pt.x = pttemp[0]+mean.x+st.x;
	pt.y = pttemp[1]+mean.y+st.y;
	pt.z = pttemp[2]+mean.z+st.z;

	SAFE_DELETE(cropped);
	//SAFE_DELETE_ARRAY(nSecArea);

*/
}

void CVoxel::HistogramNormalize(CHistogram* inHist, CHistogram *outHist)
{
	// check configurations of input and output histogram
	int de = inHist->GetNumHistBin();
	int de1 = outHist->GetNumHistBin();
	VERIFY(inHist->GetNumHistBin()==outHist->GetNumHistBin());
	VERIFY(inHist->GetIMax()==outHist->GetIMax());
	VERIFY(inHist->GetIMin()==outHist->GetIMin());
	//VERIFY(outHist->GetNumHistBin()==inHist->GetIMax()-inHist->GetIMin()+1);

	int nHistBin = outHist->GetNumHistBin();
	int *finalhist = new int[nHistBin];
	double minDiff, histDiff;
	int minDiffIdx, i, j;

	// compute cumulative normalized vectors for both in and out histograms
	double *inHistEq = inHist->CumulativeVectorNormBin();
	double *outHistEq = outHist->CumulativeVectorNormBin();
	for ( i = 0; i < nHistBin; i++ ) 	{
		minDiff = INT_MAX;
		for (j = 0; j < nHistBin; j++) {
			histDiff = fabs(inHistEq[i]-outHistEq[j]);
			if ( histDiff < minDiff )	{
				minDiff = histDiff;
				minDiffIdx = j;
			}
		}
		finalhist[i] = outHist->GetBinIValue(minDiffIdx);
	}

	short val;
	for (i = 0; i < m_nX*m_nY*m_nZ; i++) {
		val = m_pData[i];
		int valBin = (int)Round((double)(val-inHist->GetIMin())/(double)(inHist->GetIMax()-inHist->GetIMin())*
								(double)inHist->GetNumHistBin());
		m_pData[i] = finalhist[std::min(valBin, nHistBin-1)];
	}
	SAFE_DELETE_ARRAY(finalhist);
	SAFE_DELETE_ARRAY(inHistEq);
	SAFE_DELETE_ARRAY(outHistEq);
}

void VoxelPtrArray2CvMat(CTypedPtrArray<CPtrArray, CVoxel*> &voxArr, CvMat **dataMat)
{
	int i, j;
	int arrSize = (int) voxArr.GetSize();
	VERIFY( arrSize );
	int sx = voxArr[0]->m_nX;
	int sy = voxArr[0]->m_nY;
	int sz = voxArr[0]->m_nZ;
	
	// check if all CVoxel objects have same size
	for ( i = 1; i < arrSize; i++ ) {
		CVoxel *vxl = voxArr[i];
		VERIFY(sx == vxl->m_nX);
		VERIFY(sy == vxl->m_nY);
		VERIFY(sz == vxl->m_nZ);
	}

	int nV = sx*sy*sz;
	*dataMat = cvCreateMat(nV, arrSize, CV_32F);

	for( i = 0; i < arrSize; i++ ) {
		CVoxel *vxl = voxArr[i];
		for ( j = 0; j < nV; j++ ) {
			cvmSet(*dataMat, j, i, (float)vxl->m_pData[j]);
		}
	}
}

void CVoxel::SaveImagePerSlice( CString DirName, CString FileName )
{
	int x, y, z, i;
	for ( z = 0, i=0; z < m_nZ; z++ ) {
		CStringA strDir(DirName);
		CStringA strFile(FileName);
		CStringA strOut;
		strOut.Format("%s\\%s_%04d.bmp", strDir, strFile, z);

		IplImage *sliceImg = cvCreateImage(cvSize(m_nX, m_nY), IPL_DEPTH_8U, 1);
		BYTE *imPtr = (BYTE*) sliceImg->imageData;
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				imPtr[y*sliceImg->widthStep+x] = (BYTE) std::max(std::min(GetAt(x, y, z), 255),0);
			}
		}
		cvSaveImage(strOut, sliceImg);
		cvReleaseImage(&sliceImg);
	}
}

CVoxelShell* CVoxel::ExtractBoundaryShell()
{
	cvPoint3iArray arry;
	int val;
	int x,y,z, i;
	for( z = 0, i = 0; z < m_nZ; z++ ){
		for( y = 0; y < m_nY; y++ ){
			for( x = 0; x < m_nX; x++, i++ ){
				val = GetAt(x,y,z);
				
				int nxy = m_nX*m_nY;
				int nxm1 = m_nX-1;
				int nym1 = m_nY-1;
				int nzm1 = m_nZ-1;

				if ( (z>0 && m_pData[i-nxy]!=val) || 
					(z<nzm1 && m_pData[i+nxy]!=val) || 
					(y>0 && m_pData[i-m_nX]!=val) || 
					(y<nym1 && m_pData[i+m_nX]!=val) || 
					(x>0 && m_pData[i-1]!=val)  || 
					(x<nxm1 && m_pData[i+1]!=val) ) 
				{
					arry.Add(cv::Point3i(x,y,z));
				}
			}
		}
	}
	CVoxelShell* shell = new CVoxelShell(arry);

	return shell;
}

CVoxel* CVoxel::ExtractShell()
{
	CVoxelShell* shell = new CVoxelShell;
	shell->MakeShell((this));
	CVoxel* shellVoxel;
	shellVoxel = shell->Voxellize(m_nX,m_nY,m_nZ);

	delete shell;
	return shellVoxel;
}

CVoxelShell* CVoxel::ExtractBCIshell( CVoxel *carti, int bonetype )
{
	CVoxelShell boneShell(this);
	int nbpt = boneShell.npts;
	int nX = carti->m_nX;
	int nY = carti->m_nY;
	int nZ = carti->m_nZ;
	CVoxel *trueBCI = new CVoxel(nX,nY,nZ);

	int x, y, z, i;

	cv::Point3i range = cv::Point3i(1,1,1);
	cv::Point3i cpt, stpt, edpt;
	for( i = 0; i < nbpt; i++){
		cpt = cv::Point3i(boneShell.pts[i].x, boneShell.pts[i].y, boneShell.pts[i].z);
		stpt = cpt - range;
		edpt = cpt + range;

		//if ( carti->GetAt(x-1,y,z) || carti->GetAt(x+1,y,z) || carti->GetAt(x,y-1,z) || carti->GetAt(x,y+1,z) ||carti->GetAt(x,y,z-1) || carti->GetAt(x,y,z+1) ) 
		//{
		//	trueBCI->SetAt(x,y,z,1);
		//}

		// 26-neighborhood
		for ( z = stpt.z; z <= edpt.z; z++ ) {
			for ( y = stpt.y; y <= edpt.y; y++ ) {
				for ( x = stpt.x; x <= edpt.x; x++ ) {
					if (carti->GetAt(x,y,z) == bonetype )	
						trueBCI->SetAt(cpt,1);
				}
			}
		}
	}

	//trueBCI->WriteToBin( _T("F:\\eae.bin"));

	CVoxelShell* bciShell = new CVoxelShell;
	bciShell->MakeShell(trueBCI);
	SAFE_DELETE(trueBCI);

	return bciShell;


}

BOOL CVoxel::IsHoleBtwBoneAndCartilage( cv::Point3i pt, CVoxel* bone, short BT, cv::Point3i nr, int bthres, int cthres  )
{
	int bx,by,bz, fx,fy,fz;

	int cartilagecount, bonecount;
	
	bx = std::max(0, pt.x - nr.x);	fx = std::min( m_nX-1, pt.x + nr.x);
	by = std::max(0, pt.y - nr.y);	fy = std::min( m_nY-1, pt.y + nr.y);
	bz = std::max(0, pt.z - nr.z);	fz = std::min( m_nZ-1, pt.z + nr.z);

	cartilagecount=0;
	bonecount=0;


	for( int z = bz; z <= fz; z++ ){
		for( int y = by; y <= fy; y++ ){
			for( int x = bx; x <= fx; x++ ){
				//if ( GetAt(x,y,z) &&  bone->GetAt(x,y,z))	bonecount++;
				if ( bone->GetAt(x,y,z) ) {
					bonecount++;
				}
				//else if ( GetAt(x,y,z) &&  bone->GetAt(x,y,z) == 0 )	cartilagecount++;
				else if ( GetAt(x,y,z) == BT )	{
					cartilagecount++;
				}
			}
		}
	}

	if ( cartilagecount > cthres && bonecount > bthres ) {
		return TRUE;
	}
	else	return FALSE;
}




// END of CVoxel functions
//////////////////////////////////////////////////////////////////////////////////////////


void CVoxel::GetMeanIntensityInsideShape( CVoxel* shape, int label, 
	double& mean, double& standard, int& minv, int& maxv )
{
	minv = INT_MAX;
	maxv = INT_MIN;
	double tmp = 0;
	double tmpST = 0;
	int count = 0;
	int val;

	int x, y, z;
	for( z = 0; z < m_nZ ; z++ ) {
		for( y = 0 ; y < m_nY ; y++ ) {
			for( x = 0 ; x < m_nX ; x++) {
				if ( shape->GetAt(x,y,z) == label ) {
					val = GetAt(x,y,z);
					if ( minv > val ) {
						minv = val;
					}

					if ( maxv < val ) {
						maxv = val;
					}

					tmp += val;
					tmpST += (val*val);
					count++;
				}
			}
		}
	}


	mean = tmp/(double)count;
	standard = (tmpST/(double)count) - mean*mean;
	standard = sqrt(standard);
}












void CVoxel::NearPointRemoveSampling( cv::Point3i range )
{
	VERIFY( range.x > 0 && range.y > 0 && range.z > 0 );
	int x, y, z, l,m,n;
	int bx,by,bz,fx,fy,fz;

	range = range/2.0;
	for ( z = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++ ) {
				if ( GetAt(x,y,z) ) {
					cv::Point3i pt = cv::Point3i(x,y,z);

					bx = std::max(pt.x-range.x,0);	fx = std::min(pt.x+range.x, m_nX-1);
					by = std::max(pt.y-range.y,0);	fy = std::min(pt.y+range.y, m_nY-1);
					bz = std::max(pt.z-range.z,0);	fz = std::min(pt.z+range.z, m_nZ-1);

					for ( n = bz; n <= fz; n++ ) {
						for ( m = by; m <= fy; m++ ) {
							for ( l = bx; l <= fx; l++ ) {
								if ( GetAt(l,m,n) ) {
									SetAt(l,m,n,0);
								}
							}
						}
					}
					SetAt(x,y,z,1);
				}
				
			}
		}
	}
}


CVoxel* CVoxel::CreateSuperVoxels( int nSP, int nCompact, int & numlabels)
{
	CVoxel* label = new CVoxel(m_nX,m_nY,m_nZ);

	const int STEP = 0.5 + pow(double(nSP),1.0/3.0);
	int numk = 0;
	int x,y,z;

	int xstrips = (0.5+double(m_nX)/double(STEP));	int ystrips = (0.5+double(m_nY)/double(STEP));	int zstrips = (0.5+double(m_nZ)/double(STEP));
	int xerr = m_nX  - STEP*xstrips;if(xerr < 0){xstrips--;xerr = m_nX - STEP*xstrips;}	
	int yerr = m_nY - STEP*ystrips;if(yerr < 0){ystrips--;yerr = m_nY- STEP*ystrips;}
	int zerr = m_nZ  - STEP*zstrips;if(zerr < 0){zstrips--;zerr = m_nZ - STEP*zstrips;}
	double xerrperstrip = double(xerr)/double(xstrips);	double yerrperstrip = double(yerr)/double(ystrips);	double zerrperstrip = double(zerr)/double(zstrips);
	int xoff = STEP/2;	int yoff = STEP/2;	int zoff = STEP/2;
	if ( zstrips == 0 ) {
		zstrips = 1;
		zerrperstrip = 1;
		zoff = 0;
	}
	

	int numseeds = xstrips*ystrips*zstrips;
	
	//// Get Seed Points
	double* seedI = new double[numseeds];
	double* seedX = new double[numseeds];
	double* seedY = new double[numseeds];
	double* seedZ = new double[numseeds];
	for( z = 0; z < zstrips; z++ ) {
		int ze = z*zerrperstrip;
		int d = (z*STEP+zoff+ze);
		for( y = 0; y < ystrips; y++ ) {
			int ye = y*yerrperstrip;
			for( x = 0; x < xstrips; x++ ) {
				int xe = x*xerrperstrip;
				//int i = (y*STEP+yoff+ye)*m_nX + (x*STEP+xoff+xe);
				seedX[numk] = x*STEP+xoff+xe;
				seedY[numk] = y*STEP+yoff+ye;
				seedZ[numk] = z*STEP+zoff+ze;
				seedI[numk] = GetAt(seedX[numk], seedY[numk], seedZ[numk]);	//m_lvecvec[d][i];
				numk++;
			}
		}
	}

	//// Conduct Clustering
	double invwt = 1.0/((STEP/(double)nCompact)*(STEP/(double)nCompact));//compactness = 20.0 is usually good.
	int x1, y1, x2, y2, z1, z2;
	double intensity;
	double dist;
	double distxyz;

	for( int itr = 0; itr < 5; itr++ ) {
		CFVoxel* distVec = new CFVoxel(m_nX,m_nY,m_nZ);
		for( z = 0; z < m_nZ; z++ ) {
			for( y = 0; y < m_nY; y++ ) {
				for( x = 0; x < m_nX; x++ ) {
					distVec->SetAt(x,y,z,FLT_MAX);
				}
			}
		}
		for( int n = 0; n < numk; n++ ) {
			y1 = std::max(0.0,			seedY[n]-STEP);
			y2 = std::min((double)m_nY,	seedY[n]+STEP);
			x1 = std::max(0.0,			seedX[n]-STEP);
			x2 = std::min((double)m_nX,	seedX[n]+STEP);
			z1 = std::max(0.0,			seedZ[n]-STEP);
			z2 = std::min((double)m_nZ,	seedZ[n]+STEP);


			for( z = z1; z < z2; z++ ) {
				for( y = y1; y < y2; y++ ) {
					for( x = x1; x < x2; x++ ) {
						intensity = GetAt(x,y,z);

						dist = (intensity - seedI[n])*(intensity - seedI[n]);

						distxyz =		(x - seedX[n])*(x - seedX[n]) +
							(y - seedY[n])*(y - seedY[n]) +
							(z - seedZ[n])*(z - seedZ[n]);
						//------------------------------------------------------------------------
						dist += distxyz*invwt;
						//------------------------------------------------------------------------
						if( dist < distVec->GetAt(x,y,z)) {
							distVec->SetAt(x,y,z,dist);
							label->SetAt(x,y,z,n);
						}
					}
				}
			}
		}
		SAFE_DELETE(distVec);
		//-----------------------------------------------------------------
		// Recalculate the centroid and store in the seed values
		//-----------------------------------------------------------------
		//instead of reassigning memory on each iteration, just reset.

		//CString savePath;	savePath.Format(_T("F:\\Experimentation\\Supervoxel\\supervoxelMid%d.raw"),itr+1);
		//label->WriteToRaw(savePath);

		double* sigmaI = new double[numk];				InitializeArray(sigmaI, numk);
		double* sigmaX = new double[numk];				InitializeArray(sigmaX, numk);
		double* sigmaY = new double[numk];				InitializeArray(sigmaY, numk);
		double* sigmaZ = new double[numk];				InitializeArray(sigmaZ, numk);
		double* clusterSize = new double[numk];			InitializeArray(clusterSize, numk);
		double* clusterIvtSize = new double[numk];		InitializeArray(clusterIvtSize, numk);

		for( z = 0; z < m_nZ; z++ ) {
			for( y = 0; y < m_nY; y++ ) {
				for( x = 0; x < m_nX; x++ ) {
					sigmaI[label->GetAt(x,y,z)] += GetAt(x,y,z);
					sigmaX[label->GetAt(x,y,z)] += x;
					sigmaY[label->GetAt(x,y,z)] += y;
					sigmaZ[label->GetAt(x,y,z)] += z;

					clusterSize[label->GetAt(x,y,z)] += 1.0;
				}
			}
		}

		for( int k = 0; k < numk; k++ ) {
			if( clusterSize[k] <= 0 ) clusterSize[k] = 1;
			clusterIvtSize[k] = 1.0/clusterSize[k];//computing inverse now to multiply, than divide later
		}

		for( int k = 0; k < numk; k++ ) {
			seedI[k] = sigmaI[k]*clusterIvtSize[k];
			seedX[k] = sigmaX[k]*clusterIvtSize[k];
			seedY[k] = sigmaY[k]*clusterIvtSize[k];
			seedZ[k] = sigmaZ[k]*clusterIvtSize[k];
		}

		SAFE_DELETE_ARRAY(sigmaI);
		SAFE_DELETE_ARRAY(sigmaX);
		SAFE_DELETE_ARRAY(sigmaY);
		SAFE_DELETE_ARRAY(sigmaZ);
		SAFE_DELETE_ARRAY(clusterSize);
		SAFE_DELETE_ARRAY(clusterIvtSize);
	}

	
	//// SuperVoxel label Connectivity
	const int dx10[10] = {-1,  0,  1,  0, -1,  1,  1, -1,  0, 0};
	const int dy10[10] = { 0, -1,  0,  1, -1, -1,  1,  1,  0, 0};
	const int dz10[10] = { 0,  0,  0,  0,  0,  0,  0,  0, -1, 1};
	const int SUPSZ = STEP*STEP*STEP;
	int adjlabel = 0;	//adjacent label
	int* xvec = new int[SUPSZ*10];//a large enough size
	int* yvec = new int[SUPSZ*10];//a large enough size
	int* zvec = new int[SUPSZ*10];//a large enough size
	CVoxel* newLabel = new CVoxel(m_nX,m_nY,m_nZ);
	newLabel->FillWithOneValue(-1);

	int lab = 0;
	for( z = 0; z < m_nZ; z++ ) {
		int i = 0;
	
		for( y = 0; y < m_nY; y++ ) {
			for( x = 0; x < m_nX; x++ ) {
				
				int cy = i/m_nX;
				int cx = i-(cy*m_nX);
				
				if( newLabel->GetAt(cx,cy,z) < 0 ) {
					newLabel->SetAt(cx,cy,z,lab);
					//-------------------------------------------------------
					// Quickly find an adjacent label for use later if needed
					//-------------------------------------------------------
					for( int n = 0; n < 10; n++ ) {
						int nx = x + dx10[n];
						int ny = y + dy10[n];
						int nz = z + dz10[n];
						if( (nx >= 0 && nx < m_nX) && (ny >= 0 && ny < m_nY) && (nz >= 0 && nz < m_nZ) ) {
							if( newLabel->GetAt(nx,ny,nz) >= 0 ) {
								adjlabel = newLabel->GetAt(nx,ny,nz);
							}
						}
					}

					xvec[0] = x; yvec[0] = y; zvec[0] = z;
					int count = 1;
					for( int c = 0; c < count; c++ ) {
						for( int n = 0; n < 10; n++ ) {
							int nx = xvec[c] + dx10[n];
							int ny = yvec[c] + dy10[n];
							int nz = zvec[c] + dz10[n];
							if( (nx >= 0 && nx < m_nX) && (ny >= 0 && ny < m_nY) && (nz >= 0 && nz < m_nZ) ) { 
								if( 0 > newLabel->GetAt(nx,ny,nz) && label->GetAt(cx,cy,z) == label->GetAt(nx,ny,nz) ) {
									xvec[count] = nx;
									yvec[count] = ny;
									zvec[count] = nz;
									newLabel->SetAt(nx,ny,nz,lab);
									count++;
								}
							}
						}
					}

					//-------------------------------------------------------
					// If segment size is less then a limit, assign an
					// adjacent label found before, and decrement label count.
					//-------------------------------------------------------
					if( count <= (SUPSZ >> 2) ) { //this threshold can be changed according to needs
					//if( count <= 3 ) { //this threshold can be changed according to needs
						for( int c = 0; c < count; c++ ) {
							newLabel->SetAt(xvec[c],yvec[c],zvec[c],adjlabel);
						}
						lab--;
					}
					//--------------------------------------------------------
					lab++;
				}
				i++;
			}
		}
	}
	//newLabel->WriteToRaw(_T("F:\\Experimentation\\Supervoxel\\newLabelSupervoxel.raw"));

	for( z = 0; z < m_nZ; z++ ) {
		for( y = 0; y < m_nY; y++ ) {
			for( x = 0; x < m_nX; x++ ) {
				label->SetAt(x,y,z,newLabel->GetAt(x,y,z));
			}
		}
	}

	numlabels = lab;

	SAFE_DELETE_ARRAY(xvec);
	SAFE_DELETE_ARRAY(yvec);
	SAFE_DELETE_ARRAY(zvec);
	SAFE_DELETE(newLabel);
	
	

	SAFE_DELETE_ARRAY(seedI);
	SAFE_DELETE_ARRAY(seedX);
	SAFE_DELETE_ARRAY(seedY);
	SAFE_DELETE_ARRAY(seedZ);

	return label;
}


#endif
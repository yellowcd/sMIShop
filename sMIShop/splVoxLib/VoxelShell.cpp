// VoxelShell.cpp: implementation of the CVoxelShell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VoxelShell.h"
//#include "math.h"
#include "Util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// CVoxelShell functions
CVoxelShell::CVoxelShell(): pts(NULL),npts(0)
{

}

CVoxelShell::~CVoxelShell()
{
	SAFE_DELETE_ARRAY(pts);
}

template <typename T>
CVoxelShell::CVoxelShell(CVoxel<T> *vxl, bool b)
{
	std::deque<cv::Point3i> ptdeque;
	int nx, ny, nz, i, x, y, z, nxy;
	nx = vxl->m_nX;
	ny = vxl->m_nY;
	nz = vxl->m_nZ;
	int nxm1 = nx-1;
	int nym1 = ny-1;
	int nzm1 = nz-1;
	nxy = nx*ny;
	short int *pData = vxl->m_pData;

	for ( z = 0,i = 0; z < nz; z++ ) {
		for ( y = 0; y < ny; y++ ) {
			for ( x = 0; x < nx; x++, i++ ) {
				int val = pData[i];
				if ( val ) {
					if ( (z>0 && pData[i-nxy]!=val) || 
						(z<nzm1 && pData[i+nxy]!=val) || 
						(y>0 && pData[i-nx]!=val) || 
						(y<nym1 && pData[i+nx]!=val) || 
						(x>0 && pData[i-1]!=val)  || 
						(x<nxm1 && pData[i+1]!=val) )  {
							ptdeque.push_back(cv::Point3i(x,y,z));
					}
				}
			}
		}
	}

	npts = (int)ptdeque.size();
	pts = new cv::Point3i[npts];
	for ( i = 0; i < npts; i++ ) {
		pts[i] = ptdeque.front();
		ptdeque.pop_front();
	}
	/*deque<cv::Point3i> ptdeque;
	int nx, ny, nz, i, x, y, z, nxy;
	nx = vxl->m_nX;
	ny = vxl->m_nY;
	nz = vxl->m_nZ;
	int nxm1 = nx-1;
	int nym1 = ny-1;
	int nzm1 = nz-1;
	nxy = nx*ny;
	short int *pData = vxl->m_pData;

	if ( b ) {
		for ( z = 0,i = 0; z < nz; z++ ) {
			for ( y = 0; y < ny; y++ ) {
				for ( x = 0; x < nx; x++, i++ ) {
					if ( pData[i] && 
						( (z>0 && pData[i-nxy]==0) || 
						(z<nzm1 && pData[i+nxy]==0) || 
						(y>0 && pData[i-nx]==0) || 
						(y<nym1 && pData[i+nx]==0) || 
						(x>0 && pData[i-1]==0)  || 
						(x<nxm1 && pData[i+1]==0) ))  //)
					{
						ptdeque.push_back(cv::Point3i(x,y,z));
					}
				}
			}
		}
	}

	else {
		for ( z = 0,i = 0; z < nz; z++ ) {
			for ( y = 0; y < ny; y++ ) {
				for ( x = 0; x < nx; x++, i++ ) {
					int val = pData[i];
					if ( pData[i] && 
						( (z>0 && pData[i-nxy]!=val) || 
						(z<nzm1 && pData[i+nxy]!=val) || 
						(y>0 && pData[i-nx]!=val) || 
						(y<nym1 && pData[i+nx]!=val) || 
						(x>0 && pData[i-1]!=val)  || 
						(x<nxm1 && pData[i+1]!=val) ))  //)
					{
						ptdeque.push_back(cv::Point3i(x,y,z));
					}
				}
			}
		}
	}


	npts = (int)ptdeque.size();
	pts = new cv::Point3i[npts];
	for ( i = 0; i < npts; i++ ) {
		pts[i] = ptdeque.front();
		ptdeque.pop_front();
	}*/
}
template <typename T>
CVoxelShell::CVoxelShell(CVoxel<T> *vxl, int BT, bool b)
{
	std::deque<cv::Point3i> ptdeque;
	int nx, ny, nz, i, x, y, z, nxy;
	nx = vxl->m_nX;
	ny = vxl->m_nY;
	nz = vxl->m_nZ;
	int nxm1 = nx-1;
	int nym1 = ny-1;
	int nzm1 = nz-1;
	nxy = nx*ny;
	short int *pData = vxl->m_pData;
	if ( b ) {
		for ( z = 0,i = 0; z < nz; z++ ) {
			for ( y = 0; y < ny; y++ ) {
				for ( x = 0; x < nx; x++, i++ ) {
					if ( pData[i] == BT && 
						( (z>0 && pData[i-nxy]!=BT) || 
						(z<nzm1 && pData[i+nxy]!=BT) || 
						(y>0 && pData[i-nx]!=BT) || 
						(y<nym1 && pData[i+nx]!=BT) || 
						(x>0 && pData[i-1]!=BT)  || 
						(x<nxm1 && pData[i+1]!=BT) ))  //)
					{
						ptdeque.push_back(cv::Point3i(x,y,z));
					}
				}
			}
		}
	}
	else {
		for ( z = 0,i = 0; z < nz; z++ ) {
			for ( y = 0; y < ny; y++ ) {
				for ( x = 0; x < nx; x++, i++ ) {
					if ( pData[i] != BT && 
						( (z>0 && pData[i-nxy]==BT) || 
						(z<nzm1 && pData[i+nxy]==BT) || 
						(y>0 && pData[i-nx]==BT) || 
						(y<nym1 && pData[i+nx]==BT) || 
						(x>0 && pData[i-1]==BT)  || 
						(x<nxm1 && pData[i+1]==BT) ))  //)
					{
						ptdeque.push_back(cv::Point3i(x,y,z));
					}
				}
			}
		}
	}
	npts = (int)ptdeque.size();
	pts = new cv::Point3i[npts];
	for ( i = 0; i < npts; i++ ) {
		pts[i] = ptdeque.front();
		ptdeque.pop_front();
	}
}

// extract shell from CVoxel object only inside ROI
template <typename T>
CVoxelShell::CVoxelShell(CVoxel<T> *vxl, cv::Point3i &roiStart, cv::Point3i &roiEnd )
{
	std::deque<cv::Point3i> ptdeque;
	int nx, ny, nz, x, y, z, i, nxy;
	nx = vxl->m_nX;
	ny = vxl->m_nY;
	nz = vxl->m_nZ;
	int nxm1 = nx-1;
	int nym1 = ny-1;
	int nzm1 = nz-1;
	nxy = nx*ny;
	short int *pData = vxl->m_pData;
	VERIFY( roiStart > roiEnd );
	VERIFY( roiStart >= cv::Point3i(0,0,0) );
	VERIFY( roiStart < cv::Point3i(nx,ny,nz) );
	for ( z = roiStart.z; z < roiEnd.z; z++ ) {
		for ( y = roiStart.y; y < roiEnd.y; y++ ) {
			for ( x = roiStart.x; x <= roiEnd.x; x++ ) {
				i = z*nx*ny+y*nx+x;
				if ( pData[i] && 
					( (z>0 && pData[i-nxy]==0) || 
					(z<nzm1 && pData[i+nxy]==0) || 
					(y>0 && pData[i-nx]==0) || 
					(y<nym1 && pData[i+nx]==0) || 
					(x>0 && pData[i-1]==0)  || 
					(x<nxm1 && pData[i+1]==0) ))
				{
					ptdeque.push_back(cv::Point3i(x,y,z));
				}
			}
		}
	}
	npts = (int)ptdeque.size();
	pts = new cv::Point3i[npts];
	for ( i = 0; i < npts; i++ ) {
		pts[i] = ptdeque.front();
		ptdeque.pop_front();
	}	
}

CVoxelShell::CVoxelShell(cvPoint3iArray &ptArr)
{
	npts = (int) ptArr.GetSize();
	pts = new cv::Point3i[npts];
	for ( int i = 0; i < npts; i++ ) {
		pts[i] = ptArr[i];
	}
}

CVoxelShell::CVoxelShell(int *pData, int nx, int ny, int nz)
{
	std::deque<cv::Point3i> ptdeque;
	int i, x, y, z, nxy;
	nxy = nx*ny;
	int nxm1 = nx-1;
	int nym1 = ny-1;
	int nzm1 = nz-1;
	for ( z = 0,i = 0; z < nz; z++ ) {
		for ( y = 0; y < ny; y++ ) {
			for ( x = 0; x < nx; x++, i++ ) {
				if ( pData[i] && 
					//( ( x == 0 || x == nx - 1 || y == 0 || y == ny - 1 || z == 0 || z == nz - 1 ) ||
					( (z>0 && pData[i-nxy]==0) || 
					(z<nzm1 && pData[i+nxy]==0) || 
					(y>0 && pData[i-nx]==0) || 
					(y<nym1 && pData[i+nx]==0) || 
					(x>0 && pData[i-1]==0)  || 
					(x<nxm1 && pData[i+1]==0) ))  //) 
				{
					ptdeque.push_back(cv::Point3i(x,y,z));
				}
			}
		}
	}
	npts = (int)ptdeque.size();
	pts = new cv::Point3i[npts];
	for ( i = 0; i < npts; i++ ) {
		pts[i] = ptdeque.front();
		ptdeque.pop_front();
	}
}

CVoxelShell::CVoxelShell(CString str): pts(NULL), npts(0)
{
	Load(str);
}

CVoxelShell* CVoxelShell::UpSampleBCIResult()
{
	CVoxelShell* Hres = new CVoxelShell;

	int n = npts*8;
	Hres->npts = n;
	Hres->pts = new cv::Point3i[n];


	cv::Point3i cps;
	int count=0;
	for( int i = 0; i < npts; i++ ) {
		cps.x = pts[i].x*2;			cps.y = pts[i].y*2;			cps.z = pts[i].z*2;			Hres->pts[count] = cps;		count++;
		cps.x = pts[i].x*2;			cps.y = pts[i].y*2;			cps.z = pts[i].z*2+1;		Hres->pts[count] = cps;		count++;
		cps.x = pts[i].x*2;			cps.y = pts[i].y*2+1;		cps.z = pts[i].z*2;			Hres->pts[count] = cps;		count++;
		cps.x = pts[i].x*2;			cps.y = pts[i].y*2+1;		cps.z = pts[i].z*2+1;		Hres->pts[count] = cps;		count++;
		cps.x = pts[i].x*2+1;		cps.y = pts[i].y*2;			cps.z = pts[i].z*2;			Hres->pts[count] = cps;		count++;
		cps.x = pts[i].x*2+1;		cps.y = pts[i].y*2;			cps.z = pts[i].z*2+1;		Hres->pts[count] = cps;		count++;
		cps.x = pts[i].x*2+1;		cps.y = pts[i].y*2+1;		cps.z = pts[i].z*2;			Hres->pts[count] = cps;		count++;
		cps.x = pts[i].x*2+1;		cps.y = pts[i].y*2+1;		cps.z = pts[i].z*2+1;		Hres->pts[count] = cps;		count++;
	}

	return Hres;
}

template <typename T>
void CVoxelShell::MakeShell(CVoxel<T> *vxl)
{
	std::deque<cv::Point3i> ptdeque;
	int nx, ny, nz, i, x, y, z, nxy;
	nx = vxl->m_nX;
	ny = vxl->m_nY;
	nz = vxl->m_nZ;
	nxy = nx*ny;
	short int *pData = vxl->m_pData;
	int nxm1 = nx-1;
	int nym1 = ny-1;
	int nzm1 = nz-1;
	for ( z = 0,i = 0; z < nz; z++ ) {
		for ( y = 0; y < ny; y++ ) {
			for ( x = 0; x < nx; x++, i++ ) {
				if ( pData[i] && 
					//( ( x == 0 || x == nx - 1 || y == 0 || y == ny - 1 || z == 0 || z == nz - 1 ) ||
					( (z>0 && pData[i-nxy]==0) || 
					(z<nzm1 && pData[i+nxy]==0) || 
					(y>0 && pData[i-nx]==0) || 
					(y<nym1 && pData[i+nx]==0) || 
					(x>0 && pData[i-1]==0)  || 
					(x<nxm1 && pData[i+1]==0) )) // ) 
				{
					ptdeque.push_back(cv::Point3i(x,y,z));
				}
			}
		}
	}
	npts = (int)ptdeque.size();
	pts = new cv::Point3i[npts];
	for ( i = 0; i < npts; i++ ) {
		pts[i] = ptdeque.front();
		ptdeque.pop_front();
	}
}

template <typename T>
void CVoxelShell::MakeShell(CVoxel<T> *vxl, int BT)
{
	std::deque<cv::Point3i> ptdeque;
	int nx, ny, nz, i, x, y, z, nxy;
	nx = vxl->m_nX;
	ny = vxl->m_nY;
	nz = vxl->m_nZ;
	nxy = nx*ny;
	short int *pData = vxl->m_pData;
	int nxm1 = nx-1;
	int nym1 = ny-1;
	int nzm1 = nz-1;
	for ( z = 0,i = 0; z < nz; z++ ) {
		for ( y = 0; y < ny; y++ ) {
			for ( x = 0; x < nx; x++, i++ ) {
				if ( pData[i] == BT && 
					//( ( x == 0 || x == nx - 1 || y == 0 || y == ny - 1 || z == 0 || z == nz - 1 ) ||
					( (z>0 && pData[i-nxy]==0) || 
					(z<nzm1 && pData[i+nxy]==0) || 
					(y>0 && pData[i-nx]==0) || 
					(y<nym1 && pData[i+nx]==0) || 
					(x>0 && pData[i-1]==0)  || 
					(x<nxm1 && pData[i+1]==0) )) // ) 
				{
					ptdeque.push_back(cv::Point3i(x,y,z));
				}
			}
		}
	}
	npts = (int)ptdeque.size();
	pts = new cv::Point3i[npts];
	for ( i = 0; i < npts; i++ ) {
		pts[i] = ptdeque.front();
		ptdeque.pop_front();
	}
}

void CVoxelShell::Shift(int x, int y, int z)
{
	cv::Point3i pt(x,y,z);
	for ( int i = 0; i < npts; i++ ) {
		pts[i] += pt;
	}
}

CVoxel<BYTE> *CVoxelShell::Voxellize(int nx, int ny, int nz)
{
	CVoxel<BYTE> *res = new CVoxel<BYTE>(nx, ny, nz);
	for ( int i = 0; i < npts; i++ ) {
		if ( 0 <= pts[i].x && pts[i].x < nx && 
			0 <= pts[i].y && pts[i].y < ny && 
			0 <= pts[i].z && pts[i].z < nz )
			res->SetAt( pts[i], 1 );
	}
	return res;
}

CVoxelShell* CVoxelShell::ClassifyBCI( CVoxelShell* tBCI, CVoxelShell*tBOI )
{
	cvPoint3iArray bciptArray;

	int i,j;
	double dist;
	for ( i = 0; i < npts; i++ ) {
		cv::Point3i cpt = pts[i];

		double bciDist = FLT_MAX;
		for ( j = 0; j < tBCI->npts; j++ ) {
			cv::Point3i npt = tBCI->pts[j];
			dist = VectorL2Dist(cpt, npt);
			if ( dist < bciDist ) {
				bciDist = dist ;
			}
		}
		double boiDist = FLT_MAX;
		for ( j = 0; j < tBOI->npts; j++ ) {
			cv::Point3i npt = tBOI->pts[j];
			dist = VectorL2Dist(cpt, npt);
			if ( dist < boiDist ) {
				boiDist = dist ;
			}
		}

		/*double boiDist = 100000;
		for ( j = 0; j < tBOI->npts; j++ ) {
			cv::Point3i npt = tBOI->pts[j];
			dist = VectorL2Dist(cpt, npt);
			if ( dist < boiDist ) {
				dist = boiDist;
			}
		}*/

		//if ( bciDist < 2.0 ) {
		//	bciptArray.Add(cpt);
		//}
		if ( bciDist < boiDist ) {
			bciptArray.Add(cpt);
		}
	}

	CVoxelShell* bci = new CVoxelShell(bciptArray);
	return bci;

}

template <typename T>
void CVoxelShell::GetBCIBOI( CVoxel<T>* carti, CVoxelShell** bci, CVoxelShell** boi )
{
	cvPoint3iArray bciptArray;
	cvPoint3iArray boiptArray;
	int i, x, y, z; 
	BOOL bBCI = FALSE;
	// for all bone shell points
	for ( i = 0; i < npts; i++ ) {
		// check if current bone shell point is on BCI
		cv::Point3i pt = pts[i];
		
		bBCI = FALSE;
		for ( z = pt.z-1; z <=pt.z+1; z++ ) {
			if ( 0 <= z && z < carti->m_nZ ) {
				for ( y = pt.y-2; y <=pt.y+2; y++ ) {
					if ( 0 <= y && y < carti->m_nY ) {
						for ( x = pt.x-2; x <=pt.x+2; x++ ) {
							if ( 0 <= x && x < carti->m_nX ) {
								if ( carti->m_pData[z*carti->m_nX*carti->m_nY+y*carti->m_nX+x] ) {
									bBCI = TRUE;
									break;
								}
							}
						}
					}
					if (bBCI) break;
				}
			}
			if (bBCI) break;
		}
		if ( bBCI ) 	
			bciptArray.Add(pt);
		else 
		{
			boiptArray.Add(pt);
		}
	}

	*bci = new CVoxelShell(bciptArray);
	*boi = new CVoxelShell(boiptArray);	
}



// apply stratified sampling of the surface CVoxelShell
void CVoxelShell::StratifiedSampling(cv::Point3i ptStrat)
{
	VERIFY( ptStrat.x > 0 && ptStrat.y > 0 && ptStrat.z > 0 );
	int i, j, x, y, z;

	// determine size of surface 
	double maxx, maxy, maxz;
	double minx, miny, minz;
	maxx = maxy = maxz = INT_MIN;
	minx = miny = minz = INT_MAX;
	for ( i = 0; i < npts; i++ ) {
		if ( pts[i].x < minx ) minx = pts[i].x;
		if ( pts[i].x > maxx ) maxx = pts[i].x;
		if ( pts[i].y < miny ) miny = pts[i].y;
		if ( pts[i].y > maxy ) maxy = pts[i].y;
		if ( pts[i].z < minz ) minz = pts[i].z;
		if ( pts[i].z > maxz ) maxz = pts[i].z;
	}

	// determine number of voxels per axis based on stratification intervals
	int nVoxX = (int) (floor((double)(maxx-minx)/(double)ptStrat.x)+1);
	int nVoxY = (int) (floor((double)(maxy-miny)/(double)ptStrat.y)+1);
	int nVoxZ = (int) (floor((double)(maxz-minz)/(double)ptStrat.z)+1);
	int nVox = nVoxX*nVoxY*nVoxZ;
	// create 2D array to store surface points per each voxel
	CTypedPtrArray<CPtrArray, CSurfacePointVoxel *> sfPtPerVoxArr;
	for ( z = 0; z < nVoxZ; z++ ) {
		for ( y = 0; y < nVoxY; y++ ) {
			for ( x = 0; x < nVoxX; x++ ) {
				// store position of voxel
				cv::Point3i vPos((int)(minx+x*ptStrat.x), (int)(miny+y*ptStrat.y), (int)(minz+z*ptStrat.z));
				// store size of voxel
				CSurfacePointVoxel *voxPtArr = new CSurfacePointVoxel(vPos, ptStrat);
				sfPtPerVoxArr.Add(voxPtArr);
			}
		}
	}

	// store each surface point at array of corresponding voxel 
	double stx = ptStrat.x;
	double sty = ptStrat.y;
	double stz = ptStrat.z;
	for ( i = 0; i < npts; i++ ) {
		int vx = (int) ( ((double) pts[i].x-minx) / stx );
		int vy = (int) ( ((double) pts[i].y-miny) / sty );
		int vz = (int) ( ((double) pts[i].z-minz) / stz );
		sfPtPerVoxArr[vz*nVoxX*nVoxY+vy*nVoxX+vx]->m_sfPtArr.Add(pts[i]);
	}
	// remove voxels that have no surface points
	for ( i = nVox-1; i >= 0; i-- ) {
		if ( (int) sfPtPerVoxArr[i]->m_sfPtArr.GetSize() == 0 ) {
			CSurfacePointVoxel *voxPtArr = sfPtPerVoxArr[i];
			sfPtPerVoxArr.RemoveAt(i);
			SAFE_DELETE(voxPtArr);
		}
	}

	// apply octtree-type structure
	nVox = (int) sfPtPerVoxArr.GetSize();
	for ( i = 0; i < nVox; i++ ) {
		CSfPtPtrArray sfPtPtrArr;
		if ( sfPtPerVoxArr[i]->SubdivideVoxel(sfPtPtrArr) ) {
			SAFE_DELETE(sfPtPerVoxArr[i]);
			sfPtPerVoxArr.RemoveAt(i);
			for ( j = 0; j < (int) sfPtPtrArr.GetSize(); j++ ) {
				sfPtPerVoxArr.Add(sfPtPtrArr[j]);
			}
		}
	}

	// extract single surface point from each surface point array corresponding to each voxel
	srand((unsigned)time(NULL));
	npts = (int) sfPtPerVoxArr.GetSize();
	SAFE_DELETE_ARRAY(pts);
	pts = new cv::Point3i[npts];
	for ( i = 0; i < npts; i++ ) {
		pts[i] = sfPtPerVoxArr[i]->UniformSampleSurfacePoint();
		//pts[i] = sfPtPerVoxArr[i]->SampleMedianSurfacePoint();
		SAFE_DELETE(sfPtPerVoxArr[i]);
	}
	sfPtPerVoxArr.RemoveAll();
}


void CVoxelShell::StratifiedSampling(cv::Point3i ptStrat, BOOL sampleMethod)
{
	VERIFY( ptStrat.x > 0 && ptStrat.y > 0 && ptStrat.z > 0 );
	int i, j, x, y, z;

	// determine size of surface 
	double maxx, maxy, maxz;
	double minx, miny, minz;
	maxx = maxy = maxz = INT_MIN;
	minx = miny = minz = INT_MAX;
	for ( i = 0; i < npts; i++ ) {
		if ( pts[i].x < minx ) minx = pts[i].x;
		if ( pts[i].x > maxx ) maxx = pts[i].x;
		if ( pts[i].y < miny ) miny = pts[i].y;
		if ( pts[i].y > maxy ) maxy = pts[i].y;
		if ( pts[i].z < minz ) minz = pts[i].z;
		if ( pts[i].z > maxz ) maxz = pts[i].z;
	}

	// determine number of voxels per axis based on stratification intervals
	int nVoxX = (int) (floor((double)(maxx-minx)/(double)ptStrat.x)+1);
	int nVoxY = (int) (floor((double)(maxy-miny)/(double)ptStrat.y)+1);
	int nVoxZ = (int) (floor((double)(maxz-minz)/(double)ptStrat.z)+1);
	int nVox = nVoxX*nVoxY*nVoxZ;
	// create 2D array to store surface points per each voxel
	CTypedPtrArray<CPtrArray, CSurfacePointVoxel *> sfPtPerVoxArr;
	for ( z = 0; z < nVoxZ; z++ ) {
		for ( y = 0; y < nVoxY; y++ ) {
			for ( x = 0; x < nVoxX; x++ ) {
				// store position of voxel
				cv::Point3i vPos((int)(minx+x*ptStrat.x), (int)(miny+y*ptStrat.y), (int)(minz+z*ptStrat.z));
				// store size of voxel
				CSurfacePointVoxel *voxPtArr = new CSurfacePointVoxel(vPos, ptStrat);
				sfPtPerVoxArr.Add(voxPtArr);
			}
		}
	}

	// store each surface point at array of corresponding voxel 
	double stx = ptStrat.x;
	double sty = ptStrat.y;
	double stz = ptStrat.z;
	for ( i = 0; i < npts; i++ ) {
		int vx = (int) ( ((double) pts[i].x-minx) / stx );
		int vy = (int) ( ((double) pts[i].y-miny) / sty );
		int vz = (int) ( ((double) pts[i].z-minz) / stz );
		sfPtPerVoxArr[vz*nVoxX*nVoxY+vy*nVoxX+vx]->m_sfPtArr.Add(pts[i]);
	}
	// remove voxels that have no surface points
	for ( i = nVox-1; i >= 0; i-- ) {
		if ( (int) sfPtPerVoxArr[i]->m_sfPtArr.GetSize() == 0 ) {
			CSurfacePointVoxel *voxPtArr = sfPtPerVoxArr[i];
			sfPtPerVoxArr.RemoveAt(i);
			SAFE_DELETE(voxPtArr);
		}
	}

	// apply octtree-type structure
	nVox = (int) sfPtPerVoxArr.GetSize();
	for ( i = 0; i < nVox; i++ ) {
		CSfPtPtrArray sfPtPtrArr;
		if ( sfPtPerVoxArr[i]->SubdivideVoxel(sfPtPtrArr) ) {
			SAFE_DELETE(sfPtPerVoxArr[i]);
			sfPtPerVoxArr.RemoveAt(i);
			for ( j = 0; j < (int) sfPtPtrArr.GetSize(); j++ ) {
				sfPtPerVoxArr.Add(sfPtPtrArr[j]);
			}
		}
	}

	// extract single surface point from each surface point array corresponding to each voxel
	srand((unsigned)time(NULL));
	npts = (int) sfPtPerVoxArr.GetSize();
	SAFE_DELETE_ARRAY(pts);
	pts = new cv::Point3i[npts];
	for ( i = 0; i < npts; i++ ) {
		if ( sampleMethod == TRUE ) {
			pts[i] = sfPtPerVoxArr[i]->SampleMedianSurfacePoint();
		}
		else
			pts[i] = sfPtPerVoxArr[i]->UniformSampleSurfacePoint();
		//pts[i] = sfPtPerVoxArr[i]->SampleMedianSurfacePoint();
		SAFE_DELETE(sfPtPerVoxArr[i]);
	}
	sfPtPerVoxArr.RemoveAll();
}

void CVoxelShell::NearPointRemoveSampling( cv::Point3i range, int nX, int nY, int nZ )
{
	//VERIFY( range.x > 0 && range.y > 0 && range.z > 0 );
	int i,x, y, z, l,m,n;
	int bx,by,bz,fx,fy,fz;

	//cvPoint3iArray ptArr;
	cvPoint3iArray selectPtArr;
/*
	for ( i = 0; i < npts; i++ ) {
		cv::Point3i pt = pts[i];
		ptArr.Add(pt);
	}*/

	CVoxel<BYTE>* voxel = Voxellize( nX, nY, nZ );

	range = range*0.5;
	for ( z = 0; z < nZ; z++ ) {
		for ( y = 0; y < nY; y++ ) {
			for ( x = 0; x < nX; x++ ) {
				if ( voxel->GetAt(x,y,z) ) {
					cv::Point3i pt = cv::Point3i(x,y,z);
					selectPtArr.Add(pt);

					bx = std::max(pt.x-range.x,0);	fx = std::min(nX-1, pt.x+range.x);
					by = std::max(pt.y-range.y,0);	fy = std::min(nY-1, pt.y+range.y);
					bz = std::max(pt.z-range.z,0);	fz = std::min(nZ-1, pt.z+range.z);

					for ( n = bz; n <= fz; n++ ) {
						for ( m = by; m <= fy; m++ ) {
							for ( l = bx; l <= fx; l++ ) {
								if ( voxel->GetAt(l,m,n) ) {
									voxel->SetAt(l,m,n,0);
								}
							}
						}
					}
				}

			}
		}
	}
	
/*
	int nPtArr = ptArr.GetSize();
	for ( i = 0; i < nPtArr; i ) {
		cv::Point3i pt = ptArr.GetAt(i);
		selectPtArr.Add(pt);

		bx = std::max(pt.x-range.x,0);	fx = pt.x+range.x;
		by = std::max(pt.y-range.y,0);	fy = pt.y+range.y;
		bz = std::max(pt.z-range.z,0);	fz = pt.z+range.z;

		for ( z = bz; z < fz; z++ ) {
			for ( y = by; y < fy; y++ ) {
				for ( x = bx; x < fx; x++ ) {
					cv::Point3i nearPt = cv::Point3i(x,y,z);
					for ( j = 0; j < ptArr.GetSize(); j++) {
						if ( ptArr.GetAt(j) == nearPt) {
							ptArr.RemoveAt(j);
						}
					}
				}
			}
		}
		nPtArr = ptArr.GetSize();
	}
*/
	SAFE_DELETE(voxel);

	if ( pts ) {
		SAFE_DELETE_ARRAY(pts);
	}

	int nSelectPt = selectPtArr.GetSize();
	npts = nSelectPt;
	pts = new cv::Point3i[nSelectPt];

	for ( i = 0; i < nSelectPt; i++ ) {
		pts[i] = selectPtArr[i];
	}
}

void CVoxelShell::RemoveAt(int n)
{
	int new_npts = npts-1;
	cv::Point3i *new_pts = new cv::Point3i[new_npts];
	int ptc = 0;
	for ( int i = 0; i < npts; i++ ) {
		if ( i != n ) {
			new_pts[ptc++] = pts[i];
		}
	}
	npts = new_npts;
	SAFE_DELETE_ARRAY(pts);
	pts = new_pts;
}

CVoxelShell* CVoxelShell::Copy()
{
	CVoxelShell* shell = new CVoxelShell;
	shell->npts = npts;
	shell->pts = new cv::Point3i[npts];
	for ( int i = 0; i < npts; i++ ) {
		shell->pts[i] = pts[i];
	}
	return shell;
}


void CVoxelShell::SaveAsCVoxel(CString str, cv::Point3i range, int nX, int nY, int nZ)
{
	CVoxel<BYTE>* anchorVxl = Voxellize(nX, nY, nZ);
	int bx,by,bz,fx,fy,fz;
	cv::Point3i pt;
	int wdx = Round(range.x/2.0)-1;
	int wdy = Round(range.y/2.0)-1;
	int wdz = Round(range.z/2.0)-1;
	for ( int i = 0; i < npts; i++ )
	{
		bx = std::max(pts[i].x-wdx,0);	fx = std::min(anchorVxl->m_nX-1,pts[i].x+wdx);
		by = std::max(pts[i].y-wdy,0);	fy = std::min(anchorVxl->m_nY-1,pts[i].y+wdy);
		bz = std::max(pts[i].z-wdz,0);	fz = std::min(anchorVxl->m_nZ-1,pts[i].z+wdz);

		anchorVxl->SetAt(pts[i].x,pts[i].y,pts[i].z,1);
		/*for ( int l =  bx; l <= fx ; l++ ) {
			for ( int m = by  ; m <= fy  ; m++ ) {
				for ( int n = bz  ; n <= fz; n++ ) {
					anchorVxl->SetAt(l,m,n,1);
				}
			}
		}*/

		TRACE( "%d \t, %d \t, %d ", pts[i].x, pts[i].y, pts[i].z );
		//timestream << i << "\t" << pts[i].x << "\t" << pts[i].y << "\t" << pts[i].z << endl;
	}

	anchorVxl->WriteToBin(str);
	SAFE_DELETE(anchorVxl);
}


void CVoxelShell::Save(CString str)
{
	using namespace std;
	ofstream outstream;
	outstream.open(str, ios::out|ios::binary|ios::trunc);
	if ( outstream.is_open() ) {
		outstream.write((char*)&npts, sizeof(int));
		//outstream.write((char*)pts, sizeof(cv::Point3i)*npts);
		for ( int i = 0; i < npts; i++ ) {
			outstream.write((char*)&(pts[i].x), sizeof(int));
			outstream.write((char*)&(pts[i].y), sizeof(int));
			outstream.write((char*)&(pts[i].z), sizeof(int));
		}
		outstream.close();
	}
	else { 
		AfxMessageBox(_T("Error at CVoxelShell::Save()!"));
	}
}

void CVoxelShell::Load(CString str)
{
	using namespace std;
	ifstream instream;
	instream.open(str, ios::in|ios::binary);
	if ( instream.is_open() ) {
		instream.read((char*)&npts, sizeof(int));
		SAFE_DELETE_ARRAY(pts);
		pts = new cv::Point3i[npts];
		for ( int i = 0; i < npts; i++ ) {
			instream.read((char*)&(pts[i].x), sizeof(int));
			instream.read((char*)&(pts[i].y), sizeof(int));
			instream.read((char*)&(pts[i].z), sizeof(int));
		}
		instream.close();
	}
	else { 
		AfxMessageBox(_T("Error at CVoxelShell::Load()!"));
	}
}

void CVoxelShell::Transform(vtkMatrix4x4 *T, CVoxelShell **transVxl)
{
	// get the inverse
	vtkMatrix4x4 *imat = vtkMatrix4x4::New();
	vtkMatrix4x4::Invert(T, imat);

	double gridpt[4];
	double transpt[4];
	cv::Point3i lt(0,0,0);
	cv::Point3i rb(192,192,80);
	int nPT = npts;
	cvPoint3iArray transPos;
	for ( int j = 0; j < nPT; j++ )
	{
		cv::Point3i trPt;
		gridpt[0] = pts[j].x;		gridpt[1] = pts[j].y;
		gridpt[2] = pts[j].z;		gridpt[3] = 1;
		//imat->MultiplyPoint(gridpt, transpt);
		imat->MultiplyPoint(gridpt, transpt);
		trPt.x = (int) Round(transpt[0]/transpt[3]);
		trPt.y = (int) Round(transpt[1]/transpt[3]);
		trPt.z = (int) Round(transpt[2]/transpt[3]);
		transPos.Add(trPt);
	}
	imat->Delete();
	*transVxl = new CVoxelShell(transPos);
}


void CVoxelShell::Transform(vtkMatrix4x4 *T)
{
	// get the inverse
	vtkMatrix4x4 *imat = vtkMatrix4x4::New();
	vtkMatrix4x4::Invert(T, imat);

	double gridpt[4];
	double transpt[4];
	cv::Point3i lt(0,0,0);
	cv::Point3i rb(192,192,80);
	int nPT = npts;
	cvPoint3iArray transPos;
	for ( int j = 0; j < nPT; j++ )
	{
		gridpt[0] = pts[j].x;		gridpt[1] = pts[j].y;
		gridpt[2] = pts[j].z;		gridpt[3] = 1;
		//imat->MultiplyPoint(gridpt, transpt);
		imat->MultiplyPoint(gridpt, transpt);
		pts[j].x = (int) Round(transpt[0]/transpt[3]);
		pts[j].y = (int) Round(transpt[1]/transpt[3]);
		pts[j].z = (int) Round(transpt[2]/transpt[3]);
	}
	imat->Delete();
}


// END of CVoxelShell functions
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
cv::Point3i CSurfacePointVoxel::UniformSampleSurfacePoint()
{
	int nVoxPt = (int) m_sfPtArr.GetSize();
	int sampPtIdx = (int)((double)(rand()-1)/(double)RAND_MAX*nVoxPt);
	return m_sfPtArr[sampPtIdx];
}

cv::Point3i CSurfacePointVoxel::SampleMedianSurfacePoint()
{
	int nVoxPt = (int) m_sfPtArr.GetSize();
	cv::Point3i mean(0,0,0);
	for ( int i = 0; i < nVoxPt; i++ ) {
		mean += m_sfPtArr[i];
	}
	mean = mean*(1.0/(double)nVoxPt);
	cv::Point3i minpt;
	double minDist = FLT_MAX;
	for ( int i = 0; i < nVoxPt; i++ ) {
		if ( cv::norm(m_sfPtArr[i]-mean) < minDist )
			minpt = m_sfPtArr[i];
	}
	return minpt;
}

BOOL CSurfacePointVoxel::SubdivideVoxel(CSfPtPtrArray &sfPtArr)
{
	// for each voxel, count the number of surface points in voxel
	int nThreshVox = (int)( (double)(m_size.x*m_size.y*m_size.z)/
		(double)std::min(std::min(m_size.x, m_size.y), m_size.z) )+
		std::max(std::max(m_size.x, m_size.y), m_size.z);
	int nVoxPt = (int) m_sfPtArr.GetSize();
	int i;
	// if the number of points for the current voxel is larger than a certain number, 
	if ( nVoxPt > nThreshVox ) {

		// *** subdivide that voxel in half based on a surface with maximum area parallel to an axis *** //
		// project surface points in voxel to XY, YZ, and ZX planes, respectively
		int *projXY = new int[m_size.x*m_size.y];
		int *projYZ = new int[m_size.y*m_size.z];
		int *projZX = new int[m_size.z*m_size.x];
		for ( i = 0; i < m_size.x*m_size.y; i++ )	projXY[i] = 0;
		for ( i = 0; i < m_size.y*m_size.z; i++ )	projYZ[i] = 0;
		for ( i = 0; i < m_size.z*m_size.x; i++ )	projZX[i] = 0;
		int nCountXY, nCountYZ, nCountZX;
		nCountXY = nCountYZ = nCountZX = 0;

		for ( i = 0; i < nVoxPt; i++ ) {
			cv::Point3i pt = m_sfPtArr.GetAt(i);
			pt = pt-m_pos;
			VERIFY ( 0 <= pt.x && pt.x < m_size.x );
			VERIFY ( 0 <= pt.y && pt.y < m_size.y );
			VERIFY ( 0 <= pt.z && pt.z < m_size.z );
			projXY[pt.y*m_size.x+pt.x]++;
			projYZ[pt.z*m_size.y+pt.y]++;
			projZX[pt.x*m_size.z+pt.z]++;
		}

		// determine plane with largest projected area
		int nXY, nYZ, nZX;
		nXY = nYZ = nZX = 0;
		for ( i = 0; i < m_size.x*m_size.y; i++ ) { if ( projXY[i] )  nXY++; }
		for ( i = 0; i < m_size.y*m_size.z; i++ ) { if ( projYZ[i] )  nYZ++; }
		for ( i = 0; i < m_size.z*m_size.x; i++ ) { if ( projZX[i] )  nZX++; }

		// subdivide the plane with largest projected area
		// create surface point arrays corresponding to the subdivided voxels
		// voxel position: voxPosArr[i]
		int maxProjArea = MaxIdx3(nXY, nYZ, nZX);
		int nSplitAxis;
		switch( maxProjArea )
		{
			// XY plane has largest projected area
		case 1:
			// if size in x axis is larger: set split axis
			if ( m_size.x > m_size.y )	{ nSplitAxis = 1;	}
			// if size in y axis is larger
			else											{ nSplitAxis = 2; }
			break;
			// YZ plane has largest projected area
		case 2:
			if ( m_size.y > m_size.z )	{ nSplitAxis = 2;	}
			else											{ nSplitAxis = 3; }
			break;
			// ZX plane has largest projected area
		case 3:
			if ( m_size.z > m_size.x )	{ nSplitAxis = 3;	}
			else											{ nSplitAxis = 1; }
			break;
		}

		CSurfacePointVoxel *vox1, *vox2;
		cv::Point3i pos1(m_pos), size1, pos2, size2;
		int s;
		switch ( nSplitAxis ) {
		case 1:
			size1 = cv::Point3i( (s=(m_size.x>>1)), m_size.y, m_size.z);
			size2 = cv::Point3i(m_size.x-s, m_size.y, m_size.z);
			pos2 = cv::Point3i(m_pos.x+s, m_pos.y, m_pos.z);
			break;
		case 2:
			size1 = cv::Point3i( m_size.x, (s=(m_size.y>>1)), m_size.z);
			size2 = cv::Point3i(m_size.x, m_size.y-s, m_size.z);
			pos2 = cv::Point3i(m_pos.x, m_pos.y+s, m_pos.z);
			break;
		case 3:
			size1 = cv::Point3i( m_size.x, m_size.y, (s=(m_size.x>>1)) );
			size2 = cv::Point3i(m_size.x, m_size.y, m_size.z-s);
			pos2 = cv::Point3i(m_pos.x, m_pos.y, m_pos.z+s);
			break;
		}
		vox1 = new CSurfacePointVoxel(pos1, size1);
		vox2 = new CSurfacePointVoxel(pos2, size2);

		// add surface points to relevant surface point arrays
		for ( i = 0; i < nVoxPt; i++ ) {
			cv::Point3i pt = m_sfPtArr.GetAt(i);
			if ( vox1->m_pos.x <= pt.x && pt.x < vox1->m_pos.x+vox1->m_size.x && 
				vox1->m_pos.y <= pt.y && pt.y < vox1->m_pos.y+vox1->m_size.y && 
				vox1->m_pos.z <= pt.z && pt.z < vox1->m_pos.z+vox1->m_size.z ) {
					vox1->m_sfPtArr.Add(pt);
			}
			else {
				vox2->m_sfPtArr.Add(pt);
			}
		}
		if ( !(vox1->SubdivideVoxel(sfPtArr)) )	sfPtArr.Add(vox1);
		else SAFE_DELETE(vox1);
		if ( !(vox2->SubdivideVoxel(sfPtArr)) )	sfPtArr.Add(vox2);
		else SAFE_DELETE(vox2);
		SAFE_DELETE_ARRAY(projXY);
		SAFE_DELETE_ARRAY(projYZ);
		SAFE_DELETE_ARRAY(projZX);
		return TRUE;
	}
	else return FALSE;
}



void CVoxelShell::Set(cvPoint3iArray &ptArr)
{
	npts = (int) ptArr.GetSize();
	pts = new cv::Point3i[npts];
	for ( int i = 0; i < npts; i++ ) {
		pts[i] = ptArr[i];
	}
}
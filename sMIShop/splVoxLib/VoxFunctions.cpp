
#include "../stdafx.h"
#include "VoxFunctions.h"

CVoxel *ConstructWeightedVoxelModel(CVoxel *v1, CVoxel *v2, int n1, int n2)
{
	CVoxel *nv;
	int x, y, z, i;
	// size in x, y, z axes
	int st1x = v1->m_nX;
	int st1y = v1->m_nY;
	int st1z = v1->m_nZ;
	int st2x = v2->m_nX;
	int st2y = v2->m_nY;
	int st2z = v2->m_nZ;
	// new size
	// offset for smaller template
	int newX = st1x > st2x ? st1x : st2x;
	int newY = st1y > st2y ? st1y : st2y;
	int newZ = st1z > st2z ? st1z : st2z;
	// create new merged shape template
	nv = new CVoxel;
	nv->Allocate(newX, newY, newZ);
	short int *pStN = nv->m_pData;
	short int *pSt1 = v1->m_pData;
	short int *pSt2 = v2->m_pData;
	int newIdxX, newIdxY, newIdxZ;
	short int b1, b2;
	int offset1X, offset1Y, offset1Z, offset2X, offset2Y, offset2Z;
	// mark pixels with left child labels
	offset1X = (newX-st1x)>>1;
	offset1Y = (newY-st1y)>>1;
	offset1Z = (newZ-st1z)>>1;
	double w1 = (double)n1/(double)(n1+n2);
	double w2 = (double)n2/(double)(n1+n2);
	for ( z = 0, i = 0; z < st1z; z++) {
		newIdxZ = (z+offset1Z)*newX*newY;
		for ( y = 0; y < st1y; y++) {
			newIdxY = (y+offset1Y)*newX;
			for ( x = 0; x < st1x; x++, i++) {
				b1 = pSt1[i];
				newIdxX = newIdxZ+newIdxY+(x+offset1X);
				pStN[newIdxX] += (short int) Round((double)b1*w1);
			}
		}
	}
	// mark pixels with right child labels
	offset2X = (newX-st2x)>>1;
	offset2Y = (newY-st2y)>>1;
	offset2Z = (newZ-st2z)>>1;
	for ( z = 0, i = 0; z < st2z; z++) {
		newIdxZ = (z+offset2Z)*newX*newY;
		for ( y = 0; y < st2y; y++) {
			newIdxY = (y+offset2Y)*newX;
			for ( x = 0; x < st2x; x++, i++) {
				b2 = pSt2[i];
				newIdxX = newIdxZ+newIdxY+(x+offset2X);
				pStN[newIdxX] += (short int) Round((double)b2*w2);
			}
		}
	}
	return nv;
}


void VoxelArrayComputePCABase(DoublePtrArray &voxArr, int dim,
							  CvMat **dataMat, CvMat **dataMean, CvMat **pcaEvects, CvMat **pcaEvals)
{
	int i, j;
	int arrSize = (int) voxArr.GetSize();

	// compute mean 
	int nV = dim;
	double* sumVxl = new double[nV];
	for( i = 0; i < nV; i++ )
		sumVxl[i] = 0;

	if ( *dataMat )	cvReleaseMat(dataMat);
	*dataMat = cvCreateMat(nV, arrSize, CV_32F);
	if ( *dataMean )	cvReleaseMat(dataMean);
	*dataMean = cvCreateMat(nV, 1, CV_32F);

	//double *vxl1 = voxArr.GetAt(0);
	//double *vxl2 = voxArr.GetAt(1);
	//double *vxl3 = voxArr.GetAt(2);
	//double *vxl4 = voxArr.GetAt(3);

	for( i = 0; i < arrSize; i++ ) {
		for ( j = 0; j < nV; j++ ) {
			double *vxl = voxArr[i];
			sumVxl[j] += vxl[j];
			cvmSet(*dataMat, j, i, (float)vxl[j]);
		}
	}
	for( i = 0; i < nV; i++ )
		cvmSet(*dataMean, i, 0, (float)sumVxl[i]/(float)arrSize);

	//PCA 계산
	if ( *pcaEvects )	cvReleaseMat(pcaEvects);
	*pcaEvects = cvCreateMat(nV, nV, CV_32F);

	//if ( nV > arrSize)	*pcaEvects = cvCreateMat(nV, arrSize, CV_32F);
	//else	*pcaEvects = cvCreateMat(nV, nV, CV_32F);

	if ( *pcaEvals )	cvReleaseMat(pcaEvals);
	*pcaEvals = cvCreateMat(nV, 1, CV_32F);
	
	//if ( nV > arrSize)	*pcaEvals = cvCreateMat(arrSize, 1, CV_32F);	
	//else	*pcaEvals = cvCreateMat(nV, 1, CV_32F);
	
	//cvCalcPCA( *dataMat, *dataMean, *pcaEvals, *pcaEvects, CV_PCA_DATA_AS_COL );

	// 패치가 커져서 nV > arrSize 일때 
	CvMat** tmp_dataMat = new CvMat*[arrSize]; //cvCreateMat(nV, arrSize, CV_32F);
	for( i = 0; i < arrSize; i++ ) {
		tmp_dataMat[i] = cvCreateMat(nV, 1, CV_32F);
		for ( j = 0; j < nV; j++ ) {
			double *vxl = voxArr[i];
			sumVxl[j] += vxl[j];
			cvmSet(tmp_dataMat[i], j, 0, (float)vxl[j]);
		}
	}
	//double **ta = new double*[]
	

	CvMat* cov_mat = cvCreateMat(nV, nV, CV_32F);
	CvMat* tmp_dataMean = cvCreateMat(nV, 1, CV_32F);

	cvCalcCovarMatrix( (const void **)tmp_dataMat, arrSize, cov_mat, tmp_dataMean, CV_COVAR_COLS | CV_COVAR_NORMAL);
	cvEigenVV( cov_mat, *pcaEvects, *pcaEvals, 1e-10 );

	for ( i = 0; i < arrSize; i++ )	cvReleaseMat(&tmp_dataMat[i]);
	delete tmp_dataMat;
	cvReleaseMat(&cov_mat);
	cvReleaseMat(&tmp_dataMean);
	SAFE_DELETE_ARRAY(sumVxl);
}

void VoxelArrayComputePCABase(CTypedPtrArray<CPtrArray, CVoxel*> &voxArr, 
							  CvMat **dataMat, CvMat **dataMean, CvMat **pcaEvects, CvMat **pcaEvals)
{
	int i, j;
	int sx = voxArr[0]->m_nX;
	int sy = voxArr[0]->m_nY;
	int sz = voxArr[0]->m_nZ;
	int arrSize = (int) voxArr.GetSize();
	// check if all CVoxel objects have same size
	for ( i = 1; i < arrSize; i++ ) {
		CVoxel *vxl = voxArr[i];
		VERIFY(sx == vxl->m_nX);
		VERIFY(sy == vxl->m_nY);
		VERIFY(sz == vxl->m_nZ);
	}

	// compute mean 
	int nV = sx*sy*sz;
	int* sumVxl = new int[nV];
	for( i = 0; i < nV; i++ )
		sumVxl[i] = 0;

	if ( *dataMat )	cvReleaseMat(dataMat);
	*dataMat = cvCreateMat(nV, arrSize, CV_32F);
	if ( *dataMean )	cvReleaseMat(dataMean);
	*dataMean = cvCreateMat(nV, 1, CV_32F);

	for( i = 0; i < arrSize; i++ ) {
		for ( j = 0; j < nV; j++ ) {
			CVoxel *vxl = voxArr[i];
			sumVxl[j] += vxl->m_pData[j];
			cvmSet(*dataMat, j, i, (float)vxl->m_pData[j]);
		}
	}
	for( i = 0; i < nV; i++ )
		cvmSet(*dataMean, i, 0, (float)sumVxl[i]/(float)arrSize);

	//PCA 계산
	if ( *pcaEvects )	cvReleaseMat(pcaEvects);
	*pcaEvects = cvCreateMat(nV, nV, CV_32F);
	if ( *pcaEvals )	cvReleaseMat(pcaEvals);
	*pcaEvals = cvCreateMat(nV, 1, CV_32F);
	cvCalcPCA( *dataMat, *dataMean, *pcaEvals, *pcaEvects, CV_PCA_DATA_AS_COL );

	SAFE_DELETE_ARRAY(sumVxl);
}

void VoxelArrayComputePCACoeff(CvMat *dataMat, CvMat *dataMean, CvMat *pcaEvects, CvMat *pcaEvals, 
												 CvMat **patCoeff, int &pcadim, double dimERate)
{
	int nDim = dataMean->rows;
	int nArr = dataMat->cols;
	VERIFY(nDim = pcaEvects->rows);
	VERIFY(nDim = pcaEvects->cols);
	VERIFY(nDim = pcaEvals->rows);
	int i;//, j;

	// if pcadim == 0, determine number of principal components based on energy
	if ( pcadim == 0 ) {
		double sumEval=0;
		double thres, eachVal;
		for ( i = 0; i < nDim; i++ )
		{
			eachVal = cvmGet(pcaEvals,i,0);
			sumEval += eachVal;
		}
		thres = sumEval*dimERate;

		while (thres > 0)
		{
			eachVal = cvmGet(pcaEvals, pcadim, 0);
			thres -= eachVal;
			pcadim++;
		}
	}

	/*
	CvMat* selectEvects = cvCreateMat(pcadim, nDim, CV_32F);
	for ( i = 0; i < pcadim; i++ )	{
		for ( j = 0; j < nDim; j++ )	{
			cvmSet(selectEvects, i, j, cvmGet(pcaEvects, i, j));
		}
	}
	*/

	*patCoeff = cvCreateMat(nArr, pcadim, CV_32F);
	//cvProjectPCA( dataMat, dataMean, selectEvects, *patCoeff );
	cvProjectPCA( dataMat, dataMean, pcaEvects, *patCoeff );
	//cvReleaseMat(&selectEvects);
}

void Voxel2DArrayComputePCABase(CTypedPtrArray<CPtrArray, CVoxel*> *vox2DArr, int nArr, 
								CvMat **dataMat, CvMat **dataMean, CvMat **pcaEvects, CvMat **pcaEvals)
{
	// aggregate vox array
	CTypedPtrArray<CPtrArray, CVoxel*> voxArr;
	int i, j;
	for ( i = 0; i < nArr; i++ ) {
		int n2DArr = (int) vox2DArr[i].GetSize();
		for ( j = 0; j < n2DArr; j++ ) {
			voxArr.Add(vox2DArr[i].GetAt(j));
		}
	}
	VoxelArrayComputePCABase(voxArr, dataMat, dataMean, pcaEvects, pcaEvals);
}



void SaveImagePerSlice2Voxels( CVoxel *vox1, CVoxel *vox2, CString DirName, CString FileName )
{
	VERIFY( vox1->m_nX == vox2->m_nX );
	VERIFY( vox1->m_nY == vox2->m_nY );
	VERIFY( vox1->m_nZ == vox2->m_nZ );
	int x, y, z, i, nCh = 3;
	for ( z = 0, i=0; z < vox1->m_nZ; z++ ) {
		CStringA strDir(DirName);
		CStringA strFile(FileName);
		CStringA strOut;
		strOut.Format("%s\\%s_%04d.bmp", strDir, strFile, z);

		IplImage *sliceImg = cvCreateImage(cvSize(vox1->m_nX, vox1->m_nY), IPL_DEPTH_8U, nCh);
		cvSet(sliceImg, cvScalar(0,0,0));
		BYTE *imPtr = (BYTE*) sliceImg->imageData;
		for ( y = 0; y < vox1->m_nY; y++ ) {
			for ( x = 0; x < vox1->m_nX; x++, i++ ) {
				imPtr[y*sliceImg->widthStep+x*nCh+0] = (BYTE) std::max(std::min(vox1->GetAt(x, y, z), 255),0);
				imPtr[y*sliceImg->widthStep+x*nCh+2] = (BYTE) std::max(std::min(vox2->GetAt(x, y, z)*40, 255),0);
			}
		}
		cvSaveImage(strOut, sliceImg);
		cvReleaseImage(&sliceImg);
	}
}


void ConvertINTtoCVOXEL(int* intarray, CVoxel* voxel)
{
	int x, y, z, pix;
	for ( z = 0, pix = 0; z < voxel->m_nZ; z++ ) {
		for ( y = 0; y < voxel->m_nY; y++ ) {
			for ( x = 0; x < voxel->m_nX; x++, pix++ ) {
				voxel->SetAt( x, y, z, intarray[pix]);
			}
		}
	}
}


void ConvertCVOXELtoINT(CVoxel* voxel, int* intarray)
{
	int x, y, z, pix, val;
	for ( z = 0, pix = 0; z < voxel->m_nZ; z++ ) {
		for ( y = 0; y < voxel->m_nY; y++ ) {
			for ( x = 0; x < voxel->m_nX; x++, pix++ ) {
				val = voxel->GetAt(x,y,z);
				intarray[pix] = val;
			}
		}
	}
}

void RegionGrowing(CVoxel* startReg, CVoxel* searchReg, CFVoxel* distProb, CFVoxel* appProb)
{
	int x, y, z, l, m, n, pix, ms_pix, fx, bx, fy, by, fz, bz, wd, npoint, nAddpoint;
	wd =1;
	nAddpoint = 1;
	npoint = startReg->GetNumber();
	double thres = 0.35;

	CVoxel* updatedregion = new CVoxel(startReg->m_nX,startReg->m_nY,startReg->m_nZ);
	while (nAddpoint != 0)
	{
		for(z=0, pix = 0;z<startReg->m_nZ;z++) {
			for(y=0;y<startReg->m_nY;y++) {
				for(x=0;x<startReg->m_nX;x++, pix++) {
					
					//Find update point
					if (startReg->GetAt(x,y,z))
					{
						bx = std::max(x-wd,0);	fx = std::min(startReg->m_nX-1,x+wd);
						by = std::max(y-wd,0);	fy = std::min(startReg->m_nY-1,y+wd);
						bz = std::max(z-wd,0);	fz = std::min(startReg->m_nZ-1,z+wd);

						for ( l =  bx; l <= fx; l++ ) {
							for (m = by; m <= fy; m++ ) {
								for ( n = bz; n <= fz; n++ ) {
									ms_pix = l+ m*startReg->m_nX + n*startReg->m_nX*startReg->m_nY;
									if (searchReg->GetAt(l,m,n) && startReg->GetAt(l,m,n) == 0)
									{
										if (distProb->GetAt(l,m,n) > thres && appProb->GetAt(l,m,n) > thres)
										{
											updatedregion->SetAt(l,m,n,1);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		nAddpoint = updatedregion->GetNumber();

		//Add update point to startregion
		for(z=0;z<startReg->m_nZ;z++) {
			for(y=0;y<startReg->m_nY;y++) {
				for(x=0;x<startReg->m_nX;x++) {
					if (startReg->GetAt(x,y,z) == 0 && updatedregion->GetAt(x,y,z)) {
						startReg->SetAt(x,y,z,1);
					}
				}
			}
		}
		npoint += nAddpoint;
	}
	delete updatedregion;

}

CVoxel *Union(CVoxel *v1, CVoxel *v2, short v )
{
	VERIFY(*v1 == *v2);
	int nX = v1->m_nX;
	int nY = v1->m_nY;
	int nZ = v1->m_nZ;
	CVoxel *vout = new CVoxel(nX, nY, nZ, FALSE);
	for( int z = 0; z < nZ; z++ ){
		for( int y = 0; y < nY; y++ ){
			for( int x = 0; x < nX; x++ ){
				if( v1->GetAt(x,y,z) || v2->GetAt(x,y,z) ) {
					vout->SetAt(x,y,z,v);
				}
				else	vout->SetAt(x,y,z,0);
			}
		}
	}
	return vout;
}

CVoxel *Union(CVoxel *v1, CVoxel *v2, short v, int v1bonetype )
{
	VERIFY(*v1 == *v2);
	int nX = v1->m_nX;
	int nY = v1->m_nY;
	int nZ = v1->m_nZ;
	CVoxel *vout = new CVoxel(nX, nY, nZ, FALSE);
	for( int z = 0; z < nZ; z++ ){
		for( int y = 0; y < nY; y++ ){
			for( int x = 0; x < nX; x++ ){
				if( v1->GetAt(x,y,z) == v1bonetype || v2->GetAt(x,y,z) ) {
					vout->SetAt(x,y,z,v);
				}
				else	vout->SetAt(x,y,z,0);
			}
		}
	}
	return vout;
}



#include "../stdafx.h"
#include "ClosestPoints.h"

void SaveKClosestPoints(CF3DPoint pt, C3DPointArray &ptArr, int K)
{
	int i, j, k, nPtArr = (int) ptArr.GetSize();
	double *knnDistArr = new double[K];
	C3DPoint *knnPtArr = new C3DPoint[K];
	for ( i =0; i < K; i++ ) {
		knnDistArr[i] = FLT_MAX;
	}
	for ( i =0; i < nPtArr; i++ ) {
		double dist = (pt-ptArr[i]).Norm();
		for ( j = 0; j < K; j++ ) {
			if ( dist < knnDistArr[j] ) {
				for ( k = K-1; k > j; k-- ) {
					knnDistArr[k] = knnDistArr[k-1];
					knnPtArr[k] = knnPtArr[k-1];
				}
				knnDistArr[j] = dist;
				knnPtArr[j] = ptArr[i];
				break;
			}
		}
	}
	ptArr.RemoveAll();
	for ( j = 0; j < K; j++ ) {
		ptArr.Add(knnPtArr[j]);
	}
	delete [] knnDistArr;
	delete [] knnPtArr;
}

void GetClosestPoints(CF3DPoint pt, C3DPointArray &ptArr,  C3DPoint &ptOut)
{
	int nBd = ptArr.GetSize();
	
	double mindist = FLT_MAX;
	double minIdx = 0;
	for ( int i = 0; i < nBd; i++ ) {
		double dist = (pt-ptArr[i]).Norm();
		if ( dist < mindist ) {
			mindist = dist;
			minIdx = i;
		}	 
	}
	ptOut.m_x = ptArr.GetAt(minIdx).m_x;
	ptOut.m_y = ptArr.GetAt(minIdx).m_y;
	ptOut.m_z = ptArr.GetAt(minIdx).m_z;
}

void GetKClosestPoints(CF3DPoint pt, C3DPointArray &ptArr, int K, C3DPointArray &ptOutArr )
{
	int i, j, k, nPtArr = (int) ptArr.GetSize();
	double *knnDistArr = new double[K];
	C3DPoint *knnPtArr = new C3DPoint[K];
	for ( i =0; i < K; i++ ) {
		knnDistArr[i] = FLT_MAX;
	}
	for ( i =0; i < nPtArr; i++ ) {
		double dist = (pt-ptArr[i]).Norm();
		for ( j = 0; j < K; j++ ) {
			if ( dist < knnDistArr[j] ) {
				for ( k = K-1; k > j; k-- ) {
					knnDistArr[k] = knnDistArr[k-1];
					knnPtArr[k] = knnPtArr[k-1];
				}
				knnDistArr[j] = dist;
				knnPtArr[j] = ptArr[i];
				break;
			}
		}
	}
	ptOutArr.RemoveAll();
	for ( j = 0; j < K; j++ ) {
		ptOutArr.Add(knnPtArr[j]);
	}
	delete [] knnDistArr;
	delete [] knnPtArr;
}

void GetKClosestPoints(CF3DPoint pt, C3DPointArray &ptArr, int K, C3DPointArray &ptOutArr, 
	C3DPoint patchStPos, C3DPoint patchPos, C3DPoint patSize, 
	int mri_nx, int mri_ny, int mri_nz )
{
	int i, j, k, nPtArr = (int) ptArr.GetSize();
	double *knnDistArr = new double[K];
	C3DPoint *knnPtArr = new C3DPoint[K];
	for ( i =0; i < K; i++ ) {
		knnDistArr[i] = FLT_MAX;
	}

	C3DPoint dPt = patchPos - patchStPos;
	int stX,stY,stZ,edX,edY,edZ;
	for ( i =0; i < nPtArr; i++ ) {
		stX = ptArr[i].m_x - dPt.m_x;
		stY = ptArr[i].m_y - dPt.m_y;
		stZ = ptArr[i].m_z - dPt.m_z;
		edX = stX + patSize.m_x;
		edY = stY + patSize.m_y;
		edZ = stZ + patSize.m_z;
		if ( 0 <= stX && stX < mri->m_nX && 0 <= stY && stY < mri->m_nY && 0 <= stZ && stZ < mri->m_nZ && 
			0 <= edX && edX < mri->m_nX && 0 <= edY && edY < mri->m_nY && 0 <= edZ && edZ < mri->m_nZ ) {
			double dist = (pt-ptArr[i]).Norm();
			for ( j = 0; j < K; j++ ) {
				if ( dist < knnDistArr[j] ) {
					for ( k = K-1; k > j; k-- ) {
						knnDistArr[k] = knnDistArr[k-1];
						knnPtArr[k] = knnPtArr[k-1];
					}
					knnDistArr[j] = dist;
					knnPtArr[j] = ptArr[i];
					break;
				}
			}
		}
	}
	ptOutArr.RemoveAll();
	for ( j = 0; j < K; j++ ) {
		ptOutArr.Add(knnPtArr[j]);
	}
	delete [] knnDistArr;
	delete [] knnPtArr;
}

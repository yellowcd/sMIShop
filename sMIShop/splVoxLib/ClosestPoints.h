
#pragma once

#include "F3DPoint.h"
#include "Voxel.h"

void GetClosestPoints(CF3DPoint pt, C3DPointArray &ptArr,  C3DPoint &ptOut);
void SaveKClosestPoints(CF3DPoint pt, C3DPointArray &ptArr, int K);
void GetKClosestPoints(CF3DPoint pt, C3DPointArray &ptArr, int K, C3DPointArray &ptOutArr);
void GetKClosestPoints(CF3DPoint pt, C3DPointArray &ptArr, int K, C3DPointArray &ptOutArr,
	C3DPoint patchStPos, C3DPoint patchPos, C3DPoint patSize, 
	int mri_nx, int mri_ny, int mri_nz );


#pragma once

#include "Util.h"
#include "Voxel.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Graph Cut Segmentation related methods
//////////////////////////////////////////////////////////////////////////////////////////
#define U_MAX 1000000
void ComputeGradientPairwiseTerms(IplImage *img, gtype *pairwise, gtype lambda);
void ComputeColorGradientPairwiseTerms(IplImage *img, gtype *pairwise, gtype lambda);
void SavePairwise(int w, int h, int lambda, gtype *pairwise);
void ComputeVoxelPairwiseTerms(CVoxel *vox, gtype *pairwise, gtype lambda);
void ComputeVoxelPairwiseTerms(CVoxel *vox, CFVoxelPtrArray& pairwise, gtype lambda);
void ComputeBCIPairwiseTerms(CVoxel *vox, int *pairwise, int lambda);
void Prob2Potential(float *f, int N);

void ConstantPairwiseTerms(CVoxel *vox, gtype *pairwise, gtype constVal);
void ComputeUnaryPairwiseSums(CFVoxel *fgU, CFVoxel *bgU, CTypedPtrArray<CPtrArray, CFVoxel*> &pw, cvPoint3iArray &pwnc, 
							  CVoxel *segm, double &usum, double &pwsum);
void ComputeUnaryPairwiseSums(CFVoxel *fgU, CFVoxel *bgU, 
							  CTypedPtrArray<CPtrArray, CFVoxel*> &pw, cvPoint3iArray &pwnc, int idx,
							  CVoxel *segm, double &usum, double &pwsum);

void ComputeTRWSUnaryPairwiseSums( float** unaryCost, int * nPair, int ** idxArray, float** pCostArray, int nNode, int* nlabel, int *label, double &usum, double &pwsum);

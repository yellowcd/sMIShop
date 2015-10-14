#include "Voxel.h"

CVoxel *ConstructWeightedVoxelModel(CVoxel *v1, CVoxel *v2, int n1, int n2);
void VoxelArrayComputePCABase(CTypedPtrArray<CPtrArray, CVoxel*> &voxArr, CvMat **dataMat, CvMat **dataMean, CvMat **pcaEvects, CvMat **pcaEvals);
void Voxel2DArrayComputePCABase(CTypedPtrArray<CPtrArray, CVoxel*> *vox2DArr, int nArr, CvMat **dataMat, CvMat **dataMean, CvMat **pcaEvects, CvMat **pcaEvals);
void VoxelArrayComputePCACoeff(CvMat *dataMat, CvMat *dataMean, CvMat *pcaEvects, CvMat *pcaEvals, CvMat **patCoeff, int &pcadim, double dimERate=0.9);
void SaveImagePerSlice2Voxels( CVoxel *vox1, CVoxel *vox2, CString DirName, CString FileName );
void VoxelPtrArray2CvMat(CTypedPtrArray<CPtrArray, CVoxel*> &voxArr, CvMat **dataMat);

//by sanghyun
void ConvertINTtoCVOXEL(int* intarray, CVoxel* voxel);
void ConvertCVOXELtoINT(CVoxel* voxel, int* intarray);
void RegionGrowing(CVoxel* startReg, CVoxel* searchReg, CFVoxel* distProb, CFVoxel* appProb);
void VoxelArrayComputePCABase(DoublePtrArray &voxArr, int dim,
					  CvMat **dataMat, CvMat **dataMean, CvMat **pcaEvects, CvMat **pcaEvals);
CVoxel *Union(CVoxel *v1, CVoxel *v2, short v);
CVoxel *Union(CVoxel *v1, CVoxel *v2, short v, int v1bonetype );

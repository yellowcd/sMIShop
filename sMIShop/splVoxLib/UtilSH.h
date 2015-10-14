#pragma once

#include "Util.h"
#include "Voxel.h"

//by shpark
double CalChiSquareDistance( double* arr1, double* arr2, int nArr );
double CalOverlaycost(CVoxel* v1, CVoxel* v2);
double CalDSCcost(CVoxel* v1, CVoxel* v2);
double* CalMultilabelDSCcost(CVoxel* v1, CVoxel* v2, int nLabel);
double CalSubRegionDSCcost(CVoxel* v1, CVoxel* v2, cv::Point3i stpt, cv::Point3i enpt);
void SortMaxDistances( double * data, int* dataidx, int p, int r );
int maxPartition( double * data,int* dataidx , int p, int r );
int minPartition( double * data,int* dataidx , int p, int r );
void SortMinDistances( double * data, int* dataidx, int p, int r );
void RearrangeFileArray(CStringArray &fileArr);
void RearrangeFileArrayWithFileName(CStringArray &fileArr);
void RearrangeFileArrayWithDoubleFileName(CStringArray &fileArr);

void MakePairwiseIdx( cvPoint3iArray& nPtArr, int nPt );

double** Create2DDoubleMat( int nRows, int nCols );
CVoxel* VTKImageToCVoxel(vtkImageData* vtkIamge);
CVoxel* VTKImageToCVoxel(vtkImageData* vtkIamge, int nX, int nY, int nZ );

void LinearNormalizeOfMRImage( CVoxel *inputvoxel, CPoint range, int maxval );
void LinearNormalizeOfMRImage( CVoxel *inputvoxel, CPoint range );

void SaveArrayColorProfile( CString str, float* colorProfile, int m_nDist );
void CalMeanAndVariance(double* arry, int nArry, double& mean, double& var, double& min, double& max);

float ComputeNCC(CVoxel* dVxl, float patMean, float patVar, CVoxel* img, cv::Point3i stpt, cv::Point3i enpt, float cMean, float cVar);
float ComputeNCC(CVoxel* dVxl, float patMean, float patVar, CVoxel* img, float cMean, float cVar);
float ComputeSubRegionNCC(CVoxel* img, CVoxel* cimg, cv::Point3i stpt, cv::Point3i enpt, float cMean, float cVar);
float ComputeSubRegionNCC(CVoxel* img, cv::Point3i istpt, CVoxel* cimg, cv::Point3i cstpt, cv::Point3i size, float cMean, float cVar);
float ComputeSubRegionNCC(CVoxel* img, CVoxel* cimg, cv::Point3i stpt, cv::Point3i enpt, CVoxel* label, float cMean, float cVar);
float ComputeSubRegionNCC(CVoxel* img, float iMean, float iVar, CVoxel* cimg, cv::Point3i stpt, cv::Point3i enpt, float cMean, float cVar);
float ComputeSubRegionNCC(CVoxel* img, cv::Point3i istpt, float iMean, float iVar, CVoxel* cimg, cv::Point3i cstpt, cv::Point3i size, float cMean, float cVar);
float ComputeWeightedSubRegionNCC(CVoxel* img, CVoxel* cimg, cv::Point3i stpt, cv::Point3i enpt, float cMean, float cVar, CFVoxel* w);
float ComputeWeightedSubRegionNCC(CVoxel* img, CVoxel* cimg, cv::Point3i stpt, cv::Point3i enpt, float cMean, float cVar, CVoxel* roi, CFVoxel* w);
float ComputeSubRegionOverlayCount(CFVoxel* prob, cv::Point3i stpt, cv::Point3i enpt, CVoxel* uimg, cv::Point3i uStpt);
float ComputeSubRegionOverlayCount2(CFVoxel* prob, cv::Point3i stpt, int userInputType, CVoxel* cimg, CVoxel* uimg, cv::Point3i uStpt, cv::Point3i size);
float ComputeGradientSum(CVoxel* gradientMap, CFVoxel* probPat);

template <typename T>
void InitializeArray(T* arr, int size) {
	for ( int i = 0; i < size; i++ ) {
		arr[i]=0;
	}
}

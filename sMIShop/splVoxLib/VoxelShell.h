#pragma once

#include "Voxel.h"

class CVoxelShell 
{
public:
	cv::Point3i *pts;
	int npts;

	CVoxelShell();
	~CVoxelShell();
	// extract shell from CVoxel object
	template <typename T>
	CVoxelShell(CVoxel<T> *vxl, bool b = TRUE);
	template <typename T>
	CVoxelShell(CVoxel<T> *vxl, int BT, bool b=true);
	// extract shell from CVoxel object only inside ROI
	template <typename T>
	CVoxelShell(CVoxel<T> *vxl, cv::Point3i &roiStart, cv::Point3i &roiEnd );
	CVoxelShell(cvPoint3iArray &ptArr);
	CVoxelShell(CString str);
	CVoxelShell(int *pData, int nx, int ny, int nz);
	template <typename T>
	void MakeShell(CVoxel<T> *vxl);
	template <typename T>
	void MakeShell(CVoxel<T> *vxl, int BT);
	void Shift(int x, int y, int z);
	CVoxel<BYTE> *Voxellize(int nx, int ny, int nz);
	void StratifiedSampling(cv::Point3i ptStrat=cv::Point3i(3,3,2));
	void StratifiedSampling(cv::Point3i ptStrat, BOOL sampleMethod);
	void RemoveAt(int n);
	void Save(CString str);
	void Load(CString str);
	void SaveAsCVoxel(CString str, cv::Point3i range, int nX, int nY, int nZ);

	//shpark
	void Transform(vtkMatrix4x4 *T, CVoxelShell **transVxl);
	void Transform(vtkMatrix4x4 *T);
	CVoxelShell* Copy();
	CVoxelShell* UpSampleBCIResult();
	void NearPointRemoveSampling( cv::Point3i range, int nX, int nY, int nZ );
	void Set(cvPoint3iArray &ptArr);

	CVoxelShell* ClassifyBCI( CVoxelShell* tBCI, CVoxelShell*tBOI );
	template <typename T>
	void GetBCIBOI(CVoxel<T>* carti, CVoxelShell** bci, CVoxelShell** boi);
};

class CPointProb
{
public:
	cv::Point3i pt;
	double prob;

	CPointProb():pt(0,0,0), prob(0) {}
	~CPointProb() {}
	CPointProb& operator=( CPointProb const& a )
	{
		pt = a.pt;
		prob = a.prob;
		return *this;
	}
	bool operator ==(const CPointProb& a) const {
		if ( (pt == a.pt) && (prob == a.prob) )
			return true;
		else
			return false;
	}
};
typedef CArray<CPointProb	> CPtProbArray;
typedef CTypedPtrArray<CPtrArray, CPointProb*> CPtProbPtrArray;

class CSurfacePointVoxel
{
public:
	cv::Point3i m_pos;
	cv::Point3i m_size;
	cvPoint3iArray m_sfPtArr;

	CSurfacePointVoxel() : m_pos(0,0,0), m_size(0,0,0) {}
	CSurfacePointVoxel(cv::Point3i &pos, cv::Point3i &size) : m_pos(pos), m_size(size) {}
	~CSurfacePointVoxel() {}
	cv::Point3i UniformSampleSurfacePoint();
	cv::Point3i SampleMedianSurfacePoint();
	BOOL SubdivideVoxel( CTypedPtrArray<CPtrArray, CSurfacePointVoxel*> &sfPtArr );
};

typedef CTypedPtrArray<CPtrArray, CSurfacePointVoxel*> CSfPtPtrArray;
typedef CTypedPtrArray<CPtrArray, CVoxelShell*> CVoxelShellPtrArray;

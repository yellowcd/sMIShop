// Histogram.h: interface for the CBilayerHistogram class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Util.h"

enum {FGHIST = 1, BGHIST = 0};
class cv::Point3i;

class CHistogram
{
public:
	double *m_hist;
	int m_nHistBin;
	float m_iMin;
	float m_iMax;

	CHistogram( );
	CHistogram(int n);
	CHistogram(short imin, short imax, int n);
	CHistogram(CVoxel *vxl);
	CHistogram(CVoxel *vxl, CVoxel *roi);
	CHistogram(CString str);
	CHistogram(CTypedPtrArray<CPtrArray, CHistogram*> &histArr);

	~CHistogram( );
	void		InitHistogram( );
	void		ClearHistogram( );
	
	//void		ComputeGradHistogram(CVoxel *pat, CVoxel *lbl, int fg, CVoxel *exc, cv::Point3i pPt, int nSmoothHist=2);
	void		ComputeFGGradHistogram(CVoxel *pat, CVoxel *lbl, int fg, CVoxel *exc, cv::Point3i pPt, int nSmoothHist=2);
	void		ComputeBGGradHistogram(CVoxel *pat, CVoxel *lbl, int fg, CVoxel *exc, cv::Point3i pPt, int nSmoothHist=2);

	void		ComputeBdHistogram(CVoxel *pat, CVoxelShell *bd, cv::Point3i pPt, int nSmoothHist=2);
	void		ComputeHistogram( CVoxel *mr, int nHistBin=-1, int nSmoothHist = 2 );
	void		ComputeHistogram( double *arr, int N, int nSmoothHist, double *w=NULL );
	void		ComputeHistogram( CVoxel *mr, cv::Point3i &stpt, cv::Point3i &edpt , int nSmoothHist = 2);
	void		ComputeFixedLengthHistogram( CVoxel *mr, int nHistBin=HIST_BIN, int iMin=I_MIN, int iMax=I_MAX, int nSmoothHist = 2 );
	void		ComputeFixedLengthHistogram( CVoxel *mr, cv::Point3i &stpt, cv::Point3i &edpt, 
															   int nHistBin=HIST_BIN, int iMin=I_MIN, int iMax=I_MAX, int nSmoothHist = 2 );
	void		ComputeFixedLengthHistogramWithROI( CVoxel *mr, CVoxel* roi, 
																int nHistBin=HIST_BIN, int iMin=I_MIN, int iMax=I_MAX, int nSmoothHist = 2 );

	void		ComputeMeanHistogram(CTypedPtrArray<CPtrArray, CHistogram*> &histArr);
	void		ComputeHistogramWithSmoothRound( double *arr, int N, int nSmoothHist, double *w );
	void		SmoothHistogramRound();
	void		SmoothHistogram();
	int			GetNumHistBin();
	void		SetNumHistBin(int n);
	void		SetIMax(short i);
	void		SetIMin(short i);
	short		GetIMax();
	short		GetIMin();
	void		Save(CString str);
	void		SaveToWofstream(std::wofstream& outstream);
	void		Load(CString str);
	void		LoadShortHist(CString str);
	double *CumulativeVector();
	double *CumulativeVectorNormBin();
	short		GetBinIValue(int n);
	void		SaveHistImage(CStringA str);
	void		SaveHistImage(CStringA str, double magnitude);
	void		SaveVariableLengthImage(CStringA str);
	

	//shpark
	void		ComputeBDGradHistogram(CVoxel *pat, CVoxel *lbl, int fg, int bg, cv::Point3i pPt, int nSmoothHist = 2);
	void		ComputeGradHistogram(CVoxel *pat, CVoxel *lbl, int fg, cv::Point3i pPt, int nSmoothHist = 2);
	double		ComputeChiSquareDistance(CHistogram& thist);
	void		SavefloatHist(CString str);
	void		LoadfloatHist(CString str);
	void		MakeCombinePairwiseHist( CHistogram *inputHist, CHistogram *bdHist );
	inline double ComputeGradProbability(short pI, short pnI) {
		int iV = std::max(std::min((int)abs(pI-pnI), I_MAX), I_MIN);
		float intensity = ( (float)iV - m_iMin ) / ( (float)m_iMax - m_iMin );
		int binidx = (int) Round((m_nHistBin-1)*intensity);
		return m_hist[binidx];
	}
	inline double ComputeProbability(int i) {
		//short iV = std::max(std::min(abs(i), m_iMax), m_iMin);
		float iV = std::max(std::min((float)i, m_iMax), m_iMin);
		//int iV = i;
		float intensity = ( iV - m_iMin ) / ( m_iMax - m_iMin );
		//int binidx = (int) Round((m_nHistBin-1)*intensity);
		int binidx = (int)( (float)( m_nHistBin - 1 ) * intensity + 0.5f ) ;
		return m_hist[binidx];
	}
};

class CBilayerHistogram  
{
public:
	double *m_hist[2] ;
	int m_nHistBin;
	float m_iMin;
	float m_iMax;

	CBilayerHistogram( );
	CBilayerHistogram(int n);
	CBilayerHistogram(CString str);

	~CBilayerHistogram( );
	void		InitHistogram( );
	void		ClearHistogram( );
	CBilayerHistogram*		Copy( );
	bool		ComputeHistogram( CVoxel *pVoxel, CVoxel *pVMarkings, int iCropX0, int iCropY0, int iCropZ0, BOOL bGc3D, int iZ, BYTE byColor );
	bool		ComputeHistogram( CVoxel *mr, CVoxel *st );
	bool		ComputeHistogram( CVoxel *mr, CVoxel *st, int fg, int nSmooth=2 );
	bool		ComputeHistogram( CVoxel *mr, CVoxel *st, cv::Point3i &stpt, cv::Point3i &edpt, int fg, int bg, int nSmooth=2 );
	
	bool		ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, int fg, int bg, int nSmooth );
	bool		ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, int fg, int nSmooth=2 );
	bool		ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, cv::Point3i &stpt, cv::Point3i &edpt, int fg, int nSmooth =2 );
	bool		ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, cv::Point3i &stpt, cv::Point3i &edpt, int fg, int bg, int nSmooth );

	bool		ComputeFixedLengthHistogram( CVoxel *mr, CFVoxel *st, CVoxel *exc, int fg, int nSmooth );
	bool		ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, CVoxel *exc, int fg, int bg, int nSmooth );
	bool		ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, CVoxel *exc, cv::Point3i &stpt, cv::Point3i &edpt, int fg, int bg, int nSmooth );
	bool		ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, CVoxel *exc, cv::Point3i &stpt, cv::Point3i &edpt, int fg, int nSmooth );
	bool		ComputeFixedLengthHistogram( CVoxel *mr, CVoxel *st, CVoxel *exc, int fg, int nSmooth );

	bool		ComputeFixedLengthHistogramWithROI( CVoxel *mr, CVoxel *st, CVoxel *roi, cv::Point3i &stpt, cv::Point3i &edpt, int fg, int nSmooth );

	void		ComputeMeanHistogram(CTypedPtrArray<CPtrArray, CBilayerHistogram*> &histArr);
	void		Save(CString str);
	void		SaveToWofstream(std::wofstream& outstream);
	void		Load(CString str);
	void		SaveHistImage(CString str);
	
	void		SaveFHistImage(CStringA str);
	void		SaveBHistImage(CStringA str);
	void		SmoothHistogram();
	int			GetNumHistBin();
	void		SetNumHistBin(int n);
	void		SetIMax(short i);
	void		SetIMin(short i);
	short		GetIMax();
	short		GetIMin();
	void		ComputeVUnaries(CVoxel *v, CFVoxel *fgU, CFVoxel  *bgU);
	void		ComputeVProbabilities(CVoxel *v, CFVoxel *fgU, CFVoxel  *bgU);
	inline void ComputeFUnary(short i, float &fu, float &bu)	{
		float nbin = (float)(m_nHistBin)-1.0;
		float intensity, normint, denom;
		intensity = std::min( std::max((float)i, 0.f), m_iMax );
		normint = (intensity-m_iMin)/(m_iMax-m_iMin);
		int nb = (int)Round(nbin*normint);
		denom = m_hist[BGHIST][nb]+m_hist[FGHIST][nb];
		bu = ( m_hist[BGHIST][nb] == 0 ) ? MAX_INF : -(float)log( m_hist[BGHIST][nb]/denom );
		fu = ( m_hist[FGHIST][nb] == 0 ) ? MAX_INF : -(float)log( m_hist[FGHIST][nb]/denom );
		bu = std::min(bu, (float)MAX_INF);
		fu = std::min(fu, (float)MAX_INF);
	}
	inline void ComputeFProbability(short i, float &fu, float &bu)	{
		float nbin = (float)(m_nHistBin)-1.0;
		float intensity, normint, denom;
		intensity = std::min( std::max((float)i, 0.f), m_iMax );
		normint = (intensity-m_iMin)/(m_iMax-m_iMin);
		int nb = (int)Round(nbin*normint);
		denom = m_hist[BGHIST][nb]+m_hist[FGHIST][nb];
		if ( denom > 0 ) {
			fu = m_hist[FGHIST][nb]/denom;
			bu = 1-fu;
		}
		else {
			fu = 0.5;
			bu = 0.5;
		}
	}
};

typedef CTypedPtrArray<CPtrArray, CBilayerHistogram*> CBiHistPtrArray;

CBilayerHistogram *MeanHistogram(CBilayerHistogram *h1, CBilayerHistogram *h2);
CBilayerHistogram *CombineHistogram(CBilayerHistogram *h1, CBilayerHistogram *h2);
CBilayerHistogram *AggregateFixedLengthHistogram(CBilayerHistogram *h1, CBilayerHistogram *h2);


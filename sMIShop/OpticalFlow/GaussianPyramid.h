#ifndef _GaussianPyramid_h
#define _GaussianPyramid_h

#include "Image.h"

class GaussianPyramid
{
private:
	DImage* ImPyramid;
	int nLevels;
public:
	GaussianPyramid(void);
	~GaussianPyramid(void);
	void ConstructPyramid(const DImage& image,double ratio=0.8,int minWidth=30);
	void ConstructPyramidLevels(const DImage& image,double ratio =0.8,int _nLevels = 2);
	void displayTop(const char* filename);
	inline int nlevels() const {return nLevels;};
	inline DImage& Image(int index) {return ImPyramid[index];};
};

DImage MakeColorWheel();
void ViewColorWheel(DImage *colorwheel);
void OptFlowVec2Color(DImage *colorwheel, double pvx, double pvy, double &r, double &g, double &b);
DImage OptFlowVec2Color_DImage(DImage *vx, DImage *vy);
DImage OptFlowVecNorm2DImage(DImage *vx, DImage *vy, double norm_max=-1);
cv::Mat DImage2cvMat(DImage *dimg);
#endif
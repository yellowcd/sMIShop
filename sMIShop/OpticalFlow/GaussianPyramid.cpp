#include "stdafx.h"

#include "GaussianPyramid.h"
#include "math.h"

GaussianPyramid::GaussianPyramid(void)
{
	ImPyramid=NULL;
}

GaussianPyramid::~GaussianPyramid(void)
{
	if(ImPyramid!=NULL)
		delete []ImPyramid;
}

//---------------------------------------------------------------------------------------
// function to construct the pyramid
// this is the slow way
//---------------------------------------------------------------------------------------
/*void GaussianPyramid::ConstructPyramid(const DImage &image, double ratio, int minWidth)
{
	// the ratio cannot be arbitrary numbers
	if(ratio>0.98 || ratio<0.4)
		ratio=0.75;
	// first decide how many levels
	nLevels=log((double)minWidth/image.width())/log(ratio);
	if(ImPyramid!=NULL)
		delete []ImPyramid;
	ImPyramid=new DImage[nLevels];
	ImPyramid[0].copyData(image);
	double baseSigma=(1/ratio-1);
	for(int i=1;i<nLevels;i++)
	{
		DImage foo;
		double sigma=baseSigma*i;
		image.GaussianSmoothing(foo,sigma,sigma*2.5);
		foo.imresize(ImPyramid[i],pow(ratio,i));
	}
}//*/

//---------------------------------------------------------------------------------------
// function to construct the pyramid
// this is the fast way
//---------------------------------------------------------------------------------------
void GaussianPyramid::ConstructPyramid(const DImage &image, double ratio, int minWidth)
{
	// the ratio cannot be arbitrary numbers
	if(ratio>0.98 || ratio<0.4)
		ratio=0.75;
	// first decide how many levels
	nLevels=log((double)minWidth/image.width())/log(ratio);
	if(ImPyramid!=NULL)
		delete []ImPyramid;
	ImPyramid=new DImage[nLevels];
	ImPyramid[0].copyData(image);
	double baseSigma=(1/ratio-1);
	int n=log(0.25)/log(ratio);
	double nSigma=baseSigma*n;
	for(int i=1;i<nLevels;i++)
	{
		DImage foo;
		if(i<=n)
		{
			double sigma=baseSigma*i;
			image.GaussianSmoothing(foo,sigma,sigma*3);
			foo.imresize(ImPyramid[i],pow(ratio,i));
		}
		else
		{
			ImPyramid[i-n].GaussianSmoothing(foo,nSigma,nSigma*3);
			double rate=(double)pow(ratio,i)*image.width()/foo.width();
			foo.imresize(ImPyramid[i],rate);
		}
	}
}

void GaussianPyramid::ConstructPyramidLevels(const DImage &image, double ratio, int _nLevels)
{
	// the ratio cannot be arbitrary numbers
	if(ratio>0.98 || ratio<0.4)
		ratio=0.75;
	nLevels = _nLevels;
	if(ImPyramid!=NULL)
		delete []ImPyramid;
	ImPyramid=new DImage[nLevels];
	ImPyramid[0].copyData(image);
	double baseSigma=(1/ratio-1);
	int n=log(0.25)/log(ratio);
	double nSigma=baseSigma*n;
	for(int i=1;i<nLevels;i++)
	{
		DImage foo;
		if(i<=n)
		{
			double sigma=baseSigma*i;
			image.GaussianSmoothing(foo,sigma,sigma*3);
			foo.imresize(ImPyramid[i],pow(ratio,i));
		}
		else
		{
			ImPyramid[i-n].GaussianSmoothing(foo,nSigma,nSigma*3);
			double rate=(double)pow(ratio,i)*image.width()/foo.width();
			foo.imresize(ImPyramid[i],rate);
		}
	}
}

void GaussianPyramid::displayTop(const char *filename)
{
	ImPyramid[nLevels-1].imwrite(filename);
}



#define UNKNOWN_FLOW_THRESH	1e9
#define UNKNOWN_FLOW		1e10 
#define OF_EPS				1e-1 

DImage MakeColorWheel()
{
	int RY = 15;
	int YG = 6;
	int GC = 4;
	int CB = 11;
	int BM = 13;
	int MR = 6;
	int ncols = RY + YG + GC + CB + BM + MR;
	DImage colorwheel(ncols, 3, 1);
	for ( int j = 0; j < 3; j++ ) {
		for ( int i = 0; i < ncols; i++ ) {
			colorwheel.pData[j*ncols+i] = 0;
		}
	}

	int col = 0;
	// RY
	for ( int i = 0; i < RY; i++ ) {
		colorwheel.pData[0*ncols+i] = 255;
		colorwheel.pData[1*ncols+i] = floor(255.0*(double)i/(double)RY);
	}
	col = col+RY;

	// YG
	for ( int i = 0; i < YG; i++ ) {
	//colorwheel(col+(1:YG), 1) = 255 - floor(255*(0:YG-1)/YG)';
	//colorwheel(col+(1:YG), 2) = 255;
		colorwheel.pData[0*ncols+col+i] = 255 - floor(255.0*(double)i/(double)YG);
		colorwheel.pData[1*ncols+col+i] = 255;
	}
	col = col+YG;

	// GC
	for ( int i = 0; i < GC; i++ ) {
	//colorwheel(col+(1:GC), 2) = 255;
	//colorwheel(col+(1:GC), 3) = floor(255*(0:GC-1)/GC)';
		colorwheel.pData[1*ncols+col+i] = 255;
		colorwheel.pData[2*ncols+col+i] = floor(255.0*(double)i/(double)GC);
	}
	col = col+GC;

	// CB
	for ( int i = 0; i < CB; i++ ) {
	//colorwheel(col+(1:CB), 2) = 255 - floor(255*(0:CB-1)/CB)';
	//colorwheel(col+(1:CB), 3) = 255;
		colorwheel.pData[1*ncols+col+i] = 255 - floor(255.0*(double)i/(double)CB);
		colorwheel.pData[2*ncols+col+i] = 255;
	}
	col = col+CB;

	// BM
	for ( int i = 0; i < BM; i++ ) {
	//colorwheel(col+(1:BM), 3) = 255;
	//colorwheel(col+(1:BM), 1) = floor(255*(0:BM-1)/BM)';
		colorwheel.pData[2*ncols+col+i] = 255;
		colorwheel.pData[0*ncols+col+i] = floor(255.0*(double)i/(double)BM);
	}
	col = col+BM;

	// MR
	for ( int i = 0; i < MR; i++ ) {
	//colorwheel(col+(1:MR), 3) = 255 - floor(255*(0:MR-1)/MR)';
	//colorwheel(col+(1:MR), 1) = 255;
		colorwheel.pData[2*ncols+col+i] = 255 - floor(255.0*(double)i/(double)MR);
		colorwheel.pData[0*ncols+col+i] = 255;
	}
	return colorwheel;
}

void ViewColorWheel(DImage *colorwheel)
{
	int nw = colorwheel->width();
	int nh = 3;
	int sqsz = 50;
	int w = nw*sqsz;
	int h = sqsz;
	DImage vcw(w,h,3);
	int wstep = vcw.width();
	for ( int x = 0; x < nw; x++ ) {
		for ( int j = 0; j < sqsz; j++ ) {
			for ( int i = 0; i < sqsz; i++ ) {
				vcw.pData[(j*wstep+(x*sqsz+i))*3  ] = colorwheel->pData[0*nw+x]/255.0;
				vcw.pData[(j*wstep+(x*sqsz+i))*3+1] = colorwheel->pData[1*nw+x]/255.0;
				vcw.pData[(j*wstep+(x*sqsz+i))*3+2] = colorwheel->pData[2*nw+x]/255.0;
			}
		}
	}
	vcw.imwrite("vcw.png");
}

void OptFlowVec2Color(DImage *colorwheel, double pvx, double pvy, double &r, double &g, double &b)
{
	int ncols = colorwheel->width();
	double rad = sqrt(pvx*pvx+pvy*pvy);
	double a = atan2(-pvy,-pvx)/PI;
	double fk = (a+1)/2*(ncols-1);
	int k0 = (int)floor(fk);
	int k1 = k0+1;
	k1 = (k1==ncols) ? 1 : k1;
	double f = fk-k0;

	double colr0 = colorwheel->pData[0*ncols+k0]/255.0;
	double colr1 = colorwheel->pData[0*ncols+k1]/255.0;
	double colg0 = colorwheel->pData[1*ncols+k0]/255.0;
	double colg1 = colorwheel->pData[1*ncols+k1]/255.0;
	double colb0 = colorwheel->pData[2*ncols+k0]/255.0;
	double colb1 = colorwheel->pData[2*ncols+k1]/255.0;

	r = (1-f)*colr0 + f*colr1;
	g = (1-f)*colg0 + f*colg1;
	b = (1-f)*colb0 + f*colb1;

#if 1
	if ( rad <= 1 ) {
		r = 1-rad*(1-r);
		g = 1-rad*(1-g);
		b = 1-rad*(1-b);
	}
	else {
		r *= 0.75;
		g *= 0.75;
		b *= 0.75;
	}
#endif
}

DImage OptFlowVec2Color_DImage(DImage *vx, DImage *vy)
{
	int w = vx->width();
	int h = vx->height();
	double max_vnorm = -1;
	for ( int y = 0; y < h; y++ ) {
		for ( int x = 0; x < w; x++ ) {
			double pvx = vx->pData[x+y*w];
			double pvy = vy->pData[x+y*w];
			double vn = sqrt(pvx*pvx+pvy*pvy);
			if ( vn > max_vnorm )
				max_vnorm = vn;
		}
	}
	max_vnorm += OF_EPS;
	double max_vnorm_inv = 1.0/max_vnorm;
	DImage cimg(w, h, 3);
	
	DImage colorwheel = MakeColorWheel();

#if 0
	ViewColorWheel(&colorwheel);
#endif

	for ( int y = 0; y < h; y++ ) {
		for ( int x = 0; x < w; x++ ) {
			double r, g, b;
			double pvx = vx->pData[x+y*w]*max_vnorm_inv;
			double pvy = vy->pData[x+y*w]*max_vnorm_inv;
			OptFlowVec2Color(&colorwheel, pvx, pvy, r, g, b);
			cimg.pData[(y*w+x)*3+0] = r;
			cimg.pData[(y*w+x)*3+1] = g;
			cimg.pData[(y*w+x)*3+2] = b;
		}
	}
	return cimg;
}

DImage OptFlowVecNorm2DImage(DImage *vx, DImage *vy, double norm_max)
{
	int w = vx->width();
	int h = vx->height();
	DImage nimg(w, h, 1);
	double max_vn = -FLT_MAX;
	for ( int y = 0; y < h; y++ ) {
		for ( int x = 0; x < w; x++ ) {
			int os = x+y*w;
			double pvx = vx->pData[os];
			double pvy = vy->pData[os];
			double vn = sqrt(pvx*pvx+pvy*pvy);
			if ( vn > max_vn )
				max_vn = vn;
			nimg.pData[os] = vn;
		}
	}
	if ( norm_max < 0 ) {
		norm_max = max_vn;
	}
	double nm = 1.0/norm_max;
	for ( int i = 0; i < h*w; i++ ) {
		nimg.pData[i] *= nm;
	}
	return nimg;
}

cv::Mat DImage2cvMat(DImage *dimg)
{
	int w = dimg->width();
	int h = dimg->height();
	int nc = dimg->nchannels();
	cv::Mat cvimg(h, w, CV_MAKETYPE(cv::DataType<double>::type,nc));
	for ( int y = 0; y < h; y++ ) {
		uchar *ptr = cvimg.ptr(y);
		for ( int x = 0; x < w; x++ ) {
			for ( int c = 0; c < nc; c++ ) {
				ptr[x*nc+c] = dimg->pData[(x + y*w)*nc+c];
			}
		}
	}
	return cvimg;
}

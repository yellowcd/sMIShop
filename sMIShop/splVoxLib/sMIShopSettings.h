#pragma once

#include <queue>
#include <deque>
#include <vector>
#include <functional>
#include <fstream>
#include <direct.h>
//#include "sparse_cs.h"

//#include "math.h"
#include <omp.h> 
//#include <cuda_runtime_api.h>

// use opencv
#include "opencv2/opencv.hpp"
#include "vtkSettings.h"

#define HYPOT(x,y)		sqrt((double)(x)*(x)+(y)*(y))

typedef CTypedPtrArray<CPtrArray, double *> DoublePtrArray;
typedef CTypedPtrArray<CPtrArray, float*> floatPtrArray;
typedef CArray<cv::Point3i> cvPoint3iArray;

typedef CArray<float> CFloatArray;
typedef CArray<int> CIntArray;
typedef CArray<CPoint> CPointArray;
typedef float gtype; //working type, can be int, double or integer
//typedef double gtype;
//typedef int gtype;

enum { AXIS_X = 0, AXIS_Y = 1, AXIS_Z = 2 };

enum { FG = 255, BG = 0, UNK = 128};

#define _u 100
#define SI_U 8
#define I_U 30
#define _inf 100000

#define BYTE_SEGMENT01	BYTE_GREEN
#define LABEL_BASE 1000

#define DSC_RAW_THRESHOLD_4_MRI		0
#define DSC_RAW_THRESHOLD_4_CT		-1001

#define BCIBOI_DISTMAP_SIZE 0.3
#define MAX_INF 1000

#define FRAME_X 1400 // 1200 // 1400 // 1550
#define FRAME_Y 1050 // 1000 // 1050
#define CHILD_FRAME_X 1400 // 1200 // 1400 // 1520  // 1050 for knee
#define CHILD_FRAME_Y 1050 // 1000 // 1050

#define BYTE_RED		(BYTE)1
#define BYTE_GREEN		(BYTE)2
#define BYTE_BLUE		(BYTE)3
#define BYTE_REDGREEN	(BYTE)4
#define BYTE_GREENBLUE	(BYTE)5
#define BYTE_BLUERED	(BYTE)6
#define BYTE_RED2GREEN	(BYTE)7
#define BYTE_GREEN2BLUE	(BYTE)8
#define BYTE_BLUE2RED	(BYTE)9
#define BYTE_REDGREEN2	(BYTE)10
#define BYTE_GREENBLUE2	(BYTE)11
#define BYTE_BLUERED2	(BYTE)12

#define BYTE_ERASE		(BYTE)0x00
#define BYTE_BG			BYTE_BLUE
#define BYTE_FGF		BYTE_GREEN
#define BYTE_FGT		BYTE_RED
#define BYTE_FG			BYTE_RED

#define BYTE_BONE		BYTE_FG
//#define BYTE_BONE		BYTE_REDGREEN
#define BYTE_BONE_BG	BYTE_BG
//#define BYTE_BONE_BG	BYTE_GREENBLUE
#define BYTE_WHITE		(BYTE)0xff

enum boneType {FEMUR = 4, PATEL = 6, TIBIA = 5, COMB = 3};

#ifdef _DEBUG
	#pragma comment(lib,"opencv_core249d.lib")
	#pragma comment(lib,"opencv_highgui249d.lib")
	#pragma comment(lib,"opencv_imgproc249d.lib")
	#pragma comment(lib,"opencv_features2d249d.lib")
	#pragma comment(lib,"opencv_video249d.lib")
#else
	#pragma comment(lib,"opencv_core249.lib")
	#pragma comment(lib,"opencv_highgui249.lib")
	#pragma comment(lib,"opencv_imgproc249.lib")
	#pragma comment(lib,"opencv_features2d249.lib")
	#pragma comment(lib,"opencv_video249.lib")
#endif

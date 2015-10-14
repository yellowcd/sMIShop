#pragma once

#include "vtkImageData.h"
#include "vtkImageReader.h"
#include "vtkDICOMImageReader.h"
#include "vtkSmartPointer.h"
#include "vtkMatrix4x4.h"

#include "vtkDICOMMetaData.h"
#include "vtkDICOMSorter.h"
#include "vtkDICOMReader.h"

#define VTK_CREATE( type, var ) vtkSmartPointer< type > var = vtkSmartPointer< type >::New()

/*f
#include "vtkVolumeRayCastMapper.h"
#include "vtkImageShiftScale.h"

//VolumeRendering
#include "vtkVolumeProperty.h"
#include "vtkColorTransferFunction.h"
#include "vtkPointData.h"

//RayCast Function
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeRayCastMIPFunction.h"

//SurfaceRendering
#include "vtkImageShrink3D.h"
#include "vtkContourFilter.h"
#include "vtkPolyDataMapper.h"

//Mask
#include "vtkLookUpTable.h"
#include "vtkProperty.h"
#include "vtkStripper.h"
#include "vtkProbeFilter.h"
#include "vtkImageMapToColors.h"

//Outline
#include "vtkOutlineFilter.h"

//Actors
#include "vtkActor.h"
#include "vtkActor2D.h"

//Text
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"

//Renderer
#include "vtkRenderer.h"
#include "vtkWin32OpenGLRenderWindow.h"
#include "vtkWin32RenderWindowInteractor.h"

// 추가 설정 for Raw Image Reader
// vtkImageData
#include "vtkShortArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkPiecewiseFunction.h"

//연골 두께 측정
#include "vtkFloatArray.h"
#include "vtkMarchingCubes.h"
#include "vtkPolyData.h"
#include "vtkPolyDataNormals.h"
#include "vtkImageDataGeometryFilter.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkScalarBarActor.h"

// 2차원 이미지 디스플레이
#include "vtkImageReslice.h"
#include "vtkImageActor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkCamera.h"
#include "vtkImageMapper.h"
#include "vtkImageViewer2.h"

//미토콘드리아 노이즈 제거
#include "vtkImageMedian3D.h"
#include "vtkImageGaussianSmooth.h"

// 레이블링
// Reimplement::::On Going
#include "vtkInteractorStyleUser.h"
#include "vtkCellPicker.h"

#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"

// ICP
#include "vtkIterativeClosestPointTransform.h"
#include "vtkTransformFilter.h"
#include "vtkLandmarkTransform.h"
//#include "vtkDataSetToPolyDataFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkDataArray.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkImageTranslateExtent.h"
#include "vtkMatrixToLinearTransform.h"
#include "vtkAbstractTransform.h"
#include "vtkCellLocator.h"

// 3D ASM
#include "vtkProcrustesAlignmentFilter.h"
#include "vtkPCAAnalysisFilter.h"
#include "vtkDelaunay3D.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCleanPolyData.h"
#include "vtkXMLUnstructuredGridWriter.h"
#include "vtkXMLUnstructuredGridReader.h"
#include "vtkGeometryFilter.h"
#include "vtkDoubleArray.h"

#include "vtkXMLImageDataReader.h"
#include "vtkXMLImageDataWriter.h"

#include "vtkDecimatePro.h"
*/

#pragma comment(lib, "vtksys-6.1.lib")
#pragma comment(lib, "vtkalglib-6.1.lib")
#pragma comment(lib, "vtkCommonCore-6.1.lib")
#pragma comment(lib, "vtkCommonDataModel-6.1.lib")
#pragma comment(lib, "vtkCommonMath-6.1.lib")
#pragma comment(lib, "vtkCommonExecutionModel-6.1.lib")
#pragma comment(lib, "vtkCommonMisc-6.1.lib")
#pragma comment(lib, "vtkCommonSystem-6.1.lib")
#pragma comment(lib, "vtkCommonTransforms-6.1.lib")
#pragma comment(lib, "vtkImagingCore-6.1.lib")
#pragma comment(lib, "vtkRenderingCore-6.1.lib")
#pragma comment(lib, "vtkDICOMParser-6.1.lib")
#pragma comment(lib, "vtkIOImage-6.1.lib")
#pragma comment(lib, "vtkIOCore-6.1.lib")
#pragma comment(lib, "vtkDICOM-6.1.lib")

/*
#pragma comment(lib, "vtkImagingHybrid-6.1.lib")
#pragma comment(lib, "vtkCommonComputationalGeometry-6.1.lib")
#pragma comment(lib,"vtkFiltersGeneral-6.1.lib")
#pragma comment(lib,"vtkFiltersSources-6.1.lib")
#pragma comment(lib,"vtkFiltersCore-6.1.lib")
#pragma comment(lib,"vtkInteractionStyle-6.1.lib")
#pragma comment(lib,"vtkFiltersExtraction-6.1.lib")
#pragma comment(lib,"vtkFiltersStatistics-6.1.lib")
#pragma comment(lib,"vtkImagingFourier-6.1.lib")
#pragma comment(lib,"vtkFiltersGeometry-6.1.lib")
#pragma comment(lib,"vtkRenderingOpenGL-6.1.lib")
#pragma comment(lib,"vtkImagingHybrid-6.1.lib")
#pragma comment(lib,"vtkzlib-6.1.lib")
#pragma comment(lib,"vtkmetaio-6.1.lib")
#pragma comment(lib,"vtkjpeg-6.1.lib")
#pragma comment(lib,"vtkpng-6.1.lib")
#pragma comment(lib,"vtktiff-6.1.lib")
#pragma comment(lib,"vtktiff-6.1.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
/**/
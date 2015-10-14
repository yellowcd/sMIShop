// F3DPoint.cpp: implementation of the CF3DPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "F3DPoint.h"
#include "math.h"
#include "VoxelShell.h"
#include "Util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DPoint ZERO_3DPT(0,0,0);

CF3DPoint::CF3DPoint()
{
	m_fX = 0;
	m_fY = 0; 
	m_fZ = 0;
}

CF3DPoint::CF3DPoint(float fX, float fY, float fZ)
{
	m_fX = fX;
	m_fY = fY; 
	m_fZ = fZ;
}

CF3DPoint::CF3DPoint( const CF3DPoint& pt )
{
	m_fX = pt.m_fX;
	m_fY = pt.m_fY; 
	m_fZ = pt.m_fZ;	
}

CF3DPoint::CF3DPoint( const C3DPoint& pt )
{
	m_fX = pt.m_x;
	m_fY = pt.m_y; 
	m_fZ = pt.m_z;	
}

CF3DPoint::~CF3DPoint()
{

}

void CF3DPoint::MakeUnitVector( )
{
	float fRad ;

	fRad = (float)sqrt( m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ ) ;
	if( fRad != 0.0f )
	{
		m_fX = m_fX / fRad ;
		m_fY = m_fY / fRad ;
		m_fZ = m_fZ / fRad ;
	}
}

C3DPoint CF3DPoint::Round() {
	return C3DPoint((int)cvRound(m_fX), (int)cvRound(m_fY), (int)cvRound(m_fZ));
}

CF3DPoint& CF3DPoint::operator=( const CF3DPoint& a )
{
	m_fX = a.m_fX;
	m_fY = a.m_fY;
	m_fZ = a.m_fZ;
	return *this;
}
CF3DPoint CF3DPoint::operator-( CF3DPoint const& a )
{
	CF3DPoint pt;
	pt.m_fX = m_fX - a.m_fX;
	pt.m_fY = m_fY - a.m_fY;
	pt.m_fZ = m_fZ - a.m_fZ;
	return pt;
}
CF3DPoint CF3DPoint::operator-( C3DPoint const& a )
{
	CF3DPoint pt;
	pt.m_fX = m_fX - a.m_x;
	pt.m_fY = m_fY - a.m_y;
	pt.m_fZ = m_fZ - a.m_z;
	return pt;
}
CF3DPoint& CF3DPoint::operator-=( CF3DPoint const& a )
{
	m_fX -= a.m_fX;
	m_fY -= a.m_fY;
	m_fZ -= a.m_fZ;		
	return *this;
}
CF3DPoint CF3DPoint::operator+( CF3DPoint const& a )
{
	CF3DPoint pt;
	pt.m_fX = m_fX + a.m_fX;
	pt.m_fY = m_fY + a.m_fY;
	pt.m_fZ = m_fZ + a.m_fZ;
	return pt;
}
CF3DPoint CF3DPoint::operator+( C3DPoint const& a )
{
	CF3DPoint pt;
	pt.m_fX = m_fX + a.m_x;
	pt.m_fY = m_fY + a.m_y;
	pt.m_fZ = m_fZ + a.m_z;
	return pt;
}
CF3DPoint& CF3DPoint::operator+=( CF3DPoint const& a )
{
	m_fX += a.m_fX;
	m_fY += a.m_fY;
	m_fZ += a.m_fZ;		
	return *this;
}

CF3DPoint CF3DPoint::operator/( double const& f )
{
	float ff = (float)f;
	CF3DPoint pt;
	pt.m_fX = m_fX /ff;
	pt.m_fY = m_fY /ff;
	pt.m_fZ = m_fZ /ff;
	return pt;
}

CF3DPoint CF3DPoint::operator/( float const& f )
{
	CF3DPoint pt;
	pt.m_fX = m_fX / f;
	pt.m_fY = m_fY /f;
	pt.m_fZ = m_fZ /f;
	return pt;
}

CF3DPoint CF3DPoint::operator*( double const& f )
{
	float ff = (float)f;

	CF3DPoint pt;
	pt.m_fX = m_fX*ff;
	pt.m_fY = m_fY*ff;
	pt.m_fZ = m_fZ*ff;
	return pt;
}

CF3DPoint CF3DPoint::operator*( float const& f )
{
	CF3DPoint pt;
	m_fX *= f;
	m_fY *= f;
	m_fZ *= f;
	return pt;
}

bool CF3DPoint::operator>(const C3DPoint& pt) const {
	if ( (m_fX > pt.m_x) && (m_fY > pt.m_y) && (m_fZ > pt.m_z) )
		return true;
	else
		return false;
}
bool CF3DPoint::operator>=(const C3DPoint& pt) const {
	if ( (m_fX >= pt.m_x) && (m_fY >= pt.m_y) && (m_fZ >= pt.m_z) )
		return true;
	else
		return false;
}
bool CF3DPoint::operator<(const C3DPoint& pt) const {
	if ( (m_fX < pt.m_x) && (m_fY < pt.m_y) && (m_fZ < pt.m_z) )
		return true;
	else
		return false;
}
bool CF3DPoint::operator<=(const C3DPoint& pt) const {
	if ( (m_fX <= pt.m_x) && (m_fY <= pt.m_y) && (m_fZ <= pt.m_z) )
		return true;
	else
		return false;
}

bool CF3DPoint::operator>(const CF3DPoint& pt) const {
	if ( (m_fX > pt.m_fX) && (m_fY > pt.m_fY) && (m_fZ > pt.m_fZ) )
		return true;
	else
		return false;
}
bool CF3DPoint::operator>=(const CF3DPoint& pt) const {
	if ( (m_fX >= pt.m_fX) && (m_fY >= pt.m_fY) && (m_fZ >= pt.m_fZ) )
		return true;
	else
		return false;
}
bool CF3DPoint::operator<(const CF3DPoint& pt) const {
	if ( (m_fX < pt.m_fX) && (m_fY < pt.m_fY) && (m_fZ < pt.m_fZ) )
		return true;
	else
		return false;
}
bool CF3DPoint::operator<=(const CF3DPoint& pt) const {
	if ( (m_fX <= pt.m_fX) && (m_fY <= pt.m_fY) && (m_fZ <= pt.m_fZ) )
		return true;
	else
		return false;
}

CLF3DPoint::CLF3DPoint()
{
	m_fX = 0;
	m_fY = 0; 
	m_fZ = 0;
}

CLF3DPoint::CLF3DPoint(double fX, double fY, double fZ)
{
	m_fX = fX;
	m_fY = fY; 
	m_fZ = fZ;
}

CLF3DPoint::CLF3DPoint( const CLF3DPoint& pt )
{
	m_fX = pt.m_fX;
	m_fY = pt.m_fY; 
	m_fZ = pt.m_fZ;	
}

CLF3DPoint::~CLF3DPoint()
{

}

void CLF3DPoint::MakeUnitVector( )
{
	double fRad ;

	fRad = (double)sqrt( m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ ) ;
	if( fRad != 0.0f )
	{
		m_fX = m_fX / fRad ;
		m_fY = m_fY / fRad ;
		m_fZ = m_fZ / fRad ;
	}
}

C3DPoint::C3DPoint()
{
	m_x = 0; 
	m_y = 0;
	m_z = 0;
}

C3DPoint::C3DPoint(int x, int y, int z)
{
	m_x = x; 
	m_y = y;
	m_z = z;
}

C3DPoint::C3DPoint( const C3DPoint& pt )
{
	m_x = pt.m_x; 
	m_y = pt.m_y;
	m_z = pt.m_z;
}

C3DPoint::~C3DPoint()
{

}

void C3DPoint::Set(int x, int y, int z)
{
	m_x=x; m_y=y; m_z=z;	
}

void C3DPoint::Set(const C3DPoint &pt)
{
	m_x=pt.m_x; m_y=pt.m_y; m_z=pt.m_z;	
}


double C3DPoint::GetNearestShellDistance( CVoxelShell* bci)
{
	int nPt = bci->npts;

	double* dist = new double[nPt];
	double minVal = FLT_MAX;
	int minIdx;
	for ( int i = 0; i < nPt; i++ ) {
		C3DPoint cpt(m_x,m_y,m_z);
		dist[i] = VectorL2Dist( cpt, bci->pts[i]);
		if ( dist[i] < minVal ) {
			minVal = dist[i];
			minIdx = i;
		}
	}

	SAFE_DELETE_ARRAY(dist);
	return minVal;
}

void C3DPoint::Transform(vtkMatrix4x4 *T, C3DPoint &trPt)
{
	vtkMatrix4x4 *imat = vtkMatrix4x4::New();
	vtkMatrix4x4::Invert(T, imat);

	double gridpt[4];
	double transpt[4];

	gridpt[0] = m_x;		gridpt[1] = m_y;
	gridpt[2] = m_z;		gridpt[3] = 1;
	//imat->MultiplyPoint(gridpt, transpt);
	imat->MultiplyPoint(gridpt, transpt);
	trPt.m_x = (int) cvRound(transpt[0]/transpt[3]);
	trPt.m_y = (int) cvRound(transpt[1]/transpt[3]);
	trPt.m_z = (int) cvRound(transpt[2]/transpt[3]);

	imat->Delete();
}

void C3DPoint::Transform(vtkMatrix4x4 *T)
{
	vtkMatrix4x4 *imat = vtkMatrix4x4::New();
	vtkMatrix4x4::Invert(T, imat);

	double gridpt[4];
	double transpt[4];
	C3DPoint trPt;

	gridpt[0] = m_x;		gridpt[1] = m_y;
	gridpt[2] = m_z;		gridpt[3] = 1;
	//imat->MultiplyPoint(gridpt, transpt);
	imat->MultiplyPoint(gridpt, transpt);
	trPt.m_x = (int) cvRound(transpt[0]/transpt[3]);
	trPt.m_y = (int) cvRound(transpt[1]/transpt[3]);
	trPt.m_z = (int) cvRound(transpt[2]/transpt[3]);

	m_x = trPt.m_x;
	m_y = trPt.m_y;
	m_z = trPt.m_z;

	imat->Delete();

}


void CF3DPoint::Transform(vtkMatrix4x4 *T, CF3DPoint &trPt)
{
	vtkMatrix4x4 *imat = vtkMatrix4x4::New();
	vtkMatrix4x4::Invert(T, imat);

	double gridpt[4];
	double transpt[4];

	gridpt[0] = m_fX;		gridpt[1] = m_fY;
	gridpt[2] = m_fZ;		gridpt[3] = 1;
	//imat->MultiplyPoint(gridpt, transpt);
	imat->MultiplyPoint(gridpt, transpt);
	trPt.m_fX = transpt[0]/transpt[3];
	trPt.m_fY = transpt[1]/transpt[3];
	trPt.m_fZ = transpt[2]/transpt[3];
	imat->Delete();
}

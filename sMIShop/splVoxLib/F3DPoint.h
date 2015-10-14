// F3DPoint.h: interface for the CF3DPoint class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "sMIShopSettings.h"

class CVoxelShell;
class C3DPoint;

class CF3DPoint  
{
public:
	float m_fX ;
	float m_fY ;
	float m_fZ ;

	CF3DPoint();
	CF3DPoint( float fX, float fY, float fZ );
	CF3DPoint( const CF3DPoint& pt );
	CF3DPoint( const C3DPoint& pt );
	virtual ~CF3DPoint();
	CF3DPoint& operator=( const CF3DPoint& a );
	CF3DPoint operator-( CF3DPoint const& a );
	CF3DPoint operator-( C3DPoint const& a );
	CF3DPoint& operator-=( CF3DPoint const& a );
	CF3DPoint operator+( CF3DPoint const& a );
	CF3DPoint operator+( C3DPoint const& a );
	CF3DPoint& operator+=( CF3DPoint const& a );
	CF3DPoint operator/( double const& f );
	CF3DPoint operator/( float const& f );
	CF3DPoint operator*( double const& f );
	CF3DPoint operator*( float const& f );
	bool operator >(const C3DPoint& pt) const;
	bool operator >=(const C3DPoint& pt) const;
	bool operator <(const C3DPoint& pt) const;
	bool operator <=(const C3DPoint& pt) const;
	bool operator >(const CF3DPoint& pt) const;
	bool operator >=(const CF3DPoint& pt) const;
	bool operator <(const CF3DPoint& pt) const;
	bool operator <=(const CF3DPoint& pt) const;

	inline void Set(float x, float y, float z) {m_fX = x; m_fY = y; m_fZ = z;}
	C3DPoint Round();
	inline double Norm() {return sqrt((double)(m_fX*m_fX+m_fY*m_fY+m_fZ*m_fZ)); }
	inline int MaxDirCompAxis() {
		if ( fabs(m_fX) > fabs(m_fY) ) {
			if ( fabs(m_fX) > fabs(m_fZ) )
				return AXIS_X;
			else return AXIS_Z;
		}
		else {
			if ( fabs(m_fY) > fabs(m_fZ) )
				return AXIS_Y;
			else return AXIS_Z;
		}
	}	
	void MakeUnitVector( ) ;

	//shpark
	void Transform(vtkMatrix4x4 *T, CF3DPoint &trPt);
};

class CLF3DPoint  
{
public:
	double m_fX ;
	double m_fY ;
	double m_fZ ;

	CLF3DPoint();
	CLF3DPoint( double fX, double fY, double fZ );
	CLF3DPoint( const CLF3DPoint& pt );
	virtual ~CLF3DPoint();
	CLF3DPoint& operator=( const CLF3DPoint& a )
	{
		m_fX = a.m_fX;
		m_fY = a.m_fY;
		m_fZ = a.m_fZ;
		return *this;
	}
	CLF3DPoint& operator+( CLF3DPoint const& a )
	{
		m_fX += a.m_fX;
		m_fY += a.m_fY;
		m_fZ += a.m_fZ;		
		return *this;
	}
	CLF3DPoint& operator+=( CLF3DPoint const& a )
	{
		m_fX += a.m_fX;
		m_fY += a.m_fY;
		m_fZ += a.m_fZ;		
		return *this;
	}
	CLF3DPoint& operator-( CLF3DPoint const& a )
	{
		m_fX -= a.m_fX;
		m_fY -= a.m_fY;
		m_fZ -= a.m_fZ;		
		return *this;
	}
	CLF3DPoint& operator-=( CLF3DPoint const& a )
	{
		m_fX -= a.m_fX;
		m_fY -= a.m_fY;
		m_fZ -= a.m_fZ;		
		return *this;
	}

	inline void Set(double x, double y, double z) {m_fX = x; m_fY = y; m_fZ = z;}
	inline double Norm() {return sqrt((double)(m_fX*m_fX+m_fY*m_fY+m_fZ*m_fZ)); }
	void MakeUnitVector( ) ;
};

class C3DPoint  
{
public:
	int m_x ;
	int m_y ;
	int m_z ;

	C3DPoint();
	C3DPoint( int x, int y, int z ) ;
	C3DPoint( const C3DPoint& pt );
	virtual ~C3DPoint();
	C3DPoint& operator=( C3DPoint const& a )
	{
		m_x = a.m_x;
		m_y = a.m_y;
		m_z = a.m_z;
		return *this;
	}
	C3DPoint operator+( C3DPoint const& a )
	{
		C3DPoint pt;
		pt.m_x = m_x + a.m_x;
		pt.m_y = m_y + a.m_y;
		pt.m_z = m_z + a.m_z;
		return pt;
	}
	C3DPoint operator+( int const& a )
	{
		C3DPoint pt;
		pt.m_x = m_x + a;
		pt.m_y = m_y + a;
		pt.m_z = m_z + a;
		return pt;
	}
	C3DPoint operator-( int const& a )
	{
		C3DPoint pt;
		pt.m_x = m_x - a;
		pt.m_y = m_y - a;
		pt.m_z = m_z - a;
		return pt;
	}
	C3DPoint operator*( int const & a )
	{
		C3DPoint pt;
		pt.m_x = m_x*a;
		pt.m_y = m_y*a;
		pt.m_z = m_z*a;
		return pt;
	}
	C3DPoint operator*( float const & a )
	{
		C3DPoint pt;
		pt.m_x = (int)cvRound((float)m_x*a);
		pt.m_y = (int)cvRound((float)m_y*a);
		pt.m_z = (int)cvRound((float)m_z*a);
		return pt;
	}
	C3DPoint operator*( double const & a )
	{
		C3DPoint pt;
		pt.m_x = (int)cvRound((double)m_x*a);
		pt.m_y = (int)cvRound((double)m_y*a);
		pt.m_z = (int)cvRound((double)m_z*a);
		return pt;
	}
	C3DPoint operator/( float const & a )
	{
		C3DPoint pt;
		pt.m_x = (int)cvRound((float)m_x/a);
		pt.m_y = (int)cvRound((float)m_y/a);
		pt.m_z = (int)cvRound((float)m_z/a);
		return pt;
	}
	C3DPoint operator/( double const & a )
	{
		C3DPoint pt;
		pt.m_x = (int)cvRound((double)m_x/a);
		pt.m_y = (int)cvRound((double)m_y/a);
		pt.m_z = (int)cvRound((double)m_z/a);
		return pt;
	}
	C3DPoint& operator+=( C3DPoint const& a )
	{
		m_x += a.m_x;
		m_y += a.m_y;
		m_z += a.m_z;		
		return *this;
	}
	C3DPoint operator-( C3DPoint const& a )
	{
		C3DPoint pt;
		pt.m_x = m_x - a.m_x;
		pt.m_y = m_y - a.m_y;
		pt.m_z = m_z - a.m_z;		
		return pt;
	}
	C3DPoint& operator-=( C3DPoint const& a )
	{
		m_x -= a.m_x;
		m_y -= a.m_y;
		m_z -= a.m_z;		
		return *this;
	}
	CF3DPoint operator-( CF3DPoint const& a )
	{
		return CF3DPoint((float)m_x-a.m_fX, (float)m_y-a.m_fY, (float)m_z-a.m_fZ);
	}
	bool operator ==(const C3DPoint& pt) const {
		if ( (m_x == pt.m_x) && (m_y == pt.m_y) && (m_z == pt.m_z) )
			return true;
		else
			return false;
	}
	bool operator >(const C3DPoint& pt) const {
		if ( (m_x > pt.m_x) && (m_y > pt.m_y) && (m_z > pt.m_z) )
			return true;
		else
			return false;
	}
	bool operator >=(const C3DPoint& pt) const {
		if ( (m_x >= pt.m_x) && (m_y >= pt.m_y) && (m_z >= pt.m_z) )
			return true;
		else
			return false;
	}
	bool operator <(const C3DPoint& pt) const {
		if ( (m_x < pt.m_x) && (m_y < pt.m_y) && (m_z < pt.m_z) )
			return true;
		else
			return false;
	}
	bool operator <=(const C3DPoint& pt) const {
		if ( (m_x <= pt.m_x) && (m_y <= pt.m_y) && (m_z <= pt.m_z) )
			return true;
		else
			return false;
	}

	bool operator >(const CF3DPoint& pt) const {
		if ( (m_x > pt.m_fX) && (m_y > pt.m_fY) && (m_z > pt.m_fZ) )
			return true;
		else
			return false;
	}
	bool operator >=(const CF3DPoint& pt) const {
		if ( (m_x >= pt.m_fX) && (m_y >= pt.m_fY) && (m_z >= pt.m_fZ) )
			return true;
		else
			return false;
	}
	bool operator <(const CF3DPoint& pt) const {
		if ( (m_x < pt.m_fX) && (m_y < pt.m_fY) && (m_z < pt.m_fZ) )
			return true;
		else
			return false;
	}
	bool operator <=(const CF3DPoint& pt) const {
		if ( (m_x <= pt.m_fX) && (m_y <= pt.m_fY) && (m_z <= pt.m_fZ) )
			return true;
		else
			return false;
	}
	void Set(int x, int y, int z);
	void Set(const C3DPoint &pt);
	inline int Product() {return m_x*m_y*m_z;}
	inline double Norm() {return sqrt((double)(m_x*m_x+m_y*m_y+m_z*m_z)); }
	inline C3DPoint PerElemMult(const C3DPoint &p) {
		return C3DPoint( m_x*p.m_x, m_y*p.m_y, m_z*p.m_z );
	}
	inline C3DPoint PerElemMult(const CF3DPoint &p) {
		return C3DPoint( (int)(cvRound)((float)m_x*p.m_fX), 
								 (int)(cvRound)((float)m_y*p.m_fY),
								 (int)(cvRound)((float)m_z*p.m_fZ) );
	}
	inline CF3DPoint PerElemDiv(const C3DPoint &p) {
		return CF3DPoint( (float)m_x/(float)p.m_x, (float)m_y/(float)p.m_y, (float)m_z/(float)p.m_z );
	}
	inline CF3DPoint PerElemDiv(const CF3DPoint &p) {
		return CF3DPoint( (float)m_x/p.m_fX, (float)m_y/p.m_fY, (float)m_z/p.m_fZ );
	}

	//shpark
	void Transform(vtkMatrix4x4 *T, C3DPoint &trPt);
	void Transform(vtkMatrix4x4 *T);
	double GetNearestShellDistance( CVoxelShell* bci);
};

typedef CArray<C3DPoint> C3DPointArray;
typedef CArray<CF3DPoint> CF3DPointArray;

extern C3DPoint ZERO_3DPT;



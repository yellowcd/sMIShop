// FVoxel.cpp: implementation of the CFVoxel class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
//#include "MyNeolit.h"
#include "FVoxel.h"
#include "Voxel.h"
#include "Util.h"
//#include "Histogram.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFVoxel::CFVoxel()
{
	m_pfData = NULL ;
	m_nX = 0;
	m_nY = 0;
	m_nZ = 0;

	Allocate();
}

CFVoxel::CFVoxel(int XSize, int YSize, int ZSize, bool alloc )
{
	m_pfData = NULL ;
	m_nX = XSize;
	m_nY = YSize;
	m_nZ = ZSize;
	
	if ( alloc ) {
		Allocate();
	}
	else
		AllocateNoInitialize();
}

CFVoxel::CFVoxel(float *arr, int XSize, int YSize, int ZSize)
{
	m_pfData = NULL ;
	m_nX = XSize;
	m_nY = YSize;
	m_nZ = ZSize;

	Allocate();
	for ( int i = 0; i < m_nX*m_nY*m_nZ; i++ ) {
		m_pfData[i] = arr[i];
	}
}

CFVoxel::CFVoxel( C3DPoint pt )
{
	m_pfData = NULL ;
	m_nX = pt.m_x;
	m_nY = pt.m_y;
	m_nZ = pt.m_z;

	Allocate();
}

CFVoxel::CFVoxel(CFVoxel &Source)
{	
	int x, y, z ;

	m_pfData = NULL ;
	m_nX = Source.m_nX ;
	m_nY = Source.m_nY ;
	m_nZ = Source.m_nZ ;

    Allocate();

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
		SetAt( x, y, z, Source.GetAt(x, y, z) ) ;
}

CFVoxel::CFVoxel( CString FileName )
{
	CString Ext ;
	
	m_pfData = NULL ;
	Ext = FileName.Right( 3 ) ;

	if( Ext == "mhd" || Ext == "MHD" ) {
		ReadFromMhd( FileName ) ;
	}

	else {
		m_nX = 0 ;
		m_nY = 0 ;
		m_nZ = 0 ;
		Load( FileName ) ;
	}
}

CFVoxel::~CFVoxel()
{
	Free( ) ;
}

void CFVoxel::Allocate()
{
	int i, x, y, z, tSize ;
	
	tSize = m_nX * m_nY * m_nZ ;
	if( m_pfData )
	{
		delete [] m_pfData ;
		m_pfData = NULL ;
	}
	m_pfData = new float[tSize] ;

	for( z = 0, i = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++, i++ ) {
		m_pfData[i] = 0.0f ;
	}
}

void CFVoxel::AllocateNoInitialize()
{
	int tSize ;

	tSize = m_nX * m_nY * m_nZ ;
	if( m_pfData )
	{
		delete [] m_pfData ;
		m_pfData = NULL ;
	}
	m_pfData = new float[tSize] ;
}

void CFVoxel::Allocate(int XSize, int YSize, int ZSize)
{
	m_nX = XSize;
	m_nY = YSize;
	m_nZ = ZSize;

	Allocate();	
}

void CFVoxel::Free()
{
	delete [] m_pfData ;
	m_pfData = NULL ;
}

void CFVoxel::ClearToZeros( )
{
	int x, y, z ;

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
		SetAt( x, y, z, 0.0f ) ;
}

void CFVoxel::Copy( CFVoxel &source )
{
	int x, y, z ;

	if( m_nX == source.m_nX && m_nY == source.m_nY && m_nZ == source.m_nZ )
	{
		for( z = 0 ; z < m_nZ ; z++ )
			for( y = 0 ; y < m_nY ; y++ )
				for( x = 0 ; x < m_nX ; x++ )
					SetAt( x, y, z, source.GetAt( x, y, z ) ) ;
	}
	else
	{
		Free( ) ;

		m_nX = source.m_nX ;
		m_nY = source.m_nY ;
		m_nZ = source.m_nZ ;

		Allocate( ) ;

		for( z = 0 ; z < m_nZ ; z++ )
			for( y = 0 ; y < m_nY ; y++ )
				for( x = 0 ; x < m_nX ; x++ )
					SetAt( x, y, z, source.GetAt( x, y, z ) ) ;
	}

	//CopySpOrg( source ) ;
}

CFVoxel *CFVoxel::Copy()
{
	return new CFVoxel(*this);
}


bool CFVoxel::ReadFromMhd(CString PathName)
{
	//int iSpOrg;
	int ImageSize, ImageBytes, Offset ;

	CFile FP ;
	CFileException FE ;
	CString strDataFile = _T(" ");

	//
	CStdioFile fpMeta;
	if( !fpMeta.Open( PathName, CStdioFile::modeRead|CFile::shareDenyNone ) ) {
		return false;
	}

	CString strMetaType = _T(" ");
	CString strMetaData = _T(" ");
	CString strLine = _T(" ");
	CString strElementType = _T(" ");
	while( strLine.GetLength() > 0)
	{
		fpMeta.ReadString(strLine);
		if (strLine.GetLength() < 0) break; 

		int sep = strLine.Find(_T("="));
		strMetaType = strLine.Left(sep-1);
		strMetaData = strLine.Right(strLine.GetLength() - 2 - sep);

		if( strMetaType == "DimSize" ) {
			//sscanf((const char*)strMetaData, "%d %d %d", &m_nX, &m_nY, &m_nZ);
			sscanf_s((CStringA)strMetaData, "%d %d %d", &m_nX, &m_nY, &m_nZ);
		} else if ( strMetaType == "ElementSpacing" ) {
			//sscanf((const char*)strMetaData, "%f %f %f", &m_fXSp, &m_fYSp, &m_fZSp);
			sscanf_s((CStringA)strMetaData, "%f %f %f", &m_fXSp, &m_fYSp, &m_fZSp);
		} else if ( strMetaType == "ElementDataFile" ) {
			strDataFile = PathName.Left(PathName.ReverseFind('\\') + 1) + strMetaData;
		} 
		else if ( strMetaType == "ElementType" ) {
			strElementType =  strMetaData;
		} 
		else {

		}
	}
	fpMeta.Close();


	if( !FP.Open( strDataFile, CFile::modeRead|CFile::shareDenyNone, &FE ) ) // 007/06/04
	{
		CString str;
		switch ( FE.m_cause ) {
			case CFileException::none:
				str = "none"; break;
			case CFileException::genericException:
				str = "genericException"; break;
			case CFileException::fileNotFound:
				str = "fileNotFound"; break;
			case CFileException::badPath:
				str = "badPath"; break;
			case CFileException::tooManyOpenFiles:
				str = "tooManyOpenFiles"; break;
			case CFileException::accessDenied:
				str = "accessDenied"; break;
			case CFileException::invalidFile:
				str = "invalidFile"; break;
			case CFileException::removeCurrentDir:
				str = "removeCurrentDir"; break;
			case CFileException::directoryFull:
				str = "directoryFull"; break;
			case CFileException::badSeek:
				str = "badSeek"; break;
			case CFileException::hardIO:
				str = "hardIO"; break;
			case CFileException::sharingViolation:
				str = "sharingViolation"; break;
			case CFileException::lockViolation:
				str = "lockViolation"; break;
			case CFileException::diskFull:
				str = "diskFull"; break;
			case CFileException::endOfFile:
				str = "endOfFile"; break;
		}
		CString str_out; str_out.Format(_T("Cannot open mhd file %s"), str);
		AfxMessageBox(str_out);
		return false ;
	}
	Allocate( ) ;
	
	if ( strElementType == "MET_FLOAT") {
		float *Origin ;
		ImageSize = m_nX * m_nY ;
		ImageBytes = 4*ImageSize ;
		Origin = new float[ ImageSize ] ;

		int x,y,z;
		for( z = 0 ; z < m_nZ ; z++ )
		{
			FP.Read( ( void FAR *)Origin, ImageBytes ) ;
			for( y = 0 ; y < m_nY ; y++ )
				for( x = 0 ; x < m_nX ; x++ )
				{
					Offset = y * m_nX + x ;
					SetAt( x, y, z, (float)Origin[Offset] ) ;
				}
		}

		delete [] Origin ;
	}

	else {
		AfxMessageBox(_T("This is not floating type data. Please open the data as CVoxel"));
	}
	FP.Close( ) ;
	return true;



}


bool CFVoxel::Load(CString FileName)
{
	int x, y, z, nX, nY, nZ, ImageSize, ImageBytes, Offset ;
	float *Origin ;
	CFile FP ;
	CFileException FE ;

	if( !FP.Open( FileName, CFile::modeRead, &FE ) )
		return false ;
	FP.Read( &nX, sizeof( int ) ) ;
	FP.Read( &nY, sizeof( int ) ) ;
	FP.Read( &nZ, sizeof( int ) ) ;
	if( !( nX == m_nX && nY == m_nY && nZ == m_nZ ) ) // quite cursory code 17DEC007
	{
		m_nX = nX ;
		m_nY = nY ;
		m_nZ = nZ ;
		Allocate( ) ;
	}

	ImageSize = m_nX * m_nY ;
	ImageBytes = sizeof(float) * ImageSize ;
	Origin = new float[ ImageSize ] ;

	for( z = 0 ; z < m_nZ ; z++ )
	{
		FP.Read( ( void FAR *)Origin, ImageBytes ) ;
		for( y = 0 ; y < m_nY ; y++ )
		for( x = 0 ; x < m_nX ; x++ )
		{
			Offset = y * m_nX + x ;
			SetAt( x, y, z, (float)Origin[Offset] ) ;
		}
	}

	delete [] Origin ;
	FP.Close( ) ;
	return true ;
}

void CFVoxel::Save( CString FileName )
{
	int x, y, z ;
	int ImageSize, ImageBytes, Offset ;
	float *Buffer ;
	CFile FP ;

	FP.Open( FileName, CFile::modeCreate | CFile::modeWrite, NULL ) ;
	FP.Write( &m_nX, 4 ) ;
	FP.Write( &m_nY, 4 ) ;
	FP.Write( &m_nZ, 4 ) ;

	ImageSize = m_nX * m_nY ;
	ImageBytes = sizeof(float) * ImageSize ;
	Buffer = new float[ ImageSize ] ;

	for( z = 0 ; z < m_nZ ; z++ )
	{
		for( y = 0 ; y < m_nY ; y++ )
		for( x = 0 ; x < m_nX ; x++ )
		{
			Offset = y * m_nX + x ;
			Buffer[Offset] = (float)GetAt( x, y, z ) ;
		}
		FP.Write( (void FAR *)Buffer, ImageBytes ) ;
	}
	delete [] Buffer ;
	FP.Close( ) ;
}

void CFVoxel::SaveFabs( CString FileName )
{
	int x, y, z ;
	int ImageSize, ImageBytes, Offset ;
	float *Buffer ;
	CFile FP ;

	FP.Open( FileName, CFile::modeCreate | CFile::modeWrite, NULL ) ;
	FP.Write( &m_nX, 4 ) ;
	FP.Write( &m_nY, 4 ) ;
	FP.Write( &m_nZ, 4 ) ;

	ImageSize = m_nX * m_nY ;
	ImageBytes = sizeof(float) * ImageSize ;
	Buffer = new float[ ImageSize ] ;

	for( z = 0 ; z < m_nZ ; z++ )
	{
		for( y = 0 ; y < m_nY ; y++ )
		for( x = 0 ; x < m_nX ; x++ )
		{
			Offset = y * m_nX + x ;
			Buffer[Offset] = fabs((float)GetAt( x, y, z ));
		}
		FP.Write( (void FAR *)Buffer, ImageBytes ) ;
	}
	delete [] Buffer ;
	FP.Close( ) ;
}

void CFVoxel::SaveAsCVoxel( CString FileName )
{
	CVoxel voxl(m_nX, m_nY, m_nZ);
	ToVoxel(&voxl);
	voxl.WriteToRaw(FileName);
}

/*void CFVoxel::SaveProbabilityAsCVoxel( CString FileName )
{
	CVoxel voxl(m_nX, m_nY, m_nZ);
	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		fValue = GetAt( x, y, z ) ;
		pVoxel->SetAt( x, y, z, (int)( fValue + 0.5f ) ) ;
	}
	voxl.WriteToRaw(FileName);
}*/


bool CFVoxel::ToVoxel( CVoxel *pVoxel )
{
	int x, y, z ;
	float fValue ;

	if( m_nX != pVoxel->m_nX || m_nY != pVoxel->m_nY || m_nZ != pVoxel->m_nZ )
		return false ;

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		fValue = GetAt( x, y, z ) ;
		pVoxel->SetAt( x, y, z, (int)( fValue + 0.5f ) ) ;
	}

	return true ;
}

void CFVoxel::SaveweightAsCVoxel( CString FileName )
{
	CVoxel voxl(m_nX, m_nY, m_nZ);
	WeightToVoxel(&voxl);
	voxl.WriteToRaw(FileName);
}

bool CFVoxel::WeightToVoxel( CVoxel *pVoxel )
{
	int i, n = m_nX*m_nY*m_nZ;
	for ( i = 0; i < n; i++ ) {
		if (m_pfData[i] == 0)
		{
			pVoxel->m_pData[i] = 0;
		}
		else if ( m_pfData[i] > 0 && m_pfData[i] <= 0.2)
		{
			pVoxel->m_pData[i] = 50;
		}
		else if ( m_pfData[i] > 0.2 && m_pfData[i] <= 0.4)
		{
			pVoxel->m_pData[i] = 100;
		}
		else if ( m_pfData[i] > 0.4 && m_pfData[i] <= 0.6 )
		{
			pVoxel->m_pData[i] = 150;
		}
		else if ( m_pfData[i] > 0.6 && m_pfData[i] <= 0.8 )
		{
			pVoxel->m_pData[i] = 200;
		}
		else
		{
			pVoxel->m_pData[i] = 250;
		}
	}	
	return true ;
}

bool CFVoxel::FromVoxel( CVoxel *pVoxel )
{
	int x, y, z ;
	int iValue ;

	if( m_nX != pVoxel->m_nX || m_nY != pVoxel->m_nY || m_nZ != pVoxel->m_nZ )
		return false ;

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		iValue = pVoxel->GetAt( x, y, z ) ;
		SetAt( x, y, z, (float)iValue ) ;
	}

	return true ;
}

bool CFVoxel::FromReverseVoxel( CVoxel *pVoxel )
{
	int x, y, z ;
	int iValue ;

	if( m_nX != pVoxel->m_nX || m_nY != pVoxel->m_nY || m_nZ != pVoxel->m_nZ )
		return false ;

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		iValue = 1-pVoxel->GetAt( x, y, z ) ;
		SetAt( x, y, z, (float)iValue ) ;
	}

	return true ;
}

BOOL CFVoxel::IsLocalMaximum( C3DPoint &pt, C3DPoint &localReg )
{
	int i, x, y, z;
	int ptIdx = pt.m_z*m_nX*m_nY+pt.m_y*m_nX+pt.m_x;
	BOOL isMaxInLocReg = TRUE;
	for ( z = pt.m_z-localReg.m_z; z <= pt.m_z+localReg.m_z; z++ ) {
		for ( y = pt.m_y-localReg.m_y; y <= pt.m_y+localReg.m_y; y++ ) {
			for ( x = pt.m_x-localReg.m_x; x <= pt.m_x+localReg.m_x; x++ ) {
				if ( 0 <= x && x < m_nX && 0 <= y && y < m_nY && 0 <= z && z < m_nZ ) {
					i = z*m_nX*m_nY+y*m_nX+x;
					if ( m_pfData[ptIdx] < m_pfData[i] ) {
						isMaxInLocReg = FALSE;
						goto stop;
					}
				}
			}
		}
	}
	stop:
	return isMaxInLocReg;
}

bool CFVoxel::operator ==(const CVoxel& v) const {
	if ( (m_nX == v.m_nX) && (m_nY == v.m_nY) && (m_nZ == v.m_nZ) )
		return true;
	else
		return false;
}
bool CFVoxel::operator ==(const CFVoxel& v) const {
	if ( (m_nX == v.m_nX) && (m_nY == v.m_nY) && (m_nZ == v.m_nZ) )
		return true;
	else
		return false;
}
bool CFVoxel::operator !=(const CVoxel& v) const {
	if ( (m_nX != v.m_nX) || (m_nY != v.m_nY) || (m_nZ != v.m_nZ) )
		return true;
	else
		return false;
}
bool CFVoxel::operator !=(const CFVoxel& v) const {
	if ( (m_nX != v.m_nX) || (m_nY != v.m_nY) || (m_nZ != v.m_nZ) )
		return true;
	else
		return false;
}

double CFVoxel::Mean()
{
	int i, nV = m_nX*m_nY*m_nZ;
	double sum = 0;
	for ( i = 0; i < nV; i++ ) {
		sum += m_pfData[i];
	}
	return sum/(double)nV;
}

CFVoxel *CFVoxel::GetGradient(const C3DPoint &npt)
{
	CFVoxel *grad = new CFVoxel(m_nX, m_nY, m_nZ, FALSE);
	//grad->ClearToZeros();
	int x, y, z, i;
	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				C3DPoint cnpt = C3DPoint(x,y,z)+npt;
				if ( IsInBounds(cnpt) )
					grad->m_pfData[i] = m_pfData[i]-GetAt(cnpt);
				else	
					grad->m_pfData[i] = 0;
			}
		}
	}
	return grad;
}


CFVoxel *CFVoxel::Subtract(CFVoxel *vin)
{
	if ( *this != *vin ) {
		AfxMessageBox(_T("Error at CFVoxel::GetSubtracted(): vin dimension does not match!"));
	}
	CFVoxel *sub = new CFVoxel(m_nX, m_nY, m_nZ);
	int i, N = m_nX*m_nY*m_nZ;
	for ( i = 0; i < N; i++ ) {
		sub->m_pfData[i] = m_pfData[i]-vin->m_pfData[i];
	}
	return sub;
}

void CFVoxel::IncrementSumProduct(CFVoxel *v1, CFVoxel *v2)
{
	if ( *this != *v1 ) {
		AfxMessageBox(_T("Error at CFVoxel::IncrementSumProduct(): v1 dimension does not match"));
	}
	if ( *this != *v2 ) {
		AfxMessageBox(_T("Error at CFVoxel::IncrementSumProduct(): v2 dimension does not match"));
	}
	int i, N = m_nX*m_nY*m_nZ;
	for ( i = 0; i < N; i++ ) {
		m_pfData[i] += v1->m_pfData[i]*v2->m_pfData[i];
	}
}

void CFVoxel::IncrementExpSumProduct(CFVoxel *v1, int r, CFVoxel *v2)
{
	if ( *this != *v1 ) {
		AfxMessageBox(_T("Error at CFVoxel::IncrementSumProduct(): v1 dimension does not match"));
	}
	if ( *this != *v2 ) {
		AfxMessageBox(_T("Error at CFVoxel::IncrementSumProduct(): v2 dimension does not match"));
	}
	int i, N = m_nX*m_nY*m_nZ;
	for ( i = 0; i < N; i++ ) {
		m_pfData[i] += pow(v1->m_pfData[i], r)*v2->m_pfData[i];
	}
}

void CFVoxel::ResetPairwiseProbUsingGradientDT(CFVoxel *weight, int r)
{
	if ( *this != *weight ) {
		AfxMessageBox(_T("Error at CFVoxel::IncrementSumProduct(): dimension of v1 and v2 does not match"));
	}

	int i, N = m_nX*m_nY*m_nZ;
	for ( i = 0; i < N; i++ ) {
		m_pfData[i] = m_pfData[i] * pow(weight->m_pfData[i],r);
	}
}

double CFVoxel::GetZeroCrossingBoundaryEdgeLength(C3DPointArray &nPtArr)
{
	C3DPoint zpt(0,0,0);
	C3DPoint ept(m_nX,m_nY,m_nZ);
	int x, y, z, i, nn;
	int npairwise = (int) nPtArr.GetSize();
	double bdl = 0;
	double *nbdLArr = new double[npairwise];
	for ( nn = 0; nn < npairwise; nn++ ) {
		nbdLArr[nn] = nPtArr[nn].Norm();
	}
	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				C3DPoint cpt(x,y,z);
				for ( nn = 0; nn < npairwise; nn++ ) {
					C3DPoint npt = cpt+nPtArr[nn];
					if ( zpt <= npt && npt < ept && sign(GetAt(cpt)) != sign(GetAt(npt)) ) {
						bdl += nbdLArr[nn];
					}
				}
			}
		}
	}
	return bdl;
}

double CFVoxel::GetZeroCrossingBoundaryEdgeLength(C3DPointArray &nPtArr, CVoxel *exc)
{
	VERIFY(*exc == *this);
	C3DPoint zpt(0,0,0);
	C3DPoint ept(m_nX,m_nY,m_nZ);
	int x, y, z, i, nn;
	int npairwise = (int) nPtArr.GetSize();
	double bdl = 0;
	double *nbdLArr = new double[npairwise];
	for ( nn = 0; nn < npairwise; nn++ ) {
		nbdLArr[nn] = nPtArr[nn].Norm();
	}
	/*for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				C3DPoint cpt(x,y,z);
				if ( sign(GetAt(cpt)) == 1 ) {
					for ( nn = 0; nn < npairwise; nn++ ) {
						C3DPoint npt = cpt+nPtArr[nn];
						if ( zpt <= npt && npt < ept && 
							!(exc->GetAt(cpt)) && !(exc->GetAt(npt)) &&
							sign(GetAt(npt)) != 1 ) {
							bdl += nbdLArr[nn];
						}
					}
				}
			}
		}
	}*/

	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				C3DPoint cpt(x,y,z);
				for ( nn = 0; nn < npairwise; nn++ ) {
					C3DPoint npt = cpt+nPtArr[nn];
					if ( zpt <= npt && npt < ept && 
						!(exc->GetAt(cpt)) && !(exc->GetAt(npt)) &&
						sign(GetAt(cpt)) != sign(GetAt(npt)) ) {
						bdl += nbdLArr[nn];
					}
				}
			}
		}
	}


	SAFE_DELETE_ARRAY(nbdLArr);
	return bdl;
}

float CFVoxel::GetMax()
{
	float maxv = -FLT_MAX;
	int i, n = m_nX*m_nY*m_nZ;
	for ( i = 0; i < n; i++ ) {
		if ( m_pfData[i] > maxv )
			maxv = m_pfData[i];
	}
	return maxv;
}

CVoxel *CFVoxel::MakeDTShell(double dist)
{
	CVoxel *shl = new CVoxel(m_nX, m_nY, m_nZ);
	int x, y, z, i;
	int nxm1 = m_nX-1;
	int nym1 = m_nY-1;
	int nzm1 = m_nZ-1;
	int nxy = m_nX*m_nY;
	for ( z = 0, i = 0; z < m_nZ; z++ ) {
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				if ( m_pfData[i] > dist &&
					( ( z > 0 && m_pfData[i-nxy] < dist ) || 
					  ( z < nzm1 && m_pfData[i+nxy] < dist ) || 
					  ( y > 0 && m_pfData[i-m_nX] < dist ) || 
					  ( y < nym1 && m_pfData[i+m_nX] < dist ) || 
					  ( x > 0 && m_pfData[i-1] < dist )  || 
					  ( x < nxm1 && m_pfData[i+1] < dist ) ) ) {
					shl->m_pData[i] = 1;
				}
				else
					shl->m_pData[i] = 0;
			}
		}
	}
	return shl;
}

#define VAR_N_HIST_BIN 256
void CFVoxel::WeightedMeanAndVar(CFVoxel *w, double &m, double &v, int nAnchor, int  idx)
{
	if ( *this != *w ) {
		AfxMessageBox(_T("Error at CFVoxel::WeightedMeanAndVar(): w dimension does not match"));
	}
	float maxd_inv = 1.f/GetMax();
	int i, N = m_nX*m_nY*m_nZ;
	double sum = 0;
	double sqr_sum = 0;
	double wsum = 0;
	float _w, wd;
	double *vars = new double[N];
	double *ws = new double[N];
	for ( i = 0; i < N; i++ ) {
		wsum += (_w=fabs(w->m_pfData[i]));
		wd = _w*m_pfData[i];
		sum += wd;
		//sqr_sum += wd*wd;
		vars[i] = m_pfData[i];
		ws[i] = _w;
	}
	//double dblN = (double) N;
	//m = sum / dblN;
	//v = sqr_sum/dblN-m*m;
	m = (sum/wsum)*maxd_inv;
	for ( i = 0; i < N; i++ ) {
		//wsum += (_w=fabs(w->m_pfData[i]));
		wd = fabs(w->m_pfData[i])*(m_pfData[i]*maxd_inv-m)*(m_pfData[i]*maxd_inv-m);
		sqr_sum += wd;
	}
	/*
	CHistogram var_hist(VAR_N_HIST_BIN);
	var_hist.ComputeHistogram(vars, N, 0, ws);
	CStringA str;
	str.Format("UPratio\\%d_%d_variance.bmp", nAnchor, idx );
	var_hist.SaveVariableLengthImage(str);*/
	v = sqr_sum/wsum;
	SAFE_DELETE(vars);
	SAFE_DELETE(ws);
}

void CFVoxel::WeightedMeanAndVar(CFVoxel *w, double &m, double &v)
{
	if ( *this != *w ) {
		AfxMessageBox(_T("Error at CFVoxel::WeightedMeanAndVar(): w dimension does not match"));
	}
	float maxd_inv = 1.f/GetMax();
	int i, N = m_nX*m_nY*m_nZ;
	double sum = 0;
	double sqr_sum = 0;
	double wsum = 0;
	float _w, wd;
	double *vars = new double[N];
	double *ws = new double[N];
	for ( i = 0; i < N; i++ ) {
		wsum += (_w=fabs(w->m_pfData[i]));
		wd = _w*m_pfData[i];
		sum += wd;
		//sqr_sum += wd*wd;
		vars[i] = m_pfData[i];
		ws[i] = _w;
	}
	//double dblN = (double) N;
	//m = sum / dblN;
	//v = sqr_sum/dblN-m*m;
	m = (sum/wsum)*maxd_inv;
	for ( i = 0; i < N; i++ ) {
		//wsum += (_w=fabs(w->m_pfData[i]));
		wd = fabs(w->m_pfData[i])*(m_pfData[i]*maxd_inv-m)*(m_pfData[i]*maxd_inv-m);
		sqr_sum += wd;
	}
	/*
	CHistogram var_hist(VAR_N_HIST_BIN);
	var_hist.ComputeHistogram(vars, N, 0, ws);
	CStringA str;
	str.Format("UPratio\\%d_%d_variance.bmp", nAnchor, idx );
	var_hist.SaveVariableLengthImage(str);*/
	v = sqr_sum/wsum;
	SAFE_DELETE(vars);
	SAFE_DELETE(ws);
}

void CFVoxel::SetData(float f) 
{
	int x, y, z ;

	for( z = 0 ; z < m_nZ ; z++ )
		for( y = 0 ; y < m_nY ; y++ )
			for( x = 0 ; x < m_nX ; x++ )
				SetAt( x, y, z, f );
}

void CFVoxel::PlusConstant( float fOperand )
{
	int x, y, z ;
	float fValue ;
	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		fValue = GetAt( x, y, z ) ;
		SetAt( x, y, z, fValue + fOperand ) ;
	}
}

void CFVoxel::MinusConstant( float fOperand )
{
	int x, y, z ;
	float fValue ;
	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		fValue = GetAt( x, y, z ) ;
		SetAt( x, y, z, fValue - fOperand ) ;
	}
}

void CFVoxel::LinearScale( float fLower, float fUpper )
{
	int x, y, z ;
	float fValue, fMinValue, fMaxValue, fRangeNum, fRangeDen ;

	if( fLower > fUpper )
		return ;

	fMinValue = FLT_MAX ;
	fMaxValue = -FLT_MAX + 1 ;

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		fValue = GetAt( x, y, z ) ;
		if( fValue > fMaxValue )
			fMaxValue = fValue ;
		if( fValue < fMinValue )
			fMinValue = fValue ;
	}
	
	fRangeNum = fUpper - fLower ;
	fRangeDen = fMaxValue - fMinValue ;
	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		fValue = fLower + fRangeNum * ( GetAt( x, y, z ) - fMinValue ) / fRangeDen ;
		SetAt( x, y, z, fValue ) ;
	}
}

void CFVoxel::Differentiate( CFVoxel* lu )
{
	int x,y,z;
	int xn1 = m_nX-1; int xn2 = m_nX-2; int yn1 = m_nY-1; int yn2 = m_nY-2; int zn1 = m_nZ-1; int zn2 = m_nZ-2;
	double val;
	CFVoxel* u = this;

	// Deal with corners 
	val = (u->GetAt(0,0,1)+u->GetAt(0,1,0)+u->GetAt(1,0,0))/3.0 - u->GetAt(0,0,0);
	lu->SetAt(0,0,0,val);
	val = (u->GetAt(1,0,zn1)+u->GetAt(0,1,zn1)+u->GetAt(0,0,zn2))/3.0 - u->GetAt(0,0,zn1);
	lu->SetAt(0,0,zn1,val);

	val = (u->GetAt(1,yn1,0)+u->GetAt(0,yn2,0)+u->GetAt(0,yn1,1))/3.0 - u->GetAt(0,yn1,0);
	lu->SetAt(0,yn1,0,val);
	val = (u->GetAt(1,yn1,zn1)+u->GetAt(0,yn2,zn1)+u->GetAt(0,yn1,zn2))/3.0 - u->GetAt(0,yn1,zn1);
	lu->SetAt(0,yn1,zn1,val);

	val = (u->GetAt(xn2,0,0)+u->GetAt(xn1,1,0)+u->GetAt(xn1,0,1))/3.0 - u->GetAt(xn1,0,0);
	lu->SetAt(xn1,0,0,val);
	val = (u->GetAt(xn2,0,zn1)+u->GetAt(xn1,1,zn1)+u->GetAt(xn1,0,zn2))/3.0 - u->GetAt(xn1,0,zn1);
	lu->SetAt(xn1,0,zn1,val);

	val = (u->GetAt(xn2,yn1,0)+u->GetAt(xn1,yn2,0)+u->GetAt(xn1,yn1,1))/3.0 - u->GetAt(xn1,yn1,0);
	lu->SetAt(xn1,yn1,0,val);
	val = (u->GetAt(xn2,yn1,zn1)+u->GetAt(xn1,yn2,zn1)+u->GetAt(xn1,yn1,zn2))/3.0 - u->GetAt(xn1,yn1,zn1);
	lu->SetAt(xn1,yn1,zn1,val);


	// Deal with edges
	for ( z = 1; z <= zn2; z++) { 
		 val = (2*u->GetAt(1,0,z) + 2*u->GetAt(0,1,z) + u->GetAt(0,0,z-1) + u->GetAt(0,0,z+1))/6.0 - u->GetAt(0,0,z);
		 lu->SetAt(0,0,z,val);

		 val = (2*u->GetAt(1,yn1,z) + 2*u->GetAt(0,yn2,z) + u->GetAt(0,yn1,z-1) + u->GetAt(0,yn1,z+1))/6.0 - u->GetAt(0,yn1,z);
		 lu->SetAt(0,yn1,z,val);

		 val = (2*u->GetAt(xn2,0,z) + 2*u->GetAt(xn1,1,z) + u->GetAt(xn1,0,z-1) + u->GetAt(xn1,0,z+1))/6.0 - u->GetAt(xn1,0,z);
		 lu->SetAt(xn1,0,z,val);

		 val = (2*u->GetAt(xn2,yn1,z) + 2*u->GetAt(xn1,yn2,z) + u->GetAt(xn1,yn1,z-1) + u->GetAt(xn1,yn1,z+1))/6.0 - u->GetAt(xn1,yn1,z);
		 lu->SetAt(xn1,yn1,z,val);
	 }

	for ( y = 1; y <= yn2; y++) { 
		 val = (2*u->GetAt(1,y,0) + 2*u->GetAt(0,y,1) + u->GetAt(0,y-1,0) + u->GetAt(0,y+1,0))/6.0 - u->GetAt(0,y,0);
		 lu->SetAt(0,y,0,val);

		 val = (2*u->GetAt(1,y,zn1) + 2*u->GetAt(0,y,zn2) + u->GetAt(0,y-1,zn1) + u->GetAt(0,y+1,zn1))/6.0 - u->GetAt(0,y,zn1);
		 lu->SetAt(0,y,zn1,val);

		 val = (2*u->GetAt(xn2,y,0) + 2*u->GetAt(xn1,y,1) + u->GetAt(xn1,y-1,0) + u->GetAt(xn1,y+1,0))/6.0 - u->GetAt(xn1,y,0);
		 lu->SetAt(xn1,y,0,val);

		 val = (2*u->GetAt(xn2,y,zn1) + 2*u->GetAt(xn1,y,zn2) + u->GetAt(xn1,y-1,zn1) + u->GetAt(xn1,y+1,zn1))/6.0 - u->GetAt(xn1,y,zn1);
		 lu->SetAt(xn1,y,zn1,val);
	 }

	for ( x = 1; x <= xn2; x++) { 
		 val = (2*u->GetAt(x,0,1) + 2*u->GetAt(x,1,0) + u->GetAt(x-1,0,0) + u->GetAt(x+1,0,0))/6.0 - u->GetAt(x,0,0);
		 lu->SetAt(x,0,0,val);

		 val = (2*u->GetAt(x,yn1,1) + 2*u->GetAt(x,yn2,0) + u->GetAt(x-1,yn1,0) + u->GetAt(x+1,yn1,0))/6.0 - u->GetAt(x,yn1,0);
		 lu->SetAt(x,yn1,0,val);

		 val = (2*u->GetAt(x,0,zn2) + 2*u->GetAt(x,1,zn1) + u->GetAt(x-1,0,zn1) + u->GetAt(x+1,0,zn1))/6.0 - u->GetAt(x,0,zn1);
		 lu->SetAt(x,0,zn1,val);

		 val = (2*u->GetAt(x,yn1,zn2) + 2*u->GetAt(x,yn2,zn1) + u->GetAt(x-1,yn1,zn1) + u->GetAt(x+1,yn1,zn1))/6.0 - u->GetAt(x,yn1,zn1);
		 lu->SetAt(x,yn1,zn1,val);
	 }

	// Deal with faces
	for (z=1; z<=zn2; z++) {
		for (y=1; y<=yn2; y++) {
			val = (2*u->GetAt(1,y,z) + u->GetAt(0,y+1,z) + u->GetAt(0,y-1,z) + u->GetAt(0,y,z+1) + u->GetAt(0,y,z-1))/6.0 - u->GetAt(0,y,z);
			lu->SetAt(0,y,z,val);
			val = (2*u->GetAt(xn2,y,z) + u->GetAt(xn1,y+1,z) + u->GetAt(xn1,y-1,z) + u->GetAt(xn1,y,z+1) + u->GetAt(xn1,y,z-1))/6.0 - u->GetAt(xn1,y,z);
			lu->SetAt(xn1,y,z,val);
		}
	}

	for (x=1; x<=xn2; x++) {
		for (y=1; y<=yn2; y++) {
			val = (2*u->GetAt(x,y,1) + u->GetAt(x,y+1,0) + u->GetAt(x,y-1,0) + u->GetAt(x+1,y,0) + u->GetAt(x-1,y,0))/6.0 - u->GetAt(x,y,0);
			lu->SetAt(x,y,0,val);
			val = (2*u->GetAt(x,y,zn2) + u->GetAt(x,y+1,zn1) + u->GetAt(x,y-1,zn1) + u->GetAt(x+1,y,zn1) + u->GetAt(x-1,y,zn1))/6.0 - u->GetAt(x,y,zn1);
			lu->SetAt(x,y,zn1,val);
		}
	}

	for (z=1; z<=zn2; z++) {
		for (x=1; x<=xn2; x++) {
			val = (2*u->GetAt(x,1,z) + u->GetAt(x+1,0,z) + u->GetAt(x-1,0,z) + u->GetAt(x,0,z+1) + u->GetAt(x,0,z-1))/6.0 - u->GetAt(x,0,z);
			lu->SetAt(x,0,z,val);
			val = (2*u->GetAt(x,yn2,z) + u->GetAt(x+1,yn1,z) + u->GetAt(x-1,yn1,z) + u->GetAt(x,yn1,z+1) + u->GetAt(x,yn1,z-1))/6.0 - u->GetAt(x,yn1,z);
			lu->SetAt(x,yn1,z,val);
		}
	}

	// Deal with centers
	for (z=1; z<=zn2; z++) {
		for (y=1; y<=yn2; y++) {
			for (x=1; x<=xn2; x++) {
				val = ( u->GetAt(x+1,y,z) + u->GetAt(x-1,y,z) + u->GetAt(x,y+1,z) + u->GetAt(x,y-1,z) + u->GetAt(x,y,z+1) + u->GetAt(x,y,z-1))/6.0 - u->GetAt(x,y,z);
				lu->SetAt(x,y,z,val);
			}
		}
	}



}


float CFVoxel::GetTriLinearInterpolation( CF3DPoint fPoint )
{
	int x1, x2, y1, y2, z1, z2 ;
	float i, j, k, v0, v111, v211, v121, v221, v112, v212, v122, v222 ;

	x1 = (int)( fPoint.m_fX ) ; // fixed at 007/03/20
	y1 = (int)( fPoint.m_fY ) ;
	z1 = (int)( fPoint.m_fZ ) ;
	x2 = x1 + 1 ;
	y2 = y1 + 1 ;
	z2 = z1 + 1 ;

	v111 = (float)GetAt( x1, y1, z1 ) ;
	
	if( x2 == m_nX )
		v211 = (float)( v111 + ( v111 - GetAt( x1-1, y1, z1 ) ) ) ;
	else
		v211 = (float)GetAt( x2, y1, z1 ) ;
	
	if( y2 == m_nY )
		v121 = (float)( v111 + ( v111 - GetAt( x1, y1-1, z1 ) ) ) ;
	else
		v121 = (float)GetAt( x1, y2, z1 ) ;
	
	if( x2 == m_nX && y2 != m_nY ) // for boundary condition only in X
		v221 = (float)( v121 + ( v121 - GetAt( x1-1, y2, z1 ) ) ) ;
	else if( x2 != m_nX && y2 == m_nY ) // for boundary condition only in Y
		v221 = (float)( v211 + ( v211 - GetAt( x2, y1-1, z1 ) ) ) ;
	else if( x2 == m_nX && y2 == m_nY ) // for boundary condition both in X and Y
		v221 = (float)( v111 + ( v111 - GetAt( x1-1, y1-1, z1 ) ) ) ;
	else
		v221 = (float)GetAt( x2, y2, z1 )  ;

	if( z2 == m_nZ )
		v112 = (float)( v111 + ( v111 - GetAt( x1, y1, z1-1 ) ) ) ;
	else
		v112 = (float)GetAt( x1, y1, z2 ) ;

	if( x2 == m_nX && z2 != m_nZ ) // for boundary condition only in X
		v212 = (float)( v112 + ( v112 - GetAt( x1-1, y1, z2 ) ) ) ;
	else if( x2 != m_nX && z2 == m_nZ ) // for boundary condition only in Z
		v212 = (float)( v211 + ( v211 - GetAt( x2, y1, z1-1 ) ) ) ;
	else if( x2 == m_nX && z2 == m_nZ ) // for boundary condition both in X and Z
		v212 = (float)( v111 + ( v111 - GetAt( x1-1, y1, z1-1 ) ) ) ;
	else
		v212 = (float)GetAt( x2, y1, z2 ) ;

	if( y2 == m_nY && z2 != m_nZ ) // for boundary condtion only in Y
		v122 = (float)( v112 + ( v112 - GetAt( x1, y1-1, z2 ) ) ) ;
	else if( y2 != m_nY && z2 == m_nZ ) // for boundary condition only in Z
		v122 = (float)( v121 + ( v121 - GetAt( x1, y2, z1-1 ) ) ) ;
	else if( y2 == m_nY && z2 == m_nZ ) // for boundary condition both in Y and Z
		v122 = (float)( v111 + ( v111 - GetAt( x1, y1-1, z1-1 ) ) ) ;
	else
		v122 = (float)GetAt( x1, y2, z2 ) ;

	if( x2 == m_nX && y2 != m_nY && z2 != m_nZ ) // for boundary condition only in X
		v222 = (float)( v122 + ( v122 - GetAt( x1-1, y2, z2 ) ) ) ;
	else if( x2 != m_nX && y2 == m_nY && z2 != m_nZ ) // for boundary condition only in Y
		v222 = (float)( v212 + ( v212 - GetAt( x2, y1-1, z2 )  ) ) ;
	else if( x2 != m_nX && y2 != m_nY && z2 == m_nZ ) // for boundary condition only in Z
		v222 = (float)( v221 + ( v221 - GetAt( x2, y2, z1-1 ) ) ) ;
	else if( x2 == m_nX && y2 == m_nY && z2 != m_nZ ) // for boundary condition only in X and Y
		v222 = (float)( v112 + ( v112 - GetAt( x1-1, y1-1, z2 ) ) ) ;
	else if( x2 != m_nX && y2 == m_nY && z2 == m_nZ ) // for boundary condition only in Y and Z
		v222 = (float)( v211 + ( v211 - GetAt( x2, y1-1, z1-1 ) ) ) ;
	else if( x2 == m_nX && y2 != m_nY && z2 == m_nZ ) // for boundary condition only in Z and X
		v222 = (float)( v121 + ( v121 - GetAt( x1-1, y2, z1-1 ) ) ) ;
	else if( x2 == m_nX && y2 == m_nY && z2 == m_nZ ) // for boundary condition in all of X, Y, and Z
		v222 = (float)( v111 + ( v111 - GetAt( x1-1, y1-1, z1-1 ) ) ) ;
	else
		v222 = (float)GetAt( x2, y2, z2 ) ;

	i = ( fPoint.m_fX - x1 ) / ( (float)x2- x1 ) ;
	j = ( fPoint.m_fY - y1 ) / ( (float)y2- y1 ) ;
	k = ( fPoint.m_fZ - z1 ) / ( (float)z2- z1 ) ;

	v0 = (float)( ( 1.0 - i ) * ( 1.0 - j ) * ( 1.0 - k ) * v111 + i * ( 1.0 - j ) * ( 1.0 - k ) * v211 +
				  ( 1.0 - i ) * j * ( 1.0 - k ) * v121           + i * j * ( 1.0 - k ) * v221 + 
				  ( 1.0 - i ) * ( 1.0 - j ) * k * v112           + i * ( 1.0 - j ) * k * v212 +
				  ( 1.0 - i ) * j * k * v122                     + i * j * k * v222 ) ;

	return v0 ;
}

// [ 0  1  0  ; 1  3  1 ; 0  1  0 ]
// [ 1  3  1  ; 3  9  3 ; 1  3  1 ] / 39
// [ 0  1  0  ; 1  3  1 ; 0  1  0 ]
bool CFVoxel::Get3DFixedGaussianSmoothing( CFVoxel &Source )
{
	int x, y, z ;
	float fValue ;
	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		if( x == 0 || y == 0 || z == 0 || x == ( m_nX - 1 ) || y == ( m_nY - 1 ) || z == ( m_nZ - 1 ) )
			fValue = Source.GetAt( x, y, z ) ;
		else
		{
			fValue = Source.GetAt( x, y - 1, z - 1 ) + Source.GetAt( x - 1, y, z - 1 ) + 3 * Source.GetAt( x, y, z - 1 ) + Source.GetAt( x + 1, y, z - 1 ) + Source.GetAt( x, y + 1, z - 1 ) ;
			fValue += Source.GetAt( x - 1, y - 1, z ) + 3 * Source.GetAt( x, y - 1, z ) + Source.GetAt( x + 1, y - 1, z ) ;
			fValue += 3 * Source.GetAt( x - 1, y, z ) + 9 * Source.GetAt( x, y, z ) + 3 * Source.GetAt( x + 1, y, z ) ;
			fValue += Source.GetAt( x - 1, y + 1, z ) + 3 * Source.GetAt( x, y + 1, z ) + Source.GetAt( x + 1, y + 1, z ) ;
			fValue += Source.GetAt( x, y - 1, z + 1 ) + Source.GetAt( x - 1, y, z + 1 ) + 3 * Source.GetAt( x, y, z + 1 ) + Source.GetAt( x + 1, y, z + 1 ) + Source.GetAt( x, y + 1, z + 1 ) ;
			fValue /= 39.0f ;
		}
		SetAt( x, y, z, fValue ) ;
	}

	return true ;
}

// for each of xyz direction
// [ -1 -2 -1 | -2 -4 -2 | -1 -2 -1 ] , [ 1 2 1 | 2 4 2 | 1 2 1 ]
bool CFVoxel::Get3DSobel( CFVoxel *pfGradMag, CFVoxel *pfGradX, CFVoxel *pfGradY, CFVoxel *pfGradZ )
{
	int x, y, z ;
	float fGradMag, fGradX, fGradY, fGradZ ;

	for( z = 1 ; z < m_nZ - 1 ; z++ )
	for( y = 1 ; y < m_nY - 1 ; y++ )
	for( x = 1 ; x < m_nX - 1 ; x++ )
	{
		fGradX = GetAt( x + 1, y - 1, z - 1 ) + GetAt( x + 1, y - 1, z + 1 ) + GetAt( x + 1, y + 1, z - 1 ) + GetAt( x + 1, y + 1, z + 1 ) ;
		fGradX += 2 * ( GetAt( x + 1, y - 1, z ) + GetAt( x + 1, y, z - 1 ) + GetAt( x + 1, y + 1, z ) + GetAt( x + 1, y, z + 1 ) ) + 4 * GetAt( x + 1, y, z ) ;
		fGradX -= GetAt( x - 1, y - 1, z - 1 ) + GetAt( x - 1, y - 1, z + 1 ) + GetAt( x - 1, y + 1, z - 1 ) + GetAt( x - 1, y + 1, z + 1 ) ;
		fGradX -= 2 * ( GetAt( x - 1, y - 1, z ) + GetAt( x - 1, y, z - 1 ) + GetAt( x - 1, y + 1, z ) + GetAt( x - 1, y, z + 1 ) ) + 4 * GetAt( x - 1, y, z ) ;
		
		fGradY = GetAt( x - 1, y + 1, z - 1 ) + GetAt( x - 1, y + 1, z + 1 ) + GetAt( x + 1, y + 1, z - 1 ) + GetAt( x + 1, y + 1, z + 1 ) ;
		fGradY += 2 * ( GetAt( x - 1, y + 1, z ) + GetAt( x, y + 1, z - 1 ) + GetAt( x + 1, y + 1, z ) + GetAt( x, y + 1, z + 1 ) ) + 4 * GetAt( x, y + 1, z ) ;
		fGradY -= GetAt( x - 1, y - 1, z - 1 ) + GetAt( x - 1, y - 1, z + 1 ) + GetAt( x + 1, y - 1, z - 1 ) + GetAt( x + 1, y - 1, z + 1 ) ;
		fGradY -= 2 * ( GetAt( x - 1, y - 1, z ) + GetAt( x, y - 1, z - 1 ) + GetAt( x + 1, y - 1, z ) + GetAt( x, y - 1, z + 1 ) ) + 4 * GetAt( x, y - 1, z ) ;
		
		fGradZ = GetAt( x - 1, y - 1, z + 1 ) + GetAt( x - 1, y + 1, z + 1 ) + GetAt( x + 1, y - 1, z + 1 ) + GetAt( x + 1, y + 1, z + 1 ) ;
		fGradZ += 2 * ( GetAt( x - 1, y, z + 1 ) + GetAt( x, y - 1, z + 1 ) + GetAt( x + 1, y, z + 1 ) + GetAt( x, y + 1, z + 1 ) ) + 4 * GetAt( x, y, z + 1 ) ;
		fGradZ -= GetAt( x - 1, y - 1, z - 1 ) + GetAt( x - 1, y + 1, z - 1 ) + GetAt( x + 1, y - 1, z - 1 ) + GetAt( x + 1, y + 1, z - 1 ) ;
		fGradZ -= 2 * ( GetAt( x - 1, y, z - 1 ) + GetAt( x, y - 1, z - 1 ) + GetAt( x + 1, y, z - 1 ) + GetAt( x, y + 1, z - 1 ) ) + 4 * GetAt( x, y, z - 1 ) ;

		fGradMag = (float)sqrt( fGradX * fGradX + fGradY * fGradY + fGradZ * fGradZ ) ;
		pfGradMag->SetAt( x, y, z, fGradMag ) ;
		pfGradX->SetAt( x, y, z, fGradX ) ;
		pfGradY->SetAt( x, y, z, fGradY ) ;
		pfGradZ->SetAt( x, y, z, fGradZ ) ;

	}

	return true ;
}

bool CFVoxel::Get3DNonMaximumSuppression( CFVoxel *pfGradX, CFVoxel *pfGradY, CFVoxel *pfGradZ )
{
	int x, y, z ;
	float fGradMag, fBfMag, fAftMag ;
	CF3DPoint fBfPnt, fAftPnt, fGradV ;
	CFVoxel *pTemp = new CFVoxel( *this ) ;

	for( z = 1 ; z < m_nZ - 1 ; z++ )
	for( y = 1 ; y < m_nY - 1 ; y++ )
	for( x = 1 ; x < m_nX - 1 ; x++ )
	{
		fGradMag = GetAt( x, y, z ) ;
		fGradV.m_fX = pfGradX->GetAt( x, y, z ) ;
		fGradV.m_fY = pfGradY->GetAt( x, y, z ) ;
		fGradV.m_fZ = pfGradZ->GetAt( x, y, z ) ;
		fGradV.MakeUnitVector( ) ;

		fBfPnt.m_fX = (float)x - fGradV.m_fX ;
		fBfPnt.m_fY = (float)y - fGradV.m_fY ;
		fBfPnt.m_fZ = (float)z - fGradV.m_fZ ;
		fBfMag = pTemp->GetTriLinearInterpolation( fBfPnt ) ;
		
		fAftPnt.m_fX = (float)x + fGradV.m_fX ;
		fAftPnt.m_fY = (float)y + fGradV.m_fY ;
		fAftPnt.m_fZ = (float)z + fGradV.m_fZ ;
		fAftMag = pTemp->GetTriLinearInterpolation( fAftPnt ) ;

		if( fGradMag < fBfMag || fGradMag < fAftMag )
			SetAt( x, y, z, 0.0f ) ; // suppress!!
	}

	delete pTemp ;
	return true ;
}

/*
bool CFVoxel::HystresisThresholding( float fLower, float fUpper )
{
	bool bConnected2Upper ;
	int i, n, x, y, z, nParts, nPnts, iValue ;
	float fValue ;
	CParts parts ;

	CVoxel *pLower = new CVoxel( m_nX, m_nY, m_nZ ) ;

	for( z = 0 ; z < m_nZ ; z++ ) // thresholding
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		fValue = GetAt( x, y, z ) ;
		if( fValue >= fLower )
			pLower->SetAt( x, y, z, 1 ) ;
		else
			pLower->SetAt( x, y, z, 0 ) ;
	}

	pLower->GetParts( parts, 26, 20 ) ;
	pLower->ClearToZeroes( ) ;
	nParts = parts.GetSize( ) ;
	for( n = 0 ; n < nParts ; n++ )
	{
		bConnected2Upper = false ;
		nPnts = parts[n].GetSize( ) ;
		for( i = 0 ; i < nPnts && bConnected2Upper == false ; i++ )
		{
			x = parts[n][i].m_x ;
			y = parts[n][i].m_y ;
			z = parts[n][i].m_z ;
			fValue = GetAt( x, y, z ) ;
			if( fValue >= fUpper )
				bConnected2Upper = true ;
		}
		
		if( bConnected2Upper == true )
		{
			for( i = 0 ; i < nPnts ; i++ )
			{
				x = parts[n][i].m_x ;
				y = parts[n][i].m_y ;
				z = parts[n][i].m_z ;
				pLower->SetAt( x, y, z, 1 ) ;
			}
		}
	}

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		iValue = pLower->GetAt( x, y, z ) ;
		if( iValue == 0 )
			SetAt( x, y, z, 0 ) ; // suppress!!
	}

	delete pLower ;
	return true ;
}
*/

bool CFVoxel::Thresholding2Bounds( float fLower, float fUpper )
{
	int x, y, z ;
	float fValue ;

	for( z = 0 ; z < m_nZ ; z++ )
	for( y = 0 ; y < m_nY ; y++ )
	for( x = 0 ; x < m_nX ; x++ )
	{
		fValue = GetAt( x, y, z ) ;
		if( fValue < fLower || fValue > fUpper )
			SetAt( x, y, z, 0.0f ) ; // suppress!!
	}
	return true ;
}

void CFVoxel::AddFVoxel( CFVoxel *addvxl )
{
	VERIFY(m_nX == addvxl->m_nX);	
	VERIFY(m_nY == addvxl->m_nY);
	VERIFY(m_nZ == addvxl->m_nZ);
	int i, n = m_nX*m_nY*m_nZ;
	for ( i = 0; i < n; i++ ) {
		m_pfData[i] += addvxl->m_pfData[i];
	}
}

void CFVoxel::DivideConst( int nVal )
{
	int i, n = m_nX*m_nY*m_nZ;
	float d = 1.f/(float)nVal;
	for ( i = 0; i < n; i++ ) {
		m_pfData[i] = m_pfData[i]*d;
	}	
}

void CFVoxel::DivideConst( float nVal )
{
	int i, n = m_nX*m_nY*m_nZ;
	float d = 1.f/nVal;
	for ( i = 0; i < n; i++ ) {
		m_pfData[i] = m_pfData[i]*d;

		if (m_pfData[i] != 0)
		{
			int ege = 0;
		}
	}	
}

void CFVoxel::MultiplyConst( float fv )
{
	int i, n = m_nX*m_nY*m_nZ;
	for ( i = 0; i < n; i++ ) {
		m_pfData[i] *= fv;
	}	
}

void CFVoxel::MultiplyConst( double fv )
{
	int i, n = m_nX*m_nY*m_nZ;
	for ( i = 0; i < n; i++ ) {
		m_pfData[i] *= fv;
	}	
}

bool CFVoxel::CheckValRange( double s, double e )
{
	bool bInRange = TRUE;
	int i, n = m_nX*m_nY*m_nZ;
	for ( i = 0; i < n; i++ ) {
		if ( m_pfData[i] < s || m_pfData[i] > e ) {
			bInRange = FALSE;
			break;
		}
	}	
	return bInRange;
}

void CFVoxel::MultiplyConst( int nVal )
{
	int i, n = m_nX*m_nY*m_nZ;
	float d = (float)nVal;
	for ( i = 0; i < n; i++ ) {
		/*if (m_pfData[i] == 0)
		{
			m_pfData[i] = 0;
		}
		
		else if ( m_pfData[i] > 0 && m_pfData[i] <= 0.2)
		{
			m_pfData[i] = 50;
		}
		else if ( m_pfData[i] > 0.2 && m_pfData[i] <= 0.4)
		{
			m_pfData[i] = 100;
		}
		else if ( m_pfData[i] > 0.4 && m_pfData[i] <= 0.6 )
		{
			m_pfData[i] = 150;
		}
		else if ( m_pfData[i] > 0.6 && m_pfData[i] <= 0.8 )
		{
			m_pfData[i] = 200;
		}
		else
		{
			m_pfData[i] = 250;
		}*/
		m_pfData[i] *= d;
	}	
}

CFVoxel *CFVoxel::Crop(const C3DPoint &pt, const C3DPoint &size)
{
	if ( 0 <= pt.m_x && pt.m_x < m_nX && 
		0 <= pt.m_y && pt.m_y < m_nY && 
		0 <= pt.m_z && pt.m_z < m_nZ ) {
			if ( 0 <= pt.m_x+size.m_x && pt.m_x+size.m_x < m_nX && 
				0 <= pt.m_y+size.m_y && pt.m_y+size.m_y < m_nY && 
				0 <= pt.m_z+size.m_z && pt.m_z+size.m_z < m_nZ ) {
					int x, y, z, i, idx;
					CFVoxel *cout = new CFVoxel(size);
					for ( z = pt.m_z, i = 0; z < pt.m_z+size.m_z; z++ ) {
						for ( y = pt.m_y; y < pt.m_y+size.m_y; y++ ) {
							for ( x = pt.m_x; x < pt.m_x+size.m_x; x++, i++ ) {
								idx = z*m_nX*m_nY+y*m_nX+x;
								cout->m_pfData[i] = m_pfData[idx];
							}
						}
					}
					return cout;
			}
			else {
				return NULL;
			}
	}
	else return NULL;
}

void CFVoxel::CropFilled(C3DPoint &st, C3DPoint &end, double thres, int BUF)
{
	int i, x, y, z;
	// determine [min max] range in x, y, z directions
	int minX, maxX, minY, maxY, minZ, maxZ;
	minX = minY = minZ = INT_MAX;
	maxX = maxY = maxZ = INT_MIN;
	for( z = 0, i = 0; z < m_nZ ; z++ ) {
		for( y = 0 ; y < m_nY ; y++ ) {
			for( x = 0 ; x < m_nX ; x++, i++ ) {
				if ( m_pfData[i] > thres ) {
					if ( x < minX ) minX = x;
					if ( y < minY ) minY = y;
					if ( z < minZ ) minZ = z;
					if ( x > maxX ) maxX = x;
					if ( y > maxY ) maxY = y;
					if ( z > maxZ ) maxZ = z;
				}
			}
		}
	}

	// assign buffers for shape templates
	minX = std::max(0, minX-BUF);
	minY = std::max(0, minY-BUF);
	minZ = std::max(0, minZ-BUF);
	maxX = std::min(m_nX-1, maxX+BUF);
	maxY = std::min(m_nY-1, maxY+BUF);
	maxZ = std::min(m_nZ-1, maxZ+BUF);

	st.m_x = minX;
	st.m_y = minY;
	st.m_z = minZ;
	end.m_x = maxX;
	end.m_y = maxY;
	end.m_z = maxZ;

	// crop only relevant voxels
	int nX, nY, nZ;
	nX = maxX-minX+1;
	nY = maxY-minY+1;
	nZ = maxZ-minZ+1;

	int tSize = nX * nY * nZ ;

	float *pData = new float[tSize] ;
	for( z = minZ, i = 0; z <= maxZ; z++ ) {
		for( y = minY; y <= maxY; y++ ) {
			for( x = minX; x <= maxX; x++, i++ ) {
				if ( GetAt(x,y,z) > thres ) {
					pData[i] = GetAt(x,y,z);
				}
			}
		}
	}
	// assign new dimensions
	if( m_pfData )
	{
		delete [] m_pfData ;
		m_pfData = NULL ;
	}
	m_nX = nX;
	m_nY = nY;
	m_nZ = nZ;
	m_pfData = pData;
}

int CFVoxel::GetNumber()
{
	int i, j, k;
	int count = 0;
	for ( i = 0 ; i < m_nX ; i++ ) {
		for ( j = 0 ; j < m_nY ; j++ ) {
			for ( k = 0 ; k < m_nZ ; k++ ) {
				if ( (*this).GetAt(i, j, k ) > 0 ) count++;
			}
		}
	}

	return count;
}

double CFVoxel::GetTemplateHammingDistance( CFVoxel* temp, C3DPoint stpt )
{
	C3DPoint enpt;
	enpt.m_x = stpt.m_x + temp->m_nX;
	enpt.m_y = stpt.m_y + temp->m_nY;
	enpt.m_z = stpt.m_z + temp->m_nZ;

	int x,y,z, nx,ny,nz;
	double count = 0;

	for (z = stpt.m_z; z <= enpt.m_z ; z++) {
		for (y = stpt.m_y; y <= enpt.m_y; y++) {
			for (x = stpt.m_x; x <= enpt.m_x; x++) {
				if ( x >= 0 && x < m_nX && y >= 0 && y < m_nY && z >= 0 && z < m_nZ && GetAt(x,y,z) ) {
					
					nx = x - stpt.m_x;
					ny = y - stpt.m_y;
					nz = z - stpt.m_z;

					if ( nx >= 0 && nx < temp->m_nX && ny >= 0 && ny < temp->m_nY && nz >= 0 && nz < temp->m_nZ && temp->GetAt(nx,ny,nz) > 0.5) {
						count += GetAt(x,y,z) * temp->GetAt(nx,ny,nz);
					}
				}
			}
		}
	}

	return count;
}

void CFVoxel::SaveImagePerSlice( CString DirName, CString FileName )
{
	int x, y, z, i;
	for ( z = 0, i=0; z < m_nZ; z++ ) {
		CStringA strDir(DirName);
		CStringA strFile(FileName);
		CStringA strOut;
		strOut.Format("%s\\%s_%04d.bmp", strDir, strFile, z);

		IplImage *sliceImg = cvCreateImage(cvSize(m_nX, m_nY), IPL_DEPTH_8U, 1);
		BYTE *imPtr = (BYTE*) sliceImg->imageData;
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				imPtr[y*sliceImg->widthStep+x] = (BYTE) std::max(std::min(GetAt(x, y, z), 255.f),0.f);
			}
		}
		cvSaveImage(strOut, sliceImg);
		cvReleaseImage(&sliceImg);
	}
}

void CFVoxel::SaveBCIImagePerSlice( C3DPointArray *bciPtArr, C3DPointArray *boiPtArr, 
														CF3DPoint *originPtArr, double *meanDistArr, int nDB, 
														C3DPoint &maporig, double mapSize, CString DirName, CString FileName)
{
	int x, y, z, c, i, j, k;
	int nCh = 3;
	for ( z = 0, i=0; z < m_nZ; z++ ) {
		CStringA strDir(DirName);
		CStringA strFile(FileName);
		CStringA strOut;
		strOut.Format("%s\\%s_%04d.bmp", strDir, strFile, z);

		IplImage *sliceImg = cvCreateImage(cvSize(m_nX, m_nY), IPL_DEPTH_8U, nCh);
		BYTE *imPtr = (BYTE*) sliceImg->imageData;
		for ( y = 0; y < m_nY; y++ ) {
			for ( x = 0; x < m_nX; x++, i++ ) {
				for ( c = 0; c < nCh; c++ ) 
					imPtr[y*sliceImg->widthStep+x*nCh+c] = (BYTE) std::max(std::min(GetAt(x, y, z), 255.f),0.f);
			}
		}

		if ( maporig.m_z == z ) {
			imPtr[maporig.m_y*sliceImg->widthStep+maporig.m_x*nCh+0] = 0;
			imPtr[maporig.m_y*sliceImg->widthStep+maporig.m_y*nCh+1] = 0;
			imPtr[maporig.m_y*sliceImg->widthStep+maporig.m_x*nCh+2] = 255;
		}

		for ( j = 0; j < nDB; j++ ) {
		//j = 0;
			strOut.Format("%s\\%s_BCIBOIPt%d_%04d.bmp", strDir, strFile, j, z);
			IplImage *sliceImg2 = cvCreateImage(cvSize(m_nX, m_nY), IPL_DEPTH_8U, nCh);
			cvSet(sliceImg2, cvScalar(0, 0, 0));
			imPtr = (BYTE*) sliceImg2->imageData;

			double mapX = ( m_nX-maporig.m_x < maporig.m_x ) ? m_nX-maporig.m_x : maporig.m_x;
			double mapY = ( m_nY-maporig.m_y < maporig.m_y ) ?  m_nY-maporig.m_y : maporig.m_y;
			double mapZ = ( m_nZ-maporig.m_z < maporig.m_z ) ?  m_nZ-maporig.m_z : maporig.m_z;
			double c = std::min(1.0,sqrt(mapX*mapX+mapY*mapY+mapZ*mapZ)*mapSize/meanDistArr[j]);

			int nBciPt = (int) bciPtArr[j].GetSize();
			for ( k = 0; k < nBciPt; k++ ) {
				int vx = (int) Round((double)(bciPtArr[j][k].m_x-originPtArr[j].m_fX)*c+maporig.m_x);
				int vy = (int) Round((double)(bciPtArr[j][k].m_y-originPtArr[j].m_fY)*c+maporig.m_y);
				int vz = (int) Round((double)(bciPtArr[j][k].m_z-originPtArr[j].m_fZ)*c+maporig.m_z);
				if ( vz == z ) {
					imPtr[vy*sliceImg->widthStep+vx*nCh+0] = 255;
					imPtr[vy*sliceImg->widthStep+vx*nCh+1] = 0;
					imPtr[vy*sliceImg->widthStep+vx*nCh+2] = 0;
				}
			}
			int nBoiPt = (int) boiPtArr[j].GetSize();
			for ( k = 0; k < nBoiPt; k++ ) {
				int vx = (int) Round((double)(boiPtArr[j][k].m_x-originPtArr[j].m_fX)*c+maporig.m_x);
				int vy = (int) Round((double)(boiPtArr[j][k].m_y-originPtArr[j].m_fY)*c+maporig.m_y);
				int vz = (int) Round((double)(boiPtArr[j][k].m_z-originPtArr[j].m_fZ)*c+maporig.m_z);
				if ( vz == z ) {
					imPtr[vy*sliceImg->widthStep+vx*nCh+0] = 0;
					imPtr[vy*sliceImg->widthStep+vx*nCh+1] = 255;
					imPtr[vy*sliceImg->widthStep+vx*nCh+2] = 0;
				}
			}
			cvSaveImage(strOut, sliceImg2);
			cvReleaseImage(&sliceImg2);
		}
		cvSaveImage(strOut, sliceImg);
		cvReleaseImage(&sliceImg);
	}
}

void CFVoxel::InsertCVoxelIntoCenter(CVoxel* vxl)
{
	int cfx = (int)(m_nX/2.0);
	int cfy = (int)(m_nY/2.0);
	int cfz = (int)(m_nZ/2.0);

	int cvx = (int)(vxl->m_nX/2.0);
	int cvy = (int)(vxl->m_nY/2.0);
	int cvz = (int)(vxl->m_nZ/2.0);

	int nX = vxl->m_nX;
	int nY = vxl->m_nY;
	int nZ = vxl->m_nZ;

	int tx,ty,tz;

	int x,y,z;
	float val;
	for ( z = 0; z < nZ; z++ ) {
		for ( y = 0; y < nY; y++ ) {
			for ( x = 0; x < nX; x++ ) {
				tx = cfx+x-cvx;
				ty = cfy+y-cvy;
				tz = cfz+z-cvz;
				val = GetAt(tx,ty,tz);
				val += vxl->GetAt(x,y,z);
				SetAt(tx,ty,tz,val);
			}
		}
	}

}

// normalize vectors stored as CFVoxel arrays
void NormalizeFVoxelArr(CTypedPtrArray<CPtrArray, CFVoxel*> &vArr)
{
	// i_th element of vector corresponding to point X is stored in vArr[i]->GetAt(X)
	int nv = (int) vArr.GetSize();
	// check if vArr has more than one element
	if ( nv > 0 ) {
		// check if all CFVoxel objects in array have same dimension
		int nx = vArr[0]->m_nX;
		int ny = vArr[0]->m_nY;
		int nz = vArr[0]->m_nZ;
		int x, y, z, i;
		double dtVal; //, sumVal, divVal;
		CFVoxel sumv(nx,ny,nz);
		for ( i = 0; i < nv; i++ ) {
			for ( z = 0; z < nz; z++ ) {
				for ( y = 0; y < ny; y++ ) {
					for ( x = 0; x < nx; x++ ) {
						dtVal = vArr[i]->GetAt(x,y,z);
						dtVal *= dtVal;
						sumv.SetAt(x,y,z,sumv.GetAt(x,y,z)+dtVal);
					}
				}
			}
		}
		for ( i = 0; i < nv; i++ ) {
			for ( z = 0; z < nz; z++ ) {
				for ( y = 0; y < ny; y++ ) {
					for ( x = 0; x < nx; x++ ) {
						if (sumv.GetAt(x,y,z) !=0)	{
							float ke = vArr[i]->GetAt(x,y,z)/sqrt(sumv.GetAt(x,y,z));
							if ( ke < 0 )
							{
								int kew =0;
							}
							vArr[i]->SetAt(x,y,z,vArr[i]->GetAt(x,y,z)/sqrt(sumv.GetAt(x,y,z)));
						}
					}
				}
			}
		}
	}
}



void ConvertFLOATtoCVOXEL(float* farray, CFVoxel* voxel)
{
	int x, y, z, pix;
	for ( z = 0, pix = 0; z < voxel->m_nZ; z++ ) {
		for ( y = 0; y < voxel->m_nY; y++ ) {
			for ( x = 0; x < voxel->m_nX; x++, pix++ ) {
				voxel->SetAt( x, y, z, farray[pix]);
			}
		}
	}
}


void ConvertCVOXELtoFLOAT(CFVoxel* voxel, float* farray)
{
	int x, y, z, pix;
	float val;
	for ( z = 0, pix = 0; z < voxel->m_nZ; z++ ) {
		for ( y = 0; y < voxel->m_nY; y++ ) {
			for ( x = 0; x < voxel->m_nX; x++, pix++ ) {
				val = voxel->GetAt(x,y,z);
				farray[pix] = val;
			}
		}
	}
}

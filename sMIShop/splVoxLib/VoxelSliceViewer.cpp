// Viewer.cpp : implementation file
//

#include "stdafx.h"
#include "VoxelSliceViewer.h"
#include "Voxel.h"
#include "FVoxel.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CVoxelSliceViewer

CVoxelSliceViewer::CVoxelSliceViewer()
{
	m_iRatio = 1.0;
	m_curI = 0;
	m_curOrigI = 0;
	m_nCurCoordX = 0;
	m_nCurCoordY = 0;
	m_nCurCoordZ = 0;
	m_nMinI = SHRT_MAX;
	m_nMaxI = SHRT_MIN;
	m_mode = ZV;
	m_pVoxel = NULL;
	m_bInit = false;
	m_iCurSliceZ = 0;
	m_iCurSliceY = 0;
	m_iCurSliceX = 0;
	m_pGT = NULL;
	m_pSegRes = NULL;
	m_pMK = NULL;
	m_pROI = NULL;
	m_pSlice = NULL;
	m_pFGUProb = NULL;
	m_pFGPProb = NULL;
	m_curPos.SetPoint(0,0);
	m_curTrans.SetPoint(0,0);
	m_bLButtonDown = false;
	m_bRButtonDown = false;
	m_bViewGT = false;
	m_bViewSegRes = false;
	m_iProbTH = 0;
	m_bViewProb = FALSE;
	m_bMarking = FALSE;
	m_bMk3D = FALSE;
	m_brSize = 2.0;
	m_pInputType = 0;
}

CVoxelSliceViewer::~CVoxelSliceViewer()
{
	//m_pVoxel = NULL;
	SAFE_DELETE(m_pSlice);
	

	/*// 추가한것
	SAFE_DELETE(m_pVoxel);
	SAFE_DELETE(m_pMK);
	SAFE_DELETE(m_pGT);
	SAFE_DELETE(m_pSegRes);
	SAFE_DELETE(m_pFGUProb);
	SAFE_DELETE(m_pFGPProb);*/
	
}

BEGIN_MESSAGE_MAP(CVoxelSliceViewer, CStatic)
	//{{AFX_MSG_MAP(CVoxelSliceViewer)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoxelSliceViewer message handlers

void CVoxelSliceViewer::Initialize()
{
	if ( !m_bInit ) {
		CDC *m_pDC = GetDC();
		m_hDC = m_pDC->GetSafeHdc();
		CRect rect;
		this->GetWindowRect(rect);
		m_bInit= true;
		m_PntLeftTop.x = m_PntLeftTop.y = 0;
		m_mode = ZV;
		m_wndWidth = rect.Width()%2==0 ? rect.Width():rect.Width()+1;
		m_wndHeight = rect.Height()%2==0 ? rect.Height():rect.Height()+1;
		//m_wndWidth = rect.Width()+1;
		//m_wndHeight = rect.Height();
		int nByte = m_wndWidth*m_wndHeight*3;
		m_pSlice = new BYTE[nByte];
		for ( int i = 0; i < nByte; i++ ) {
			m_pSlice[i] = 128;
		}

		m_bmi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
		//m_bmi.bmiHeader.biWidth = rect.Width();
		//m_bmi.bmiHeader.biHeight = -rect.Height();
		m_bmi.bmiHeader.biWidth = m_wndWidth;
		m_bmi.bmiHeader.biHeight = -m_wndHeight;
		m_bmi.bmiHeader.biPlanes = 1;
		m_bmi.bmiHeader.biBitCount = 24;
		m_bmi.bmiHeader.biCompression = BI_RGB;
		m_bmi.bmiHeader.biSizeImage = nByte;
		m_bmi.bmiHeader.biXPelsPerMeter=0;
		m_bmi.bmiHeader.biYPelsPerMeter=0;
		m_bmi.bmiHeader.biClrUsed =0;
		m_bmi.bmiHeader.biClrImportant =0;
	}

	Invalidate(FALSE);
}

void CVoxelSliceViewer::OnDestroy() 
{
	CStatic::OnDestroy();
}

void CVoxelSliceViewer::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if( !m_pVoxel )
		return;

	::SetStretchBltMode( m_hDC, COLORONCOLOR );

	if( !SetupDisplayBuffer( ) )  	// setup m_pDisp with the current slice
		return;

	::SetDIBitsToDevice( m_hDC, 
								  m_PntLeftTop.x, m_PntLeftTop.y, 
								  m_wndWidth, m_wndHeight,
								  0, 0, 
								  0, m_wndHeight, 
								  m_pSlice, &m_bmi, 
								  DIB_RGB_COLORS ); // send DIB to DC
}

BYTE HIGHLIGHT = 30;
//BYTE HIGHLIGHT = 70;
BYTE HIGHLIGHT2 = 70;
bool CVoxelSliceViewer::SetupDisplayBuffer()
{
	int nX = m_pVoxel->m_nX;
	int nY = m_pVoxel->m_nY;
	int nZ = m_pVoxel->m_nZ;
	int w = m_wndWidth;
	int h = m_wndHeight;
	int i, j, x, y, z, idx;
	BYTE br, bg, bb, iV;
	double iRange = 255.0/(double)(m_nMaxI-m_nMinI);
	//double iRange = 1;
	switch( m_mode )
	{
	case ZV:
		// check current slice
		if( m_iCurSliceZ < 0 || m_iCurSliceZ >= nZ )
			return false;
		z = m_iCurSliceZ;
		// m_iRatio: 1.0 if 1 pixel for 1 voxel
		//				  2.0 if 2 pixels for 1 voxel
		for ( j = 0; j < h; j++ ) {
			y = (int)Round((double)(j+m_curPos.y)/m_iRatio);
			for ( i = 0; i < w; i++ ) {
				idx = 3*(j*w+i);
				x = (int)Round((double)(i+m_curPos.x)/m_iRatio);
				if ( 0 <= y && y < nY  && 0 <= x && x < nX ) {

					if ( m_pVoxel->GetAt(x, y, z) < m_nMinI ) {
						iV = 0;
					}
					else if ( m_pVoxel->GetAt(x, y, z) > m_nMaxI ) {
						iV = 255;
					}
					else {
						iV = (BYTE)Round((double)(m_pVoxel->GetAt(x, y, z)-m_nMinI)*iRange);
					}
					
					br = bg = bb = iV;


					if ( m_bViewGT && m_pGT && m_bViewSegRes && m_pSegRes ) {
						if ( m_pGT->GetAt(x, y, z) && m_pSegRes->GetAt(x, y, z) ) {
							bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*0.5+HIGHLIGHT, 255.0);
						}

						else if (  m_pGT->GetAt(x, y, z) == 0 && m_pSegRes->GetAt(x, y, z) ) {
							bg = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*0.5+HIGHLIGHT, 255.0);
						}

						else if ( m_pGT->GetAt(x, y, z) && m_pSegRes->GetAt(x, y, z) == 0 ) {
							bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
						}
						else {

						}
					}

					else {
						if ( m_bViewGT && m_pGT ) {
							if ( m_pGT->GetAt(x, y, z) ) {
								br = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
								bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
							}
						}

						if ( m_bViewSegRes && m_pSegRes ) {
							if ( m_pSegRes->GetAt(x, y, z) == 0 ) {
								
							}

							else if ( m_pSegRes->GetAt(x, y, z) == 1 ) {
								//bg = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
								//bb = (BYTE)std::min(bb*0.5+HIGHLIGHT, 255.0);
								bg = (BYTE)std::min(bg*1.3+HIGHLIGHT, 255.0);
								bb = (BYTE)std::min(bb*0.7+HIGHLIGHT, 255.0);
							}
							else if ( m_pSegRes->GetAt(x, y, z) == 2 ) {
								//bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
								//bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
								bg = (BYTE)std::min(bg*0.7+HIGHLIGHT, 255.0);
								bb = (BYTE)std::min(bb*1.3+HIGHLIGHT, 255.0);
								//bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
								//bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
							}
							else if ( m_pSegRes->GetAt(x, y, z) == 3 ) {
								br = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
								bb = (BYTE)std::min(bb*0.5+HIGHLIGHT, 255.0);
								//bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
								//bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
							}
							else if ( m_pSegRes->GetAt(x, y, z) == 4 ) {
								br = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
								bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
							}
							else if ( m_pSegRes->GetAt(x, y, z) == 5 ) {
								br = (BYTE)std::min(br*0.5+HIGHLIGHT, 255.0);
								bg = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
							}
							else if ( m_pSegRes->GetAt(x, y, z) == 6 ) {
								br = (BYTE)std::min(br*1.5+HIGHLIGHT, 255.0);
								bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
							}
							else if ( m_pSegRes->GetAt(x, y, z) == 7 ) {
								br = (BYTE)std::min(br*1.0+HIGHLIGHT, 255.0);
								bg = (BYTE)std::min(bg*1.0+HIGHLIGHT, 255.0);
							}
							else if ( m_pSegRes->GetAt(x, y, z) == 8 ) {
								br = (BYTE)std::min(bb*1.0+HIGHLIGHT, 255.0);
								bg = (BYTE)std::min(bg*1.0+HIGHLIGHT, 255.0);
							}
							else if ( m_pSegRes->GetAt(x, y, z) == 9 ) {
								br = (BYTE)std::min(br*1.0+HIGHLIGHT, 255.0);
								bg = (BYTE)std::min(bb*1.0+HIGHLIGHT, 255.0);
							}
							else if ( m_pSegRes->GetAt(x, y, z) == 10 ) {
								br = (BYTE)std::min(br*0.5+HIGHLIGHT, 255.0);
								bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
							}
							else {
								bg = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
								bb = (BYTE)std::min(bb*0.5+HIGHLIGHT, 255.0);
							}
						}
					}

					if ( m_bViewProb && m_pFGUProb ) {
						if ( m_pFGUProb->GetAt(x, y, z) > m_iProbTH ) {
							//bg = (BYTE)std::min(br*1.5+HIGHLIGHT, 255.0);
							//bb = (BYTE)std::min(bb*2.0+HIGHLIGHT, 255.0);
							br = (BYTE)std::min(br*2.0+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
						}
					}
					if ( m_bViewProb && m_pFGPProb ) {
						if ( m_pFGPProb->GetAt(x, y, z) > m_iProbTH ) {
							//bg = (BYTE)std::min(br*2.0+HIGHLIGHT, 255.0);
							//bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*2.0+HIGHLIGHT, 255.0);
						}
					}

					if ( m_bMarking ) {
						if ( m_pMK ) {
							if ( m_pMK->GetAt(x,y,z) == BYTE_GREEN ) {
								br = 0;
								bg = 255;
								bb = 0;
							}

							else if ( m_pMK->GetAt(x,y,z) == BYTE_RED ) {
								br = 0;
								bg = 0;
								bb = 255;
							}
								
							else if ( m_pMK->GetAt(x,y,z) == BYTE_BG ) {
								br = 255;
								bg = 0;
								bb = 0;
							}

							else if ( m_pMK->GetAt(x,y,z) == BYTE_ERASE ) {
								
								if ( (m_pSegRes && m_pSegRes->GetAt(x,y,z)) || (m_pGT && m_pGT->GetAt(x,y,z)) ) {
									
								}
								else {
									br = bg = bb = iV;
								}
							}

							else {

							}
						}
						if ( m_pROI ) {
							if ( m_pROI->GetAt(x,y,z) ) {
								br = 0;
								bg = 255;
								bb = 0;
							}
						}
						else {
							//br = bg = bb = iV;
						}
					}
				}
				else {
					br = 222; bg = 235; bb = 239;
				}
				m_pSlice[idx] = br;
				m_pSlice[idx+1] = bg;
				m_pSlice[idx+2] = bb;
			}
		}
		break;
	case YV:
		if( m_iCurSliceY < 0 || m_iCurSliceY >= nY )
			return false;
		y = m_iCurSliceY;
		for ( j = 0; j < h; j++ ) {
			z = (int)Round((double)(j+m_curPos.y)/m_iRatio);
			for ( i = 0; i < w; i++ ) {
				idx = 3*(j*w+i);
				x = (int)Round((double)(i+m_curPos.x)/m_iRatio);
				if ( 0 <= z && z < nZ  && 0 <= x && x < nX ) {
					iV = (BYTE)Round((double)(m_pVoxel->GetAt(x, y, z)-m_nMinI)*iRange);
					br = bg = bb = iV;
					if ( m_bViewGT && m_pGT ) {
						if ( m_pGT->GetAt(x, y, z) ) {
							br = (BYTE)std::min(br*2.0+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
						}
					}
					if ( m_bViewSegRes && m_pSegRes ) {
						if ( m_pSegRes->GetAt(x, y, z) == 0 ) {

						}

						else if ( m_pSegRes->GetAt(x, y, z) == 1 ) {
							bg = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*0.5+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 2 ) {
							bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
							//bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
							//bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 3 ) {
							br = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*0.5+HIGHLIGHT, 255.0);
							//bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
							//bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 4 ) {
							br = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 5 ) {
							br = (BYTE)std::min(br*0.5+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 6 ) {
							br = (BYTE)std::min(br*1.5+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 7 ) {
							br = (BYTE)std::min(br*1.0+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bg*1.0+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 8 ) {
							br = (BYTE)std::min(bb*1.0+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bg*1.0+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 9 ) {
							br = (BYTE)std::min(br*1.0+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bb*1.0+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 10 ) {
							br = (BYTE)std::min(br*0.5+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
						}
						else {
							bg = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*0.5+HIGHLIGHT, 255.0);
						}
					}
					if ( m_bViewProb && m_pFGUProb ) {
						if ( m_pFGUProb->GetAt(x, y, z) > m_iProbTH ) {
							bg = (BYTE)std::min(br*1.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*2.0+HIGHLIGHT, 255.0);
						}
					}
					if ( m_bViewProb && m_pFGPProb ) {
						if ( m_pFGPProb->GetAt(x, y, z) > m_iProbTH ) {
							bg = (BYTE)std::min(br*2.0+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
						}
					}


				}
				else {
					br = 222; bg = 235; bb = 239;
				}
				m_pSlice[idx] = br;
				m_pSlice[idx+1] = bg;
				m_pSlice[idx+2] = bb;
			}
		}
		break;
	case XV:
		if( m_iCurSliceX < 0 || m_iCurSliceX >= nX )
			return false;
		x = m_iCurSliceX;
		for ( j = 0; j < h; j++ ) {
			y = (int)Round((double)(j+m_curPos.y)/m_iRatio);
			for ( i = 0; i < w; i++ ) {
				idx = 3*(j*w+i);
				z = (int)Round((double)(i+m_curPos.x)/m_iRatio);
				if ( 0 <= y && y < nY  && 0 <= z && z < nZ ) {
					iV = (BYTE)Round((double)(m_pVoxel->GetAt(x, y, z)-m_nMinI)*iRange);
					br = bg = bb = iV;
					if ( m_bViewGT && m_pGT ) {
						if ( m_pGT->GetAt(x, y, z) ) {
							br = (BYTE)std::min(br*2.0+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
						}
					}
					if ( m_bViewSegRes && m_pSegRes ) {
						if ( m_pSegRes->GetAt(x, y, z) == 0 ) {

						}

						else if ( m_pSegRes->GetAt(x, y, z) == 1 ) {
							bg = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*0.5+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 2 ) {
							bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
							//bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
							//bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 3 ) {
							br = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*0.5+HIGHLIGHT, 255.0);
							//bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
							//bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 4 ) {
							br = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 5 ) {
							br = (BYTE)std::min(br*0.5+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 6 ) {
							br = (BYTE)std::min(br*1.5+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 7 ) {
							br = (BYTE)std::min(br*1.0+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bg*1.0+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 8 ) {
							br = (BYTE)std::min(bb*1.0+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bg*1.0+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 9 ) {
							br = (BYTE)std::min(br*1.0+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bb*1.0+HIGHLIGHT, 255.0);
						}
						else if ( m_pSegRes->GetAt(x, y, z) == 10 ) {
							br = (BYTE)std::min(br*0.5+HIGHLIGHT, 255.0);
							bg = (BYTE)std::min(bg*0.5+HIGHLIGHT, 255.0);
						}
						else {
							bg = (BYTE)std::min(bg*1.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*0.5+HIGHLIGHT, 255.0);
						}
					}
					if ( m_bViewProb && m_pFGUProb ) {
						if ( m_pFGUProb->GetAt(x, y, z) > m_iProbTH ) {
							bg = (BYTE)std::min(br*1.5+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*2.0+HIGHLIGHT, 255.0);
						}
					}
					if ( m_bViewProb && m_pFGPProb ) {
						if ( m_pFGPProb->GetAt(x, y, z) > m_iProbTH ) {
							bg = (BYTE)std::min(br*2.0+HIGHLIGHT, 255.0);
							bb = (BYTE)std::min(bb*1.5+HIGHLIGHT, 255.0);
						}
					}
				}
				else {
					br = 222; bg = 235; bb = 239;
				}
				m_pSlice[idx] = br;
				m_pSlice[idx+1] = bg;
				m_pSlice[idx+2] = bb;
			}
		}
		break;
	default :
		return false;
	}

	return true;
}

void CVoxelSliceViewer::SetSliceIdxX(int n)
{
	m_iCurSliceX = n;
	Invalidate( FALSE );
}

void CVoxelSliceViewer::SetSliceIdxY(int n)
{
	m_iCurSliceY = n;
	Invalidate( FALSE );
}

void CVoxelSliceViewer::SetSliceIdxZ(int n)
{
	m_iCurSliceZ = n;
	Invalidate( FALSE );
}

int CVoxelSliceViewer::GetZoomRatio()
{
	return (int) (m_iRatio*100.0);
}

void CVoxelSliceViewer::LoadInfoParams( CStatic *ctrlStaticCurCoord, 
															 CStatic *ctrlStaticCurI, 
															 CStatic *ctrlStaticMinMaxI, 
															 CSpinButtonCtrl *ctrlStaticZoomRatio)
{
	m_ctrlStaticCurCoord = ctrlStaticCurCoord;
	m_ctrlStaticCurI = ctrlStaticCurI;
	m_ctrlStaticMinMaxI = ctrlStaticMinMaxI;
	m_ctrlStaticZoomRatio = ctrlStaticZoomRatio;
}

void CVoxelSliceViewer::LoadInfoIRange( int isMin, int isMax )
{
	m_ishowMax = isMax;
	m_ishowMin = isMin;
}

void CVoxelSliceViewer::LoadVoxel(CVoxel*pVoxel)
{
	m_pVoxel = pVoxel;
	Initialize();
	GetMinMaxIntensities();
	InitZoomRatio();
}

void CVoxelSliceViewer::LoadMarkingInfo( BOOL bMarking, BOOL b3D, float brSize )
{
	m_bMarking = bMarking;
	m_bMk3D = b3D;
	m_brSize = brSize;
	Invalidate(FALSE);
	//m_pMK = mkVxl;
}

void CVoxelSliceViewer::LoadBrSize(float brSize)
{
	m_brSize = brSize;
	Invalidate(FALSE);
}

void CVoxelSliceViewer::InitZoomRatio()
{
	switch( m_mode )
	{
	case ZV:
		m_iRatio = std::min((double)(m_wndWidth)/(double)m_pVoxel->m_nX, 
							   (double)(m_wndHeight)/(double)m_pVoxel->m_nY);
		break;
	case YV:
		m_iRatio = std::min((double)m_wndWidth/(double)m_pVoxel->m_nX, 
							   (double)m_wndHeight/(double)m_pVoxel->m_nZ);
		break;
	case XV:
		m_iRatio = std::min((double)m_wndWidth/(double)m_pVoxel->m_nZ, 
							   (double)m_wndHeight/(double)m_pVoxel->m_nY);
		break;
	default:
		m_iRatio = 1.0;
		break;
	}
	Invalidate(FALSE);
}

void CVoxelSliceViewer::InitCurPos() 
{
	m_curPos.SetPoint(0,0);
}

// Setting slice mode
void CVoxelSliceViewer::SetMode(int n)
{
	int oldMode = m_mode;
	// set slice mode
	m_mode = n;
	// if new mode, re-initialize viewing buffer
	if ( m_mode != oldMode ) {
		Initialize();
	}
}

void CVoxelSliceViewer::SetSlice(int n)
{
	switch (m_mode) {
		case ZV: 
			m_iCurSliceZ = n;
			break;
		case YV:
			m_iCurSliceY = n;
			break;
		case XV:
			m_iCurSliceX = n;
			break;
	}
}

void CVoxelSliceViewer::GetMinMaxIntensities( )
{
	m_nMinI = SHRT_MAX;
	m_nMaxI = SHRT_MIN;

	int i, x, y, z;
	for ( z = 0, i = 0; z < m_pVoxel->m_nZ; z++ ) {
		for ( y = 0; y < m_pVoxel->m_nY; y++ ) {
			for ( x = 0; x < m_pVoxel->m_nX; x++, i++ ) {
				if ( m_pVoxel->m_pData[i] > m_nMaxI ) {
					m_nMaxI = m_pVoxel->m_pData[i];
				}
				if ( m_pVoxel->m_pData[i] < m_nMinI ) 
					m_nMinI = m_pVoxel->m_pData[i];
			}
		}
	}
	CString str; str.Format(_T("[%d %d]"), m_nMinI, m_nMaxI);
	m_ctrlStaticMinMaxI->SetWindowText(str);
	//m_nMaxI = std::min(m_nMaxI, I_MAX);
	//m_nMinI = std::max(m_nMinI, I_MIN);
	//m_nMaxI = std::min(m_nMaxI, m_ishowMax);
	//m_nMinI = std::max(m_nMinI, m_ishowMin);
	m_nMaxI = std::min((int)m_nMaxI,m_ishowMax);
	m_nMinI = std::max((int)m_nMinI,m_ishowMin);
}

#define ZOOM_DENOM 100.0
void CVoxelSliceViewer::SetZoomRatio(int zoom)
{
	m_iRatio = (double)zoom/ZOOM_DENOM;
	Invalidate(FALSE);
}

#define PROB_DENOM 100.0
void CVoxelSliceViewer::SetViewProbTH(int th)
{
	m_iProbTH = (double)th/PROB_DENOM;
	Invalidate(FALSE);
}

bool CVoxelSliceViewer::LoadMarkingVxl( CVoxel* mkVxl )
{
	if (m_pVoxel->m_nX == mkVxl->m_nX &&
		m_pVoxel->m_nY == mkVxl->m_nY &&
		m_pVoxel->m_nZ == mkVxl->m_nZ ) {
		m_pMK = mkVxl;
		Invalidate(FALSE);
		return true;
	}
	else {
		AfxMessageBox(_T("ground truth dimension does not match current view voxel\n"));
		return false;
	}
}

bool CVoxelSliceViewer::LoadROIVxl( CVoxel* roiVxl )
{
	if (m_pVoxel->m_nX == roiVxl->m_nX &&
		m_pVoxel->m_nY == roiVxl->m_nY &&
		m_pVoxel->m_nZ == roiVxl->m_nZ ) {
			m_pROI = roiVxl;
			Invalidate(FALSE);
			return true;
	}
	else {
		AfxMessageBox(_T("ground truth dimension does not match current view voxel\n"));
		return false;
	}
}

bool CVoxelSliceViewer::LoadGT(CVoxel *pGT)
{
	// check if loaded ground truth has same dimensions as current voxel
	if (m_pVoxel->m_nX == pGT->m_nX &&
		m_pVoxel->m_nY == pGT->m_nY &&
		m_pVoxel->m_nZ == pGT->m_nZ ) {
		m_pGT = pGT;
		m_bViewGT = true;
		Invalidate(FALSE);
		return true;
	}
	else {
		AfxMessageBox(_T("ground truth dimension does not match current view voxel\n"));
		return false;
	}
}

bool CVoxelSliceViewer::LoadUP(CFVoxel *pUP)
{
	// check if loaded ground truth has same dimensions as current voxel
	if (m_pVoxel->m_nX == pUP->m_nX &&
		m_pVoxel->m_nY == pUP->m_nY &&
		m_pVoxel->m_nZ == pUP->m_nZ ) {
		m_pFGUProb = pUP;
		m_bViewProb = true;
		Invalidate(FALSE);
		return true;
	}
	else {
		AfxMessageBox(_T("unary probability dimension does not match current view voxel\n"));
		return false;
	}
}

bool CVoxelSliceViewer::LoadPP(CFVoxel *pPP)
{
	// check if loaded ground truth has same dimensions as current voxel
	if (m_pVoxel->m_nX == pPP->m_nX &&
		m_pVoxel->m_nY == pPP->m_nY &&
		m_pVoxel->m_nZ == pPP->m_nZ ) {
		m_pFGPProb = pPP;
		m_bViewProb = true;
		Invalidate(FALSE);
		return true;
	}
	else {
		AfxMessageBox(_T("pairwise probability dimension does not match current view voxel\n"));
		return false;
	}
}

bool CVoxelSliceViewer::LoadSegRes(CVoxel *pSegRes)
{
	// check if loaded ground truth has same dimensions as current voxel
	if (m_pVoxel->m_nX == pSegRes->m_nX &&
		m_pVoxel->m_nY == pSegRes->m_nY &&
		m_pVoxel->m_nZ == pSegRes->m_nZ ) {
		m_pSegRes = pSegRes;
		m_bViewSegRes = true;
		Invalidate(FALSE);
		return true;
	}
	else {
		AfxMessageBox(_T("segmentation result dimension does not match current view voxel\n"));
		return false;
	}
}

void CVoxelSliceViewer::ClearMK()
{
	m_pMK = NULL;
	Invalidate(FALSE);
}


void CVoxelSliceViewer::ClearGT()
{
	m_pGT = NULL;
	m_bViewGT = false;
	Invalidate(FALSE);
}

void CVoxelSliceViewer::ClearSegRes()
{
	m_pSegRes = NULL;
	m_bViewSegRes = false;
	Invalidate(FALSE);
}

void CVoxelSliceViewer::ClearPP()
{
	m_pFGPProb = NULL;
	m_bViewProb = false;
	Invalidate(FALSE);
}

void CVoxelSliceViewer::ClearUP()
{
	m_pFGUProb = NULL;
	m_bViewProb = false;
	Invalidate(FALSE);
}

void CVoxelSliceViewer::SetInputType( int inputType )
{
	m_pInputType = inputType;
	Invalidate(FALSE);
}

void CVoxelSliceViewer::SaveMRSlice()
{
	CString str =  _T("*bitmap|*.bmp|jpeg compressed|*.jpg|All Files(*.*)|*.*||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_READONLY, str, NULL);
	if( dlg.DoModal() == IDOK ) {
		CString str = dlg.GetPathName();
		SaveBYTE2IplImage(m_pSlice, m_wndWidth, m_wndHeight, 3, CStringA(str));
	}
}

cv::Mat CVoxelSliceViewer::GetCurrentSlice(CVoxel *pVoxel)
{
	switch( m_mode )
	{
	case ZV:
		return pVoxel->VoxelZSlice2cvMat_16S(m_iCurSliceZ);
	case YV:
		return pVoxel->VoxelYSlice2cvMat_16S(m_iCurSliceY);
	case XV:
		return pVoxel->VoxelXSlice2cvMat_16S(m_iCurSliceX);
	default :
		return cv::Mat();
	}
}

void CVoxelSliceViewer::SaveSlice(cv::Mat img, bool norm)
{
	//path 입력시 ~~~.bmp로 입력해야함
	CString str =  _T("*bitmap|*.bmp|jpeg compressed|*.jpg|All Files(*.*)|*.*||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_READONLY, str, NULL);
	if( dlg.DoModal() == IDOK ) {
		CString str = dlg.GetPathName();
		if ( norm ) {
			double minv, maxv;
			cv::minMaxIdx(img, &minv, &maxv);
			cv::Mat ns;
			img.convertTo(ns, CV_8U, 255.0/(maxv-minv));
			cv::imwrite(CStringA(str).GetBuffer(), ns);
		}
		else {
			cv::imwrite(CStringA(str).GetBuffer(), img);
		}
	}
}

void CVoxelSliceViewer::SaveGTSlice()
{
	if ( m_pGT ) {
		UpdateData( TRUE );
		cv::Mat gts = GetCurrentSlice(m_pGT);
		cv::Mat gtvs(gts.rows, gt.cols, CV_8U);
	}
}

void CVoxelSliceViewer::SaveSRSlice()
{
	if ( m_pSegRes ) {
		UpdateData( TRUE );
		cv::Mat srs = GetCurrentSlice(m_pSegRes);
		cv::Mat srvs(srs.rows, srs.cols, CV_8U);
		for ( int y = 0; y < srs.rows; y++ ) {

			for ( int x = 0; x < srs.cols; x++ ) {
				switch (
			}
		}
				uChar = m_pSegRes->GetAt(x, y, iCurSlice, m_mode);
				if ( uChar == 0 ) {
					for ( b= 0; b < c; b++ ) {
						pImg[c*(y*ws+x)+b]=0;
					}
				}
				else if ( uChar == 1 ) {
					for ( b= 0; b < c; b++ ) {
						pImg[c*(y*ws+x)+b]=50;
					}
				}
				else if ( uChar == 2 ) {
					for ( b= 0; b < c; b++ ) {
						pImg[c*(y*ws+x)+b]=100;
					}
				}
				else if ( uChar == 3 ) {
					for ( b= 0; b < c; b++ ) {
						pImg[c*(y*ws+x)+b]=150;
					}
				}
				else if ( uChar == 4 ) {
					for ( b= 0; b < c; b++ ) {
						pImg[c*(y*ws+x)+b]=200;
					}
				}
				else {
					for ( b= 0; b < c; b++ ) {
						pImg[c*(y*ws+x)+b]=FG;
					}
				}
			}
		}
		CString str =  _T("*bitmap|*.bmp|jpeg compressed|*.jpg|All Files(*.*)|*.*||");
		CFileDialog dlg(TRUE, NULL, NULL, OFN_READONLY, str, NULL);
		if( dlg.DoModal() == IDOK ) {
			CString str = dlg.GetPathName();
			SaveImage(str, img);
		}
		

		/*IplImage *img = cvCreateImage(cvSize(nXDisp, nYDisp), IPL_DEPTH_8U, 1);
		BYTE *iptr = (BYTE*)img->imageData;
		for( y = 0; y < nYDisp; y++ ) {
			for( x = 0; x < nXDisp; x++ )	{
				uChar = m_pSegRes->GetAt(x, y, iCurSlice, m_mode);
				if ( uChar > 0) {
					iptr[y*img->widthStep+x] = FG;
				}
				else
					iptr[y*img->widthStep+x] = 0;
			}
		}
		CString str =  _T("*bitmap|*.bmp|jpeg compressed|*.jpg|All Files(*.*)|*.*||");
		CFileDialog dlg(TRUE, NULL, NULL, OFN_READONLY, str, NULL);
		if( dlg.DoModal() == IDOK ) {
			CString str = dlg.GetPathName();
			SaveImage(str, img);
		}*/
		cvReleaseImage(&img);
	}
}

void CVoxelSliceViewer::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( m_pVoxel && m_bInit ) {
		// if L button is currently down
		if ( m_bLButtonDown ) {
			if ( m_bMarking ) {
				int x = (int)((double)(m_curPos.x+point.x)/m_iRatio);
				int y = (int)((double)(m_curPos.y+point.y)/m_iRatio);

				(int)Round((double)(point.x+m_curPos.x)/(double)m_iRatio);

				BYTE byteColor = BYTE_FGF;
				if( nFlags & MK_LBUTTON ) {
					if( ( nFlags & MK_SHIFT ) && !( nFlags & MK_CONTROL ) ) {
						byteColor = BYTE_ERASE ;
					}
					else if( ( nFlags & MK_CONTROL ) && !( nFlags & MK_SHIFT ) )  {
						byteColor = BYTE_BG ;
					}
					else {
						if ( m_pInputType == 0 ) {
							byteColor = BYTE_FGF;
						}
						else {
							byteColor = BYTE_FGT;
						}
					}
					
					Splat( x, y, byteColor, m_bMk3D ) ;
				}
			}
			/*else {
				m_curPos.x += (int)((double)(m_curTrans.x-point.x)*m_iRatio);
				m_curPos.y += (int)((double)(m_curTrans.y-point.y)*m_iRatio);
				m_curTrans = point;
			}*/
		}

		if ( m_bRButtonDown ) {
			m_curPos.x += (int)((double)(m_curTrans.x-point.x)*m_iRatio);
			m_curPos.y += (int)((double)(m_curTrans.y-point.y)*m_iRatio);
			m_curTrans = point;
		}
			 

		CString str;
		int xc, yc, zc;
		int nX = m_pVoxel->m_nX;
		int nY = m_pVoxel->m_nY;
		int nZ = m_pVoxel->m_nZ;
		switch ( m_mode ) {
			case ZV:
				xc = (int)Round((double)(point.x+m_curPos.x)/(double)m_iRatio);
				yc = (int)Round((double)(point.y+m_curPos.y)/(double)m_iRatio);
				zc = m_iCurSliceZ;
				break;
			case YV:
				xc = (int)Round((double)(point.x+m_curPos.x)/(double)m_iRatio);
				yc = m_iCurSliceY;
				zc = (int)Round((double)(point.y+m_curPos.y)/(double)m_iRatio);
				break;
			case XV:
				xc = m_iCurSliceX;
				yc = (int)Round((double)(point.y+m_curPos.y)/(double)m_iRatio);
				zc = (int)Round((double)(point.x+m_curPos.x)/(double)m_iRatio);
				break;
		}
		if ( 0 <= point.x && point.x < m_wndWidth && 
			 0 <= point.y && point.y < m_wndHeight ) {
			m_curI = m_pSlice[3*(point.y*m_wndWidth+point.x)];
			if ( 0 <= xc && xc < nX && 0 <= yc && yc < nY  && 0 <= zc && zc < nZ ) {
				m_curOrigI = m_pVoxel->GetAt( xc, yc, zc );
			}
			else {
				m_curOrigI = -1;
			}
		}
		else {
			m_curI = 0;
			m_curOrigI = 0;
		}
		str.Format(_T("<%d, %d>"), m_curI, m_curOrigI);
		m_ctrlStaticCurI->SetWindowText(str);
		str.Format(_T("(%d, %d, %d)"), xc, yc, zc);
		m_ctrlStaticCurCoord->SetWindowText(str);
	}

    Invalidate(FALSE);
	CStatic::OnMouseMove(nFlags, point);
}

void CVoxelSliceViewer::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = TRUE;
	m_curTrans = point;

	if ( m_bMarking ) {
		int x = (int)((double)(m_curPos.x+point.x)/m_iRatio);
		int y = (int)((double)(m_curPos.y+point.y)/m_iRatio);

		if( ( nFlags & MK_SHIFT ) && !( nFlags & MK_CONTROL ) )
			Splat( x, y, BYTE_ERASE, m_bMk3D ) ;
		else if( ( nFlags & MK_CONTROL ) && !( nFlags & MK_SHIFT ) ) // 02/05/008
		{
			Splat( x, y, BYTE_BG, m_bMk3D ) ;
		}
		//else if( ( nFlags & MK_CONTROL ) && ( nFlags & MK_SHIFT ) )
		//	Splat( x, y, BYTE_BONE, m_bMk3D ) ;
		else {
			BYTE byteColor; 
			if ( m_pInputType == 0 ) {
				byteColor = BYTE_FGF;
			}
			else {
				byteColor = BYTE_FGT;
			}
			Splat( x, y, byteColor, m_bMk3D ) ; // making FG
		}
	}

	CStatic::OnLButtonDown(nFlags, point);
}

void CVoxelSliceViewer::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_bRButtonDown = TRUE;
	m_curTrans = point;
	CStatic::OnRButtonDown(nFlags, point);
}

void CVoxelSliceViewer::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_bRButtonDown = FALSE;
	m_curTrans = point;
	CStatic::OnRButtonUp(nFlags, point);
}


void CVoxelSliceViewer::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDown = FALSE;
	m_curTrans = point;
	/*if ( m_bMarking ) {
		int x = (int)((double)(m_curPos.x+point.x)/m_iRatio);
		int y = (int)((double)(m_curPos.y+point.y)/m_iRatio);

		if( ( nFlags & MK_SHIFT ) && !( nFlags & MK_CONTROL ) )
			Splat( x, y, BYTE_ERASE, m_bMk3D ) ;
		else if( ( nFlags & MK_CONTROL ) && !( nFlags & MK_SHIFT ) ) // 02/05/008
		{
			Splat( x, y, BYTE_BG, m_bMk3D ) ;
		}
		//else if( ( nFlags & MK_CONTROL ) && ( nFlags & MK_SHIFT ) )
		//	Splat( x, y, BYTE_BONE, m_bMk3D ) ;
		else {
			BYTE byteColor; 
			if ( m_pInputType == 0 ) {
				byteColor = BYTE_FGF;
			}
			else {
				byteColor = BYTE_FGT;
			}
			Splat( x, y, byteColor, m_bMk3D ) ; // making FG
		}
	}*/
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CVoxelSliceViewer::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDblClk(nFlags, point);
}

void CVoxelSliceViewer::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	InitZoomRatio();
	m_ctrlStaticZoomRatio->SetPos(GetZoomRatio());
	InitCurPos();
	//m_ctrlStaticZoomRatio->UpdateData(FALSE);
	CStatic::OnRButtonDblClk(nFlags, point);
}

// for editing with the mouse on m_pLabels
// color temporarily set up: FG as vessel of green, BG as ROI of red
bool CVoxelSliceViewer::Splat( int x, int y, BYTE byInLabel, BOOL bMk3D )
{
	UpdateData();

	float fDec, fBrSize0 = m_brSize;	
	if( fBrSize0 < 4.5f ) // for deeper propagation 04/15/009
		fDec = 1.0f;
	else
		fDec = 2.0f;
	
	int si = 0;
	switch( m_mode )
	{
	case ZV:
		for( float fBrSize = fBrSize0; fBrSize > 0.0f; fBrSize -= fDec ) {
			m_pMK->DrawCircleZ(x,y,m_iCurSliceZ+si, fBrSize, byInLabel);
			si++;
			if( !bMk3D ) break;
		}
		break;
	case YV:
		for( float fBrSize = fBrSize0; fBrSize > 0.0f; fBrSize -= fDec ) {
			m_pMK->DrawCircleY(x,y,m_iCurSliceY+si, fBrSize, byInLabel);
			si++;
			if( !bMk3D ) break;
		}
		break;
	case XV:
		for( float fBrSize = fBrSize0; fBrSize > 0.0f; fBrSize -= fDec ) {
			m_pMK->DrawCircleX(x,y,m_iCurSliceY+si, fBrSize, byInLabel);
			si++;
			if( !bMk3D ) break;
		}
		break;
	default :
	}

	/*
	int z, left, right, top, bottom, nXDisp, nYDisp, nZDisp, iCurSlice ;
	float fBrSize, fDec ;

	int nX = m_pVoxel->m_nX;
	int nY = m_pVoxel->m_nY;
	int nZ = m_pVoxel->m_nZ;

	switch( m_mode )
	{
	case ZV:
		nXDisp = nX;
		nYDisp = nY;
		nZDisp = nZ;
		iCurSlice = m_iCurSliceZ;
		break;
	case YV:
		nXDisp = nZ;
		nYDisp = nX;
		nZDisp = nY;
		iCurSlice = m_iCurSliceY;
		break;
	case XV:
		nXDisp = nY;
		nYDisp = nZ;
		nZDisp = nX;
		iCurSlice = m_iCurSliceX;
		break;
	default :
		nXDisp = nX;
		nYDisp = nY;
		nZDisp = nZ;
		iCurSlice = m_iCurSliceZ;
	}

	float fBrSize0 = m_brSize;
	
	if( fBrSize0 < 4.5f ) // for deeper propagation 04/15/009
		fDec = 1.0f ;
	else
		fDec = 2.0f ;
	
	z = 0 ;

	for( fBrSize = fBrSize0  ; fBrSize > 0.0f ; fBrSize -= fDec ) {
		CRect brRc( int( x - fBrSize - 1 ), int( y - fBrSize - 1 ), int( x + fBrSize + 1 ), int( y + fBrSize + 1 ) ) ;
		CRect iRc;
		iRc.IntersectRect( &brRc, &CRect( 0, 0, nXDisp, nYDisp ) ) ;
		if( iRc.Width( ) == 0 || iRc.Height( ) == 0 )
			return false ;

		//for( int k  = 0; k < 2; k++ ) {

			// mark the current slice of m_pMarkings in this viewing class with red or blue and construct a splat mark image
			left = (int)( x - fBrSize ) ;
			right = (int)( x + fBrSize ) ;
			top = (int)( y - fBrSize ) ;
			bottom = (int)( y + fBrSize ) ;
			for ( int i = left; i < right; i++ )
			{
				for ( int j = top; j < bottom; j++ )
				{
					if ( ( i - x ) * ( i - x ) + ( j - y ) * ( j - y ) < fBrSize * fBrSize &&
						i >= 0 && i < nXDisp && j >= 0  && j < nYDisp &&
						iCurSlice + z >= 0 && iCurSlice + z < nZDisp && 
						iCurSlice - z >= 0 && iCurSlice - z < nZDisp )
					{
						int _i, _j ;
						_i = (int)( (float) i + 0.5f ) ;
						_j = (int)( (float) j + 0.5f ) ;

						m_pMK->SetAt( i, j, iCurSlice + z, byInLabel, m_mode ) ;
						m_pMK->SetAt( i, j, iCurSlice - z, byInLabel, m_mode ) ;
						m_pMK->SetAt( _i, _j, iCurSlice + z, byInLabel, m_mode ) ;
						m_pMK->SetAt( _i, _j, iCurSlice - z, byInLabel, m_mode ) ;

						// when editing on m_pLabel largely modified
						// for the ROI boundary of BYTE_RED not to be affected by BYTE_GREEN and BYTE_ERASE
						/*int iValue ;
						if( !m_bMkBin && !m_bEnableRed )
						{
							iValue = m_pMK->GetAt( i, j, iCurSlice + z, m_mode ) ;
							if( iValue != BYTE_RED )
								m_pMK->SetAt( i, j, iCurSlice + z, byLabel, m_mode ) ;

							iValue = m_pMK->GetAt( i, j, iCurSlice - z, m_mode ) ;
							if( iValue != BYTE_RED )
								m_pMK->SetAt( i, j, iCurSlice - z, byLabel, m_mode ) ;

							iValue = m_pMK->GetAt( _i, _j, iCurSlice + z, m_mode ) ;
							if( iValue != BYTE_RED )
								m_pMK->SetAt( _i, _j, iCurSlice + z, byLabel, m_mode ) ;

							iValue = m_pMK->GetAt( _i, _j, iCurSlice - z, m_mode ) ;
							if( iValue != BYTE_RED )
								m_pMK->SetAt( _i, _j, iCurSlice - z, byLabel, m_mode ) ;
						}
						else
						{
							m_pMK->SetAt( i, j, iCurSlice + z, byLabel, m_mode ) ;
							m_pMK->SetAt( i, j, iCurSlice - z, byLabel, m_mode ) ;
							m_pMK->SetAt( _i, _j, iCurSlice + z, byLabel, m_mode ) ;
							m_pMK->SetAt( _i, _j, iCurSlice - z, byLabel, m_mode ) ;
						}* /
					}
				}
			}

			z++ ;
		//}

		if( !bMk3D )
			break ;
	}
	*/
	return true ;
}



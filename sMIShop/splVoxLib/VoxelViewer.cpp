// Viewer.cpp : implementation file
//

#include "stdafx.h"
#include "VoxelViewer.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImg
/*
CCaptureImg::CCaptureImg()
{
	width = 0;
	height = 0;
	pBuffer = NULL;
}

CCaptureImg::CCaptureImg(int w, int h)
{ 
	width = w;
	height = h;
	pBuffer = new BYTE[w*h*3];
}

CCaptureImg::~CCaptureImg()
{
	if ( pBuffer ) {
		delete [] pBuffer;
	}
}

void CCaptureImg::SetImg(int w, int h)
{
	width = w-1;
	height = h;
	pBuffer = new BYTE[(w)*h*3];
}	


void CCaptureImg::SaveCaptureImg(CString file)
{
	if ( pBuffer ) {
		int i, j;
		IplImage *pImage = cvCreateImage(cvSize(width, height ), IPL_DEPTH_8U, 3 );
		for ( i = 0 ; i < width; i++ ) {
			for ( j = 0 ; j < height ; j++ ) {
				pImage->imageData[i,height-1-j,2] = pBuffer[(j*(width)+i)*3  ];
				(*pImage)(i,height-1-j,1) = pBuffer[(j*(width)+i)*3+1];
				(*pImage)(i,height-1-j,0) = pBuffer[(j*(width)+i)*3+2];
				//(*pImage)(i,j,2) = pBuffer[j*3*width+3*i];
				//(*pImage)(i,j,1) = pBuffer[j*3*width+3*i+1];			
				//(*pImage)(i,j,0) = pBuffer[j*3*width+3*i+2];			
			}
		}
		cvSaveImage(file, pImage);
		cvReleaseImage(&pImage);
	}
}
*/

/////////////////////////////////////////////////////////////////////////////
// CVoxelViewer

CVoxelViewer::CVoxelViewer()
{
	m_Mode = 0;
	
	m_Azimuth = 0;
	m_Elevation = 0;
	m_stepAzimuth = m_stepElevation = 0.5f;

	m_FOV = 60.0f;

	m_TransX = m_TransY = 0;
	m_stepTranX = m_stepTranY = 0;
	
	m_Distance = 0;	
	m_stepDistance = 0;

	m_Near = 1/INF;
	m_Far = INF;

	m_PrevPoint.x = 0;
	m_PrevPoint.y = 0;

	m_bAxis = TRUE;

	m_fEyePositionX = 0;
	m_fEyePositionY = 0;
	m_fEyePositionZ = 0;

	m_fEyeNormX = 0;
	m_fEyeNormY = 1;
	m_fEyeNormZ = 0;

	m_pVoxel = NULL;
	m_pVoxel2 = NULL;

}

CVoxelViewer::~CVoxelViewer()
{
}


BEGIN_MESSAGE_MAP(CVoxelViewer, CStatic)
	//{{AFX_MSG_MAP(CVoxelViewer)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoxelViewer message handlers

void CVoxelViewer::Initialize()
{
	PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
        1,	// version number
        PFD_DRAW_TO_WINDOW |	// support window
        PFD_SUPPORT_OPENGL |	// support OpenGL
        PFD_DOUBLEBUFFER,	// double buffered
        PFD_TYPE_RGBA,	// RGBA type
        24,	// 24-bit color depth
        0, 0, 0, 0, 0, 0,	// color bits ignored
        0,	// no alpha buffer
        0,	// shift bit ignored
        0,	// no accumulation buffer
        0, 0, 0, 0, 	// accum bits ignored
        32,	// 32-bit z-buffer	
        0,	// no stencil buffer
        0,	// no auxiliary buffer
        PFD_MAIN_PLANE,	// main layer
        0,	// reserved
        0, 0, 0	// layer masks ignored
    };
		
	// get the device context's best-available-match pixel format
	//CClientDC *m_pDC = new CClientDC(this);
	CDC *m_pDC = GetDC();
	m_hDC = m_pDC->GetSafeHdc();
    int iPixelFormat = ChoosePixelFormat( m_hDC, &pfd );
	// make that the device conext's current pixel format
	SetPixelFormat(m_hDC, iPixelFormat, &pfd);
	// if we can create a rendering context ...
	m_hRC = wglCreateContext ( m_hDC );
	// if we can use GL functions for initializing
	wglMakeCurrent(m_hDC,m_hRC);

	//////////////////// OpenGL state variable  ///////////////////////
	glShadeModel (GL_SMOOTH);
    
	// Initialize the lighting condition
    
    glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHTING);
	
	glDepthFunc(GL_LEQUAL);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);

	//glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
	
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glEnable (GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	CRect rect;
	GetClientRect(rect);
    glViewport( 0, 0, rect.Width(), rect.Height() );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( m_FOV, (double) rect.Width()/ (double) rect.Height(), 0.01, m_Far );

	wglMakeCurrent(m_hDC,NULL);
	Invalidate(FALSE);
}



void CVoxelViewer::OnDestroy() 
{
	wglDeleteContext( m_hRC );

	CStatic::OnDestroy();
}

void CVoxelViewer::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	wglMakeCurrent(m_hDC,m_hRC);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawScene();
	glEnable(GL_LIGHTING);
	glPushAttrib(GL_CURRENT_BIT|GL_ENABLE_BIT|GL_POINT_BIT|GL_LINE_BIT);

	//if ( m_nMode != 2 ) {	
	//	glDisable(GL_LIGHTING);
	//	glColor3f(0.0f,0.0f,0.0f);
	//	glPointSize(2.0);
	//}

	if ( m_pVoxel ) {
	//	if ( m_nMode == 2 ) {
			//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
			GLfloat params[4] = {0.2f, 0.2f, 0.2f, 1.0f};
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, params);
			DrawVoxel(m_pVoxel);
			//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
			//glLightModeliv(GL_LIGHT_MODEL_AMBIENT, GL_FALSE);
	//	} else {
	//		glColor3f(0.0f, 1.0f, 0.0f); DrawVoxel();
	//	}
	}
	if ( m_pVoxel2 ) {
		//	if ( m_nMode == 2 ) {
		//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		GLfloat params[4] = {0.2f, 0.2f, 0.2f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, params);
		DrawVoxel(m_pVoxel2, true);
		//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
		//glLightModeliv(GL_LIGHT_MODEL_AMBIENT, GL_FALSE);
		//	} else {
		//		glColor3f(0.0f, 1.0f, 0.0f); DrawVoxel();
		//	}
	}

	glPopAttrib();

	glFlush();
	SwapBuffers( m_hDC );
	wglMakeCurrent(m_hDC,NULL);
}

void CVoxelViewer::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
    m_Mode |= LBUTTON;
    m_PrevPoint = point;
	Invalidate(FALSE);
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CVoxelViewer::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( GetCapture() != this ) return;
    m_Mode &= ~LBUTTON;
    if ( m_Mode == 0 ) ReleaseCapture();
	Invalidate(FALSE);
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CVoxelViewer::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( GetCapture() != this ) return;

    int dx = point.x - m_PrevPoint.x;
    int dy = point.y - m_PrevPoint.y;
    
    if ( m_Mode == LBUTTON ) {
		if ( !(nFlags & MK_CONTROL) ) {
			m_Azimuth -= m_stepAzimuth * dx;
			m_Elevation += m_stepElevation * dy;
			if ( m_Elevation <= -90.0 ) {
				m_Elevation = -90.0 + 1e-5;
			}
			else if ( m_Elevation >= 90.0 ) {
				m_Elevation = 90.0 - 1e-5;
			}
		}
		else {
			m_TransX += m_stepTranX * dx;
			m_TransY -= m_stepTranY * dy;
		}
    }
    else if ( m_Mode == RBUTTON ) {
		m_Distance -= m_stepDistance * dy;
		
        if ( m_Distance < 1e-5 ) {
            m_Distance = 1e-5;
        }
        if ( m_Distance > INF ) {
            m_Distance = INF;
        }
    }
	
    Invalidate(FALSE);

    m_PrevPoint = point;
	
	CStatic::OnMouseMove(nFlags, point);
}

void CVoxelViewer::OnRButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
    m_Mode |= RBUTTON;
    m_PrevPoint = point;
	Invalidate(FALSE);
	
	CStatic::OnRButtonDown(nFlags, point);
}

void CVoxelViewer::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if ( GetCapture() != this ) return;
    m_Mode &= ~RBUTTON;    
    if ( m_Mode == 0 ) ReleaseCapture();
	Invalidate(FALSE);
	
	CStatic::OnRButtonUp(nFlags, point);
}

void CVoxelViewer::DrawScene()
{ 
	m_fEyePositionX = m_Distance*sin(m_Azimuth*PI/180.0)*cos(m_Elevation*PI/180.0);
	m_fEyePositionY = m_Distance*sin(m_Elevation*PI/180.0);
	m_fEyePositionZ = m_Distance*cos(m_Azimuth*PI/180.0)*cos(m_Elevation*PI/180.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslated(m_TransX, m_TransY, 0);

	gluLookAt( m_fEyePositionX, m_fEyePositionY, m_fEyePositionZ, 
			   0, 0, 0, m_fEyeNormX, m_fEyeNormY, m_fEyeNormZ );

	//switch ( m_nMode ) {
	//case 0:
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	//	break;	
	//case 1:
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//	break;
	//case 2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//	break;
	//}

	if ( m_bAxis ) 
		DrawAxis();
}

void CVoxelViewer::DrawAxis()
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 100000.0f, 0.0f, 0.0f );

		glColor3f( 0.0f, 1.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 100000.0f, 0.0f );

		glColor3f( 0.0f, 0.0f, 1.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 100000.0f );
	glEnd();
	glPopAttrib();
}

void CVoxelViewer::SetupVoxelDistance(float scale)
{
	float length = 0;
	if ( m_pVoxel) {
		length = (float) sqrt((double)(SQR(m_pVoxel->GetXSize())+
												   SQR(m_pVoxel->GetXSize())+
												   SQR(m_pVoxel->GetXSize())));
	}
	m_Distance = 0.5f*length/tan(m_FOV*PI/360.0);	
	m_stepDistance = m_Distance/10;

	m_TransX = m_TransY = 0;
	m_stepTranX = m_stepTranY = length/100;
}

void CVoxelViewer::DrawCube(float a, float b, float c)
{
	glPushMatrix();
	glScalef(a, b, c);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, 0.5f);		
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, 0.5f);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(0.5f, 0.5f, -0.5f);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glEnd();
	glPopMatrix();
}

void CVoxelViewer::DrawWireCube(float a, float b, float c)
{
	glPushMatrix();
	glScalef(a, b, c);
	glBegin(GL_LINE_STRIP);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, 0.5f);		
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glEnd();
	
	glBegin(GL_LINE_STRIP);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glEnd();
	glPopMatrix();
}

#define VOXEL_SCALE 1
void CVoxelViewer::DrawVoxel(CVoxel *pVoxel, bool b)
{	
	int i, j, k;
	int XSize = pVoxel->GetXSize();
	int YSize = pVoxel->GetYSize();
	int ZSize = pVoxel->GetZSize();
	short int pv;
	for ( i = 0 ; i < XSize ; i++ ) {
		for ( j = 0 ; j < YSize ; j++ ) {
			for ( k = 0 ; k < ZSize ; k++ ) {
				/*
				if ( i == 0 && j == 0 && k == 0 ) {
					glPushMatrix();
					//glTranslatef(i - (float) XSize/2,j - (float) YSize/2,k - (float) ZSize/2);
					glTranslatef(i- (float) XSize/2,(YSize-j)-(float) YSize/2,(ZSize-k) - (float) ZSize/2);
					DrawCube(VOXEL_SCALE, VOXEL_SCALE, VOXEL_SCALE);
					glPopMatrix();
				}
				*/
				pv = pVoxel->GetAt(i, j, k);
				if ( pv ) {
					if ( ( 0 <= i-1 && i+1 < XSize && 
						0 <= j-1 && j+1 < YSize &&
						0 <= k-1 && k+1 < ZSize ) &&
						( pVoxel->GetAt(i-1,j,k) != pv|| 
						  pVoxel->GetAt(i+1,j,k) != pv || 
						  pVoxel->GetAt(i,j-1,k) != pv ||
						  pVoxel->GetAt(i,j+1,k) != pv ||
						  pVoxel->GetAt(i,j,k-1) != pv ||
						  pVoxel->GetAt(i,j,k+1) != pv ) ||
						  ( 0 == i || i+1 == XSize ||
							0 == j || j+1 == YSize ||
							0 == k || k+1 == ZSize ) ) {
							glPushMatrix();
							//glTranslatef((XSize-i) - (float) XSize/2,(YSize-j)-(float) YSize/2,(ZSize-k) - (float) ZSize/2);
							glTranslatef(i- (float) XSize/2,(YSize-j)-(float) YSize/2,(ZSize-k) - (float) ZSize/2);
							if ( b )
							switch (pv) {
							case FG:
								glColor4f(0.8f, 0.7f, 0.9f, 1.0f); 
								DrawCube(VOXEL_SCALE, VOXEL_SCALE, VOXEL_SCALE);
								break;
							case UNK:
								glColor4f(0.7f, 0.7f, 0.9f, 0.2f); 
								DrawCube(VOXEL_SCALE, VOXEL_SCALE, VOXEL_SCALE);
								break;
							case 1:
								glColor4f(0.5f, 0.5f, 0.7f, 0.5f); 
								DrawCube(VOXEL_SCALE, VOXEL_SCALE, VOXEL_SCALE);
								break;
							case 2:
								glColor4f(0.5f, 0.7f, 0.7f, 0.5f); 
								DrawCube(VOXEL_SCALE, VOXEL_SCALE, VOXEL_SCALE);
								break;
							}
							else 
							switch (pv) {
								case FG:
									glColor4f(0.7f, 0.6f, 0.6f, 1.0f); 
									DrawCube(VOXEL_SCALE, VOXEL_SCALE, VOXEL_SCALE);
									break;
								case UNK:
									glColor4f(0.5f, 0.5f, 0.5f, 0.2f); 
									DrawCube(VOXEL_SCALE, VOXEL_SCALE, VOXEL_SCALE);
									break;
								case 1:
									glColor4f(0.3f, 0.3f, 0.3f, 0.5f); 
									DrawCube(VOXEL_SCALE, VOXEL_SCALE, VOXEL_SCALE);
									break;
								case 2:
									glColor4f(0.3f, 0.5f, 0.3f, 0.5f); 
									DrawCube(VOXEL_SCALE, VOXEL_SCALE, VOXEL_SCALE);
									break;
							}
							glPopMatrix();
					}
				}
			}
		}
	}
	glPushMatrix();
	DrawWireCube(VOXEL_SCALE*(float)XSize, VOXEL_SCALE*(float)YSize, VOXEL_SCALE*(float)ZSize);
	glPopMatrix();
}

/*
const int tex_width = 512;
const int tex_height = 512;
static GLubyte tex_rgb[tex_height][tex_width][3];
static GLuint tex_index[1];
void CVoxelViewer::DrawObject()
{
	int i;
	double x_dif = (m_pObject->m_Max[0] + m_pObject->m_Min[0])/2;
	double y_dif = (m_pObject->m_Max[1] + m_pObject->m_Min[1])/2;
	double z_dif = (m_pObject->m_Max[2] + m_pObject->m_Min[2])/2;

	for ( i = 0 ; i < m_pObject->m_pMesh.GetSize() ; i++ ) {
		int indexA = m_pObject->m_pMesh[i]->a;
		int indexB = m_pObject->m_pMesh[i]->b;
		int indexC = m_pObject->m_pMesh[i]->c;
		
		glBegin(GL_POLYGON);		
		glNormal3d( m_pObject->m_pMesh[i]->normal.x,
			m_pObject->m_pMesh[i]->normal.y,
			m_pObject->m_pMesh[i]->normal.z);
		glVertex3d( m_pObject->m_pVertex[indexA]->x - x_dif,
            m_pObject->m_pVertex[indexA]->y - y_dif,
            m_pObject->m_pVertex[indexA]->z - z_dif);
        glVertex3d( m_pObject->m_pVertex[indexB]->x - x_dif,
            m_pObject->m_pVertex[indexB]->y - y_dif,
            m_pObject->m_pVertex[indexB]->z - z_dif);
        glVertex3d( m_pObject->m_pVertex[indexC]->x - x_dif,
            m_pObject->m_pVertex[indexC]->y - y_dif,
            m_pObject->m_pVertex[indexC]->z - z_dif);
		glEnd();
	}
}
*/

/*
void CVoxelViewer::LoadModel(CModel *pModel)
{
	m_pObject = pModel;
	SetupAngle();
	SetupDistance();
	//Invalidate(FALSE);
}
*/

/*
void CVoxelViewer::ClearModel()
{
	m_pObject = NULL;
	Invalidate(FALSE);
}
*/

void CVoxelViewer::LoadVoxel(CVoxel*pVoxel)
{
	m_pVoxel = pVoxel;
	SetupAngle();
	SetupVoxelDistance(VOXEL_SCALE);
	Invalidate(FALSE);
}

void CVoxelViewer::LoadVoxel2(CVoxel*pVoxel)
{
	m_pVoxel2 = pVoxel;
	SetupAngle();
	SetupVoxelDistance(VOXEL_SCALE);
	Invalidate(FALSE);
}

void CVoxelViewer::SetupAngle()
{
	m_Azimuth = 0;
	m_Elevation = 0;
}
/*
// BYTE *CVoxelViewer::CaptureView(): returns the captured image of current view as BYTE pBuffer
CCaptureImg *CVoxelViewer::CaptureView()
{
	OnPaint();
	CRect rect;
	GetClientRect(rect);

	CCaptureImg *img = new CCaptureImg(rect.Width()-1, rect.Height());

	int size[4];
	
	wglMakeCurrent(m_hDC,m_hRC);
	glGetIntegerv(GL_VIEWPORT, size);
	
	glReadPixels(size[0], size[1], size[2]-1, size[3], GL_RGB,  GL_UNSIGNED_BYTE, img->pBuffer);
	wglMakeCurrent(m_hDC,NULL);
	return img;
}
*/

double CVoxelViewer::GetEyePositionX()
{
	return m_fEyePositionX;
}

double CVoxelViewer::GetEyePositionY()
{
	return m_fEyePositionY;
}

double CVoxelViewer::GetEyePositionZ()
{
	return m_fEyePositionZ;
}

void CVoxelViewer::SetEyePositionX(double x)
{
	m_fEyePositionX = x;
}

void CVoxelViewer::SetEyePositionY(double y)
{
	m_fEyePositionY = y;
}

void CVoxelViewer::SetEyePositionZ(double z)
{
	m_fEyePositionZ = z;
}

double CVoxelViewer::GetEyeNormX()
{
	return m_fEyeNormX;
}

double CVoxelViewer::GetEyeNormY()
{
	return m_fEyeNormY;
}

double CVoxelViewer::GetEyeNormZ()
{
	return m_fEyeNormZ;
}

void CVoxelViewer::SetEyeNormX(double x)
{
	m_fEyeNormX = x;
}

void CVoxelViewer::SetEyeNormY(double y)
{
	m_fEyeNormY = y;
}

void CVoxelViewer::SetEyeNormZ(double z)
{
	m_fEyeNormZ = z;
}

void CVoxelViewer::SetViewPoint()
{
	m_Distance = sqrt(m_fEyePositionX*m_fEyePositionX + 
					  m_fEyePositionY*m_fEyePositionY + 
					  m_fEyePositionZ*m_fEyePositionZ);
	if ( m_fEyePositionZ >= 0 ) {
		m_Azimuth = 180.0/PI*asin(m_fEyePositionX / sqrt(m_fEyePositionX*m_fEyePositionX+m_fEyePositionZ*m_fEyePositionZ));
	}
	else {
		m_Azimuth = -180.0/PI*asin(m_fEyePositionX / sqrt(m_fEyePositionX*m_fEyePositionX+m_fEyePositionZ*m_fEyePositionZ))+180;
	}
	m_Elevation = 180.0/PI*asin(m_fEyePositionY/m_Distance);

	m_stepDistance = m_Distance/10;

	m_TransX = m_TransY = 0;
	m_stepTranX = m_stepTranY = m_Distance/100;

}
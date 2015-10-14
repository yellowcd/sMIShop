// Navigator.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "sMIShop.h"
#include "Navigator.h"
#include "Resource.h"
#include "MainFrm.h"
#include "FileView.h"
//#include "sMIShopView.h"
#include "sMIShopDoc.h"



// CNavigator

IMPLEMENT_DYNAMIC(CNavigator, CDockablePane)


CNavigator::CNavigator()
{
	m_bOpenFile = false;
	m_iOldZoomValue = 100;
	m_bZoomSlider = false;
	m_bUpdateSlicePos = true;
	m_iCurPos = 0;
	m_iXYZSelected = 0; // Z = 0; Y = 1; X = 2;
	m_iCurSliceX = 0; m_iCurSliceY = 0; m_iCurSliceZ = 0;
}

CNavigator::~CNavigator()
{
}


BEGIN_MESSAGE_MAP(CNavigator, CDockablePane)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_UPDATE_COMMAND_UI(IDC_EXTENDED_EDIT, &CNavigator::OnUpdateExtendedEdit)
	ON_WM_PAINT()
	ON_COMMAND(ID_VIEW_NAVIVIEW, &CNavigator::OnViewNaviview)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NAVIVIEW, &CNavigator::OnUpdateViewNaviview)
	ON_COMMAND(IDC_EXTENDED_EDIT, &CNavigator::OnExtendedEdit)
	ON_UPDATE_COMMAND_UI(IDS_GETDATA, &CNavigator::OnUpdateIdsGetdata)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_UPDATE_COMMAND_UI(IDC_RADIO_Z, &CNavigator::OnUpdateRadioZ)
	ON_COMMAND(IDC_RADIO_Z, &CNavigator::OnRadioZ)
	ON_COMMAND(IDC_RADIO_Y, &CNavigator::OnRadioY)
	ON_COMMAND(IDC_RADIO_X, &CNavigator::OnRadioX)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



// CNavigator 메시지 처리기입니다.




int CNavigator::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	//// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	//// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;


	////레이어
	//m_wndBkLayer.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, rectDummy, this, ID_NAVIGATOR_LAYER);

	//if (!m_wndNaviView.Create(dwViewStyle, rectDummy, this, 4))
	//{
	//	TRACE0("Failed to create file view\n");
	//	return -1;      // fail to create
	//}

	//// Load view images:
	//m_NaviViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	//m_wndNaviView.SetImageList(&m_NaviViewImages, TVSIL_NORMAL);

	//m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	//m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE);// Is locked /);

	//OnChangeVisualStyle();

	//m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	//m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	//m_wndToolBar.SetOwner(this);

	//// All commands will be routed via this control , not via the parent frame:
	//m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	//// Fill in some static tree view data (dummy code, nothing magic here)
	
	

	AdjustLayout();

	FillNaviView();

	return 0;
}



void CNavigator::FillNaviView()
{
	//네비게이터 확대 slider
	m_NaviExtendedSlider.Create(TBS_HORZ | WS_BORDER, CRect(10, 150, 200, 180), this, IDC_EXTENDED_SLIDER);
	m_NaviExtendedSlider.ShowWindow(SW_SHOW);
	m_NaviExtendedSlider.SetRange(5, 800);
	m_NaviExtendedSlider.SetPos(100);
	int Extended_Ratio = 0;

	Extended_Ratio = m_NaviExtendedSlider.GetPos();

	//확대 에디트 ctrl
	CString Set_Ratio;
	Set_Ratio.Format(_T("%d"), Extended_Ratio);
	m_NaviExtendedEdit.Create(ES_AUTOHSCROLL | WS_BORDER, CRect(210, 150, 240, 180), this, IDC_EXTENDED_EDIT);
	m_NaviExtendedEdit.SetWindowText(Set_Ratio );
	//m_NaviExtendedEdit.SetCueBanner(Set_Ratio);
	m_NaviExtendedEdit.ShowWindow(SW_SHOW);
	
	//네비게이터 view and 확대 static
	m_NaviExtendedView.Create(CString(""), /*WS_BORDER*/NULL, CRect(10, 10, 110, 110), this);
	m_NaviExtendedView.ShowWindow(SW_SHOW);

	//axis select button ctrl
	m_Button_XYZGroupBox.Create(_T("XYZ Slider Control"), BS_GROUPBOX, CRect(10, 200, 300, 320), this, IDC_BUTTON_XYZ_GROUPBOX);
	m_Button_Z.Create(_T("Z"), BS_AUTORADIOBUTTON | WS_GROUP, CRect(20, 230, 50, 260), this, IDC_RADIO_Z);
	m_Button_Y.Create(_T("Y"), BS_AUTORADIOBUTTON, CRect(20, 260, 50, 290), this, IDC_RADIO_Y);
	m_Button_X.Create(_T("X"), BS_AUTORADIOBUTTON, CRect(20, 290, 50, 320), this, IDC_RADIO_X);
	
	//axis select button init
	m_Button_Z.SetCheck(1);


	m_Button_XYZGroupBox.ShowWindow(SW_SHOW);
	m_Button_Z.ShowWindow(SW_SHOW);
	m_Button_Y.ShowWindow(SW_SHOW);
	m_Button_X.ShowWindow(SW_SHOW);

	//네비게이터 View slider ctrl
	m_ViewSlider.Create(SBS_HORZ, CRect(70, 250, 270, 280), this, IDC_VIEW_SLIDER);
	m_ViewSlider.ShowWindow(SW_SHOW);

	//네비게이터 View slider position ctrl
	m_SlicePos.Create(_T("0 / 0"), NULL, CRect(50, 230, 200, 250), this);
	m_SlicePos.ShowWindow(SW_SHOW);

	//CDC *boxdc = m_Button_XYZGroupBox.GetDC();
	//CRect boxrc;
	//m_Button_XYZGroupBox.GetClientRect(&boxrc);

	//m_Button_XYZGroupBox.GetDC()->FillSolidRect(&boxrc, RGB(0, 0, 0));

}

void CNavigator::AdjustLayout()
{
	
	/*if (GetSafeHwnd() == NULL)
	{
		return;
	}*/

	CRect rectClient;
	GetClientRect(rectClient);

	
//	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	//m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	//m_wndNaviView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	//m_wndBkLayer.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	//m_wndBkLayer.GetDC()->FillSolidRect(&rectClient, RGB(255, 255, 255));

	
}

void CNavigator::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (!m_bOpenFile)
		return;

	CString tmep_str;
	int temp_int;
	if (*pScrollBar == m_NaviExtendedSlider)
	{
		temp_int = m_NaviExtendedSlider.GetPos();
		tmep_str.Format(_T("%d"), temp_int);
		SetWindowTextW(tmep_str);
		OnPaint();
		if (m_iOldZoomValue != temp_int)
		{
			m_bZoomSlider = true;
			m_pView->m_fRatio = temp_int / 100.0f;
			m_pView->OnDraw(m_pView->GetDC());
		}
	}
	else if (*pScrollBar == m_ViewSlider)
	{
		//m_pView->
		//m_bUpdateSlicePos = true;
		if (m_iXYZSelected == 0)
			m_pView->m_iVSliceZ = m_ViewSlider.GetPos();
		else if (m_iXYZSelected == 1)
			m_pView->m_iVSliceY = m_ViewSlider.GetPos();
		else if (m_iXYZSelected == 2)
			m_pView->m_iVSliceX = m_ViewSlider.GetPos();
		
		m_pView->bUpdateSlice = true;
		m_pView->OnDraw(m_pView->GetDC());
	
		//Invalidate(false);
	}

	CDockablePane::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CNavigator::OnUpdateExtendedEdit(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	if (!m_bOpenFile)
			return;
	CString str_temp;
	m_NaviExtendedEdit.GetWindowTextW(str_temp);
	m_int_NaviExtendedEdit = _ttoi(str_temp);
	m_NaviExtendedSlider.SetPos(m_int_NaviExtendedEdit);


}

void CNavigator::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	
	ScreenToClient(&rc);

	m_NaviExtendedView.GetClientRect(&rc);

	CDC *pDC = m_NaviExtendedView.GetDC();

	//CDC *memDC;
	
	//CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	//CsMIShopDoc *pDoc = (CsMIShopDoc*)pFrame->GetActiveDocument();

	if (m_bOpenFile)
	{
		m_iOldZoomValue = m_int_NaviExtendedEdit;
		m_Mat_ImageView = m_pView->m_slice_8uc;
		float RatioX = m_Mat_ImageView.cols/rc.Width();
		float RatioY = m_Mat_ImageView.rows / rc.Height();

		cv::resize(m_Mat_ImageView, m_Mat_ImageView, cv::Size(rc.Width(), rc.Height()));
		cv::Rect zoomroi(m_pView->m_viewroi.x / RatioX, m_pView->m_viewroi.y / RatioY,
			m_pView->m_viewroi.width / RatioX, m_pView->m_viewroi.height / RatioY);

		/*cv::rectangle(m_Mat_ImageView, cv::Rect(Center.x - (rc.Width() * 100 / m_int_NaviExtendedEdit) / 2, Center.y - (rc.Height() * 100 / m_int_NaviExtendedEdit) / 2,
			rc.Width() * 100 / m_int_NaviExtendedEdit, rc.Height() * 100 / m_int_NaviExtendedEdit), cv::Scalar(0, 0, 255));*/

		cv::rectangle(m_Mat_ImageView, cv::Rect(zoomroi.x, zoomroi.y, zoomroi.width, zoomroi.height), cv::Scalar(0, 0, 255));



		int m_wndWidth = rc.Width();
		int m_wndHeight = rc.Height();
		int m_nViewBufStep = m_Mat_ImageView.step;

		m_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_bmi.bmiHeader.biWidth = m_wndWidth;
		m_bmi.bmiHeader.biHeight = -m_wndHeight;
		m_bmi.bmiHeader.biPlanes = 1;
		m_bmi.bmiHeader.biBitCount = 24;
		m_bmi.bmiHeader.biCompression = BI_RGB;
		m_bmi.bmiHeader.biSizeImage = m_nViewBufStep*m_wndHeight;
		m_bmi.bmiHeader.biXPelsPerMeter = 0;
		m_bmi.bmiHeader.biYPelsPerMeter = 0;
		m_bmi.bmiHeader.biClrUsed = 0;
		m_bmi.bmiHeader.biClrImportant = 0;

		SetDIBitsToDevice(pDC->GetSafeHdc(),
			0, 0,
			m_wndWidth, m_wndHeight,
			0, 0,
			0, m_wndHeight,
			m_Mat_ImageView.data, &m_bmi,
			DIB_RGB_COLORS); // send DIB to DC

		pDC->SetStretchBltMode(COLORONCOLOR);
		pDC->BitBlt(0, 0, m_wndWidth, m_wndHeight, pDC, 0, 0, SRCCOPY);

		if (m_iXYZSelected == 0){
			int nZ = m_pView->m_nZ;
			m_iCurSliceZ = m_pView->m_iVSliceZ;
			m_ViewSlider.SetRange(0, nZ);
			m_ViewSlider.SetPos(m_iCurSliceZ);

			CString temp_str;
			temp_str.Format(_T("%d / %d"), m_iCurSliceZ, nZ);
			m_SlicePos.SetWindowTextW(temp_str);
		}
		else if (m_iXYZSelected == 1)
		{
			int nY = m_pView->m_nY;
			m_iCurSliceY = m_pView->m_iVSliceY;
			m_ViewSlider.SetRange(0, nY);
			m_ViewSlider.SetPos(m_iCurSliceY);

			CString temp_str;
			temp_str.Format(_T("%d / %d"), m_iCurSliceY, nY);
			m_SlicePos.SetWindowTextW(temp_str);
		}
		else if (m_iXYZSelected == 2)
		{
			int nX = m_pView->m_nX;
			m_iCurSliceX = m_pView->m_iVSliceX;
			m_ViewSlider.SetRange(0, nX);
			m_ViewSlider.SetPos(m_iCurSliceX);

			CString temp_str;
			temp_str.Format(_T("%d / %d"), m_iCurSliceX, nX);
			m_SlicePos.SetWindowTextW(temp_str);
		}


	}

	dc.TextOutW(250, 155, _T("%"));
}



void CNavigator::OnViewNaviview()
{

	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CNavigator::OnUpdateViewNaviview(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CNavigator::OnExtendedEdit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CNavigator::OnUpdateIdsGetdata(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	m_iCurPos = m_pView->m_iVSliceZ;
	m_ViewSlider.SetPos(m_iCurPos);
}


void CNavigator::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
	OnPaint();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CNavigator::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDockablePane::OnSizing(fwSide, pRect);

	//OnEraseBkgnd(this->GetDC());

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


BOOL CNavigator::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rc;

	GetClientRect(&rc);
	
	pDC->FillSolidRect(&rc, RGB(240, 240, 240));
	//OnPaint();

	return CDockablePane::OnEraseBkgnd(pDC);
}


void CNavigator::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDockablePane::OnLButtonDown(nFlags, point);
}


void CNavigator::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDockablePane::OnMouseMove(nFlags, point);
}


void CNavigator::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	//m_wndBkLayer.SetFocus();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CNavigator::OnUpdateRadioZ(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


void CNavigator::OnRadioZ()
{
	
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (!m_bOpenFile)
		return;

	m_iXYZSelected = 0;

	m_pView->m_mode = (VoxViewMode)m_iXYZSelected;
	m_pView->bUpdateSlice = true;
	/*m_ViewSlider.SetPos(m_iCurSliceZ);*/
	m_pView->OnDraw(m_pView->GetDC());
	OnPaint();
}


void CNavigator::OnRadioY()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (!m_bOpenFile)
		return;

	m_iXYZSelected = 1;

	m_pView->m_mode = (VoxViewMode)m_iXYZSelected;
	m_pView->bUpdateSlice = true;
	//m_ViewSlider.SetPos(m_iCurSliceY);
	m_pView->OnDraw(m_pView->GetDC());
	OnPaint();
}


void CNavigator::OnRadioX()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (!m_bOpenFile)
		return;

	m_iXYZSelected = 2;

	m_pView->m_mode = (VoxViewMode)m_iXYZSelected;
	m_pView->bUpdateSlice = true;
	//m_ViewSlider.SetPos(m_iCurSliceX);
	m_pView->OnDraw(m_pView->GetDC());
	OnPaint();

}


HBRUSH CNavigator::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	/// 배경 색상 맞춤

	HBRUSH hbr = CDockablePane::OnCtlColor(pDC, pWnd, nCtlColor);
	CBrush Brush;
	if (pWnd->GetDlgCtrlID() == IDC_BUTTON_XYZ_GROUPBOX)
	{
		RECT r;
		//pDC->SetBkColor(RGB(130, 13., 130));
		pWnd->GetClientRect(&r);
		pDC->FillSolidRect(&r, RGB(240, 240, 240));

		//pDC->SetBkMode(TRANSPARENT);

		//CreateSolidBrush(RGB(146, 248, 168));
		

		//Brush.DeleteObject();
	}
	else if (pWnd->GetDlgCtrlID() == IDC_RADIO_Z || IDC_RADIO_Y || IDC_RADIO_X)
	{
		RECT r;
		pWnd->GetClientRect(&r);
		pDC->FillSolidRect(&r, RGB(240, 240, 240));
	}
	return hbr;
}

// ToolBox.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "sMIShop.h"
#include "ToolBox.h"
#include "MainFrm.h"
#include "sMIShopView.h"


// CToolBox

IMPLEMENT_DYNAMIC(CToolBox, CDockablePane)

CToolBox::CToolBox()
{
	m_bSelectedDraw = false;
	m_iSelectedWork = 0;
}

CToolBox::~CToolBox()
{
}


BEGIN_MESSAGE_MAP(CToolBox, CDockablePane)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CToolBox �޽��� ó�����Դϴ�.




int CToolBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	m_wndToolBox.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, rectDummy, this, 20010);
	//m_Button_Draw.SetWindowPos(NULL, 20, 20, 50, 50, NULL);
	//m_Button_Draw.EnableWindow(1);
	
	//GetDlgItem(IDC_BUTTON_DRAW)->EnableWindow(1);

	//m_Button_Draw.SetState();
	//m_wndDCLayer.CreateObject();

	//m_wndDCLayer.
	
	adjustlayout();
	

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	return 0;
}


void CToolBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CDockablePane::OnPaint()��(��) ȣ������ ���ʽÿ�.

}


BOOL CToolBox::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	return CDockablePane::OnEraseBkgnd(pDC);
}



void CToolBox::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	//m_Button_Draw.EnableWindow(1);
	//m_wndStatic.SetFocus();
	//m_wndList.SetFocus();

	m_wndToolBox.SetFocus();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


void CToolBox::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	adjustlayout();
}

void CToolBox::adjustlayout()
{
	CRect rc;
	GetClientRect(&rc);

	this->GetDC()->FillSolidRect(&rc, RGB(240, 240, 240));

	m_wndToolBox.SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), NULL);

	
}


BEGIN_MESSAGE_MAP(CToolBoxWnd, CWnd)
	ON_COMMAND(IDC_BUTTON_DRAW, &CToolBoxWnd::OnButtonDraw)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(IDC_BUTTON_ERAZE, &CToolBoxWnd::OnButtonEraze)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_BRSIZE, &CToolBoxWnd::OnUpdateEditBrsize)
	ON_COMMAND(IDC_EDIT_BRSIZE, &CToolBoxWnd::OnEditBrsize)
END_MESSAGE_MAP()


int CToolBoxWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	//�׷�ڽ� ����
	m_Button_Group.Create(_T("Draw"), /*WS_CHILD | WS_VISIBLE|*/BS_GROUPBOX, CRect(10, 10, 300, 300), this, 2501);
	m_Button_Group.ShowWindow(SW_SHOW);

	//Draw��ư ����
	m_Button_Draw.Create(_T("Draw"), WS_CHILD | WS_VISIBLE, CRect(30, 40, 80, 80), this, IDC_BUTTON_DRAW);
	m_Button_Draw.ShowWindow(SW_SHOW);

	//Eraze��ư ����
	m_Button_Eraze.Create(_T("Eraze"), WS_CHILD | WS_VISIBLE, CRect(100, 40, 150, 80), this, IDC_BUTTON_ERAZE);
	m_Button_Eraze.ShowWindow(SW_SHOW);

	//�귯�� ������ ����Ʈ�ڽ� ����
	m_Edit_BrSize.Create(WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER, CRect(170, 40, 220, 80), this, IDC_EDIT_BRSIZE);
	m_Edit_BrSize.ShowWindow(SW_SHOW);
	m_Edit_BrSize.SetWindowTextW(_T("10"));

	//m_bSelectedDraw = false;
	//m_bSelectedEraze = false;

	adjustlayout();

	FillToolBox();

	return 0;
}
void CToolBoxWnd::adjustlayout()
{
	CRect rc;
	GetClientRect(&rc);

	//ScreenToClient(rc);

	//m_wndStatic.SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOACTIVATE);

	//m_wndStatic.SetWindowTextW(CString(_T("a")));

	this->GetDC()->FillSolidRect(&rc, RGB(240, 240, 240));

	this->SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOCOPYBITS);


	

}

void CToolBoxWnd::FillToolBox()
{

	//m_wndList.GetDC()->TextOutW(50, 50, _T("test"));

}

void CToolBoxWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	adjustlayout();
}

void CToolBoxWnd::OnButtonDraw()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CsMIShopView* pView = pFrame->m_wndToolBox.pView;


	//m_iSelectedWork = pFrame->m_wndToolBox.m_iSelectedWork;

	//m_iSelectedWork = 1;

	pView->m_iSelectedWork = 1;

	//if (m_iSelectedWork == true)
	//{
	//	//pFrame->m_wndToolBox.m_bSelectedDraw = false;
	//	//m_bSelectedDraw = false;
	//}
//	else if (m_bSelectedDraw == false)
//	{
//		//pFrame->m_wndToolBox.m_bSelectedDraw = true;
//		//pFrame->m_wndToolBox.m_bSelectedEraze = false;
//		//m_bSelectedDraw = true;
//	}
}

void CToolBoxWnd::OnButtonEraze()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CsMIShopView* pView = pFrame->m_wndToolBox.pView;

	pView->m_iSelectedWork = 2;

	//m_iSelectedWork = pFrame->m_wndToolBox.m_bSelectedEraze;

	//m_iSelectedWork = 2;
	//if (m_bSelectedEraze == true)
	//{
	//	pFrame->m_wndToolBox.m_bSelectedEraze = false;
	//	//m_bSelectedDraw = false;
	//}
	//else if (m_bSelectedEraze == false)
	//{
	//	pFrame->m_wndToolBox.m_bSelectedEraze = true;
	//	pFrame->m_wndToolBox.m_bSelectedDraw = false;
	//	//m_bSelectedDraw = true;
	//}
}

BOOL CToolBoxWnd::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CWnd* pWnd = GetFocus();
	if (pWnd->GetSafeHwnd() == GetDlgItem(IDC_EDIT_BRSIZE)->GetSafeHwnd())
	{
		if (pMsg->wParam == VK_RETURN)
		{
			OnEditBrsize();
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}


void CToolBoxWnd::OnUpdateEditBrsize(CCmdUI *pCmdUI)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.

	int a = 0;
}


void CToolBoxWnd::OnEditBrsize()
{
	
	
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CsMIShopView* pView = pFrame->m_wndToolBox.pView;
	
	if (pFrame->m_wndNaviView.m_bOpenFile == false)
		return;


	CString str;
	m_Edit_BrSize.GetWindowTextW(str);
	LPWSTR a = str.GetBuffer();
	int size = _ttoi(a);

	if (size > 50)
	{
		size = 50;
	}
	else if (size < 1)
	{
		size = 1;
	}

	pView->m_fBrSize = size;
	

	pView->OnDraw(pView->GetDC());
	
}

// ShellListFilewView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "sMIShop.h"
#include "ShellListFilewView.h"


// CShellListFilewView

IMPLEMENT_DYNAMIC(CShellListFilewView, CMFCShellListCtrl)

CShellListFilewView::CShellListFilewView()
{

}

CShellListFilewView::~CShellListFilewView()
{
}


BEGIN_MESSAGE_MAP(CShellListFilewView, CMFCShellListCtrl)
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CShellListFilewView �޽��� ó�����Դϴ�.




int CShellListFilewView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCShellListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	return 0;
}


void CShellListFilewView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CMFCShellListCtrl::OnPaint()��(��) ȣ������ ���ʽÿ�.

	
}

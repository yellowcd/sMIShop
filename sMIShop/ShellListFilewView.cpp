// ShellListFilewView.cpp : 구현 파일입니다.
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



// CShellListFilewView 메시지 처리기입니다.




int CShellListFilewView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCShellListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


void CShellListFilewView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CMFCShellListCtrl::OnPaint()을(를) 호출하지 마십시오.

	
}

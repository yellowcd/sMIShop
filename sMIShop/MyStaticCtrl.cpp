// MyStaticCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "sMIShop.h"
#include "MyStaticCtrl.h"


// CMyStaticCtrl

IMPLEMENT_DYNAMIC(CMyStaticCtrl, CStatic)

CMyStaticCtrl::CMyStaticCtrl()
{

}

CMyStaticCtrl::~CMyStaticCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyStaticCtrl, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(STN_CLICKED, &CMyStaticCtrl::OnStnClicked)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CMyStaticCtrl 메시지 처리기입니다.




HBRUSH CMyStaticCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// TODO:  여기서 DC의 특성을 변경합니다.
	
	// TODO:  부모 처리기,가 호출되지 않을 경우 Null이 아닌 브러시를 반환합니다.
	return NULL;
}


void CMyStaticCtrl::OnStnClicked()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	MessageBox(_T("clicked"));
}


int CMyStaticCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc;
	GetClientRect(&rc);

	this->GetDC()->FillSolidRect(&rc, RGB(255, 255, 255));

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

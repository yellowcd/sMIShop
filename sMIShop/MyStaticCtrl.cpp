// MyStaticCtrl.cpp : ���� �����Դϴ�.
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



// CMyStaticCtrl �޽��� ó�����Դϴ�.




HBRUSH CMyStaticCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// TODO:  ���⼭ DC�� Ư���� �����մϴ�.
	
	// TODO:  �θ� ó����,�� ȣ����� ���� ��� Null�� �ƴ� �귯�ø� ��ȯ�մϴ�.
	return NULL;
}


void CMyStaticCtrl::OnStnClicked()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	MessageBox(_T("clicked"));
}


int CMyStaticCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc;
	GetClientRect(&rc);

	this->GetDC()->FillSolidRect(&rc, RGB(255, 255, 255));

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	return 0;
}

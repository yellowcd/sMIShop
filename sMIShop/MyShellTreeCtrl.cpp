#include "stdafx.h"
#include "MyShellTreeCtrl.h"
#include "MainFrm.h"


CMyShellTreeCtrl::CMyShellTreeCtrl()
{
}


CMyShellTreeCtrl::~CMyShellTreeCtrl()
{
}
BEGIN_MESSAGE_MAP(CMyShellTreeCtrl, CMFCShellTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CMyShellTreeCtrl::OnTvnSelchanged)
END_MESSAGE_MAP()


void CMyShellTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (pNMHDR->idFrom == 1200)
	{
		CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
		pFrm->m_wndFileView.m_bChangedSell = true;
		pFrm->m_wndFileView.OnPaint();
	}
	*pResult = 0;
}

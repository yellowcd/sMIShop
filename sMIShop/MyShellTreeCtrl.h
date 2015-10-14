#pragma once
#include "afxshelltreectrl.h"
class CMyShellTreeCtrl :
	public CMFCShellTreeCtrl
{
public:
	CMyShellTreeCtrl();
	virtual ~CMyShellTreeCtrl();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
};


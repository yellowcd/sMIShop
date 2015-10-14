#pragma once


// CShellListFilewView

class CShellListFilewView : public CMFCShellListCtrl
{
	DECLARE_DYNAMIC(CShellListFilewView)

public:
	CShellListFilewView();
	virtual ~CShellListFilewView();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
};



#pragma once


// CMyStaticCtrl

class CMyStaticCtrl : public CStatic
{
	DECLARE_DYNAMIC(CMyStaticCtrl)

public:
	CMyStaticCtrl();
	virtual ~CMyStaticCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnStnClicked();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};



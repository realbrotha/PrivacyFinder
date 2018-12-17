#pragma once
#include "afxcmn.h"


// PopUpDialog 대화 상자입니다.

class PopUpDialog : public CDialogEx
{
	DECLARE_DYNAMIC(PopUpDialog)

public:
	PopUpDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	//PopUpDialog(int i);
	virtual ~PopUpDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	// 화면 출력
	virtual BOOL OnInitDialog();

	void fnListInsert();
	int nPosition;
	CListCtrl m_list;
};

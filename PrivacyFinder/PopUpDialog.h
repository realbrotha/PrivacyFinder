#pragma once
#include "afxcmn.h"


// PopUpDialog ��ȭ �����Դϴ�.

class PopUpDialog : public CDialogEx
{
	DECLARE_DYNAMIC(PopUpDialog)

public:
	PopUpDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	//PopUpDialog(int i);
	virtual ~PopUpDialog();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	// ȭ�� ���
	virtual BOOL OnInitDialog();

	void fnListInsert();
	int nPosition;
	CListCtrl m_list;
};

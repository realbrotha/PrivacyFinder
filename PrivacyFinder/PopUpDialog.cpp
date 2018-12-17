// PopUpDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PrivacyFinder.h"
#include "PopUpDialog.h"
#include "afxdialogex.h"
#include "PrivacyFinderDlg.h"
// PopUpDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(PopUpDialog, CDialogEx)

PopUpDialog::PopUpDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

PopUpDialog::~PopUpDialog()
{
}

void PopUpDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(PopUpDialog, CDialogEx)
END_MESSAGE_MAP()


// PopUpDialog �޽��� ó�����Դϴ�.

BOOL PopUpDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, _T("�߰� ������"), LVCFMT_CENTER, 100, -1);
	m_list.InsertColumn(1, _T("�߰� ����"), LVCFMT_LEFT, 150, -1);
	m_list.InsertColumn(2, _T("���ڿ�"), LVCFMT_LEFT, 300, -1);
	//m_list.InsertColumn(3, _T("���ڿ� ����"), LVCFMT_CENTER, 80, -1);
	fnListInsert();

	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void PopUpDialog::fnListInsert()
{
	DWORD dwCnt = 0;

	std::list<FILE_INFO>::const_iterator mmDetect = pThis->fileInfoList.begin();
	
	for (int n = 0; n < nPosition; n++)
	{
		mmDetect++;
	}
	
	std::list<DETECT_INFO>::const_iterator mmDetect1 = mmDetect->detectList.begin();
	
	while (mmDetect1 != mmDetect->detectList.end())
	{

		m_list.InsertItem(dwCnt, mmDetect1->position);
		m_list.SetItem(dwCnt, 1, LVIF_TEXT, mmDetect1->pattern, 0, 0, 0, NULL);
		m_list.SetItem(dwCnt, 2, LVIF_TEXT, mmDetect1->originalString, 0, 0, 0, NULL);
	//	m_list.SetItem(dwCnt, 3, LVIF_TEXT, mmDetect1->position, 0, 0, 0, NULL);
		mmDetect1++;
		dwCnt++;
	}
}
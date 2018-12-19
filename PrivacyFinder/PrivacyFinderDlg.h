#pragma once

#include "FolderTreeCtrl.h"
#include "PatternParser.h"

#include <vector>
#include <deque>
#include <list>
#include <atomic>
#include <thread>

typedef struct DETECT_INFO_
{
	CString	originalString;			// ���� ���ڿ�
	CString position;				// ��ġ (����) = ǥ���� ����
	CString	pattern;			    // ���� (###- #### ����)
	CString filePath;
}DETECT_INFO;

typedef struct FILE_INFO_
{
	CString detectType;							// ���� Ÿ��
	CString	fileName;							// �����̸�
	CString fileSize;							// ���� ũ��
	CString	detectCount;						// Ž�� ����
	std::list<DETECT_INFO> detectList;			// ������ ����
}FILE_INFO;


class CPrivacyFinderDlg : public CDialogEx
{
public:
	CPrivacyFinderDlg(CWnd* pParent = NULL);

protected:
    HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnItemexpandingTree1(NMHDR *pNMHDR, LRESULT *pResult); // ����Ʈ expanding
	afx_msg void OnClickTree1(NMHDR *pNMHDR, LRESULT *pResult); // ����Ʈ Ŭ��
	afx_msg void OnBnClickedStartScan();   //�˻� ���� ��ư 
	afx_msg void OnNMRClickLick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnListItemClear();
    afx_msg void OnListListClear();
    afx_msg void OnListViewDetails();
    
    afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()

    std::vector<CString>::iterator rootScanFilePath;
    std::vector<CString> scanPathFromTree; // TreeControl���� üũ�� �˻��� ����.
    std::deque<CString>	detailScanFilePath; // �˻����� ���� ��� ���ϵ� ����.

public:

    std::list<DETECT_INFO> detectedInfoList; // TODO : ������ �߸� ������ �ϴ�. �ð������� �� ���ľߵ� 
    std::list<FILE_INFO> fileInfoList; // TODO : ����

	std::atomic<bool> timeThreadFlag; // todo : ��� ī��Ʈ ��� or �ð� ���� ��� ���� �ϳ� ���� 

    void SearchFileFromTop();
    BOOL SearchFileDetails(CString& path);
    void StartPolicyFinder();
    void PrivacyScan();

private:
    static void TimerThread(void* arg);
    static void SearchFileThread(void* arg);
    static void PrivacyScanThread(void* arg);

    CRITICAL_SECTION crtSection;
    FolderTreeCtrl folderTree;
	HANDLE fileSearchEvent;

    int GetFilesize(CString& filePath);
    void InitDetectList();
    void InsertDetectList();
    
    // for ui
	CStatic scanTime_static; // �ð� ǥ��
	CStatic scanCount_static;// �˻� ����
	CStatic privacyCount_static;// ���ⰳ��
	CStatic detectedFilePath_static;// �˻� ��� �ַ��
	CListCtrl privacyList; //�˿� ����Ʈ
    CButton startButton;
};


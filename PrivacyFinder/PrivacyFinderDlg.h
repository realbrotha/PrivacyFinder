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
	CString	originalString;			// 실제 문자열
	CString position;				// 위치 (패턴) = 표시할 내용
	CString	pattern;			    // 패턴 (###- #### 등등등)
	CString filePath;
}DETECT_INFO;

typedef struct FILE_INFO_
{
	CString detectType;							// 검출 타입
	CString	fileName;							// 파일이름
	CString fileSize;							// 파일 크기
	CString	detectCount;						// 탐지 갯수
	std::list<DETECT_INFO> detectList;			// 디테일 정보
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
	afx_msg void OnItemexpandingTree1(NMHDR *pNMHDR, LRESULT *pResult); // 리스트 expanding
	afx_msg void OnClickTree1(NMHDR *pNMHDR, LRESULT *pResult); // 리스트 클릭
	afx_msg void OnBnClickedStartScan();   //검사 시작 버튼 
	afx_msg void OnNMRClickLick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnListItemClear();
    afx_msg void OnListListClear();
    afx_msg void OnListViewDetails();
    
    afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()

    std::vector<CString>::iterator rootScanFilePath;
    std::vector<CString> scanPathFromTree; // TreeControl에서 체크한 검사경로 저장.
    std::deque<CString>	detailScanFilePath; // 검사경로의 하위 모든 파일들 저장.

public:

    std::list<DETECT_INFO> detectedInfoList; // TODO : 구조를 잘못 잡은듯 하다. 시간있을때 싹 고쳐야됨 
    std::list<FILE_INFO> fileInfoList; // TODO : 동일

	std::atomic<bool> timeThreadFlag; // todo : 상수 카운트 방식 or 시간 연산 방식 둘중 하나 고를것 

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
	CStatic scanTime_static; // 시간 표시
	CStatic scanCount_static;// 검사 갯수
	CStatic privacyCount_static;// 검출개수
	CStatic detectedFilePath_static;// 검사 경로 주루룩
	CListCtrl privacyList; //검역 리스트
    CButton startButton;
};


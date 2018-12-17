#include "stdafx.h"
#include "PrivacyFinder.h"
#include "PrivacyFinderDlg.h"
#include "afxdialogex.h"
#include "PatternParser.h"
#include "FileAnaylze.h"
#include "DocumentParser.h"
#include "PopUpDialog.h"

#include <list>

CPrivacyFinderDlg *pThis = NULL;

#define SCAN_TIMER_SET			1

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CPrivacyFinderDlg::CPrivacyFinderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PRIVACY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPrivacyFinderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, folderTree);
	DDX_Control(pDX, IDC_Static_Time, scanTime_static);
	DDX_Control(pDX, IDC_Static_Count, scanCount_static);
	DDX_Control(pDX, IDC_Static_PrivacyCount, privacyCount_static);
	DDX_Control(pDX, IDC_STATIC_Path, detectedFilePath_static);
	DDX_Control(pDX, IDC_LIST2, privacyList);
	DDX_Control(pDX, IDOK, startButton);
}

BEGIN_MESSAGE_MAP(CPrivacyFinderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE1, &CPrivacyFinderDlg::OnItemexpandingTree1)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CPrivacyFinderDlg::OnClickTree1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, &CPrivacyFinderDlg::OnNMRClickLick)
    ON_BN_CLICKED(IDOK, &CPrivacyFinderDlg::OnBnClickedStartScan)
    ON_COMMAND(ID_LISTDel, &CPrivacyFinderDlg::OnListItemClear)
    ON_COMMAND(ID_DB32777, &CPrivacyFinderDlg::OnListListClear)
	ON_COMMAND(ID_DB32778, &CPrivacyFinderDlg::OnListViewDetails)
    ON_WM_DESTROY()

END_MESSAGE_MAP()

BOOL CPrivacyFinderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
		
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	pThis = this;
    InitializeCriticalSection(&crtSection);

	folderTree.TreeInitialize();      // TREE 초기화 
	
	InitDetectList(); // ListCtrl 초기화 

    PatternParser::GetInstance().Initialize();  // init 예외처리 없음 . 
	
	return TRUE;
}

void CPrivacyFinderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CPrivacyFinderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CPrivacyFinderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPrivacyFinderDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 타이머가 이상하게 돈다. Sync를 맞춰야함
	switch (nIDEvent)
	{

	case SCAN_TIMER_SET:
		KillTimer(nIDEvent);
        CString	scanTime; //검사 시간
		scanTime.Format(_T("%02d:%02d:%02d"), m_nHour, m_nMin, m_nSec);
		scanTime_static.SetWindowText(scanTime);
		SetTimer(nIDEvent, 100, NULL);
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CPrivacyFinderDlg::OnBnClickedStartScan()
{
    // TODO:  여기에 추가 초기화 작업을 추가합니다.
	detectedInfoList.clear();
	fileInfoList.clear();
    scanPathFromTree.clear();

    startButton.EnableWindow(FALSE);
	privacyList.DeleteAllItems();               //리스트를 날리고 시작해야됨

    HTREEITEM hRoot = folderTree.GetRootItem();	// Root Tree부터 Check된 폴더 저장.
    folderTree.GetCheckedFolderList(scanPathFromTree, hRoot);

	if (scanPathFromTree.empty())
	{
		MessageBox(_T("검사할 폴더를 선택하세요.\n"), _T("PrivacyFinder"), MB_OK);
		return;
	}
	StartPolicyFinder();
}

void CPrivacyFinderDlg::StartPolicyFinder()
{
    timeThreadFlag = TRUE;

    detailScanFilePath.clear();

    fileSearchEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    ::_beginthread(TimerThread, 0, this);
    ::_beginthread(SearchFileThread, 0, this);
    ::_beginthread(PrivacyScanThread, 0, this);
}

void CPrivacyFinderDlg::TimerThread(void* arg) // todo : 왜이렇게 했을까 .... 시간을 이렇게 구할 필요없다. TimerThread에서 UI단으로 직접 쏴주는게 현명함.
{
	CPrivacyFinderDlg *dlg = reinterpret_cast<CPrivacyFinderDlg *>(arg);

	dlg->m_nOldTime = 0;
	dlg->m_nCurTime = 0;
	dlg->m_nEndTime = 0;
	dlg->m_nBackupTime = 0;

    dlg->m_nOldTime = static_cast<INT_PTR>(time(NULL));

	dlg->SetTimer(SCAN_TIMER_SET, 100, NULL);

	while (dlg->timeThreadFlag)
	{
		dlg->m_nCurTime = static_cast<INT_PTR>(time(NULL));
		dlg->m_nEndTime = (dlg->m_nCurTime + dlg->m_nBackupTime) - dlg->m_nOldTime;

		dlg->m_nHour = dlg->m_nEndTime / 3600;
		dlg->m_nMin = dlg->m_nEndTime / 60 % 60;
		dlg->m_nSec = dlg->m_nEndTime % 60;
	}

	return ;
}

void CPrivacyFinderDlg::SearchFileThread(void* arg) // 파일 검출 쓰레드
{
	CPrivacyFinderDlg *dlg = reinterpret_cast<CPrivacyFinderDlg *>(arg);

	dlg->SearchFileFromTop();
	return;
}

void CPrivacyFinderDlg::SearchFileFromTop()
{
    rootScanFilePath = scanPathFromTree.begin();

    while (rootScanFilePath != scanPathFromTree.end())
    {
        if (SearchFileDetails(*rootScanFilePath))
            rootScanFilePath++;
        else
            break;
    }
    SetEvent(fileSearchEvent);
    CloseHandle(fileSearchEvent);

    fileSearchEvent = NULL;
}

BOOL CPrivacyFinderDlg::SearchFileDetails(CString& strPath)
{
    CFileFind cFileFind = NULL;
    BOOL bWorking = cFileFind.FindFile(strPath + _T("\\*.*"));

    while (bWorking)
    {
		bWorking = cFileFind.FindNextFile();
		if (cFileFind.IsDots())
			continue;

		if (cFileFind.IsDirectory())
			SearchFileDetails(cFileFind.GetFilePath());

		else
		{
			EnterCriticalSection(&crtSection);
			detailScanFilePath.push_back(cFileFind.GetFilePath());
			SetEvent(fileSearchEvent);
			LeaveCriticalSection(&crtSection);
		}
    }
    return TRUE;
}
void CPrivacyFinderDlg::PrivacyScanThread(void* arg)  // 개인정보 검출 쓰레드 
{
	CPrivacyFinderDlg *dlg = reinterpret_cast<CPrivacyFinderDlg *>(arg);

	dlg->PrivacyScan();
	dlg->timeThreadFlag = FALSE;
	
    return;
}

void CPrivacyFinderDlg::PrivacyScan()
{
    CString	scanFilePath = _T("");                     // 개별 파일 경로 

    int privacyFileCount = 0;     // 탐지 갯수 
    int scanSuccessFileCount = 0; // 전체 갯수

	while(true)
	{
		if (fileSearchEvent != NULL)
			WaitForSingleObject(fileSearchEvent, INFINITE);

		if (detailScanFilePath.empty())
			break;

		EnterCriticalSection(&crtSection);

		std::deque<CString>::iterator cIterPath = detailScanFilePath.begin();
		scanFilePath.Format(_T("%s"), *cIterPath);

        CString fileExtention = _T("");
		fileExtention = *cIterPath;
		fileExtention = fileExtention.Mid(fileExtention.ReverseFind('.'));

		fileExtention.MakeLower();

		if (0 == fileExtention.Compare(_T(".txt"))) // todo : 여러개의 확장자 지원 예정
		{
			char* originalString = nullptr;
			char* sharpString = nullptr;
			int bufferSize = 0;

            bufferSize = DocumentParser::GetInstace().TextToChar(scanFilePath.GetBuffer(scanFilePath.GetLength()), originalString);

			if (bufferSize)
			{
                DocumentParser::GetInstace().CharToSharp(originalString, sharpString);

                CString detectedType =_T("");
   				int detectCount = FileAnalyzer::GetInstance().BreadthSearch(scanFilePath, originalString, sharpString, bufferSize, detectedType);
				
				if (detectCount)
				{
                    FILE_INFO fileInfo = { 0, };
					DETECT_INFO detectInfo = { 0, };
					
					fileInfo.fileName = scanFilePath;										   //검출 경로
					fileInfo.fileSize.Format(_T("%d (Byte)"), GetFilesize(scanFilePath));   //파일 크기
					fileInfo.detectCount.Format(_T("%d (개)"), detectCount);					   //검출 갯수
					fileInfo.detectType = detectedType;

					for (std::list<DETECT_INFO>::iterator detectedIt = detectedInfoList.begin(); detectedIt != detectedInfoList.end(); ++detectedIt)
					{
						detectInfo.position = detectedIt->position;
						detectInfo.pattern = detectedIt->pattern;
						detectInfo.originalString = detectedIt->originalString;
						
						fileInfo.detectList.push_back(detectInfo);
					}
					fileInfoList.push_back(fileInfo); // FileInfo Push
					
					privacyFileCount++;							
				}
				detectedInfoList.clear();  //여기서 클리어 안해주면 중복 표기됨 
                sharpString = NULL;
                delete[] sharpString;

			}
            originalString = NULL;
			delete[] originalString;
		}
        scanSuccessFileCount++;

        CString	scanFileCount; //총 카운트 갯수
        CString	detectedPrivacyCount; //검출된 개인정보 파일

		detectedPrivacyCount.Format(_T("%d"), privacyFileCount);
 		scanFileCount.Format(_T("%d"), scanSuccessFileCount);

        scanCount_static.SetWindowText(scanFileCount);
        privacyCount_static.SetWindowText(detectedPrivacyCount);
        detectedFilePath_static.SetWindowText(scanFilePath);

		detailScanFilePath.pop_front();

		if (fileSearchEvent != NULL)
			ResetEvent(fileSearchEvent);

		LeaveCriticalSection(&crtSection);
	}

	InsertDetectList();  //그냥 리스트에서 리스트 팝 해서 그림만 그려준다.
	scanFilePath.Format(_T("검사가 완료되었습니다."));
	detectedFilePath_static.SetWindowText(_T("검사가 완료되었습니다."));
    startButton.EnableWindow(TRUE);
}

void CPrivacyFinderDlg::OnItemexpandingTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 컨트롤 알림 처리기 코드를 추가합니다.
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	if (pNMTreeView->action & TVE_EXPAND)
	{
		if (folderTree.GetChildItem(pNMTreeView->itemNew.hItem) == NULL)
		{
			SetCursor(LoadCursor(NULL, IDC_WAIT));

			LPITEMIDLIST	itemList = NULL;
			LPSHELLFOLDER	shellFolder = NULL;

			itemList = (LPITEMIDLIST)folderTree.GetItemData(pNMTreeView->itemNew.hItem);

			if (pNMTreeView->itemNew.hItem != folderTree.destopItem)
			{
				folderTree.desktopFolder->BindToObject(itemList, NULL, IID_IShellFolder, (LPVOID *)&shellFolder);
				folderTree.EnumChildItems(shellFolder, itemList, pNMTreeView->itemNew.hItem);
				shellFolder->Release();
			}
			else
				folderTree.EnumChildItems(folderTree.desktopFolder, itemList, pNMTreeView->itemNew.hItem);

			SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
	}

	if (folderTree.GetCheck(pNMTreeView->itemNew.hItem))
        folderTree.CheckChildItems(pNMTreeView->itemNew.hItem);

	*pResult = 0;
}


void CPrivacyFinderDlg::OnClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
    CPoint Point;
	GetCursorPos(&Point);
	::ScreenToClient(folderTree.m_hWnd, &Point);

    UINT unFlags = 0;
	HTREEITEM hItem = folderTree.HitTest(Point, &unFlags);

	// 아이템 존재 및 체크박스 이벤트 발생 시.
	if (hItem != NULL && (unFlags & TVHT_ONITEMSTATEICON) != 0)
	{
		if (folderTree.GetCheck(hItem))	// 체크 ON 상태.
            folderTree.UnCheckChildItems(hItem);
		else								// 체크 OFF.
            folderTree.CheckChildItems(hItem);

        folderTree.ParentCheckOnOff(hItem);
	}
	*pResult = 0;
}

void CPrivacyFinderDlg::InitDetectList()
{
	privacyList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	privacyList.InsertColumn(0, _T("파일 경로"), LVCFMT_LEFT, 200, -1);
	privacyList.InsertColumn(1, _T("파일 크기"), LVCFMT_RIGHT, 70, -1);
	privacyList.InsertColumn(2, _T("발견 개수"), LVCFMT_RIGHT, 70, -1);
	privacyList.InsertColumn(3, _T("발견 패턴"), LVCFMT_LEFT, 130, -1);
	InsertDetectList();
}

void CPrivacyFinderDlg::InsertDetectList()
{
    int indexCount = 0;

    for (std::list<FILE_INFO>::iterator it = fileInfoList.begin(); it != fileInfoList.end(); ++it)
    {
        privacyList.InsertItem(indexCount, it->fileName);
        privacyList.SetItem(indexCount, 1, LVIF_TEXT, it->fileSize, 0, 0, 0, NULL);
        privacyList.SetItem(indexCount, 2, LVIF_TEXT, it->detectCount, 0, 0, 0, NULL);
        privacyList.SetItem(indexCount, 3, LVIF_TEXT, it->detectType, 0, 0, 0, NULL);
        indexCount++;
    }
    return;
}

void CPrivacyFinderDlg::OnNMRClickLick(NMHDR *pNMHDR, LRESULT *pResult) // 리스트 컨트롤에서 우클릭시 메뉴 처리
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (fileInfoList.size()) // 검출된게 있을경우만 메뉴
	{
        CMenu menuList;
		menuList.LoadMenu(IDR_MENU1);
		CMenu *p_sub_menu = menuList.GetSubMenu(0);

		CPoint pos;
		GetCursorPos(&pos);

		// 마우스 커서가 위치한 곳에 팝업메뉴를 출력한다.
		p_sub_menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this);

		// 생성된 팝업 메뉴를 삭제한다.
		menuList.DestroyMenu();
	}
	*pResult = 0;
}

void CPrivacyFinderDlg::OnListItemClear()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
    POSITION pos = privacyList.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
        int nItem = privacyList.GetNextSelectedItem(pos);
		privacyList.DeleteItem(nItem);
		pos = privacyList.GetFirstSelectedItemPosition();

		int	j = 0;

		std::list<FILE_INFO>::const_iterator tmp = fileInfoList.begin();

		while (tmp != fileInfoList.end())
		{
			if (j == nItem)
			{
				tmp = fileInfoList.erase(tmp);
				break;
			}
			tmp++;
			j++;
		}
	}
}

void CPrivacyFinderDlg::OnListListClear()
{
	privacyList.DeleteAllItems();
}

void CPrivacyFinderDlg::OnListViewDetails()
{
    POSITION pos = privacyList.GetFirstSelectedItemPosition();
	int nItem = privacyList.GetNextSelectedItem(pos);

	PopUpDialog popupDlg;

	popupDlg.nPosition = nItem;
	popupDlg.DoModal();
}

int CPrivacyFinderDlg::GetFilesize(CString& filePath)
{
    HANDLE file = CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    int fileSize = ::GetFileSize(file, NULL);
    return fileSize;
}

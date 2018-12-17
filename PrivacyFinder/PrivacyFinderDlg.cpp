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

	folderTree.TreeInitialize();      // TREE �ʱ�ȭ 
	
	InitDetectList(); // ListCtrl �ʱ�ȭ 

    PatternParser::GetInstance().Initialize();  // init ����ó�� ���� . 
	
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
	// TODO: Ÿ�̸Ӱ� �̻��ϰ� ����. Sync�� �������
	switch (nIDEvent)
	{

	case SCAN_TIMER_SET:
		KillTimer(nIDEvent);
        CString	scanTime; //�˻� �ð�
		scanTime.Format(_T("%02d:%02d:%02d"), m_nHour, m_nMin, m_nSec);
		scanTime_static.SetWindowText(scanTime);
		SetTimer(nIDEvent, 100, NULL);
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CPrivacyFinderDlg::OnBnClickedStartScan()
{
    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	detectedInfoList.clear();
	fileInfoList.clear();
    scanPathFromTree.clear();

    startButton.EnableWindow(FALSE);
	privacyList.DeleteAllItems();               //����Ʈ�� ������ �����ؾߵ�

    HTREEITEM hRoot = folderTree.GetRootItem();	// Root Tree���� Check�� ���� ����.
    folderTree.GetCheckedFolderList(scanPathFromTree, hRoot);

	if (scanPathFromTree.empty())
	{
		MessageBox(_T("�˻��� ������ �����ϼ���.\n"), _T("PrivacyFinder"), MB_OK);
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

void CPrivacyFinderDlg::TimerThread(void* arg) // todo : ���̷��� ������ .... �ð��� �̷��� ���� �ʿ����. TimerThread���� UI������ ���� ���ִ°� ������.
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

void CPrivacyFinderDlg::SearchFileThread(void* arg) // ���� ���� ������
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
void CPrivacyFinderDlg::PrivacyScanThread(void* arg)  // �������� ���� ������ 
{
	CPrivacyFinderDlg *dlg = reinterpret_cast<CPrivacyFinderDlg *>(arg);

	dlg->PrivacyScan();
	dlg->timeThreadFlag = FALSE;
	
    return;
}

void CPrivacyFinderDlg::PrivacyScan()
{
    CString	scanFilePath = _T("");                     // ���� ���� ��� 

    int privacyFileCount = 0;     // Ž�� ���� 
    int scanSuccessFileCount = 0; // ��ü ����

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

		if (0 == fileExtention.Compare(_T(".txt"))) // todo : �������� Ȯ���� ���� ����
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
					
					fileInfo.fileName = scanFilePath;										   //���� ���
					fileInfo.fileSize.Format(_T("%d (Byte)"), GetFilesize(scanFilePath));   //���� ũ��
					fileInfo.detectCount.Format(_T("%d (��)"), detectCount);					   //���� ����
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
				detectedInfoList.clear();  //���⼭ Ŭ���� �����ָ� �ߺ� ǥ��� 
                sharpString = NULL;
                delete[] sharpString;

			}
            originalString = NULL;
			delete[] originalString;
		}
        scanSuccessFileCount++;

        CString	scanFileCount; //�� ī��Ʈ ����
        CString	detectedPrivacyCount; //����� �������� ����

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

	InsertDetectList();  //�׳� ����Ʈ���� ����Ʈ �� �ؼ� �׸��� �׷��ش�.
	scanFilePath.Format(_T("�˻簡 �Ϸ�Ǿ����ϴ�."));
	detectedFilePath_static.SetWindowText(_T("�˻簡 �Ϸ�Ǿ����ϴ�."));
    startButton.EnableWindow(TRUE);
}

void CPrivacyFinderDlg::OnItemexpandingTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

	// ������ ���� �� üũ�ڽ� �̺�Ʈ �߻� ��.
	if (hItem != NULL && (unFlags & TVHT_ONITEMSTATEICON) != 0)
	{
		if (folderTree.GetCheck(hItem))	// üũ ON ����.
            folderTree.UnCheckChildItems(hItem);
		else								// üũ OFF.
            folderTree.CheckChildItems(hItem);

        folderTree.ParentCheckOnOff(hItem);
	}
	*pResult = 0;
}

void CPrivacyFinderDlg::InitDetectList()
{
	privacyList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	privacyList.InsertColumn(0, _T("���� ���"), LVCFMT_LEFT, 200, -1);
	privacyList.InsertColumn(1, _T("���� ũ��"), LVCFMT_RIGHT, 70, -1);
	privacyList.InsertColumn(2, _T("�߰� ����"), LVCFMT_RIGHT, 70, -1);
	privacyList.InsertColumn(3, _T("�߰� ����"), LVCFMT_LEFT, 130, -1);
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

void CPrivacyFinderDlg::OnNMRClickLick(NMHDR *pNMHDR, LRESULT *pResult) // ����Ʈ ��Ʈ�ѿ��� ��Ŭ���� �޴� ó��
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (fileInfoList.size()) // ����Ȱ� ������츸 �޴�
	{
        CMenu menuList;
		menuList.LoadMenu(IDR_MENU1);
		CMenu *p_sub_menu = menuList.GetSubMenu(0);

		CPoint pos;
		GetCursorPos(&pos);

		// ���콺 Ŀ���� ��ġ�� ���� �˾��޴��� ����Ѵ�.
		p_sub_menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this);

		// ������ �˾� �޴��� �����Ѵ�.
		menuList.DestroyMenu();
	}
	*pResult = 0;
}

void CPrivacyFinderDlg::OnListItemClear()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
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

#include "stdafx.h"
#include "FolderTreeCtrl.h"

IMPLEMENT_DYNAMIC(FolderTreeCtrl, CTreeCtrl)

FolderTreeCtrl::FolderTreeCtrl()
{
}

FolderTreeCtrl::~FolderTreeCtrl()
{
	if (idList)
		pMalloc->Free(idList);

	if (pMalloc)
		pMalloc->Release();

	if (desktopFolder)
		desktopFolder->Release();
}

BEGIN_MESSAGE_MAP(FolderTreeCtrl, CTreeCtrl)
END_MESSAGE_MAP()

void FolderTreeCtrl::TreeInitialize()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	if (!GetItemImage())
		return;

	if (SHGetDesktopFolder(&desktopFolder) != S_OK)
		return;

	GetDesktopInformation();

	if (SHGetMalloc(&pMalloc) != S_OK)
		return;
	
	EnumChildItems(desktopFolder, idList, destopItem);
	Expand(destopItem, TVE_EXPAND);
}
TCHAR* FolderTreeCtrl::GetPidlToFolderPath(HTREEITEM hItem)
{
    static TCHAR	strPath[MAX_PATH] = { 0 };
    LPITEMIDLIST	pidl = NULL;

    pidl = (LPITEMIDLIST)GetItemData(hItem);

    SHGetPathFromIDList(pidl, strPath);

    return strPath;
}

BOOL FolderTreeCtrl::GetCheckedFolderList(std::vector<CString>& pathList, HTREEITEM hItem)
{
    BOOL result = FALSE;
    HTREEITEM hChildItem = GetChildItem(hItem);
    // C:, D: 등 최상위 드라이버가 체크되어 있을 시에는 하위 무시.
    // 최상위 드라이버 미체크 시에 하위 아이템 체크 조사.
    while (hChildItem != NULL)
    {
        if (hChildItem != NULL)
        {
            if (GetCheck(hChildItem))
            {
                pathList.push_back(GetPidlToFolderPath(hChildItem));
                hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
                result = TRUE;

                continue;
            }
            else // 재귀태움
            {
                GetCheckedFolderList(pathList, hChildItem);
            }
        }
        hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
    }
    return result;
}

BOOL FolderTreeCtrl::GetItemImage() 
{
	if (SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &idList) != S_OK) // 데스크탑의 ITEMIDLIST 구조체 포인터 검색.
		return FALSE;

	HIMAGELIST	hSysImageList = (HIMAGELIST)SHGetFileInfo((LPCTSTR)idList, 0, &shInfo, sizeof(SHFILEINFO), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	
	TreeView_SetImageList(this->m_hWnd, hSysImageList, TVSIL_NORMAL);// 트리뷰 이미지 목록 설정하고 이미지 사용해 컨트롤 다시 그리기.

	UINT uFlags = SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_DISPLAYNAME; // 바탕화면의 정보 구한다.
	memset(&shInfo, 0x00, sizeof(SHFILEINFO));

	SHGetFileInfo((LPCTSTR)idList, 0, &shInfo, sizeof(SHFILEINFO), uFlags);

	return TRUE;
}

void FolderTreeCtrl::GetDesktopInformation()
{
	LPENUMIDLIST pEnumIDList = NULL;

	memset(&tvi, 0x00, sizeof(TVINSERTSTRUCT));

	tvi.hParent			= TVI_ROOT;
	tvi.hInsertAfter		= TVI_LAST;
	tvi.item.pszText		= shInfo.szDisplayName;
	tvi.item.mask			= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
	tvi.item.iImage		= tvi.item.iSelectedImage = shInfo.iIcon;
	tvi.item.cChildren	= 1;
	tvi.item.lParam		= (LPARAM)idList;

	// 바탕화면 추가하는 곳이지만 컴퓨터와 관련된 폴더들만 보여주기 위해서 제외.
	destopItem			= InsertItem(&tvi);
}

void FolderTreeCtrl::EnumChildItems(LPSHELLFOLDER pFolder, LPITEMIDLIST pParent, HTREEITEM hParent)
{
	LPENUMIDLIST	pEnumIDList = NULL;
	LPITEMIDLIST	pItem		= NULL;
	LPITEMIDLIST	pidlAbs		= NULL;
	ULONG			ulFetched	= 0;
	CString			strBuf		= _T("");
	TVINSERTSTRUCT	tvi;

	memset(&tvi, 0x00, sizeof(TVINSERTSTRUCT));

	if (pFolder->EnumObjects(this->m_hWnd, SHCONTF_FOLDERS | SHCONTF_INCLUDEHIDDEN, &pEnumIDList) == NO_ERROR)
	{
		while (pEnumIDList->Next(1, &pItem, &ulFetched) == NO_ERROR)
		{
			// PIDL 저장.
			pidlAbs = ILAppend(pParent, pItem);

			// Tree Item 설정.
			tvi.hParent			= hParent;
			tvi.hInsertAfter	= TVI_LAST;
			tvi.item.mask		= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
			tvi.item.lParam		= (LPARAM)pidlAbs;

			currItem = TreeInsertItem(&tvi, pFolder, pidlAbs, pItem, strBuf);

			if (!_tcscmp(tvi.item.pszText, _T("컴퓨터")) || !_tcscmp(tvi.item.pszText, _T("내 PC")))
				Expand(currItem, TVE_EXPAND);

			pMalloc->Free(pItem);
		}
		pEnumIDList->Release();
	}
}


LPITEMIDLIST FolderTreeCtrl::ILAppend(LPITEMIDLIST pidlParent, LPITEMIDLIST pidlChild)
{
	LPITEMIDLIST pidl = NULL;

	if (pidlParent == NULL)
	{
		return NULL;
	}

	if (pidlChild == NULL)
	{
		return pidlParent;
	}

	UINT unCb1 = ILGetSize(pidlParent) - sizeof(pidlParent->mkid.cb);
	UINT unCb2 = ILGetSize(pidlChild);

	pidl = (LPITEMIDLIST)pMalloc->Alloc(unCb1 + unCb2);

	if (pidl)
	{
		CopyMemory(pidl, pidlParent, unCb1);
		CopyMemory(((LPSTR)pidl) + unCb1, pidlChild, unCb2);
	}

	return pidl;
}

HTREEITEM FolderTreeCtrl::TreeInsertItem(TVINSERTSTRUCT *pTvi, LPSHELLFOLDER pFolder, LPITEMIDLIST pidl, LPITEMIDLIST pidlRelative, CString &strBuf)
{
	STRRET		sName;
	DWORD		dwAttributes = 0;
	UINT		uFlags = 0;
	SHFILEINFO	sfi;

	// 가상 폴더의 경우 SHGetFileInfo.szDisplayName 불가능.
	// Display 이름 구하기.
	if (pTvi->item.mask & TVIF_TEXT)
	{
		pFolder->GetDisplayNameOf(pidlRelative, 0, &sName);
		UTStrretToString(pidlRelative, &sName, strBuf);
		pTvi->item.pszText = (LPWSTR)(LPCWSTR)strBuf;
        if (!_tcslen(strBuf))
            return NULL;
	}

	// Display 이미지 구하기.
	if (pTvi->item.mask & (TVIF_IMAGE | TVIF_SELECTEDIMAGE))
	{
		// 특성 구하기.
		dwAttributes = SFGAO_FOLDER | SFGAO_LINK;
		pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&pidlRelative, &dwAttributes);

		// 아이콘 구하기.
		uFlags = SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON;
		if (dwAttributes & SFGAO_LINK)
		{
			uFlags |= SHGFI_LINKOVERLAY;
		}

		if (pTvi->item.mask & TVIF_IMAGE)
		{
			memset(&sfi, 0x00, sizeof(SHFILEINFO));
			SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), uFlags);

			pTvi->item.iImage = sfi.iIcon;
		}

		if (pTvi->item.mask & TVIF_SELECTEDIMAGE)
		{
			if (dwAttributes & SFGAO_FOLDER)
			{
				uFlags |= SHGFI_OPENICON;
			}

			memset(&sfi, 0x00, sizeof(SHFILEINFO));
			SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), uFlags);

			pTvi->item.iSelectedImage = sfi.iIcon;
		}
	}

	// 자식노드 있으면 +버튼 추가.
	if (pTvi->item.mask & (TVIF_CHILDREN))
	{
		// 특성 구하기.
		dwAttributes = SFGAO_FOLDER;
		pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&pidlRelative, &dwAttributes);

		// 자식 구하기.
		pTvi->item.cChildren = 0;
		if (dwAttributes & SFGAO_FOLDER)
		{	
			dwAttributes = SFGAO_HASSUBFOLDER;
			pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&pidlRelative, &dwAttributes);
			pTvi->item.cChildren = (dwAttributes & SFGAO_HASSUBFOLDER) ? 1 : 0;
		}
	}
	SHGetPathFromIDList(pidl, pathFromSH);

	return InsertItem(pTvi);
}

void FolderTreeCtrl::UTStrretToString(LPITEMIDLIST pidl, LPSTRRET pStr, CString &strBuf)
{
	switch (pStr->uType)
	{
	case STRRET_WSTR:	// Unicode string.
		strBuf.Format(_T("%s"), pStr->pOleStr);
		break;
	case STRRET_OFFSET: // Offset.
		strBuf.Format(_T("%s"), (LPCWSTR)reinterpret_cast<LPWSTR>(pidl)+pStr->uOffset);
		break;
	case STRRET_CSTR:	// ANSI string.
		strBuf.Format(_T("%s"), (LPCWSTR)pStr->cStr);
		break;
	}
}

void FolderTreeCtrl::CheckChildItems(HTREEITEM hItem) //  입력된 트리의 자식들 모두 체크.
{
    HTREEITEM hChildItem = GetChildItem(hItem);

    while (hChildItem != NULL)
    {
        if (hChildItem != NULL)
        {
            SetCheck(hChildItem, TRUE);
            CheckChildItems(hChildItem);
        }
        hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
    }
}

void FolderTreeCtrl::UnCheckChildItems(HTREEITEM hItem) // 입력된 트리의 자식들 모두 체크 해제.
{
    HTREEITEM hChildItem = GetChildItem(hItem);

    while (hChildItem != NULL)
    {
        if (hChildItem != NULL)
        {
            SetCheck(hChildItem, FALSE);
            UnCheckChildItems(hChildItem);
        }
        hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
    }
}

void FolderTreeCtrl::ParentCheckOnOff(HTREEITEM hItem) //  자식폴더 체크해제 시 부모폴더 체크해제.
{
    HTREEITEM hParentItem = GetParentItem(hItem);

    while (hParentItem != NULL)
    {
        if (IsAllChild(hParentItem, hItem))
        {
            IsAllParent(hParentItem, TRUE);
        }
        else
        {
            IsAllParent(hParentItem, FALSE);
        }
        hParentItem = GetParentItem(hParentItem);
    }
}

void FolderTreeCtrl::IsAllParent(HTREEITEM hParentItem, BOOL bCheck) // 자식폴더 체크해제 시 전체부모폴더 해제.
{
    while (hParentItem != NULL)
    {
        SetCheck(hParentItem, bCheck);
        hParentItem = GetParentItem(hParentItem);
    }
}

BOOL FolderTreeCtrl::IsAllChild(HTREEITEM hItem, HTREEITEM hClickItem) //자식폴더 미체크 시 FALSE, 체크 시 TRUE Return.
{
    HTREEITEM hChildItem = GetChildItem(hItem);

    while (hChildItem != NULL)
    {
        // 체크박스 해제 또는 설정 시 바로 적용이 안되는 문제 때문에 클릭한 체크박스만 값 반전시킴.
        if (hChildItem == hClickItem)
        {
            if (GetCheck(hChildItem))
            {
                return FALSE;
            }
        }
        else if (GetCheck(hChildItem) == 0)
        {
            return FALSE;
        }
        hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
    }
    return TRUE;
}
